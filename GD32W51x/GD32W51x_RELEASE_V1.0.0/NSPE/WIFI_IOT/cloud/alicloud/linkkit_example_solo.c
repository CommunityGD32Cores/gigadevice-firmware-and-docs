/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "app_cfg.h"

#ifdef CONFIG_ALICLOUD_SUPPORT
#include "infra_config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef __UBUNTU_SDK_DEMO__
    #include <unistd.h>
#endif

#include "infra_types.h"
#include "infra_defs.h"
#include "infra_compat.h"
#ifdef INFRA_MEM_STATS
    #include "infra_mem_stats.h"
#endif
#include "dev_model_api.h"
#include "wrappers.h"
#include "cJSON.h"
#ifdef ATM_ENABLED
    #include "at_api.h"
#endif

#ifdef AWSS_FRAMEWORKS
#include "wifi_provision_api.h"
#endif
#include "bsp_inc.h"
#include "dev_ap\awss_dev_ap.h"

#define USING_AWSS               1
#define USING_DEVAP              2

#define TEST_ALICLOUD            1
#define TEST_WIFI_CONFIG         2
#define TEST_CONTENT             TEST_ALICLOUD

/******************** WIFI_LED ***************************/
/* 已失效
char g_product_key[IOTX_PRODUCT_KEY_LEN + 1]       = "a1AF8a5kQDQ";
char g_product_secret[IOTX_PRODUCT_SECRET_LEN + 1] = "emsmIytLfRyNuzVl";
char g_device_name[IOTX_DEVICE_NAME_LEN + 1]       = "76:ba:ed:78:12:26";
char g_device_secret[IOTX_DEVICE_SECRET_LEN + 1]   = "rGgSciuO7KEnKhUI2E4IwncfnM3K75IM";
*/

/******************** WIFI 智能灯 ***************************/
char g_product_key[IOTX_PRODUCT_KEY_LEN + 1]       = "a1aHqC1PxcN";
char g_product_secret[IOTX_PRODUCT_SECRET_LEN + 1] = "vWtK3Z1sXSaTwTUc";
char g_device_name[IOTX_DEVICE_NAME_LEN + 1]       = "76:ba:ed:78:12:22";
char g_device_secret[IOTX_DEVICE_SECRET_LEN + 1]   = "oYecvxia5dFhSYg2ty77aXt2kHLkxwts";

/*
char g_product_key[IOTX_PRODUCT_KEY_LEN + 1]       = "a1aHqC1PxcN";
char g_product_secret[IOTX_PRODUCT_SECRET_LEN + 1] = "vWtK3Z1sXSaTwTUc";
char g_device_name[IOTX_DEVICE_NAME_LEN + 1]       = "76:ba:ed:78:12:24";
char g_device_secret[IOTX_DEVICE_SECRET_LEN + 1]   = "q2HXaEXi3tfYrZHXRi2iyg796Z082M2u";
*/
/*
char g_product_key[IOTX_PRODUCT_KEY_LEN + 1]       = "a1aHqC1PxcN";
char g_product_secret[IOTX_PRODUCT_SECRET_LEN + 1] = "vWtK3Z1sXSaTwTUc";
char g_device_name[IOTX_DEVICE_NAME_LEN + 1]       = "76:ba:ed:78:12:28";
char g_device_secret[IOTX_DEVICE_SECRET_LEN + 1]   = "6c65d8f13d96b19141349273a76e5771";
*/

#define EXAMPLE_TRACE(...)                                          \
    do {                                                            \
        HAL_Printf("\033[1;32;40m%s.%d: ", __func__, __LINE__);     \
        HAL_Printf(__VA_ARGS__);                                    \
        HAL_Printf("\033[0m\r\n");                                  \
    } while (0)

void run_ubuntu_wifi_provision_example()
{
#ifdef __UBUNTU_SDK_DEMO__
#if defined(WIFI_PROVISION_ENABLED)
    char *wifi_name = "linkkit";
    char buffer[128] = {0};
    int ret;
    extern int awss_config_press();
    extern int awss_start();
    memset(buffer, 0, 128);
    snprintf(buffer, 128, "nmcli connection down %s", wifi_name);
    ret = system(buffer);

    memset(buffer, 0, 128);
    snprintf(buffer, 128, "nmcli connection delete %s", wifi_name);
    ret = system(buffer);
    sleep(15);

    awss_config_press();
    awss_start();
#endif
#endif
}

#define EXAMPLE_MASTER_DEVID            (0)
#define EXAMPLE_YIELD_TIMEOUT_MS        (200)

typedef struct {
    int master_devid;
    int cloud_connected;
    int master_initialized;
} user_example_ctx_t;

static user_example_ctx_t g_user_example_ctx;

static void smart_led_init(int value)
{
    /* enable the LED GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    /* configure LED GPIO pin */
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_0);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_166MHZ, GPIO_PIN_0);

    /* turn on/off LED */
    if (value)
        gpio_bit_set(GPIOB, GPIO_PIN_0);
    else
        gpio_bit_reset(GPIOB, GPIO_PIN_0);

}
static int smart_led_set(int devid, const char *request, int request_len)
{
    char *find;
    int value;
    printf("%s(): devid = %d request = [%s]\r\n", __func__, devid, request);
    find = strstr(request, "LightSwitch");
    if (NULL == find)
        return -1;

    find += strlen("LightSwitch");
    find = strchr(find, ':');
    if (NULL == find)
        return -2;

    find += 1;
    value = atoi(find);

    printf("%s(): value = %d\r\n", __func__, value);

    if (value)
        gpio_bit_set(GPIOB, GPIO_PIN_0);
    else
        gpio_bit_reset(GPIOB, GPIO_PIN_0);

    return 0;
}

/** cloud connected event callback */
static int user_connected_event_handler(void)
{
    EXAMPLE_TRACE("Cloud Connected");
    g_user_example_ctx.cloud_connected = 1;

    return 0;
}

/** cloud disconnected event callback */
static int user_disconnected_event_handler(void)
{
    EXAMPLE_TRACE("Cloud Disconnected");
    g_user_example_ctx.cloud_connected = 0;

    return 0;
}

/* device initialized event callback */
static int user_initialized(const int devid)
{
    EXAMPLE_TRACE("Device Initialized");
    g_user_example_ctx.master_initialized = 1;

    smart_led_init(0);
    return 0;
}

/** recv property post response message from cloud **/
static int user_report_reply_event_handler(const int devid, const int msgid, const int code, const char *reply,
        const int reply_len)
{
    EXAMPLE_TRACE("Message Post Reply Received, Message ID: %d, Code: %d, Reply: %.*s", msgid, code,
                  reply_len,
                  (reply == NULL) ? ("NULL") : (reply));
    return 0;
}

/** recv event post response message from cloud **/
static int user_trigger_event_reply_event_handler(const int devid, const int msgid, const int code, const char *eventid,
        const int eventid_len, const char *message, const int message_len)
{
    EXAMPLE_TRACE("Trigger Event Reply Received, Message ID: %d, Code: %d, EventID: %.*s, Message: %.*s",
                  msgid, code,
                  eventid_len,
                  eventid, message_len, message);

    return 0;
}

/** recv property setting message from cloud **/
static int user_property_set_event_handler(const int devid, const char *request, const int request_len)
{
    int res = 0;
    EXAMPLE_TRACE("Property Set Received, Request: %s", request);

    smart_led_set(devid, request, request_len);
    res = IOT_Linkkit_Report(EXAMPLE_MASTER_DEVID, ITM_MSG_POST_PROPERTY,
                             (unsigned char *)request, request_len);
    EXAMPLE_TRACE("Post Property Message ID: %d", res);

    return 0;
}


static int user_service_request_event_handler(const int devid, const char *serviceid, const int serviceid_len,
        const char *request, const int request_len,
        char **response, int *response_len)
{
    int add_result = 0;
    cJSON *root = NULL, *item_number_a = NULL, *item_number_b = NULL;
    const char *response_fmt = "{\"Result\": %d}";

    EXAMPLE_TRACE("Service Request Received, Service ID: %.*s, Payload: %s", serviceid_len, serviceid, request);

    /* Parse Root */
    root = cJSON_Parse(request);
    if (root == NULL || !cJSON_IsObject(root)) {
        EXAMPLE_TRACE("JSON Parse Error");
        return -1;
    }

    if (strlen("Operation_Service") == serviceid_len && memcmp("Operation_Service", serviceid, serviceid_len) == 0) {
        /* Parse NumberA */
        item_number_a = cJSON_GetObjectItem(root, "NumberA");
        if (item_number_a == NULL || !cJSON_IsNumber(item_number_a)) {
            cJSON_Delete(root);
            return -1;
        }
        EXAMPLE_TRACE("NumberA = %d", item_number_a->valueint);

        /* Parse NumberB */
        item_number_b = cJSON_GetObjectItem(root, "NumberB");
        if (item_number_b == NULL || !cJSON_IsNumber(item_number_b)) {
            cJSON_Delete(root);
            return -1;
        }
        EXAMPLE_TRACE("NumberB = %d", item_number_b->valueint);

        add_result = item_number_a->valueint + item_number_b->valueint;

        /* Send Service Response To Cloud */
        *response_len = strlen(response_fmt) + 10 + 1;
        *response = (char *)HAL_Malloc(*response_len);
        if (*response == NULL) {
            EXAMPLE_TRACE("Memory Not Enough");
            return -1;
        }
        memset(*response, 0, *response_len);
        HAL_Snprintf(*response, *response_len, response_fmt, add_result);
        *response_len = strlen(*response);
    }

    cJSON_Delete(root);
    return 0;
}

static int user_timestamp_reply_event_handler(const char *timestamp)
{
    EXAMPLE_TRACE("Current Timestamp: %s", timestamp);

    return 0;
}

/** fota event handler **/
static int user_fota_event_handler(int type, const char *version)
{
    char buffer[1024] = {0};
    int buffer_length = 1024;

    /* 0 - new firmware exist, query the new firmware */
    if (type == 0) {
        EXAMPLE_TRACE("New Firmware Version: %s", version);

        IOT_Linkkit_Query(EXAMPLE_MASTER_DEVID, ITM_MSG_QUERY_FOTA_DATA, (unsigned char *)buffer, buffer_length);
    }

    return 0;
}

static int user_fota_module_event_handler(int type, const char *version, const char *module)
{
    char buffer[1024] = {0};
    int buffer_length = 1024;

    /* 0 - new firmware exist, query the new firmware */
    if (type == 0) {
        EXAMPLE_TRACE("New Firmware Version: %s, module: %s", version, module);

        IOT_Linkkit_Query(EXAMPLE_MASTER_DEVID, ITM_MSG_QUERY_FOTA_DATA, (unsigned char *)buffer, buffer_length);
    }

    return 0;
}

/* cota event handler */
static int user_cota_event_handler(int type, const char *config_id, int config_size, const char *get_type,
                                   const char *sign, const char *sign_method, const char *url)
{
    char buffer[128] = {0};
    int buffer_length = 128;

    /* type = 0, new config exist, query the new config */
    if (type == 0) {
        EXAMPLE_TRACE("New Config ID: %s", config_id);
        EXAMPLE_TRACE("New Config Size: %d", config_size);
        EXAMPLE_TRACE("New Config Type: %s", get_type);
        EXAMPLE_TRACE("New Config Sign: %s", sign);
        EXAMPLE_TRACE("New Config Sign Method: %s", sign_method);
        EXAMPLE_TRACE("New Config URL: %s", url);

        IOT_Linkkit_Query(EXAMPLE_MASTER_DEVID, ITM_MSG_QUERY_COTA_DATA, (unsigned char *)buffer, buffer_length);
    }

    return 0;
}

#ifdef DEV_BIND_ENABLED
static int user_dev_bind_handler(const char *detail)
{
    EXAMPLE_TRACE("get bind event:%s", detail);
    return 0;
}
#endif
void user_post_property(void)
{
    static int cnt = 0;
    int res = 0;

    char property_payload[30] = {0};
    HAL_Snprintf(property_payload, sizeof(property_payload), "{\"Counter\": %d}", cnt++);

    res = IOT_Linkkit_Report(EXAMPLE_MASTER_DEVID, ITM_MSG_POST_PROPERTY,
                             (unsigned char *)property_payload, strlen(property_payload));

    EXAMPLE_TRACE("Post Property Message ID: %d", res);
}

void user_post_event(void)
{
    int res = 0;
    char *event_id = "HardwareError";
    char *event_payload = "{\"ErrorCode\": 0}";

    res = IOT_Linkkit_TriggerEvent(EXAMPLE_MASTER_DEVID, event_id, strlen(event_id),
                                   event_payload, strlen(event_payload));
    EXAMPLE_TRACE("Post Event Message ID: %d", res);
}

void user_deviceinfo_update(void)
{
    int res = 0;
    char *device_info_update = "[{\"attrKey\":\"abc\",\"attrValue\":\"hello,world\"}]";

    res = IOT_Linkkit_Report(EXAMPLE_MASTER_DEVID, ITM_MSG_DEVICEINFO_UPDATE,
                             (unsigned char *)device_info_update, strlen(device_info_update));
    EXAMPLE_TRACE("Device Info Update Message ID: %d", res);
}

void user_deviceinfo_delete(void)
{
    int res = 0;
    char *device_info_delete = "[{\"attrKey\":\"abc\"}]";

    res = IOT_Linkkit_Report(EXAMPLE_MASTER_DEVID, ITM_MSG_DEVICEINFO_DELETE,
                             (unsigned char *)device_info_delete, strlen(device_info_delete));
    EXAMPLE_TRACE("Device Info Delete Message ID: %d", res);
}

static int user_cloud_error_handler(const int code, const char *data, const char *detail)
{
    EXAMPLE_TRACE("code =%d ,data=%s, detail=%s", code, data, detail);
    return 0;
}

static int dynreg_device_secret(const char *device_secret)
{
    EXAMPLE_TRACE("device secret: %s", device_secret);
    return 0;
}

static int user_sdk_state_dump(int ev, const char *msg)
{
    printf("received state: -0x%04X(%s)\n", -ev, msg);
    return 0;
}

int linkkit_main(int argc, char **argv)
{
    int res = 0;
    int cnt = 0;
    int auto_quit = 0;
    iotx_linkkit_dev_meta_info_t master_meta_info;
    int domain_type = 0, dynamic_register = 0, post_reply_need = 0, fota_timeout = 30;
    int wifi_config = USING_AWSS;

    if (argc == 2) {
        wifi_config = atoi(argv[1]);
        if (wifi_config > USING_AWSS)
            wifi_config = USING_DEVAP;
    }
#ifdef ATM_ENABLED
    if (IOT_ATM_Init() < 0) {
        EXAMPLE_TRACE("IOT ATM init failed!\n");
        return -1;
    }
#endif


    if (argc >= 2 && !strcmp("auto_quit", argv[1])) {
        auto_quit = 1;
        cnt = 0;
    }
    memset(&g_user_example_ctx, 0, sizeof(user_example_ctx_t));

    memset(&master_meta_info, 0, sizeof(iotx_linkkit_dev_meta_info_t));
    memcpy(master_meta_info.product_key, g_product_key, strlen(g_product_key));
    memcpy(master_meta_info.product_secret, g_product_secret, strlen(g_product_secret));
    memcpy(master_meta_info.device_name, g_device_name, strlen(g_device_name));
    memcpy(master_meta_info.device_secret, g_device_secret, strlen(g_device_secret));

    IOT_SetLogLevel(IOT_LOG_DEBUG);

    /* Register Callback */
    IOT_RegisterCallback(ITE_STATE_EVERYTHING, user_sdk_state_dump);
    IOT_RegisterCallback(ITE_CONNECT_SUCC, user_connected_event_handler);
    IOT_RegisterCallback(ITE_DISCONNECTED, user_disconnected_event_handler);
    IOT_RegisterCallback(ITE_SERVICE_REQUEST, user_service_request_event_handler);
    IOT_RegisterCallback(ITE_PROPERTY_SET, user_property_set_event_handler);
    IOT_RegisterCallback(ITE_REPORT_REPLY, user_report_reply_event_handler);
    IOT_RegisterCallback(ITE_TRIGGER_EVENT_REPLY, user_trigger_event_reply_event_handler);
    IOT_RegisterCallback(ITE_TIMESTAMP_REPLY, user_timestamp_reply_event_handler);
    IOT_RegisterCallback(ITE_INITIALIZE_COMPLETED, user_initialized);
    IOT_RegisterCallback(ITE_FOTA, user_fota_event_handler);
    IOT_RegisterCallback(ITE_FOTA_MODULE, user_fota_module_event_handler);
    IOT_RegisterCallback(ITE_COTA, user_cota_event_handler);
    IOT_RegisterCallback(ITE_CLOUD_ERROR, user_cloud_error_handler);
    IOT_RegisterCallback(ITE_DYNREG_DEVICE_SECRET, dynreg_device_secret);
#ifdef DEV_BIND_ENABLED
    IOT_RegisterCallback(ITE_BIND_EVENT, user_dev_bind_handler);
#endif
    domain_type = IOTX_CLOUD_REGION_SHANGHAI;
    IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);

    /* Choose Login Method */
    dynamic_register = 0;
    IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);

    /* post reply doesn't need */
    post_reply_need = 1;
    IOT_Ioctl(IOTX_IOCTL_RECV_EVENT_REPLY, (void *)&post_reply_need);

    IOT_Ioctl(IOTX_IOCTL_FOTA_TIMEOUT_MS, (void *)&fota_timeout);

    do {
        g_user_example_ctx.master_devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, &master_meta_info);
        if (g_user_example_ctx.master_devid >= 0) {
            break;
        }
        EXAMPLE_TRACE("IOT_Linkkit_Open failed! retry after %d ms\n", 2000);
        HAL_SleepMs(2000);
    } while (1);

#if (TEST_CONTENT == TEST_WIFI_CONFIG)
    if (wifi_config == USING_AWSS) {
        #ifdef CONFIG_WIFI_MANAGEMENT_TASK
            wifi_management_disconnect();
        #else
            wifi_netlink_disconnect_req();
            wifi_netif_stop_dhcp();
        #endif
        __awss_start();
    } else if () {
        awss_dev_ap_stop();
        awss_dev_ap_start();
    }
    goto exit;
#elif (TEST_CONTENT == TEST_ALICLOUD)
    if (!HAL_Sys_Net_Is_Ready()) {
        /* start wifi provision, until connect AP successful */
        if (wifi_config == USING_AWSS) {
            awss_start();
        } else if (wifi_config == USING_DEVAP)  {
            awss_dev_ap_stop();
            awss_dev_ap_start();
        }
    }
#endif
    /* run_ubuntu_wifi_provision_example(); */
    do {
        res = IOT_Linkkit_Connect(g_user_example_ctx.master_devid);
        if (res >= 0) {
            break;
        }
        EXAMPLE_TRACE("IOT_Linkkit_Connect failed! retry after %d ms\n", 5000);
        HAL_SleepMs(5000);
    } while (1);

    while (1) {
        IOT_Linkkit_Yield(EXAMPLE_YIELD_TIMEOUT_MS);

        /* Post Proprety Example */
        if ((cnt % 2) == 0) {
            /* user_post_property(); */
        }

        /* Post Event Example */
        if ((cnt % 10) == 0) {
            /* user_post_event(); */
        }
        cnt++;

        if (auto_quit == 1 && cnt > 3600) {
            break;
        }
    }

#if (TEST_CONTENT == TEST_WIFI_CONFIG)
exit:
#endif
    IOT_Linkkit_Close(g_user_example_ctx.master_devid);

    IOT_DumpMemoryStats(IOT_LOG_DEBUG);
    IOT_SetLogLevel(IOT_LOG_NONE);

    return 0;
}

#endif
