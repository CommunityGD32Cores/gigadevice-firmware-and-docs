# Copyright 2017 Linaro Limited
# Copyright (c) 2018-2019, Arm Limited.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""
Image signing and management.
"""

from . import version as versmod
import hashlib
from . import boot_record as br
import struct

MANIFEST_VERSION = 0
IMAGE_MAGIC = 0x96f3b83d
IMAGE_HEADER_SIZE = 32
TLV_HEADER_SIZE = 4
PAYLOAD_DIGEST_SIZE = 32  # SHA256 hash
KEYHASH_SIZE = 32

IMG_HASH_SHA256 = 1
IMG_SIG_ED25519 = 1

# Image Type.
IMG_TYPE_VALUES = {
        'MBL' : 0x1,
        'PROT': 0x2,
        'AROT': 0x4,
        'NSPE': 0x8,}

# Image header flags.
TLV_VALUES = {
        'SHA256' : 0x10,
        'KEYHASH': 0x20,
        'ED25519': 0x30,
        'BOOT_RECORD': 0x40,}
TLV_INFO_SIZE = 4
TLV_INFO_MAGIC = 0x6907

# Sizes of the image trailer, depending on flash write size.
# trailer_sizes = {
#     write_size: 128 * 3 * write_size + 8 * 2 + 16
#     for write_size in [1, 2, 4, 8]
# }

tail_magic = bytearray([
    0x77, 0xc2, 0x95, 0xf3,
    0x60, 0xd2, 0xef, 0x7f,
    0x35, 0x52, 0x50, 0x0f,
    0x2c, 0xb6, 0x79, 0x80, ])

class TLV():
    def __init__(self):
        self.buf = bytearray()

    def add(self, kind, payload):
        """Add a TLV record.  Kind should be a string found in TLV_VALUES above."""
        buf = struct.pack('<BBH', TLV_VALUES[kind], 0, len(payload))
        self.buf += buf
        self.buf += payload

    def get(self):
        header = struct.pack('<HH', TLV_INFO_MAGIC, TLV_INFO_SIZE + len(self.buf))
        return header + bytes(self.buf)

class Image():
    @classmethod
    def load(cls, path, included_header=False, **kwargs):
        """Load an image from a given file"""
        with open(path, 'rb') as f:
            payload = f.read()
        obj = cls(**kwargs)
        obj.payload = payload

        # Add the image header if needed.
        if not included_header and obj.header_size > 0:
            obj.payload = (b'\000' * obj.header_size) + obj.payload

        obj.check()
        return obj

    def __init__(self, type, version, header_size=IMAGE_HEADER_SIZE,
                 pad=0):
        self.type = type
        self.version = version
        self.header_size = header_size or IMAGE_HEADER_SIZE
        self.pad = pad

    def __repr__(self):
        return "<Image type={}, version={}, header_size={}, \
                 pad={}, payloadlen=0x{:x}>".format(
                self.type,
                self.version,
                self.header_size,
                self.pad,
                len(self.payload))

    def save(self, path):
        with open(path, 'wb') as f:
            f.write(self.payload)

    def check(self):
        """Perform some sanity checking of the image."""
        # If there is a header requested, make sure that the image
        # starts with all zeros.
        if self.header_size > 0:
            if any(v != 0 and v != b'\000' for v in self.payload[0:self.header_size]):
                raise Exception("Padding requested, but image does not start with zeros")

    def sign(self, key, cert_file):
        #image_version = (str(self.version.major) + '.'
        #                + str(self.version.minor) + '.'
        #                + str(self.version.revision))
        image_version = (str((self.version >> 24) & 0xff) + '.'
                        + str((self.version >> 16) & 0xff) + '.'
                        + str(self.version & 0xffff))

        tlv = TLV()

        # Full TLV size needs to be calculated in advance, because the
        # header will be protected as well
        tlv_size = (TLV_INFO_SIZE + len(tlv.buf) + TLV_HEADER_SIZE
                     + PAYLOAD_DIGEST_SIZE)
        if key is not None:
            tlv_size += (TLV_HEADER_SIZE + KEYHASH_SIZE
                          + TLV_HEADER_SIZE + key.sig_len())
            pub = key.get_public_bytes()
            sha = hashlib.sha256()
            sha.update(pub)
            pubbytes = sha.digest()
        else:
            pubbytes = bytes(KEYHASH_SIZE)

        # The image hash is computed over the image header, the image itself
        # and the protected TLV area. However, the boot record TLV (which is
        # part of the protected area) should contain this hash before it is
        # even calculated. For this reason the script fills this field with
        # zeros and the bootloader will insert the right value later.
        image_hash = bytes(PAYLOAD_DIGEST_SIZE)

        # Create CBOR encoded boot record
        boot_record = br.create_sw_component_data(self.type, image_version,
                                                  "SHA256", image_hash,
                                                  pubbytes)

        tlv_size += TLV_HEADER_SIZE + len(boot_record)

        if cert_file is not None:
            with open(cert_file, 'rb') as f:
               cert = f.read()
               cert_size = len(cert)
        else:
            cert_size = 0

        self.add_header(key, tlv_size, cert_size)

        hashsrc = self.payload[(self.header_size - IMAGE_HEADER_SIZE):len(self.payload)]
        sha = hashlib.sha256()
        sha.update(hashsrc)
        digest = sha.digest()
        tlv.add('SHA256', digest)

        tlv_header = struct.pack('HH', TLV_INFO_MAGIC, tlv_size)
        self.payload += tlv_header

        if key is not None:
            tlv.add('KEYHASH', pubbytes)

            # `sign` expects the full image payload (sha256 done internally),
            # while `sign_digest` expects only the digest of the payload

            if hasattr(key, 'sign'):
                sig = key.sign(bytes(self.payload))
            else:
                sig = key.sign_digest(digest)
            tlv.add(key.sig_tlv(), sig)

        tlv.add('BOOT_RECORD', boot_record)

        self.payload += tlv.get()[TLV_INFO_SIZE:]

        if cert_file is not None:
            self.payload += cert

    def add_header(self, key, tlv_size, cert_size):
        """Install the image header.

        The key is needed to know the type of signature, and
        approximate the size of the signature.
        Now use ED25519 as default.     """

        pbytes = b'\xff' * (self.header_size - IMAGE_HEADER_SIZE)

        fmt = ('<' +
            # type ImageHdr struct {
            'I' +    # Magic    uint32
            'I' +    # TotalSz  uint32
            'B' +    # ManiVer  uint8
            'B' +    # ImgType  uint8
            'B' +    # HashAlg  uint8
            'B' +    # SignAlg  uint8
            'H' +    # HdrSz    uint16
            'H' +    # PTLVSz   uint16
            'I' +    # ImgSz    uint32
            'B' +    # Major version   uint8
            'B' +    # Minor version   uint8
            'H' +    # Revision        uint16
            'I'      # Rsvd     uint32
                     # Checksum uint32
            ) # }
        #assert struct.calcsize(fmt) == IMAGE_HEADER_SIZE
        img_size = len(self.payload) - self.header_size
        tot_size = IMAGE_HEADER_SIZE + img_size + tlv_size + cert_size
        #print("img_size : ", img_size)
        #print("tlv_size : ", tlv_size)
        #print("cert_size : ", cert_size)
        #print("tot_size : ", tot_size, "  hex: ", hex(tot_size))
        major = (self.version >> 24)
        minor = ((self.version & 0xFFFFFF) >> 16)
        revision = (self.version & 0xFFFF)
        #print("major: ", major, " minor: ", minor, " rev: ", revision)
        #print("image type : ", IMG_TYPE_VALUES[self.type])
        header = struct.pack(fmt,
                IMAGE_MAGIC,
                tot_size,
                MANIFEST_VERSION,
                IMG_TYPE_VALUES[self.type],
                IMG_HASH_SHA256,
                IMG_SIG_ED25519,
                IMAGE_HEADER_SIZE,
                tlv_size,
                img_size,
                major,
                minor,
                revision,
                0)  # Rsvd

        n = 0
        chksum = 0
        while n < len(header):
            num = header[n]
            num += (header[n + 1]) << 8
            num += (header[n + 2]) << 16
            num += (header[n + 3]) << 24
            #print("num: ", num, "  hex: ", hex(num))
            chksum ^= num
            n += 4
            #print("chksum hex: ", hex(chksum))
        chksum &= 0xFFFFFFFF
        header += struct.pack('<I', chksum)

        #print("header : ", header)
        header_with_pad = pbytes + header

        self.payload = bytearray(self.payload)
        self.payload[:len(header_with_pad)] = header_with_pad

    def pad_to(self, size, align):
        """Pad the image to the given size, with the given flash alignment."""
        if align not in [1, 2, 4, 8]:
            raise Exception("Align must be one of 1, 2, 4 or 8.")
        remain = size % align
        if remain != 0:
          size += align - remain
        padding = size - len(self.payload)
        if padding < 0:
            msg = "Image size (0x{:x}) exceeds requested size 0x{:x}".format(
                    len(self.payload), size)
            raise Exception(msg)
        pbytes  = b'\xff' * padding
        self.payload += pbytes
