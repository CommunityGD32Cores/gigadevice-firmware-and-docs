/**
 * NOTE:
 *
 * HAL_TCP_xxx API reference implementation: wrappers/os/ubuntu/HAL_TCP_linux.c
 *
 */

#include "app_cfg.h"

#if defined(CONFIG_ALICLOUD_SUPPORT)
#include <stdarg.h>
#include <stdio.h>
#include "infra_types.h"
#include "infra_defs.h"
#include "infra_compat.h"
#include "wrappers_defs.h"
#include "stdarg.h"



#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "lwip/netdb.h"

#include "mbedtls/config.h"
#include "ssl_tz.h"
#include "mbedtls/error.h"
#include "mbedtls/ssl.h"
#include "mbedtls/net.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/pk.h"
#include "mbedtls/debug.h"
#include "mbedtls/platform.h"
#if defined(CONFIG_TZ_ENABLED)
#include "mbedtls_nsc.h"
#include "rom_export.h"
#include "mbl_nsc_api.h"
#else
#include "mbl_api.h"
#endif
#include "nspe_region.h"

#include "wifi_netif.h"
#include "wifi_netlink.h"
#include "wifi_management.h"
#include "wrapper_os.h"

#define platform_warn(fmt, ...) printf("AliLinkit HAL warn:" fmt, ##__VA_ARGS__)
#define platform_info(fmt, ...) printf("AliLinkit HAL info:" fmt, ##__VA_ARGS__)
#define platform_err(fmt, ...) printf("AliLinkit HAL error:" fmt, ##__VA_ARGS__)

/**
 *
 * 函数 HAL_Awss_Close_Monitor() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_Awss_Close_Monitor() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_Awss_Close_Monitor(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_Awss_Close_Monitor, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_AWSS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
void HAL_Awss_Close_Monitor(void)
{
    platform_warn("HAL_Awss_Close_Monitor\r\n");
    wifi_netlink_promisc_mode_set(0, NULL);
}


/**
 *
 * 函数 HAL_Awss_Connect_Ap() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_Awss_Connect_Ap() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_Awss_Connect_Ap(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_Awss_Connect_Ap, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_AWSS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
int HAL_Awss_Connect_Ap(
            _IN_ uint32_t connection_timeout_ms,
            _IN_ char ssid[HAL_MAX_SSID_LEN],
            _IN_ char passwd[HAL_MAX_PASSWD_LEN],
            _IN_OPT_ enum AWSS_AUTH_TYPE auth,
            _IN_OPT_ enum AWSS_ENC_TYPE encry,
            _IN_OPT_ uint8_t bssid[ETH_ALEN],
            _IN_OPT_ uint8_t channel)
{
    int32_t ret = 0;
    uint32_t retry = 2;
    uint32_t i;

    if (ssid == NULL) {
        platform_warn("connect to AP, ssid is NULL\r\n");
        return -1;
    }
#ifdef CONFIG_WIFI_MANAGEMENT_TASK
    ret = wifi_management_connect((uint8_t *)ssid, (uint8_t *)passwd, FALSE);
    if (ret != 0) {
        platform_warn("start wifi_connect failed\r\n");
    }

    for (i = 0; i < 30; i++) {
        sys_ms_sleep(1000);
        if (wifi_netif_is_ip_got()) {
            return 0;
        }
    }
#else
    if ((wifi_netlink_link_state_get() >= WIFI_NETLINK_STATUS_LINKED)
        || (wifi_netlink_link_state_get() == WIFI_NETLINK_STATUS_ROAMING)) {
        wifi_netlink_disconnect_req();
        wifi_netif_stop_dhcp();
    }

    while (retry) {
        ret = wifi_netlink_connect_req((uint8_t *)ssid, (uint8_t *)passwd);
        if (ret != 0) {
            platform_warn("wifi connect failed\r\n");
            retry--;
            continue;
        }

        for (i = 0; i < 20; i++) {
            if (wifi_netlink_link_state_get() == WIFI_NETLINK_STATUS_LINKED) {
                wifi_netif_start_dhcp();
                if (TRUE == wifi_netif_polling_dhcp()) {
                    wifi_netlink_ipaddr_set((u8*)wifi_netif_get_ip());
                    platform_warn("WIFI_CONNECT: ip configured, IP: %s\r\n", ip_ntoa(wifi_netif_get_ip()));
                    return 0;
                } else {
                    platform_warn("WIFI_CONNECT: get ip timeout!\r\n");
                    wifi_netif_stop_dhcp();
                    wifi_netlink_disconnect_req();
                    break;
                }
            } else {
                sys_ms_sleep(500);
            }
        }
        retry--;
    }
#endif

    return -1;
}


/**
 *
 * 函数 HAL_Awss_Switch_Channel() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_Awss_Switch_Channel() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_Awss_Switch_Channel(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_Awss_Switch_Channel, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_AWSS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
void HAL_Awss_Switch_Channel(
            _IN_ char primary_channel,
            _IN_OPT_ char secondary_channel,
            _IN_OPT_ uint8_t bssid[ETH_ALEN])
{
    wifi_netlink_channel_set((uint32_t)primary_channel, CHANNEL_WIDTH_20, 0);
}


/**
 *
 * 函数 HAL_Firmware_Persistence_Start() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_Firmware_Persistence_Start() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_Firmware_Persistence_Start(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_Firmware_Persistence_Start, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */

uint32_t g_firmware_offset;

void HAL_Firmware_Persistence_Start(void)
{
    uint8_t running_idx;
    uint32_t img_addr = 0xFFFFFFFF;
    uint32_t max_img_len;
    int32_t ret = 0, res;

    platform_info("OTA start...\r\n");

    g_firmware_offset = 0;

    res = mbl_sys_status_get(SYS_RUNNING_IMG, LEN_SYS_RUNNING_IMG, &running_idx);
    if (res < 0) {
        platform_err("OTA get running idx failed! (res = %d)\r\n", res);
        return;
    }
    if (running_idx == IMAGE_0) {
        img_addr = RE_IMG_1_PROT_OFFSET;
        max_img_len = RE_IMG_1_END_OFFSET - RE_IMG_1_PROT_OFFSET;
    } else {
        img_addr = RE_IMG_0_PROT_OFFSET;
        max_img_len = RE_IMG_1_PROT_OFFSET - RE_IMG_0_PROT_OFFSET;
    }

    res = mbl_flash_erase(img_addr, max_img_len);

    if (res < 0) {
        platform_err("OTA flash erase failed (res = %d)\r\n", res);
    }
}


/**
 *
 * 函数 HAL_Firmware_Persistence_Stop() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_Firmware_Persistence_Stop() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_Firmware_Persistence_Stop(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_Firmware_Persistence_Stop, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
int HAL_Firmware_Persistence_Stop(void)
{
    uint8_t running_idx;
    int32_t res;

    res = mbl_sys_status_get(SYS_RUNNING_IMG, LEN_SYS_RUNNING_IMG, &running_idx);

    if (res < 0) {
        platform_err("OTA get running idx failed! (res = %d)\r\n", res);
        return -1;
    }

    /* Set image status */
    res = mbl_sys_img_flag_set(running_idx, (IMG_FLAG_IA_MASK | IMG_FLAG_NEWER_MASK), (IMG_FLAG_IA_OK | IMG_FLAG_OLDER));
    res |= mbl_sys_img_flag_set(!running_idx, (IMG_FLAG_IA_MASK | IMG_FLAG_VERIFY_MASK | IMG_FLAG_NEWER_MASK), 0);
    res |= mbl_sys_img_flag_set(!running_idx, IMG_FLAG_NEWER_MASK, IMG_FLAG_NEWER);

    if (res != 0) {
        platform_err("OTA set image status failed! (res = %d)\r\n", res);
        return -1;
    }

    g_firmware_offset = 0;
    platform_info("OTA finish...\r\n");
    return 0;
}


/**
 *
 * 函数 HAL_Firmware_Persistence_Write() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_Firmware_Persistence_Write() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_Firmware_Persistence_Write(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_Firmware_Persistence_Write, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
int HAL_Firmware_Persistence_Write(char *buffer, uint32_t length)
{
    uint8_t running_idx;
    uint32_t img_addr = 0xFFFFFFFF;
    uint32_t max_img_len;
    int32_t res;
    res = mbl_sys_status_get(SYS_RUNNING_IMG, LEN_SYS_RUNNING_IMG, &running_idx);
    if (res < 0) {
        platform_err("OTA get running idx failed! (res = %d)\r\n", res);
        return -1;
    }

    if (running_idx == IMAGE_0) {
        img_addr = RE_IMG_1_PROT_OFFSET;
        max_img_len = RE_IMG_1_END_OFFSET - RE_IMG_1_PROT_OFFSET;
    } else {
        img_addr = RE_IMG_0_PROT_OFFSET;
        max_img_len = RE_IMG_1_PROT_OFFSET - RE_IMG_0_PROT_OFFSET;
    }

    if ((g_firmware_offset + length) > max_img_len) {
        platform_err("OTA firmware size overflow!\r\n");
        return -1;
    }

    res = mbl_flash_write((img_addr + g_firmware_offset), (uint8_t *)buffer, length);

    if (res < 0) {
        platform_err("OTA flash write failed!\r\n");
        return -1;
    }

    g_firmware_offset += length;
    return 0;
}

/**
 *
 * 函数 HAL_Awss_Open_Monitor() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_Awss_Open_Monitor() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_Awss_Open_Monitor(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_Awss_Open_Monitor, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_AWSS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
static awss_recv_80211_frame_cb_t g_ieee80211_handler = NULL;

static void monitor_data_handler(unsigned char *buf, unsigned short len, signed char rssi)
{
    int with_fcs  = 0;
    enum AWSS_LINK_TYPE link_type = AWSS_LINK_TYPE_NONE;

    if (buf == NULL) {
        return;
    }

    if (g_ieee80211_handler != NULL) {
        // platform_warn("monitor_data_handler len %d rssi %d\r\n", len, rssi);
        (*g_ieee80211_handler)((char *)buf, (int)len, link_type, with_fcs, rssi);
    }
}
void HAL_Awss_Open_Monitor(_IN_ awss_recv_80211_frame_cb_t cb)
{
    platform_warn("HAL_Awss_Open_Monitor\r\n");
    g_ieee80211_handler = cb;
    wifi_netlink_promisc_mode_set(1, monitor_data_handler);
    HAL_Awss_Switch_Channel(6, 0, NULL);
}


/**
 *
 * 函数 HAL_Free() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_Free() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_Free(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_Free, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
/**
 * @brief Deallocate memory block
 *
 * @param[in] ptr @n Pointer to a memory block previously allocated with platform_malloc.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_Free(void *ptr)
{
    sys_mfree(ptr);
}


/**
 *
 * 函数 HAL_GetFirmwareVersion() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_GetFirmwareVersion() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_GetFirmwareVersion(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_GetFirmwareVersion, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
/**
 * @brief Get firmware version
 *
 * @param [ou] version: array to store firmware version, max length is IOTX_FIRMWARE_VER_LEN
 * @return the actual length of firmware version
 */
int HAL_GetFirmwareVersion(char *version)
{
    uint32_t ver = RE_NSPE_VERSION;

    return snprintf(version, IOTX_FIRMWARE_VER_LEN, "%d.%d.%d", (ver >> 24), ((ver << 8) >> 24), (ver & 0xFFFF));
}


/**
 *
 * 函数 HAL_Kv_Get() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_Kv_Get() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_Kv_Get(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_Kv_Get, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_KV_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
int HAL_Kv_Get(const char *key, void *val, int *buffer_len)
{
    return (int)1;
}


/**
 *
 * 函数 HAL_Kv_Set() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_Kv_Set() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_Kv_Set(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_Kv_Set, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_KV_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
int HAL_Kv_Set(const char *key, const void *val, int len, int sync)
{
    return (int)1;
}


/**
 *
 * 函数 HAL_Malloc() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_Malloc() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_Malloc(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_Malloc, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
/**
 * @brief Allocates a block of size bytes of memory, returning a pointer to the beginning of the block.
 *
 * @param [in] size @n specify block size in bytes.
 * @return A pointer to the beginning of the block.
 * @see None.
 * @note Block value is indeterminate.
 */
void *HAL_Malloc(uint32_t size)
{
    return sys_malloc(size);
}


/**
 *
 * 函数 HAL_MutexCreate() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_MutexCreate() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_MutexCreate(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_MutexCreate, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
/**
 * @brief Create a mutex.
 *
 * @retval NULL : Initialize mutex failed.
 * @retval NOT_NULL : The mutex handle.
 * @see None.
 * @note None.
 */
void *HAL_MutexCreate(void)
{
    os_mutex_t mutex;

    sys_mutex_init(&mutex);

    return (void *)mutex;
}


/**
 *
 * 函数 HAL_MutexDestroy() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_MutexDestroy() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_MutexDestroy(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_MutexDestroy, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
/**
 * @brief Destroy the specified mutex object, it will release related resource.
 *
 * @param [in] mutex @n The specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_MutexDestroy(void *mutex)
{
    os_mutex_t mtx = (os_mutex_t)mutex;

    if (NULL != mutex) {
        sys_mutex_free((os_mutex_t *)&mtx);
    }
}


/**
 *
 * 函数 HAL_MutexLock() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_MutexLock() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_MutexLock(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_MutexLock, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
/**
 * @brief Waits until the specified mutex is in the signaled state.
 *
 * @param [in] mutex @n the specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_MutexLock(void *mutex)
{
    os_mutex_t mtx = (os_mutex_t)mutex;

    if (NULL != mutex) {
        sys_mutex_get((os_mutex_t *)&mtx);
    }
}


/**
 *
 * 函数 HAL_MutexUnlock() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_MutexUnlock() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_MutexUnlock(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_MutexUnlock, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
/**
 * @brief Releases ownership of the specified mutex object..
 *
 * @param [in] mutex @n the specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_MutexUnlock(void *mutex)
{
    os_mutex_t mtx = (os_mutex_t)mutex;

    if (NULL != mutex) {
        sys_mutex_put((os_mutex_t *)&mtx);
    }
}


/**
 *
 * 函数 HAL_Random() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_Random() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_Random(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_Random, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
uint32_t HAL_Random(uint32_t region)
{
    uint32_t rand_num;

    sys_random_bytes_get((void *)&rand_num, 4);
    return (region > 0) ? (rand_num % region) : 0;
}

/**
 * Mbedtls related Start
 * */

#define SEND_TIMEOUT_SECONDS (10)
#define DEBUG_LEVEL (0)

typedef struct _TLSDataParams {
    mbedtls_ssl_context ssl;     /**< mbed TLS control context. */
    mbedtls_net_context fd;      /**< mbed TLS network context. */
    mbedtls_ssl_config conf;     /**< mbed TLS configuration context. */
    mbedtls_x509_crt cacertl;    /**< mbed TLS CA certification. */
    mbedtls_x509_crt clicert;    /**< mbed TLS Client certification. */
    mbedtls_pk_context pkey;     /**< mbed TLS Client key. */
} TLSDataParams_t, *TLSDataParams_pt;

static unsigned int _avRandom()
{
    uint32_t rand_num;

    sys_random_bytes_get((void *)&rand_num, 4);
    return (((unsigned int)rand_num << 16) + rand_num);
}

static int _ssl_random(void *p_rng, unsigned char *output, size_t output_len)
{
    uint32_t rnglen = output_len;
    uint8_t rngoffset = 0;

    while (rnglen > 0) {
        *(output + rngoffset) = (unsigned char)_avRandom();
        rngoffset++;
        rnglen--;
    }
    return 0;
}

static void _ssl_debug(void *ctx, int level, const char *file, int line,
                       const char *str)
{
    ((void)level);
    if (NULL != str) {
#if 0
        fprintf((FILE *) ctx, "%s:%04d: %s", file, line, str);
        fflush((FILE *) ctx);
#endif
        printf("%s:%04d: %s", file, line, str);
    }
}

static int _real_confirm(int verify_result)
{
    platform_info("certificate verification result: 0x%02x\r\n", verify_result);

#if defined(FORCE_SSL_VERIFY)
    if ((verify_result & MBEDTLS_X509_BADCERT_EXPIRED) != 0)
    {
        platform_err("! fail ! ERROR_CERTIFICATE_EXPIRED\r\n");
        return -1;
    }

    if ((verify_result & MBEDTLS_X509_BADCERT_REVOKED) != 0)
    {
        platform_err("! fail ! server certificate has been revoked\r\n");
        return -1;
    }

    if ((verify_result & MBEDTLS_X509_BADCERT_CN_MISMATCH) != 0)
    {
        platform_err("! fail ! CN mismatch\r\n");
        return -1;
    }

    if ((verify_result & MBEDTLS_X509_BADCERT_NOT_TRUSTED) != 0)
    {
        platform_err("! fail ! self-signed or not signed by a trusted CA\r\n");
        return -1;
    }
#endif

    return 0;
}

extern void parse_cipher_suite_set(char *arg);

static int _ssl_client_init(mbedtls_ssl_context *ssl,
                            mbedtls_net_context *tcp_fd,
                            mbedtls_ssl_config * conf,
                            mbedtls_x509_crt *crt509_ca, const char *ca_crt,
                            size_t ca_len, mbedtls_x509_crt *crt509_cli,
                            const char *cli_crt, size_t cli_len,
                            mbedtls_pk_context *pk_cli, const char *cli_key,
                            size_t key_len, const char *cli_pwd, size_t pwd_len)
{
    int ret = -1;
#ifdef CONFIG_SSL_TEST
    char *str = "0";
    parse_cipher_suite_set(str);
#endif

    /*
     * 0. Initialize the RNG and the session data
     */
#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold((int)DEBUG_LEVEL);
#endif
#ifndef CONFIG_TZ_ENABLED  //  MBL has called it yet
    mbedtls_ecp_curve_val_init();
#endif
    mbedtls_net_init(tcp_fd);
    mbedtls_ssl_init(ssl);
    mbedtls_ssl_config_init(conf);
    mbedtls_x509_crt_init(crt509_ca);

    /*verify_source->trusted_ca_crt==NULL
     * 0. Initialize certificates
     */

    platform_info("Loading the CA root certificate ...\r\n");
    if (NULL != ca_crt)
    {
        if (0 != (ret = mbedtls_x509_crt_parse(
                      crt509_ca, (const unsigned char *)ca_crt, ca_len)))
        {
            platform_err(" failed ! x509parse_crt returned -0x%04x\r\n", -ret);
            return ret;
        }
    }
    platform_info(" ok (%d skipped)\r\n", ret);

    /* Setup Client Cert/Key */
#if defined(MBEDTLS_X509_CRT_PARSE_C)
#if defined(MBEDTLS_CERTS_C)
    mbedtls_x509_crt_init(crt509_cli);
    mbedtls_pk_init(pk_cli);
#endif
    if (cli_crt != NULL && cli_key != NULL)
    {
#if defined(MBEDTLS_CERTS_C)
        platform_info("start prepare client cert .\r\n");
        ret = mbedtls_x509_crt_parse(crt509_cli, (const unsigned char *)cli_crt, cli_len);
#else
        {
            ret = 1;
            platform_err("MBEDTLS_CERTS_C not defined.\r\n");
        }
#endif
        if (ret != 0)
        {
            platform_err(" failed!  mbedtls_x509_crt_parse returned -0x%x\r\n",
                         -ret);
            return ret;
        }

#if defined(MBEDTLS_CERTS_C)
        platform_info("start mbedtls_pk_parse_key[%s]\r\n", cli_pwd);
        ret =
            mbedtls_pk_parse_key(pk_cli, (const unsigned char *)cli_key, key_len,
                                 (const unsigned char *)cli_pwd, pwd_len);
#else
        {
            ret = 1;
            platform_err("MBEDTLS_CERTS_C not defined.\r\n");
        }
#endif

        if (ret != 0)
        {
            platform_err(" failed\n  !  mbedtls_pk_parse_key returned -0x%x\r\n",
                         -ret);
            return ret;
        }
    }
#endif /* MBEDTLS_X509_CRT_PARSE_C */

    return 0;
}

static int _TLSConnectNetwork(TLSDataParams_t *pTlsData, const char *addr,
                              const char *port, const char *ca_crt,
                              size_t ca_crt_len, const char *client_crt,
                              size_t client_crt_len, const char *client_key,
                              size_t client_key_len, const char *client_pwd,
                              size_t client_pwd_len)
{
    int ret = -1;
    /*
     * 0. Init
     */
    if (0 != (ret = _ssl_client_init(
                  &(pTlsData->ssl), &(pTlsData->fd), &(pTlsData->conf),
                  &(pTlsData->cacertl), ca_crt, ca_crt_len, &(pTlsData->clicert),
                  client_crt, client_crt_len, &(pTlsData->pkey), client_key,
                  client_key_len, client_pwd, client_pwd_len))) {
        platform_err(" failed ! ssl_client_init returned -0x%04x\r\n", -ret);
        return ret;
    }

    /*
     * 1. Start the connection
     */
    platform_info("Connecting to /%s/%s...\r\n", addr, port);
    if (0 != (ret = mbedtls_net_connect(&(pTlsData->fd), addr, port,
                                        MBEDTLS_NET_PROTO_TCP))) {
        platform_err(" failed ! net_connect returned -0x%04x\r\n", -ret);
        return ret;
    }
    platform_info(" ok\r\n");

    /*
     * 2. Setup stuff
     */
    platform_info("  . Setting up the SSL/TLS structure...\r\n");
    if ((ret = mbedtls_ssl_config_defaults(
             &(pTlsData->conf), MBEDTLS_SSL_IS_CLIENT,
             MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        platform_err(" failed! mbedtls_ssl_config_defaults returned %d\r\n", ret);
        return ret;
    }

    mbedtls_ssl_conf_max_version(&pTlsData->conf, MBEDTLS_SSL_MAJOR_VERSION_3,
                                 MBEDTLS_SSL_MINOR_VERSION_3);
    mbedtls_ssl_conf_min_version(&pTlsData->conf, MBEDTLS_SSL_MAJOR_VERSION_3,
                                 MBEDTLS_SSL_MINOR_VERSION_3);

    platform_info(" ok\r\n");

    /* OPTIONAL is not optimal for security, but makes interop easier in this
     * simplified example */
    if (ca_crt != NULL) {
#if defined(FORCE_SSL_VERIFY)
        mbedtls_ssl_conf_authmode(&(pTlsData->conf),
                                  MBEDTLS_SSL_VERIFY_REQUIRED);
#else
        mbedtls_ssl_conf_authmode(&(pTlsData->conf),
                                  MBEDTLS_SSL_VERIFY_OPTIONAL);
#endif
    }
    else {
        mbedtls_ssl_conf_authmode(&(pTlsData->conf), MBEDTLS_SSL_VERIFY_NONE);
    }

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_ssl_conf_ca_chain(&(pTlsData->conf), &(pTlsData->cacertl), NULL);

    if ((ret = mbedtls_ssl_conf_own_cert(
             &(pTlsData->conf), &(pTlsData->clicert), &(pTlsData->pkey))) != 0) {
        platform_err(" failed\n  ! mbedtls_ssl_conf_own_cert returned %d\r\n",
                     ret);
        return ret;
    }
#endif

#if defined(CONFIG_TZ_ENABLED)
    mbedtls_ssl_conf_rng( &(pTlsData->conf), mbedtls_random_nsc, NULL);
#else
    mbedtls_ssl_conf_rng(&(pTlsData->conf), _ssl_random, NULL);
#endif
    mbedtls_ssl_conf_dbg(&(pTlsData->conf), _ssl_debug, NULL);
    /* mbedtls_ssl_conf_dbg( &(pTlsData->conf), _ssl_debug, stdout ); */

    if ((ret = mbedtls_ssl_setup(&(pTlsData->ssl), &(pTlsData->conf))) != 0) {
        platform_err("failed! mbedtls_ssl_setup returned %d\r\n", ret);
        return ret;
    }
    mbedtls_ssl_set_hostname(&(pTlsData->ssl), addr);
    mbedtls_ssl_set_bio(&(pTlsData->ssl), &(pTlsData->fd), mbedtls_net_send,
                        mbedtls_net_recv, mbedtls_net_recv_timeout);

    /*
     * 4. Handshake
     */
    platform_info("Performing the SSL/TLS handshake...\r\n");
    while ((ret = mbedtls_ssl_handshake(&(pTlsData->ssl))) != 0) {
        if ((ret != MBEDTLS_ERR_SSL_WANT_READ) &&
            (ret != MBEDTLS_ERR_SSL_WANT_WRITE)) {
            platform_err("failed  ! mbedtls_ssl_handshake returned -0x%04x\r\n",
                         -ret);
            return ret;
        }
    }
    platform_info(" ok\r\n");
    /*
     * 5. Verify the server certificate
     */
    platform_info("  . Verifying peer X.509 certificate..\r\n");
    if (0 != (ret = _real_confirm(
                  mbedtls_ssl_get_verify_result(&(pTlsData->ssl))))) {
        platform_err(" failed  ! verify result not confirmed.\r\n");
        return ret;
    }
    /* n->my_socket = (int)((n->tlsdataparams.fd).fd); */
    /* WRITE_IOT_DEBUG_LOG("my_socket=%d", n->my_socket); */

    return 0;
}

static int _network_ssl_read(TLSDataParams_t *pTlsData, char *buffer, int len,
                             int timeout_ms)
{
    uint32_t readLen = 0;
    static int net_status = 0;
    int ret = -1;

    mbedtls_ssl_conf_read_timeout(&(pTlsData->conf), timeout_ms);
    while (readLen < len) {
        ret = mbedtls_ssl_read(&(pTlsData->ssl),
                               (unsigned char *)(buffer + readLen),
                               (len - readLen));
        if (ret > 0) {
            readLen += ret;
            net_status = 0;
        }
        else if (ret == 0) {
        /* if ret is 0 and net_status is -2, indicate the connection is closed during last call */
            return (net_status == -2) ? net_status : readLen;
        }
        else {
            if (MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY == ret) {
                // mbedtls_strerror(ret, err_str, sizeof(err_str));
                platform_err("ssl recv error: code = %d\r\n", ret);
                net_status = -2; /* connection is closed */
                break;
            }
            else if ((MBEDTLS_ERR_SSL_TIMEOUT == ret) ||
                     (MBEDTLS_ERR_SSL_CONN_EOF == ret) ||
                     (MBEDTLS_ERR_SSL_SESSION_TICKET_EXPIRED == ret) ||
                     (MBEDTLS_ERR_SSL_WANT_READ == ret) ||
                     (MBEDTLS_ERR_SSL_NON_FATAL == ret)) {
                /* read already complete */
                /* if call mbedtls_ssl_read again, it will return 0 (means EOF) */
                return readLen;
            }
            else {
                // mbedtls_strerror(ret, err_str, sizeof(err_str));
                platform_err("ssl recv error: code = %d\r\n", ret);
                net_status = -1;
                return -1; /* Connection error */
            }
        }
    }

    return (readLen > 0) ? readLen : net_status;
}

static int _network_ssl_write(TLSDataParams_t *pTlsData, const char *buffer,
                              int len, int timeout_ms)
{
    uint32_t writtenLen = 0;
    int ret = -1;

    while (writtenLen < len) {
        ret = mbedtls_ssl_write(&(pTlsData->ssl),
                                (unsigned char *)(buffer + writtenLen),
                                (len - writtenLen));
        if (ret > 0) {
            writtenLen += ret;
            continue;
        }
        else if (ret == 0) {
            platform_err("ssl write timeout\r\n");
            return 0;
        }
        else {
            // mbedtls_strerror(ret, err_str, sizeof(err_str));
            platform_err("ssl write fail, code=%d\r\n", ret);
            return -1; /* Connnection error */
        }
    }

    return writtenLen;
}

static void _network_ssl_disconnect(TLSDataParams_t *pTlsData)
{
    mbedtls_ssl_close_notify(&(pTlsData->ssl));
    mbedtls_net_free(&(pTlsData->fd));
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_x509_crt_free(&(pTlsData->cacertl));
    if ((pTlsData->pkey).pk_info != NULL)
    {
        platform_info("need release client crt&key\r\n");
#if defined(MBEDTLS_CERTS_C)
        mbedtls_x509_crt_free(&(pTlsData->clicert));
        mbedtls_pk_free(&(pTlsData->pkey));
#endif
    }
#endif
    mbedtls_ssl_free(&(pTlsData->ssl));
    mbedtls_ssl_config_free(&(pTlsData->conf));
    platform_info("ssl_disconnect\r\n");
}

/**
 * Mbedtls related End
 * */

/**
 *
 * 函数 HAL_SSL_Destroy() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_SSL_Destroy() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_SSL_Destroy(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_SSL_Destroy, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/tls/HAL_TLS_mbedtls.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
int32_t HAL_SSL_Destroy(uintptr_t handle)
{
    if ((uintptr_t)NULL == handle) {
        platform_err("handle is NULL\r\n");
        return 0;
    }

    _network_ssl_disconnect((TLSDataParams_t *)handle);
    HAL_Free((void *)handle);
    return 0;
}


/**
 *
 * 函数 HAL_SSL_Establish() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_SSL_Establish() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_SSL_Establish(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_SSL_Establish, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/tls/HAL_TLS_mbedtls.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
uintptr_t HAL_SSL_Establish(const char *host,
                            uint16_t port,
                            const char *ca_crt,
                            uint32_t ca_crt_len)
{
    char port_str[6];
    TLSDataParams_pt pTlsData;

    pTlsData = HAL_Malloc(sizeof(TLSDataParams_t));
    if (NULL == pTlsData)
    {
        return (uintptr_t)NULL;
    }
    sys_memset(pTlsData, 0x0, sizeof(TLSDataParams_t));

    snprintf(port_str, sizeof(port_str), "%u", port);

    if (0 != _TLSConnectNetwork(pTlsData, host, port_str, ca_crt, ca_crt_len,
                                NULL, 0, NULL, 0, NULL, 0))
    {
        _network_ssl_disconnect(pTlsData);
        HAL_Free((void *)pTlsData);
        return (uintptr_t)NULL;
    }

    return (uintptr_t)pTlsData;
}

/**
 *
 * 函数 HAL_SSL_Read() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_SSL_Read() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_SSL_Read(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_SSL_Read, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/tls/HAL_TLS_mbedtls.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
int HAL_SSL_Read(uintptr_t handle, char *buf, int len, int timeout_ms)
{
    return _network_ssl_read((TLSDataParams_t *)handle, buf, len, timeout_ms);
}


/**
 *
 * 函数 HAL_SSL_Write() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_SSL_Write() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_SSL_Write(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_SSL_Write, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/tls/HAL_TLS_mbedtls.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
int HAL_SSL_Write(uintptr_t handle, const char *buf, int len, int timeout_ms)
{
    return _network_ssl_write((TLSDataParams_t *)handle, buf, len, timeout_ms);
}


/**
 *
 * 函数 HAL_SemaphoreCreate() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_SemaphoreCreate() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_SemaphoreCreate(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_SemaphoreCreate, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
/**
 * @brief   create a semaphore
 *
 * @return semaphore handle.
 * @see None.
 * @note The recommended value of maximum count of the semaphore is 255.
 */
void *HAL_SemaphoreCreate(void)
{
    os_sema_t sem;

    if (0 != sys_sema_init(&sem, 0)) {
        return NULL;
    }

    return (void *)sem;
}


/**
 *
 * 函数 HAL_SemaphoreDestroy() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_SemaphoreDestroy() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_SemaphoreDestroy(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_SemaphoreDestroy, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
/**
 * @brief   destory a semaphore
 *
 * @param[in] sem @n the specified sem.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_SemaphoreDestroy(void *sem)
{
    os_sema_t sema = (os_sema_t)sem;

    if (NULL != sem) {
        sys_sema_free((os_sema_t *)&sema);
    }
}


/**
 *
 * 函数 HAL_SemaphorePost() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_SemaphorePost() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_SemaphorePost(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_SemaphorePost, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
/**
 * @brief   signal thread wait on a semaphore
 *
 * @param[in] sem @n the specified semaphore.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_SemaphorePost(void *sem)
{
    os_sema_t sema = (os_sema_t)sem;

    if (NULL != sem) {
        sys_sema_up((os_sema_t *)&sema);
    }
}


/**
 *
 * 函数 HAL_SemaphoreWait() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_SemaphoreWait() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_SemaphoreWait(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_SemaphoreWait, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
/**
 * @brief   wait on a semaphore
 *
 * @param[in] sem @n the specified semaphore.
 * @param[in] timeout_ms @n timeout interval in millisecond.
     If timeout_ms is PLATFORM_WAIT_INFINITE, the function will return only when the semaphore is signaled.
 * @return
   @verbatim
   =  0: The state of the specified object is signaled.
   =  -1: The time-out interval elapsed, and the object's state is nonsignaled.
   @endverbatim
 * @see None.
 * @note None.
 */
int HAL_SemaphoreWait(void *sem, uint32_t timeout_ms)
{
    os_sema_t sema = (os_sema_t)sem;

    if (NULL != sem) {
        if (PLATFORM_WAIT_INFINITE == timeout_ms) {
            return sys_sema_down((os_sema_t *)&sema, 0);
        } else {
            return sys_sema_down((os_sema_t *)&sema, timeout_ms);
        }
    }

    return -1;
}


/**
 *
 * 函数 HAL_SleepMs() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_SleepMs() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_SleepMs(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_SleepMs, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
/**
 * @brief Sleep thread itself.
 *
 * @param [in] ms @n the time interval for which execution is to be suspended, in milliseconds.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_SleepMs(uint32_t ms)
{
    sys_ms_sleep(ms);
}


/**
 *
 * 函数 HAL_Snprintf() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_Snprintf() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_Snprintf(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_Snprintf, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
/**
 * @brief Writes formatted data to string.
 *
 * @param [out] str: @n String that holds written text.
 * @param [in] len: @n Maximum length of character will be written
 * @param [in] fmt: @n Format that contains the text to be written, it can optionally contain embedded format specifiers
     that specifies how subsequent arguments are converted for output.
 * @param [in] ...: @n the variable argument list, for formatted and inserted in the resulting string replacing their respective specifiers.
 * @return bytes of character successfully written into string.
 * @see None.
 * @note None.
 */
int HAL_Snprintf(char *str, const int len, const char *fmt, ...)
{
    va_list args;
    int rc;

    va_start(args, fmt);
    rc = vsnprintf(str, len, fmt, args);
    va_end(args);

    return rc;
}


/**
 *
 * 函数 HAL_Srandom() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_Srandom() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_Srandom(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_Srandom, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
void HAL_Srandom(uint32_t seed)
{
    srand(seed);
}


/**
 *
 * 函数 HAL_Sys_Net_Is_Ready() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_Sys_Net_Is_Ready() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_Sys_Net_Is_Ready(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_Sys_Net_Is_Ready, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_AWSS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
int HAL_Sys_Net_Is_Ready(void)
{
    return wifi_netif_is_ip_got();
}


/**
 *
 * 函数 HAL_ThreadCreate() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_ThreadCreate() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_ThreadCreate(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_ThreadCreate, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_AWSS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
/**
 * @brief  create a thread
 *
 * @param[out] thread_handle @n The new thread handle, memory allocated before thread created and return it, free it after thread joined or exit.
 * @param[in] start_routine @n A pointer to the application-defined function to be executed by the thread.
        This pointer represents the starting address of the thread.
 * @param[in] arg @n A pointer to a variable to be passed to the start_routine.
 * @param[in] hal_os_thread_param @n A pointer to stack params.
 * @param[out] stack_used @n if platform used stack buffer, set stack_used to 1, otherwise set it to 0.
 * @return
   @verbatim
     = 0: on success.
     = -1: error occur.
   @endverbatim
 * @see None.
 * @note None.
 */

typedef struct {
    void *arg;
    void *(*routine)(void *arg);
} task_context_t;

typedef void *(*task_routine_t)(void *argv);

static void task_function(void *arg)
{
    task_context_t *task_ctx = (task_context_t *)arg;
    void *task_arg = task_ctx->arg;
    task_routine_t task_routine = task_ctx->routine;

    sys_mfree(task_ctx);

    task_routine(task_arg);
}

#define ALI_LINKIT_THREAD_NAME "AliThread"
#define ALI_LINKIT_THREAD_SIZE 4096
#define ALI_LINKIT_THREAD_PRI TASK_PRIO_APP_BASE

int HAL_ThreadCreate(
            void **thread_handle,
            void *(*work_routine)(void *),
            void *arg,
            hal_os_thread_param_t *hal_os_thread_param,
            int *stack_used)
{
    int ret = 0;
    char *task_name;
    size_t stack_size;
    unsigned int task_pri;
    void *task_handle;
    task_context_t *task_ctx;

    if (stack_used != NULL) {
        *stack_used = 0;
    }

    if (!hal_os_thread_param) {
        task_pri = ALI_LINKIT_THREAD_PRI;
    } else {
        task_pri = ALI_LINKIT_THREAD_PRI + TASK_PRIO_HIGHER(hal_os_thread_param->priority);
    }
    if (!hal_os_thread_param || !hal_os_thread_param->name) {
        task_name = DEFAULT_THREAD_NAME;
    } else {
        task_name = hal_os_thread_param->name;
    }
    if (!hal_os_thread_param || hal_os_thread_param->stack_size == 0) {
        stack_size = ALI_LINKIT_THREAD_SIZE;
    } else {
        stack_size = hal_os_thread_param->stack_size;
    }

    task_ctx = sys_malloc(sizeof(task_context_t));
    if (task_ctx == NULL) {
        return -1;
    }
    task_ctx->arg = arg;
    task_ctx->routine = work_routine;

    task_handle = sys_task_create(NULL, (const uint8_t *)task_name, NULL,
                    stack_size, 0, task_pri, task_function, task_ctx);
    if (task_handle == NULL) {
        sys_mfree(task_ctx);
        return -1;
    }
    *thread_handle = task_handle;

    return ret;
}


/**
 *
 * 函数 HAL_UDP_close_without_connect() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_UDP_close_without_connect() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_UDP_close_without_connect(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_UDP_close_without_connect, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_UDP_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
#define TRANSPORT_ADDR_LEN (16)
#define NETWORK_ADDR_LEN (16)



#ifndef IP_PKTINFO
#define IP_PKTINFO IP_MULTICAST_IF
#endif

int HAL_UDP_close_without_connect(intptr_t sockfd)
{
    return close((int)sockfd);
}


/**
 *
 * 函数 HAL_UDP_create_without_connect() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_UDP_create_without_connect() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_UDP_create_without_connect(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_UDP_create_without_connect, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_UDP_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
intptr_t HAL_UDP_create_without_connect(const char *host, unsigned short port)
{
    struct sockaddr_in addr;
    long sockfd;
    struct hostent *hp;
    struct in_addr in;
    uint32_t ip;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        platform_warn("UDP can't get socket\r\n");
        return -1;
    }

    if (0 == port) {
        return (intptr_t)sockfd;
    }

    memset(&addr, 0, sizeof(struct sockaddr_in));
/*
    if (0 != setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_BROADCAST, &opt_val, sizeof(opt_val))) {
        platform_warn("UDP setsockopt failed\r\n");
        close(sockfd);
        return -1;
    }
*/
    if (NULL == host) {
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else {
        if (inet_aton(host, &in)) {
            ip = *(uint32_t *)&in;
        }
        else {
            hp = gethostbyname(host);
            if (!hp) {
                platform_warn("UDP can't resolute the host address\r\n");
                close(sockfd);
                return -1;
            }
            ip = *(uint32_t *)(hp->h_addr);
        }
        addr.sin_addr.s_addr = ip;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (-1 == bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in))) {
        platform_warn("UDP can't bind the host address\r\n");
        close(sockfd);
        return -1;
    }
    platform_warn("success to establish udp, fd=%d\r\n", (int)sockfd);

    return (intptr_t)sockfd;
}


/**
 *
 * 函数 HAL_UDP_joinmulticast() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_UDP_joinmulticast() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_UDP_joinmulticast(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_UDP_joinmulticast, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_UDP_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
int HAL_UDP_joinmulticast(intptr_t sockfd,
                          char *p_group)
{
    int err = -1;
    int socket_id = -1;
    int loop = 1;
    struct ip_mreq mreq;

    if (NULL == p_group)
    {
        return -1;
    }

    socket_id = (int)sockfd;
    err =
        setsockopt(socket_id, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop));
    if (err < 0)
    {
        platform_err("setsockopt():IP_MULTICAST_LOOP failed\r\n");
        return err;
    }

    mreq.imr_multiaddr.s_addr = inet_addr(p_group);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY); /*default networt interface*/

    /*join to the mutilcast group*/
    err = setsockopt(socket_id, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
    if (err < 0)
    {
        platform_err("setsockopt():IP_ADD_MEMBERSHIP failed\r\n");
        return err;
    }

    return 0;
}


/**
 *
 * 函数 HAL_UDP_recvfrom() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_UDP_recvfrom() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_UDP_recvfrom(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_UDP_recvfrom, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_UDP_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
int HAL_UDP_recvfrom(intptr_t sockfd,
                     NetworkAddr *p_remote,
                     unsigned char *p_data,
                     unsigned int datalen,
                     unsigned int timeout_ms)
{
    int socket_id = -1;
    struct sockaddr from;
    int count = -1, ret = -1;
    socklen_t addrlen = 0;
    struct timeval tv;
    fd_set read_fds;

    if (NULL == p_remote || NULL == p_data)
    {
        return -1;
    }

    socket_id = (int)sockfd;

    FD_ZERO(&read_fds);
    FD_SET(socket_id, &read_fds);

    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    ret = select(socket_id + 1, &read_fds, NULL, NULL,
                 timeout_ms == 0 ? NULL : &tv);

    /* Zero fds ready means we timed out */
    if (ret == 0)
    {
        return -2; /* receive timeout */
    }

    if (ret < 0)
    {
        if (errno == EINTR)
        {
            return -3; /* want read */
        }

        return -4; /* receive failed */
    }

    addrlen = sizeof(struct sockaddr);
    count = recvfrom(socket_id, p_data, (size_t)datalen, 0, &from, &addrlen);
    if (-1 == count)
    {
        return -1;
    }
    if (from.sa_family == AF_INET)
    {
        struct sockaddr_in *sin = (struct sockaddr_in *)&from;
        inet_ntop(AF_INET, (const ip_addr_t *)&sin->sin_addr, (char *)p_remote->addr,
                  NETWORK_ADDR_LEN);
        p_remote->port = ntohs(sin->sin_port);
    }
    return count;
}


/**
 *
 * 函数 HAL_UDP_sendto() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_UDP_sendto() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_UDP_sendto(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_UDP_sendto, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_UDP_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
int HAL_UDP_sendto(intptr_t sockfd,
                   const NetworkAddr *p_remote,
                   const unsigned char *p_data,
                   unsigned int datalen,
                   unsigned int timeout_ms)
{
    int rc = -1;
    int socket_id = -1;
    struct sockaddr_in remote_addr;

    if (NULL == p_remote || NULL == p_data)
    {
        return -1;
    }

    socket_id = (int)sockfd;
    remote_addr.sin_family = AF_INET;
    if (1 !=
        (rc = inet_pton(remote_addr.sin_family, (const char *)p_remote->addr,
                        &remote_addr.sin_addr.s_addr)))
    {
        return -1;
    }
    remote_addr.sin_port = htons(p_remote->port);
    rc = sendto(socket_id, p_data, (size_t)datalen, 0,
                (const struct sockaddr *)&remote_addr, sizeof(remote_addr));
    if (-1 == rc)
    {
        return -1;
    }
    return rc;
}


/**
 *
 * 函数 HAL_UptimeMs() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_UptimeMs() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_UptimeMs(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_UptimeMs, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
/**
 * @brief Retrieves the number of milliseconds that have elapsed since the system was boot.
 *
 * @return the number of milliseconds.
 * @see None.
 * @note None.
 */
uint64_t HAL_UptimeMs(void)
{
    return sys_current_time_get();
}


/**
 *
 * 函数 HAL_Vsnprintf() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_Vsnprintf() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_Vsnprintf(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_Vsnprintf, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
int HAL_Vsnprintf(char *str, const int len, const char *format, va_list ap)
{
    return vsnprintf(str, len, format, ap);
}


/**
 *
 * 函数 HAL_Wifi_Enable_Mgmt_Frame_Filter() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_Wifi_Enable_Mgmt_Frame_Filter() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_Wifi_Enable_Mgmt_Frame_Filter(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_Wifi_Enable_Mgmt_Frame_Filter, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_AWSS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
int HAL_Wifi_Enable_Mgmt_Frame_Filter(
            _IN_ uint32_t filter_mask,
            _IN_OPT_ uint8_t vendor_oui[3],
            _IN_ awss_wifi_mgmt_frame_cb_t callback)
{
    platform_warn("=> HAL_Wifi_Enable_Mgmt_Frame_Filter\r\n");

    wifi_netlink_promisc_mgmt_cb_set(filter_mask, callback);
    return 0;
}


/**
 *
 * 函数 HAL_Wifi_Get_Ap_Info() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_Wifi_Get_Ap_Info() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_Wifi_Get_Ap_Info(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_Wifi_Get_Ap_Info, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_AWSS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
int HAL_Wifi_Get_Ap_Info(
            _OU_ char ssid[HAL_MAX_SSID_LEN],
            _OU_ char passwd[HAL_MAX_PASSWD_LEN],
            _OU_ uint8_t bssid[ETH_ALEN])
{
    wifi_netlink_linked_ap_info_get((uint8_t  *)ssid, (uint8_t  *)passwd, bssid);

    return 0;
}


/**
 *
 * 函数 HAL_Wifi_Get_IP() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_Wifi_Get_IP() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_Wifi_Get_IP(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_Wifi_Get_IP, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_UDP_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
#define HAL_IP_LEN (15 + 1)
uint32_t HAL_Wifi_Get_IP(char ip_str[NETWORK_ADDR_LEN], const char *ifname)
{
    uint8_t *ip;

    ip = (uint8_t *)wifi_netif_get_ip();
    snprintf(ip_str, HAL_IP_LEN, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

    return *(uint32_t *)ip;
}


/**
 *
 * 函数 HAL_Wifi_Get_Mac() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_Wifi_Get_Mac() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_Wifi_Get_Mac(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_Wifi_Get_Mac, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_AWSS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
char *HAL_Wifi_Get_Mac(_OU_ char mac_str[HAL_MAC_LEN])
{
    uint8_t mac[6] = {0};

    sys_memcpy(mac, wifi_netif_get_hwaddr(), 6);

    snprintf(mac_str, HAL_MAC_LEN, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0],
            mac[1], mac[2], mac[3], mac[4], mac[5]);

    return mac_str;
}


/**
 *
 * 函数 HAL_Wifi_Send_80211_Raw_Frame() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
 * ---
 * Interface of HAL_Wifi_Send_80211_Raw_Frame() requires to be implemented by user of SDK, according to target device platform
 *
 * 如果需要参考如何实现函数 HAL_Wifi_Send_80211_Raw_Frame(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
 * ---
 * If you need guidance about how to implement HAL_Wifi_Send_80211_Raw_Frame, you can check its reference implementation for Ubuntu platform
 *
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_AWSS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
int HAL_Wifi_Send_80211_Raw_Frame(_IN_ enum HAL_Awss_Frame_Type type,
                                  _IN_ uint8_t *buffer, _IN_ int len)
{
    wifi_netlink_raw_send(buffer, len - 4);

    return 0;
}

/* @brief   打开当前设备热点，并把设备由SoftAP模式切换到AP模式
 */

int HAL_Awss_Open_Ap(const char *ssid, const char *passwd, int beacon_interval, int hide)
{
    char buffer[256] = {0};
    int ret = -1;
    uint8_t channel = 1;
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

#ifdef CONFIG_WIFI_MANAGEMENT_TASK
    wifi_management_ap_start((char *)ssid, NULL, channel, 0);
#else
    wifi_netif_stop_dhcp();

    wifi_netlink_ap_start(ssid, NULL, channel, 0);

    CONFIG_IP4_ADDR(&ipaddr);
    CONFIG_IP4_ADDR_NM(&netmask);
    CONFIG_IP4_ADDR_GW(&gw);

    wifi_netif_set_addr(&ipaddr, &netmask, &gw);

    if (!ap_dhcpd_started) {
        dhcpd_daemon();
        ap_dhcpd_started = 1;
    }

    wifi_netif_set_up();
#endif

    return 0;
}

/* @brief   关闭当前设备热点，并把设备由SoftAP模式切换到Station模式
*/
int HAL_Awss_Close_Ap()
{
#ifdef CONFIG_WIFI_MANAGEMENT_TASK
    wifi_management_sta_start();
#else
    if (p_wifi_netlink->ap_started){
        /* Stop DHCPD */
        if (ap_dhcpd_started) {
            stop_dhcpd_daemon();
            ap_dhcpd_started = 0;
        }
        wifi_netlink_dev_close();
        wifi_netlink_dev_open();
    }
#endif
    return 0;
}

#endif
