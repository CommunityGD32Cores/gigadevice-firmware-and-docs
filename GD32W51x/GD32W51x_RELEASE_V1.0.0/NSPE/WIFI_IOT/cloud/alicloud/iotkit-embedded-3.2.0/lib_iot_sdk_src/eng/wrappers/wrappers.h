#ifndef _WRAPPERS_H_
#define _WRAPPERS_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdarg.h>
#include "infra_types.h"
#include "infra_defs.h"
#include "wrappers_defs.h"
#include "infra_compat.h"
#include "stdarg.h"

#define HAL_Printf printf
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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_AWSS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
void HAL_Awss_Close_Monitor(void);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_AWSS_linux.c
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
            _IN_OPT_ uint8_t channel);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_AWSS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
void HAL_Awss_Open_Monitor(_IN_ awss_recv_80211_frame_cb_t cb);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_AWSS_linux.c
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
            _IN_OPT_ uint8_t bssid[ETH_ALEN]);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_OS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
void HAL_Firmware_Persistence_Start(void);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_OS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
int HAL_Firmware_Persistence_Stop(void);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_OS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
int HAL_Firmware_Persistence_Write(char *buffer, uint32_t length);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_OS_linux.c
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
void HAL_Free(void *ptr);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_OS_linux.c
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
int HAL_GetFirmwareVersion(char *version);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_KV_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
int HAL_Kv_Get(const char *key, void *val, int *buffer_len);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_KV_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
int HAL_Kv_Set(const char *key, const void *val, int len, int sync);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_OS_linux.c
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
void *HAL_Malloc(uint32_t size);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_OS_linux.c
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
void *HAL_MutexCreate(void);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_OS_linux.c
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
void HAL_MutexDestroy(void *mutex);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_OS_linux.c
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
void HAL_MutexLock(void *mutex);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_OS_linux.c
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
void HAL_MutexUnlock(void *mutex);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_OS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
uint32_t HAL_Random(uint32_t region);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
int32_t HAL_SSL_Destroy(uintptr_t handle);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/
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
                            uint32_t ca_crt_len);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
int HAL_SSL_Read(uintptr_t handle, char *buf, int len, int timeout_ms);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
int HAL_SSL_Write(uintptr_t handle, const char *buf, int len, int timeout_ms);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_OS_linux.c
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
void *HAL_SemaphoreCreate(void);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_OS_linux.c
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
void HAL_SemaphoreDestroy(void *sem);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_OS_linux.c
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
void HAL_SemaphorePost(void *sem);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_OS_linux.c
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
int HAL_SemaphoreWait(void *sem, uint32_t timeout_ms);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_OS_linux.c
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
void HAL_SleepMs(uint32_t ms);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_OS_linux.c
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
int HAL_Snprintf(char *str, const int len, const char *fmt, ...);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_OS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
void HAL_Srandom(uint32_t seed);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_AWSS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
int HAL_Sys_Net_Is_Ready(void);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_AWSS_linux.c
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
int HAL_ThreadCreate(
            void **thread_handle,
            void *(*work_routine)(void *),
            void *arg,
            hal_os_thread_param_t *hal_os_thread_param,
            int *stack_used);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_UDP_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
int HAL_UDP_close_without_connect(intptr_t sockfd);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_UDP_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
intptr_t HAL_UDP_create_without_connect(const char *host, unsigned short port);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_UDP_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
int HAL_UDP_joinmulticast(intptr_t sockfd,
                          char *p_group);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_UDP_linux.c
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
                     unsigned int timeout_ms);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_UDP_linux.c
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
                   unsigned int timeout_ms);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_OS_linux.c
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
uint64_t HAL_UptimeMs(void);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_OS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
int HAL_Vsnprintf(char *str, const int len, const char *format, va_list ap);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_AWSS_linux.c
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
            _IN_ awss_wifi_mgmt_frame_cb_t callback);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_AWSS_linux.c
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
            _OU_ uint8_t bssid[ETH_ALEN]);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_UDP_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
uint32_t HAL_Wifi_Get_IP(char ip_str[NETWORK_ADDR_LEN], const char *ifname);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_AWSS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
char *HAL_Wifi_Get_Mac(_OU_ char mac_str[HAL_MAC_LEN]);


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
 * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/HAL_AWSS_linux.c
 *
 *
 * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
 * 
 * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
 * 
 */
int HAL_Wifi_Send_80211_Raw_Frame(_IN_ enum HAL_Awss_Frame_Type type,
                                  _IN_ uint8_t *buffer, _IN_ int len);


/* @brief   打开当前设备热点，并把设备由SoftAP模式切换到AP模式
 */
int HAL_Awss_Open_Ap(const char *ssid, const char *passwd, int beacon_interval, int hide);
/* @brief   关闭当前设备热点，并把设备由SoftAP模式切换到Station模式
*/
int HAL_Awss_Close_Ap();

#if defined(__cplusplus)
}
#endif

#endif
