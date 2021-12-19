"""
ECDSA key management
"""

from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import ed25519
from cryptography import x509
from cryptography.x509.oid import NameOID
from cryptography.hazmat.primitives import hashes

from .general import KeyClass


class Ed25519UsageError(Exception):
    pass


class Ed25519Public(KeyClass):
    def __init__(self, key):
        self.key = key

    def shortname(self):
        return "ed25519"

    def _unsupported(self, name):
        raise Ed25519UsageError("Operation {} requires private key".format(name))

    def _get_public(self):
        return self.key

    def get_public_bytes(self):
        # The key is embedded into MBUboot in "SubjectPublicKeyInfo" format
        return self._get_public().public_bytes(
                encoding=serialization.Encoding.Raw,
                format=serialization.PublicFormat.Raw)

    def export_private(self, path, passwd=None):
        self._unsupported('export_private')

    def export_public(self, path):
        """Write the public key to the given file."""
        pem = self._get_public().public_bytes(
                encoding=serialization.Encoding.PEM,
                format=serialization.PublicFormat.SubjectPublicKeyInfo)
        with open(path, 'wb') as f:
            f.write(pem)

    def sig_type(self):
        return "ED25519"

    def sig_tlv(self):
        return "ED25519"

    def sig_len(self):
        return 64


class Ed25519(Ed25519Public):
    """
    Wrapper around an ECDSA private key.
    """

    def __init__(self, key):
        """key should be an instance of EllipticCurvePrivateKey"""
        self.key = key

    @staticmethod
    def generate():
        pk = ed25519.Ed25519PrivateKey.generate()
        return Ed25519(pk)

    def _get_public(self):
        return self.key.public_key()

    def export_private(self, path, passwd=None):
        """
        Write the private key to the given file, protecting it with the
        optional password.
        """
        if passwd is None:
            enc = serialization.NoEncryption()
        else:
            enc = serialization.BestAvailableEncryption(passwd)
        pem = self.key.private_bytes(
                encoding=serialization.Encoding.PEM,
                format=serialization.PrivateFormat.PKCS8,
                encryption_algorithm=enc)
        with open(path, 'wb') as f:
            f.write(pem)

    def sign_digest(self, digest):
        """Return the actual signature"""
        return self.key.sign(data=digest)

    def verify_digest(self, signature, digest):
        """Verify that signature is valid for given digest"""
        k = self.key
        if isinstance(self.key, ed25519.Ed25519PrivateKey):
            k = self.key.public_key()
        return k.verify(signature=signature, data=digest)
    
    def create_csr(self, path):
        # Generate a CSR
        csr = x509.CertificateSigningRequestBuilder().subject_name(x509.Name([
            # Provide various details about who we are.
            x509.NameAttribute(NameOID.COUNTRY_NAME, u"CN"),
            x509.NameAttribute(NameOID.STATE_OR_PROVINCE_NAME, u"Jiangsu"),
            x509.NameAttribute(NameOID.LOCALITY_NAME, u"Suzhou"),
            x509.NameAttribute(NameOID.ORGANIZATION_NAME, u"My Company"),
            x509.NameAttribute(NameOID.COMMON_NAME, u"gigadevice.com/"),
        ])).add_extension(
            x509.SubjectAlternativeName([
                # Describe what sites we want this certificate for.
                x509.DNSName(u"gigadevice.com"),
                x509.DNSName(u"www.gigadevice.com"),
                x509.DNSName(u"subdomain.gigadevice.com"),
            ]),
            critical=False,
        # Sign the CSR with our private key.
        ).sign(self.key, hashes.SHA256(), default_backend())

        # Write our CSR out to disk. "path/to/csr.pem"
        with open(path, "wb") as f:
            f.write(csr.public_bytes(serialization.Encoding.PEM))
