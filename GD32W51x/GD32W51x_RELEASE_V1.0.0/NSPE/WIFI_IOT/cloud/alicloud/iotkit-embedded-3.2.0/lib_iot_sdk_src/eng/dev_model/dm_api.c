/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "iotx_dm_internal.h"

static dm_api_ctx_t g_dm_api_ctx;

static dm_api_ctx_t *_dm_api_get_ctx(void)
{
    return &g_dm_api_ctx;
}

static void _dm_api_lock(void)
{
    dm_api_ctx_t *ctx = _dm_api_get_ctx();
    if (ctx->mutex) {
        HAL_MutexLock(ctx->mutex);
    }
}

static void _dm_api_unlock(void)
{
    dm_api_ctx_t *ctx = _dm_api_get_ctx();
    if (ctx->mutex) {
        HAL_MutexUnlock(ctx->mutex);
    }
}

int iotx_dm_open(void)
{
    int res = 0;
    dm_api_ctx_t *ctx = _dm_api_get_ctx();
#if defined(ALCS_ENABLED) || defined(DEPRECATED_LINKKIT)
    lite_cjson_hooks hooks;
#endif
    memset(ctx, 0, sizeof(dm_api_ctx_t));

#if defined(ALCS_ENABLED) || defined(DEPRECATED_LINKKIT)
    /* lite-cjson Hooks Init */
    hooks.malloc_fn = dm_utils_malloc;
    hooks.free_fn = dm_utils_free;
    lite_cjson_init_hooks(&hooks);
#endif

    /* DM Mutex Create*/
    ctx->mutex = HAL_MutexCreate();
    if (ctx->mutex == NULL) {
        return STATE_SYS_DEPEND_MUTEX_CREATE;
    }

#if defined(OTA_ENABLED) && !defined(BUILD_AOS)
    /* DM OTA Module Init */
    res = dm_ota_init();
    if (res != SUCCESS_RETURN) {
        goto ERROR;
    }
#endif

#if !defined(DM_MESSAGE_CACHE_DISABLED)
    /* DM Message Cache Init */
    res = dm_msg_cache_init();
    if (res != SUCCESS_RETURN) {
        goto ERROR;
    }
#endif
    /* DM Cloud Message Parse And Assemble Module Init */
    res = dm_msg_init();
    if (res != SUCCESS_RETURN) {
        goto ERROR;
    }

    /* DM IPC Module Init */
    res = dm_ipc_init(CONFIG_DISPATCH_QUEUE_MAXLEN);
    if (res != SUCCESS_RETURN) {
        goto ERROR;
    }

    /* DM Manager Module Init */
    res = dm_mgr_init();
    if (res != SUCCESS_RETURN) {
        goto ERROR;
    }

#ifdef ALCS_ENABLED
    /* Open Local Connection */
    res = dm_server_open();
    if (res < SUCCESS_RETURN) {
        goto ERROR;
    }
#endif

    /* Open Cloud Connection */
    res = dm_client_open();
    if (res < SUCCESS_RETURN) {
        goto ERROR;
    }

    return SUCCESS_RETURN;

ERROR:
    dm_client_close();
#ifdef ALCS_ENABLED
    dm_server_close();
#endif
    dm_mgr_deinit();
    dm_ipc_deinit();
    dm_msg_deinit();
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    dm_msg_cache_deinit();
#endif
#if defined(OTA_ENABLED) && !defined(BUILD_AOS)
    dm_ota_deinit();
#endif

    if (ctx->mutex) {
        HAL_MutexDestroy(ctx->mutex);
    }
    return res;
}

int iotx_dm_connect(_IN_ iotx_dm_init_params_t *init_params)
{
    int res = 0;
    dm_api_ctx_t *ctx = _dm_api_get_ctx();

    /* DM Event Callback */
    if (init_params->event_callback != NULL) {
        ctx->event_callback = init_params->event_callback;
    }

    res = dm_client_connect(IOTX_DM_CLIENT_CONNECT_TIMEOUT_MS);
    if (res != SUCCESS_RETURN) {
        return res;
    }

#if defined(OTA_ENABLED) && !defined(BUILD_AOS)
    /* DM OTA Module Init */
    res = dm_ota_sub();
    if (res == SUCCESS_RETURN) {
        /* DM Config OTA Module Init */
        dm_cota_init();

        /* DM Firmware OTA Mudule Init */
        dm_fota_init();
    }
#endif

#ifdef ALCS_ENABLED
    /* DM Connect Local */
    res = dm_server_connect();
    if (res != SUCCESS_RETURN) {
        return res;
    }
#endif

    return SUCCESS_RETURN;
}

int iotx_dm_subscribe(_IN_ int devid)
{
    int res = 0, dev_type = 0;
    char product_key[IOTX_PRODUCT_KEY_LEN + 1] = {0};
    char device_name[IOTX_DEVICE_NAME_LEN + 1] = {0};
    char device_secret[IOTX_DEVICE_SECRET_LEN + 1] = {0};

    if (devid < 0) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();
    res = dm_mgr_search_device_by_devid(devid, product_key, device_name, device_secret);
    if (res < SUCCESS_RETURN) {
        _dm_api_unlock();
        return res;
    }

    res = dm_mgr_get_dev_type(devid, &dev_type);
    if (res < SUCCESS_RETURN) {
        _dm_api_unlock();
        return res;
    }

#ifdef ALCS_ENABLED
    if (devid > 0) {
        dm_server_add_device(product_key, device_name);
    }

    res = dm_server_subscribe_all(product_key, device_name);
    if (res < SUCCESS_RETURN) {
        _dm_api_unlock();
        return res;
    }
#endif

    res = dm_client_subscribe_all(devid, product_key, device_name, dev_type);
    if (res < SUCCESS_RETURN) {
        _dm_api_unlock();
        return res;
    }
    _dm_api_unlock();

    return SUCCESS_RETURN;
}

int iotx_dm_close(void)
{
    dm_api_ctx_t *ctx = _dm_api_get_ctx();

    dm_client_close();
#ifdef ALCS_ENABLED
    dm_server_close();
#endif
    dm_mgr_deinit();
    dm_ipc_deinit();
    dm_msg_deinit();
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    dm_msg_cache_deinit();
#endif
#if defined(OTA_ENABLED) && !defined(BUILD_AOS)
    dm_cota_deinit();
    dm_fota_deinit();
    dm_ota_deinit();
#endif

    if (ctx->mutex) {
        HAL_MutexDestroy(ctx->mutex);
    }

#if defined(LOG_REPORT_TO_CLOUD) && !defined(DEVICE_MODEL_RAWDATA_SOLO)
    remove_log_poll();
#endif

    return SUCCESS_RETURN;
}

int iotx_dm_yield(int timeout_ms)
{
    if (timeout_ms <= 0) {
        return STATE_USER_INPUT_INVALID;
    }

    dm_client_yield(timeout_ms);
#ifdef ALCS_ENABLED
    dm_server_yield();
#endif

    return STATE_SUCCESS;
}

void iotx_dm_dispatch(void)
{
    int count = 0;
    void *data = NULL;
    dm_api_ctx_t *ctx = _dm_api_get_ctx();

#if !defined(DM_MESSAGE_CACHE_DISABLED)
    dm_msg_cache_tick();
#endif

#if !defined(DEVICE_MODEL_RAWDATA_SOLO) && !defined(DEPRECATED_LINKKIT)
    iotx_linkkit_service_list_overtime_handle();
#endif

#if defined(OTA_ENABLED) && !defined(BUILD_AOS)
    dm_cota_status_check();
    dm_fota_status_check();
#endif
    while (CONFIG_DISPATCH_QUEUE_MAXLEN == 0 || count++ < CONFIG_DISPATCH_QUEUE_MAXLEN) {
        if (dm_ipc_msg_next(&data) == SUCCESS_RETURN) {
            dm_ipc_msg_t *msg = (dm_ipc_msg_t *)data;

            if (ctx->event_callback) {
                ctx->event_callback(msg->type, msg->data);
            }

            if (msg->data) {
                DM_free(msg->data);
            }
            DM_free(msg);
            data = NULL;
        } else {
            break;
        }
    }
}

void *g_user_topic_callback = NULL;

int iotx_dm_subscribe_user_topic(char *topic, void *user_callback)
{
    char *uri = NULL;
    char product_key[IOTX_PRODUCT_KEY_LEN + 1] = {0};
    char device_name[IOTX_DEVICE_NAME_LEN + 1] = {0};
    int res = 0;

    IOT_Ioctl(IOTX_IOCTL_GET_PRODUCT_KEY, product_key);
    IOT_Ioctl(IOTX_IOCTL_GET_DEVICE_NAME, device_name);

    res = dm_utils_service_name((char *)DM_URI_SYS_PREFIX, topic,
                                product_key, device_name, &uri);
    g_user_topic_callback = user_callback;

    if (res < SUCCESS_RETURN) {
        return res;
    }

    {
        int retry_cnt = IOTX_DM_CLIENT_SUB_RETRY_MAX_COUNTS;
        int local_sub = 0;
        do {
            res = dm_client_subscribe(uri, dm_client_user_sub_request, &local_sub);
        } while (res < SUCCESS_RETURN && --retry_cnt);
        DM_free(uri);
    }

    return res;
}

int iotx_dm_post_rawdata(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len)
{
    int res = 0;

    _dm_api_lock();
    res = dm_mgr_upstream_thing_model_up_raw(devid, payload, payload_len);
    _dm_api_unlock();

    return res;
}

int iotx_dm_set_opt(int opt, void *data)
{
    return dm_opt_set((dm_opt_t)opt, data);
}

int iotx_dm_get_opt(int opt, void *data)
{
    if (data == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    return dm_opt_get((dm_opt_t)opt, data);
}

#if !defined(DEVICE_MODEL_RAWDATA_SOLO)
#ifdef DEVICE_MODEL_SHADOW
int iotx_dm_property_desired_get(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len)
{
    int res = 0;

    _dm_api_lock();
    res = dm_mgr_upstream_thing_property_desired_get(devid, payload, payload_len);
    _dm_api_unlock();

    return res;
}

int iotx_dm_property_desired_delete(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len)
{
    int res = 0;

    _dm_api_lock();
    res = dm_mgr_upstream_thing_property_desired_delete(devid, payload, payload_len);
    _dm_api_unlock();

    return res;
}
#endif

int iotx_dm_post_property(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len)
{
    int res = 0;

    _dm_api_lock();
    res = dm_mgr_upstream_thing_property_post(devid, payload, payload_len);
    _dm_api_unlock();

    return res;
}

#ifdef DEVICE_HISTORY_POST
int iotx_dm_post_history(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len)
{
    int res = 0;

    _dm_api_lock();
    res = dm_mgr_upstream_thing_history_post(devid, payload, payload_len);
    _dm_api_unlock();

    return res;
}
#endif /* #ifdef DEVICE_HISTORY_POST */

#ifdef LOG_REPORT_TO_CLOUD
int iotx_dm_log_post(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len)
{
    int res = 0;

    _dm_api_lock();
    res = dm_mgr_upstream_thing_log_post(devid, payload, payload_len, 0);
    _dm_api_unlock();

    return res;
}
#endif


int iotx_dm_post_event(_IN_ int devid, _IN_ char *identifier, _IN_ int identifier_len, _IN_ char *payload,
                       _IN_ int payload_len)
{
    int res = 0, method_len = 0;
    const char *method_fmt = "thing.event.%.*s.post";
    char *method = NULL;

    if (devid < 0 || identifier == NULL || identifier_len == 0 || payload == NULL || payload_len <= 0) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();

    method_len = strlen(method_fmt) + strlen(identifier) + 1;
    method = DM_malloc(method_len);
    if (method == NULL) {
        _dm_api_unlock();
        return STATE_SYS_DEPEND_MALLOC;
    }
    memset(method, 0, method_len);
    HAL_Snprintf(method, method_len, method_fmt, identifier_len, identifier);

    res = dm_mgr_upstream_thing_event_post(devid, identifier, identifier_len, method, payload, payload_len);
    DM_free(method);
    _dm_api_unlock();

    return res;
}

int iotx_dm_send_service_response(_IN_ int devid, _IN_ char *msgid, _IN_ int msgid_len, _IN_ iotx_dm_error_code_t code,
                                  _IN_ char *identifier,
                                  _IN_ int identifier_len, _IN_ char *payload, _IN_ int payload_len, void *ctx)
{
    int res = 0;

    _dm_api_lock();
    res = dm_mgr_upstream_thing_service_response(devid, msgid, msgid_len, code, identifier, identifier_len, payload,
            payload_len, ctx);
    _dm_api_unlock();

    return res;
}

int iotx_dm_send_property_get_response(_IN_ int devid, _IN_ char *msgid, _IN_ int msgid_len,
                                       _IN_ iotx_dm_error_code_t code, _IN_ char *payload, _IN_ int payload_len, void *ctx)
{
    int res = 0;

    _dm_api_lock();
    res = dm_mgr_upstream_thing_property_get_response(devid, msgid, msgid_len, code, payload,
            payload_len, ctx);
    _dm_api_unlock();

    return res;
}

int iotx_dm_deviceinfo_update(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len)
{
    int res = 0;

    _dm_api_lock();
    res = dm_mgr_upstream_thing_deviceinfo_update(devid, payload, payload_len);
    _dm_api_unlock();

    return res;
}

int iotx_dm_deviceinfo_delete(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len)
{
    int res = 0;

    _dm_api_lock();
    res = dm_mgr_upstream_thing_deviceinfo_delete(devid, payload, payload_len);
    _dm_api_unlock();

    return res;
}

int iotx_dm_qurey_ntp(void)
{
    int res = 0;

    _dm_api_lock();
    res = dm_mgr_upstream_ntp_request();
    _dm_api_unlock();

    return res;
}

int iotx_dm_send_rrpc_response(_IN_ int devid, _IN_ char *msgid, _IN_ int msgid_len, _IN_ iotx_dm_error_code_t code,
                               _IN_ char *rrpcid, _IN_ int rrpcid_len, _IN_ char *payload, _IN_ int payload_len)
{
    int res = 0;

    _dm_api_lock();
    res = dm_mgr_upstream_rrpc_response(devid, msgid, msgid_len, code, rrpcid, rrpcid_len, payload, payload_len);
    _dm_api_unlock();

    return res;
}
#endif

int iotx_dm_cota_perform_sync(_OU_ char *buffer, _IN_ int buffer_len)
{
#if defined(OTA_ENABLED) && !defined(BUILD_AOS)
    return dm_cota_perform_sync(buffer, buffer_len);
#else
    return -1;
#endif
}

int iotx_dm_cota_get_config(_IN_ const char *config_scope, const char *get_type, const char *attribute_keys)
{
#if defined(OTA_ENABLED) && !defined(BUILD_AOS)
    return dm_cota_get_config(config_scope, get_type, attribute_keys);
#else
    return -1;
#endif
}

int iotx_dm_fota_perform_sync(_OU_ char *buffer, _IN_ int buffer_len)
{
#if defined(OTA_ENABLED) && !defined(BUILD_AOS)
    return dm_fota_perform_sync(buffer, buffer_len);
#else
    return STATE_DEV_MODEL_OTA_NOT_ENABLED;
#endif
}

int iotx_dm_fota_request_image(const char *version, int buffer_len)
{
#if defined(OTA_ENABLED) && !defined(BUILD_AOS)
    return dm_fota_request_image(version, buffer_len);
#else
    return -1;
#endif
}

#ifdef DEVICE_MODEL_GATEWAY
int iotx_dm_query_topo_list(void)
{
    int res = 0;

    _dm_api_lock();
    res = dm_mgr_upstream_thing_topo_get();
    _dm_api_unlock();

    return res;
}

int iotx_dm_subdev_query(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1],
                         _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1],
                         _OU_ int *devid)
{
    int res = 0;

    _dm_api_lock();
    res = dm_mgr_device_query(product_key, device_name, devid);
    _dm_api_unlock();

    return res;
}

int iotx_dm_subdev_create(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1],
                          _IN_ char product_secret[IOTX_PRODUCT_SECRET_LEN + 1],
                          _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1],
                          _IN_ char device_secret[IOTX_DEVICE_SECRET_LEN + 1], _OU_ int *devid)
{
    int res = 0;

    _dm_api_lock();
    res = dm_mgr_device_create(IOTX_DM_DEVICE_SUBDEV, product_key, product_secret, device_name, device_secret, devid);
    _dm_api_unlock();

    return res;
}

int iotx_dm_subdev_destroy(_IN_ int devid)
{
    int res = 0;

    _dm_api_lock();
    res = dm_mgr_device_destroy(devid);
    _dm_api_unlock();

    return SUCCESS_RETURN;
}

int iotx_dm_subdev_number(void)
{
    int number = 0;

    _dm_api_lock();
    number = dm_mgr_device_number();
    _dm_api_unlock();

    return number;
}

int iotx_dm_subdev_register(_IN_ int devid)
{
    int res = 0;
    dm_mgr_dev_node_t *search_node = NULL;

    _dm_api_lock();
    res = dm_mgr_search_device_node_by_devid(devid, (void **)&search_node);
    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return res;
    }

    if ((strlen(search_node->device_secret) > 0) && (strlen(search_node->device_secret) < IOTX_DEVICE_SECRET_LEN + 1)) {
        _dm_api_unlock();
        return SUCCESS_RETURN;
    }

    res = dm_mgr_upstream_thing_sub_register(devid);

    _dm_api_unlock();
    return res;
}

int iotx_dm_subdev_proxy_register(_IN_ int devid)
{
    int res = 0;
    dm_mgr_dev_node_t *search_node = NULL;

    if (devid < 0) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();
    res = dm_mgr_search_device_node_by_devid(devid, (void **)&search_node);
    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return STATE_DEV_MODEL_DEVICE_NOT_FOUND;
    }

    if ((strlen(search_node->device_secret) > 0) && (strlen(search_node->device_secret) < IOTX_DEVICE_SECRET_LEN + 1)) {
        _dm_api_unlock();
        return SUCCESS_RETURN;
    }

    res = dm_mgr_upstream_thing_proxy_product_register(devid);

    _dm_api_unlock();
    return res;
}

int iotx_dm_subdev_unregister(_IN_ int devid)
{
    int res = 0;

    if (devid < 0) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();

    res = dm_mgr_upstream_thing_sub_unregister(devid);

    _dm_api_unlock();
    return res;
}

int iotx_dm_subdev_topo_add(_IN_ int devid)
{
    int res = 0;

    if (devid < 0) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();

    res = dm_mgr_upstream_thing_topo_add(devid);

    _dm_api_unlock();
    return res;
}

int iotx_dm_subdev_topo_del(_IN_ int devid)
{
    int res = 0;

    if (devid < 0) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();

    res = dm_mgr_upstream_thing_topo_delete(devid);

    _dm_api_unlock();
    return res;
}

int iotx_dm_subdev_login(_IN_ int devid)
{
    int res = 0;

    if (devid < 0) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();

    res = dm_mgr_upstream_combine_login(devid);

    _dm_api_unlock();
    return res;
}

int iotx_dm_subdev_logout(_IN_ int devid)
{
    int res = 0;

    if (devid < 0) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();

    res = dm_mgr_upstream_combine_logout(devid);

    _dm_api_unlock();
    return res;
}

int iotx_dm_get_device_type(_IN_ int devid, _OU_ int *type)
{
    int res = 0;

    if (devid < 0 || type == NULL) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();
    res = dm_mgr_get_dev_type(devid, type);
    _dm_api_unlock();

    return res;
}

int iotx_dm_get_device_avail_status(_IN_ int devid, _OU_ iotx_dm_dev_avail_t *status)
{
    int res = 0;
    char product_key[IOTX_PRODUCT_KEY_LEN + 1] = {0};
    char device_name[IOTX_DEVICE_NAME_LEN + 1] = {0};
    char device_secret[IOTX_DEVICE_SECRET_LEN + 1] = {0};

    if (devid < 0 || status == NULL) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();
    res = dm_mgr_search_device_by_devid(devid, product_key, device_name, device_secret);
    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return res;
    }

    res = dm_mgr_get_dev_avail(product_key, device_name, status);
    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return res;
    }

    _dm_api_unlock();
    return SUCCESS_RETURN;
}

int iotx_dm_get_device_status(_IN_ int devid, _OU_ iotx_dm_dev_status_t *status)
{
    int res = 0;

    if (devid < 0 || status == NULL) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();
    res = dm_mgr_get_dev_status(devid, status);
    _dm_api_unlock();

    return res;
}

#ifdef DEVICE_MODEL_SUBDEV_OTA
int iotx_dm_firmware_version_update(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len)
{
    int res = 0;

    _dm_api_lock();
    res = dm_mgr_upstream_thing_firmware_version_update(devid, payload, payload_len);
    _dm_api_unlock();

    return res;
}

int iotx_dm_send_firmware_version(int devid, const char *version)
{
    char msg[FIRMWARE_VERSION_MSG_LEN] = {0};
    int msg_len = 0;
    /* firmware report message json data generate */
    int ret = HAL_Snprintf(msg,
                           FIRMWARE_VERSION_MSG_LEN,
                           "{\"id\":\"%d\",\"params\":{\"version\":\"%s\"}}",
                           iotx_report_id(),
                           version
                          );
    if (ret <= 0) {
        return STATE_SYS_DEPEND_SNPRINTF;
    }

    msg_len = strlen(msg);

    ret = iotx_dm_firmware_version_update(devid, msg, msg_len);
    return ret;
}

int iotx_dm_ota_switch_device(_IN_ int devid)
{
    return dm_ota_switch_device(devid);
}

#endif
#endif

#ifdef DEPRECATED_LINKKIT
int iotx_dm_deprecated_set_tsl(_IN_ int devid, _IN_ iotx_dm_tsl_source_t source, _IN_ const char *tsl, _IN_ int tsl_len)
{
    int res = 0;

    if (devid < 0) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();
    if (source == IOTX_DM_TSL_SOURCE_CLOUD) {
        res = dm_mgr_upstream_thing_dynamictsl_get(devid);

        _dm_api_unlock();
        return res;
    }

    if (source == IOTX_DM_TSL_SOURCE_LOCAL) {
        if (tsl == NULL || tsl_len <= 0) {
            _dm_api_unlock();
            return STATE_USER_INPUT_INVALID;
        }

        res = dm_mgr_deprecated_set_tsl(devid, IOTX_DM_TSL_TYPE_ALINK, tsl, tsl_len);
        if (res != SUCCESS_RETURN) {
            _dm_api_unlock();
            return FAIL_RETURN;
        }

        _dm_api_unlock();
        return SUCCESS_RETURN;
    }

    _dm_api_unlock();
    return FAIL_RETURN;
}

int iotx_dm_deprecated_set_property_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value,
        _IN_ int value_len)
{
    int res = 0;

    if (devid < 0 || key == NULL || key_len <= 0 || value == NULL) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();
    res = dm_mgr_deprecated_set_property_value(devid, key, key_len, value, value_len);
    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    _dm_api_unlock();
    return SUCCESS_RETURN;
}

int iotx_dm_deprecated_get_property_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value)
{
    int res = 0;

    if (devid < 0 || key == NULL || key_len <= 0 || value == NULL) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();
    res = dm_mgr_deprecated_get_property_value(devid, key, key_len, value);
    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    _dm_api_unlock();
    return SUCCESS_RETURN;
}

int iotx_dm_deprecated_set_event_output_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value,
        _IN_ int value_len)
{
    int res = 0;

    if (devid < 0 || key == NULL || key_len <= 0 || value == NULL) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();
    res = dm_mgr_deprecated_set_event_output_value(devid, key, key_len, value, value_len);
    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    _dm_api_unlock();
    return SUCCESS_RETURN;
}

int iotx_dm_deprecated_get_event_output_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value)
{
    int res = 0;

    if (devid < 0 || key == NULL || key_len <= 0 || value == NULL) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();
    res = dm_mgr_deprecated_get_event_output_value(devid, key, key_len, value);
    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    _dm_api_unlock();
    return SUCCESS_RETURN;
}

int iotx_dm_deprecated_get_service_input_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value)
{
    int res = 0;

    if (devid < 0 || key == NULL || key_len <= 0 || value == NULL) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();
    res = dm_mgr_deprecated_get_service_input_value(devid, key, key_len, value);
    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    _dm_api_unlock();
    return SUCCESS_RETURN;
}

int iotx_dm_deprecated_set_service_output_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value,
        _IN_ int value_len)
{
    int res = 0;

    if (devid < 0 || key == NULL || key_len <= 0 || value == NULL) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();
    res = dm_mgr_deprecated_set_service_output_value(devid, key, key_len, value, value_len);
    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    _dm_api_unlock();
    return SUCCESS_RETURN;
}

int iotx_dm_deprecated_get_service_output_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value)
{
    int res = 0;

    if (devid < 0 || key == NULL || key_len <= 0 || value == NULL) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();
    res = dm_mgr_deprecated_get_service_output_value(devid, key, key_len, value);
    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    _dm_api_unlock();
    return SUCCESS_RETURN;
}

int iotx_dm_deprecated_post_property_start(_IN_ int devid, _OU_ void **handle)
{
    dm_api_property_t *dapi_property = NULL;

    if (devid < 0 || handle == NULL || *handle != NULL) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();
    dapi_property = DM_malloc(sizeof(dm_api_property_t));
    if (dapi_property == NULL) {
        _dm_api_unlock();
        return STATE_SYS_DEPEND_MALLOC;
    }
    memset(dapi_property, 0, sizeof(dm_api_property_t));


    /* Create Mutex */
    dapi_property->mutex = HAL_MutexCreate();
    if (dapi_property->mutex == NULL) {
        DM_free(dapi_property);
        _dm_api_unlock();
        return STATE_SYS_DEPEND_MALLOC;
    }

    /* Set Devid */
    dapi_property->devid = devid;

    /* Init Json Object */
    dapi_property->lite = lite_cjson_create_object();
    if (dapi_property->lite == NULL) {
        DM_free(dapi_property->mutex);
        DM_free(dapi_property);
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    *handle = (void *)dapi_property;

    _dm_api_unlock();
    return SUCCESS_RETURN;
}

static int _iotx_dm_deprecated_post_property_add(_IN_ void *handle, _IN_ char *identifier, _IN_ int identifier_len)
{
    int res = 0;
    dm_api_property_t *dapi_property = NULL;

    if (handle == NULL || identifier == NULL || identifier_len <= 0) {
        return STATE_USER_INPUT_INVALID;
    }
    dapi_property = (dm_api_property_t *)handle;

    /* Assemble Property Payload */
    res = dm_mgr_deprecated_assemble_property(dapi_property->devid, identifier, identifier_len, dapi_property->lite);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int iotx_dm_deprecated_post_property_add(_IN_ void *handle, _IN_ char *identifier, _IN_ int identifier_len)
{
    int ret = SUCCESS_RETURN, res = 0, index = 0, number = 0;
    void *property_refer = NULL;
    char *identifier_refer = NULL;
    dm_api_property_t *dapi_property = NULL;

    if (handle == NULL) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();
    dapi_property = (dm_api_property_t *)handle;

    if (identifier != IOTX_DM_POST_PROPERTY_ALL) {
        if (identifier_len <= 0) {
            _dm_api_unlock();
            return FAIL_RETURN;
        }
        ret = _iotx_dm_deprecated_post_property_add(handle, identifier, identifier_len);

        _dm_api_unlock();
        return ret;
    }

    res = dm_mgr_deprecated_get_property_number(dapi_property->devid, &number);
    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    for (index = 0; index < number; index++) {
        property_refer = NULL;
        identifier_refer = NULL;

        res = dm_mgr_deprecated_get_property_by_index(dapi_property->devid, index, &property_refer);
        if (res != SUCCESS_RETURN) {
            continue;
        }

        res = dm_mgr_deprecated_get_property_identifier(property_refer, &identifier_refer);
        if (res != SUCCESS_RETURN) {
            continue;
        }

        res = _iotx_dm_deprecated_post_property_add(handle, identifier_refer, strlen(identifier_refer));
        if (res != SUCCESS_RETURN) {
            ret = FAIL_RETURN;
        }
    }

    _dm_api_unlock();
    return ret;
}

int iotx_dm_deprecated_post_property_end(_IN_ void **handle)
{
    int res = 0;
    char *payload = NULL;
    dm_api_property_t *dapi_property = NULL;

    if (handle == NULL) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();
    dapi_property = *((dm_api_property_t **)handle);

    payload = lite_cjson_print_unformatted(dapi_property->lite);
    if (payload == NULL) {
        lite_cjson_delete(dapi_property->lite);
        if (dapi_property->mutex) {
            HAL_MutexDestroy(dapi_property->mutex);
        }
        DM_free(dapi_property);
        _dm_api_unlock();
        return STATE_SYS_DEPEND_MALLOC;
    }

    res = dm_mgr_upstream_thing_property_post(dapi_property->devid, payload, strlen(payload));

    DM_free(payload);
    lite_cjson_delete(dapi_property->lite);
    if (dapi_property->mutex) {
        HAL_MutexDestroy(dapi_property->mutex);
    }
    DM_free(dapi_property);
    *handle = NULL;

    _dm_api_unlock();
    return res;
}

int iotx_dm_deprecated_post_event(_IN_ int devid, _IN_ char *identifier, _IN_ int identifier_len)
{
    int res = 0;
    void *event = NULL;
    lite_cjson_item_t *lite = NULL;
    char *method = NULL, *payload = NULL;

    if (devid < 0 || identifier == NULL || identifier_len <= 0) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();
    lite = lite_cjson_create_object();
    if (lite == NULL) {
        _dm_api_unlock();
        return STATE_SYS_DEPEND_MALLOC;
    }

    res = dm_mgr_deprecated_assemble_event_output(devid, identifier, identifier_len, lite);
    if (res != SUCCESS_RETURN) {
        lite_cjson_delete(lite);
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    payload = lite_cjson_print_unformatted(lite);
    lite_cjson_delete(lite);
    if (payload == NULL) {
        _dm_api_unlock();
        return STATE_SYS_DEPEND_MALLOC;
    }

    res = dm_mgr_deprecated_get_event_by_identifier(devid, identifier, &event);
    if (res != SUCCESS_RETURN) {
        DM_free(payload);
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    res = dm_mgr_deprecated_get_event_method(event, &method);
    if (res != SUCCESS_RETURN) {
        DM_free(payload);
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    res = dm_mgr_upstream_thing_event_post(devid, identifier, identifier_len, method, payload, strlen(payload));

    DM_free(payload);
    DM_free(method);

    _dm_api_unlock();
    return res;
}

int iotx_dm_deprecated_legacy_set_property_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value,
        _IN_ char *value_str)
{
    int res = 0;
    void *data = NULL;
    dm_shw_data_type_e type;

    if (devid < 0 || key == NULL || key_len <= 0 || ((value == NULL) && (value_str == NULL))) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();
    res = dm_mgr_deprecated_get_property_data(devid, key, key_len, &data);
    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    res = dm_mgr_deprecated_get_data_type(data, &type);
    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    switch (type) {
        case DM_SHW_DATA_TYPE_INT:
        case DM_SHW_DATA_TYPE_ENUM:
        case DM_SHW_DATA_TYPE_BOOL: {
            int value_int = (value == NULL) ? (atoi(value_str)) : (*(int *)value);
            res = dm_mgr_deprecated_set_property_value(devid, key, key_len, &value_int, sizeof(int));
        }
        break;
        case DM_SHW_DATA_TYPE_FLOAT: {
            float value_float = (value == NULL) ? (atof(value_str)) : (*(float *)value);
            res = dm_mgr_deprecated_set_property_value(devid, key, key_len, &value_float, sizeof(float));
        }
        break;
        case DM_SHW_DATA_TYPE_DOUBLE: {
            double value_double = (value == NULL) ? (atof(value_str)) : (*(double *)value);
            res = dm_mgr_deprecated_set_property_value(devid, key, key_len, &value_double, sizeof(double));
        }
        break;
        case DM_SHW_DATA_TYPE_TEXT:
        case DM_SHW_DATA_TYPE_DATE: {
            char *value_string = (value == NULL) ? (value_str) : (value);
            res = dm_mgr_deprecated_set_property_value(devid, key, key_len, value_string, strlen(value_string));
        }
        break;
        default: {
            res =  FAIL_RETURN;
        }
        break;
    }

    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    _dm_api_unlock();
    return SUCCESS_RETURN;
}

int iotx_dm_deprecated_legacy_get_property_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value,
        _IN_ char **value_str)
{
    int res = 0;
    void *data = NULL;
    dm_shw_data_type_e type;

    if (devid < 0 || key == NULL || key_len <= 0 || ((value == NULL) && (value_str == NULL))) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();
    res = dm_mgr_deprecated_get_property_data(devid, key, key_len, &data);
    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    res = dm_mgr_deprecated_get_data_type(data, &type);
    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    switch (type) {
        case DM_SHW_DATA_TYPE_INT:
        case DM_SHW_DATA_TYPE_ENUM:
        case DM_SHW_DATA_TYPE_BOOL: {
            int value_int = 0;
            res = dm_mgr_deprecated_get_property_value(devid, key, key_len, (void *)&value_int);
            if (res == SUCCESS_RETURN) {
                if (value) {
                    *(int *)value = value_int;
                }
                if (value_str) {
                    res = dm_utils_itoa_direct(value_int, value_str);
                }
            }
        }
        break;
        case DM_SHW_DATA_TYPE_FLOAT: {
            float value_float = 0;
            res = dm_mgr_deprecated_get_property_value(devid, key, key_len, (void *)&value_float);
            if (res == SUCCESS_RETURN) {
                if (value) {
                    *(float *)value = value_float;
                }
                if (value_str) {
                    res = dm_utils_ftoa_direct(value_float, value_str);
                }
            }
        }
        break;
        case DM_SHW_DATA_TYPE_DOUBLE: {
            double value_double = 0;
            res = dm_mgr_deprecated_get_property_value(devid, key, key_len, (void *)&value_double);
            if (res == SUCCESS_RETURN) {
                if (value) {
                    *(double *)value = value_double;
                }
                if (value_str) {
                    res = dm_utils_ftoa_direct(value_double, value_str);
                }
            }
        }
        break;
        case DM_SHW_DATA_TYPE_TEXT:
        case DM_SHW_DATA_TYPE_DATE: {
            char *value_string = NULL;
            res = dm_mgr_deprecated_get_property_value(devid, key, key_len, (void *)&value_string);
            if (res == SUCCESS_RETURN) {
                if (value) {
                    memcpy(value, value_string, strlen(value_string));
                }
                if (value_str) {
                    *value_str = value_string;
                } else {
                    HAL_Free(value_string);
                }
            }
        }
        break;
        default: {
            res =  FAIL_RETURN;
        }
        break;
    }

    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    _dm_api_unlock();
    return SUCCESS_RETURN;
}

int iotx_dm_deprecated_legacy_set_event_output_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value,
        _IN_ char *value_str)
{
    int res = 0;
    void *data = NULL;
    dm_shw_data_type_e type;

    if (devid < 0 || key == NULL || key_len <= 0 || ((value == NULL) && (value_str == NULL))) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();
    res = dm_mgr_deprecated_get_event_output_data(devid, key, key_len, &data);
    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    res = dm_mgr_deprecated_get_data_type(data, &type);
    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    switch (type) {
        case DM_SHW_DATA_TYPE_INT:
        case DM_SHW_DATA_TYPE_ENUM:
        case DM_SHW_DATA_TYPE_BOOL: {
            int value_int = (value == NULL) ? (atoi(value_str)) : (*(int *)value);
            res = dm_mgr_deprecated_set_event_output_value(devid, key, key_len, &value_int, sizeof(int));
        }
        break;
        case DM_SHW_DATA_TYPE_FLOAT: {
            float value_float = (value == NULL) ? (atof(value_str)) : (*(float *)value);
            res = dm_mgr_deprecated_set_event_output_value(devid, key, key_len, &value_float, sizeof(float));
        }
        break;
        case DM_SHW_DATA_TYPE_DOUBLE: {
            double value_double = (value == NULL) ? (atof(value_str)) : (*(double *)value);
            res = dm_mgr_deprecated_set_event_output_value(devid, key, key_len, &value_double, sizeof(double));
        }
        break;
        case DM_SHW_DATA_TYPE_TEXT:
        case DM_SHW_DATA_TYPE_DATE: {
            char *value_string = (value == NULL) ? (value_str) : (value);
            res = dm_mgr_deprecated_set_event_output_value(devid, key, key_len, value_string, strlen(value_string));
        }
        break;
        default: {
            res =  FAIL_RETURN;
        }
        break;
    }

    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    _dm_api_unlock();
    return SUCCESS_RETURN;
}

int iotx_dm_deprecated_legacy_get_event_output_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value,
        _IN_ char **value_str)
{
    int res = 0;
    void *data = NULL;
    dm_shw_data_type_e type;

    if (devid < 0 || key == NULL || key_len <= 0 || ((value == NULL) && (value_str == NULL))) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();
    res = dm_mgr_deprecated_get_event_output_data(devid, key, key_len, &data);
    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    res = dm_mgr_deprecated_get_data_type(data, &type);
    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    switch (type) {
        case DM_SHW_DATA_TYPE_INT:
        case DM_SHW_DATA_TYPE_ENUM:
        case DM_SHW_DATA_TYPE_BOOL: {
            int value_int = 0;
            res = dm_mgr_deprecated_get_event_output_value(devid, key, key_len, (void *)&value_int);
            if (res == SUCCESS_RETURN) {
                if (value) {
                    *(int *)value = value_int;
                }
                if (value_str) {
                    res = dm_utils_itoa_direct(value_int, value_str);
                }
            }
        }
        break;
        case DM_SHW_DATA_TYPE_FLOAT: {
            float value_float = 0;
            res = dm_mgr_deprecated_get_event_output_value(devid, key, key_len, (void *)&value_float);
            if (res == SUCCESS_RETURN) {
                if (value) {
                    *(float *)value = value_float;
                }
                if (value_str) {
                    res = dm_utils_ftoa_direct(value_float, value_str);
                }
            }
        }
        break;
        case DM_SHW_DATA_TYPE_DOUBLE: {
            double value_double = 0;
            res = dm_mgr_deprecated_get_event_output_value(devid, key, key_len, (void *)&value_double);
            if (res == SUCCESS_RETURN) {
                if (value) {
                    *(double *)value = value_double;
                }
                if (value_str) {
                    res = dm_utils_ftoa_direct(value_double, value_str);
                }
            }
        }
        break;
        case DM_SHW_DATA_TYPE_TEXT:
        case DM_SHW_DATA_TYPE_DATE: {
            char *value_string = NULL;
            res = dm_mgr_deprecated_get_event_output_value(devid, key, key_len, (void *)&value_string);
            if (res == SUCCESS_RETURN) {
                if (value) {
                    memcpy(value, value_string, strlen(value_string));
                }
                if (value_str) {
                    *value_str = value_string;
                } else {
                    HAL_Free(value_string);
                }
            }
        }
        break;
        default: {
            res =  FAIL_RETURN;
        }
        break;
    }

    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    _dm_api_unlock();
    return SUCCESS_RETURN;
}

int iotx_dm_deprecated_legacy_get_service_input_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len,
        _IN_ void *value,
        _IN_ char **value_str)
{
    int res = 0;
    void *data = NULL;
    dm_shw_data_type_e type;

    if (devid < 0 || key == NULL || key_len <= 0 || ((value == NULL) && (value_str == NULL))) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();

    res = dm_mgr_deprecated_get_service_input_data(devid, key, key_len, &data);
    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    res = dm_mgr_deprecated_get_data_type(data, &type);
    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    switch (type) {
        case DM_SHW_DATA_TYPE_INT:
        case DM_SHW_DATA_TYPE_ENUM:
        case DM_SHW_DATA_TYPE_BOOL: {
            int value_int = 0;
            res = dm_mgr_deprecated_get_service_input_value(devid, key, key_len, (void *)&value_int);
            if (res == SUCCESS_RETURN) {
                if (value) {
                    *(int *)value = value_int;
                }
                if (value_str) {
                    res = dm_utils_itoa_direct(value_int, value_str);
                }
            }
        }
        break;
        case DM_SHW_DATA_TYPE_FLOAT: {
            float value_float = 0;
            res = dm_mgr_deprecated_get_service_input_value(devid, key, key_len, (void *)&value_float);
            if (res == SUCCESS_RETURN) {
                if (value) {
                    *(float *)value = value_float;
                }
                if (value_str) {
                    res = dm_utils_ftoa_direct(value_float, value_str);
                }
            }
        }
        break;
        case DM_SHW_DATA_TYPE_DOUBLE: {
            double value_double = 0;
            res = dm_mgr_deprecated_get_service_input_value(devid, key, key_len, (void *)&value_double);
            if (res == SUCCESS_RETURN) {
                if (value) {
                    *(double *)value = value_double;
                }
                if (value_str) {
                    res = dm_utils_ftoa_direct(value_double, value_str);
                }
            }
        }
        break;
        case DM_SHW_DATA_TYPE_TEXT:
        case DM_SHW_DATA_TYPE_DATE: {
            char *value_string = NULL;
            res = dm_mgr_deprecated_get_service_input_value(devid, key, key_len, (void *)&value_string);
            if (res == SUCCESS_RETURN) {
                if (value) {
                    memcpy(value, value_string, strlen(value_string));
                }
                if (value_str) {
                    *value_str = value_string;
                } else {
                    HAL_Free(value_string);
                }
            }
        }
        break;
        default: {
            res =  FAIL_RETURN;
        }
        break;
    }

    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    _dm_api_unlock();
    return SUCCESS_RETURN;
}

int iotx_dm_deprecated_legacy_set_service_output_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len,
        _IN_ void *value,
        _IN_ char *value_str)
{
    int res = 0;
    void *data = NULL;
    dm_shw_data_type_e type;

    if (devid < 0 || key == NULL || key_len <= 0 || ((value == NULL) && (value_str == NULL))) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();
    res = dm_mgr_deprecated_get_service_output_data(devid, key, key_len, &data);
    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    res = dm_mgr_deprecated_get_data_type(data, &type);
    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    switch (type) {
        case DM_SHW_DATA_TYPE_INT:
        case DM_SHW_DATA_TYPE_ENUM:
        case DM_SHW_DATA_TYPE_BOOL: {
            int value_int = (value == NULL) ? (atoi(value_str)) : (*(int *)value);
            res = dm_mgr_deprecated_set_service_output_value(devid, key, key_len, &value_int, sizeof(int));
        }
        break;
        case DM_SHW_DATA_TYPE_FLOAT: {
            float value_float = (value == NULL) ? (atof(value_str)) : (*(float *)value);
            res = dm_mgr_deprecated_set_service_output_value(devid, key, key_len, &value_float, sizeof(float));
        }
        break;
        case DM_SHW_DATA_TYPE_DOUBLE: {
            double value_double = (value == NULL) ? (atof(value_str)) : (*(double *)value);
            res = dm_mgr_deprecated_set_service_output_value(devid, key, key_len, &value_double, sizeof(double));
        }
        break;
        case DM_SHW_DATA_TYPE_TEXT:
        case DM_SHW_DATA_TYPE_DATE: {
            char *value_string = (value == NULL) ? (value_str) : (value);
            res = dm_mgr_deprecated_set_service_output_value(devid, key, key_len, value_string, strlen(value_string));
        }
        break;
        default: {
            res =  FAIL_RETURN;
        }
        break;
    }

    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    _dm_api_unlock();
    return SUCCESS_RETURN;
}

int iotx_dm_deprecated_legacy_get_service_output_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len,
        _IN_ void *value,
        _IN_ char **value_str)
{
    int res = 0;
    void *data = NULL;
    dm_shw_data_type_e type;

    if (devid < 0 || key == NULL || key_len <= 0 || ((value == NULL) && (value_str == NULL))) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();

    res = dm_mgr_deprecated_get_service_output_data(devid, key, key_len, &data);
    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    res = dm_mgr_deprecated_get_data_type(data, &type);
    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    switch (type) {
        case DM_SHW_DATA_TYPE_INT:
        case DM_SHW_DATA_TYPE_ENUM:
        case DM_SHW_DATA_TYPE_BOOL: {
            int value_int = 0;
            res = dm_mgr_deprecated_get_service_output_value(devid, key, key_len, (void *)&value_int);
            if (res == SUCCESS_RETURN) {
                if (value) {
                    *(int *)value = value_int;
                }
                if (value_str) {
                    res = dm_utils_itoa_direct(value_int, value_str);
                }
            }
        }
        break;
        case DM_SHW_DATA_TYPE_FLOAT: {
            float value_float = 0;
            res = dm_mgr_deprecated_get_service_output_value(devid, key, key_len, (void *)&value_float);
            if (res == SUCCESS_RETURN) {
                if (value) {
                    *(float *)value = value_float;
                }
                if (value_str) {
                    res = dm_utils_ftoa_direct(value_float, value_str);
                }
            }
        }
        break;
        case DM_SHW_DATA_TYPE_DOUBLE: {
            double value_double = 0;
            res = dm_mgr_deprecated_get_service_output_value(devid, key, key_len, (void *)&value_double);
            if (res == SUCCESS_RETURN) {
                if (value) {
                    *(double *)value = value_double;
                }
                if (value_str) {
                    res = dm_utils_ftoa_direct(value_double, value_str);
                }
            }
        }
        break;
        case DM_SHW_DATA_TYPE_TEXT:
        case DM_SHW_DATA_TYPE_DATE: {
            char *value_string = NULL;
            res = dm_mgr_deprecated_get_service_output_value(devid, key, key_len, (void *)&value_string);
            if (res == SUCCESS_RETURN) {
                if (value) {
                    memcpy(value, value_string, strlen(value_string));
                }
                if (value_str) {
                    *value_str = value_string;
                } else {
                    HAL_Free(value_string);
                }
            }
        }
        break;
        default: {
            res =  FAIL_RETURN;
        }
        break;
    }

    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    _dm_api_unlock();
    return SUCCESS_RETURN;
}

int iotx_dm_deprecated_legacy_get_pkdn_by_devid(_IN_ int devid, _OU_ char product_key[IOTX_PRODUCT_KEY_LEN + 1],
        _OU_ char device_name[IOTX_DEVICE_NAME_LEN + 1])
{
    int res = 0;
    char device_secret[IOTX_DEVICE_SECRET_LEN + 1] = {0};

    if (devid < 0 || product_key == NULL || device_name == NULL) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();
    res = dm_mgr_search_device_by_devid(devid, product_key, device_name, device_secret);
    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    _dm_api_unlock();
    return SUCCESS_RETURN;
}

int iotx_dm_deprecated_legacy_get_devid_by_pkdn(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1],
        _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1], _OU_ int *devid)
{
    int res = 0;

    if (devid == NULL || product_key == NULL || device_name == NULL ||
        (strlen(product_key) >= IOTX_PRODUCT_KEY_LEN + 1) ||
        (strlen(device_name) >= IOTX_DEVICE_NAME_LEN + 1)) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();
    res = dm_mgr_search_device_by_pkdn(product_key, device_name, devid);
    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    _dm_api_unlock();
    return SUCCESS_RETURN;
}

int iotx_dm_deprecated_legacy_get_thingid_by_devid(_IN_ int devid, _OU_ void **thing_id)
{
    int res = 0;

    if (devid < 0 || thing_id == NULL || *thing_id != NULL) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();
    res = dm_mgr_search_device_node_by_devid(devid, thing_id);
    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    _dm_api_unlock();
    return SUCCESS_RETURN;
}

int iotx_dm_deprecated_legacy_get_devid_by_thingid(_IN_ void *thing_id, _OU_ int *devid)
{
    int res = 0;

    if (thing_id == NULL || devid == NULL) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();
    res = dm_mgr_deprecated_search_devid_by_device_node(thing_id, devid);
    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    _dm_api_unlock();
    return SUCCESS_RETURN;
}

int iotx_dm_deprecated_legacy_get_pkdn_ptr_by_devid(_IN_ int devid, _OU_ char **product_key, _OU_ char **device_name)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || product_key == NULL || *product_key != NULL ||
        device_name == NULL || *device_name != NULL) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();
    res = dm_mgr_search_device_node_by_devid(devid, (void **)&node);
    if (res != SUCCESS_RETURN) {
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    *product_key = node->product_key;
    *device_name = node->device_name;

    _dm_api_unlock();
    return SUCCESS_RETURN;
}

int iotx_dm_deprecated_legacy_send_service_response(_IN_ int devid, _IN_ int msgid, _IN_ iotx_dm_error_code_t code,
        _IN_ char *identifier, _IN_ int identifier_len, _IN_ char *payload, _IN_ int payload_len)
{
    int res = 0;

    _dm_api_lock();

    res = dm_mgr_deprecated_upstream_thing_service_response(devid, msgid, code, identifier, identifier_len, payload,
            payload_len);

    _dm_api_unlock();
    return res;
}

int iotx_dm_deprecated_send_service_response(_IN_ int devid, _IN_ int msgid, _IN_ iotx_dm_error_code_t code,
        _IN_ char *identifier,
        _IN_ int identifier_len)
{
    int res = 0;
    lite_cjson_item_t *lite = NULL;
    char *payload = NULL;

    if (devid < 0 || msgid < 0 || identifier == NULL || identifier_len <= 0) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();
    lite = lite_cjson_create_object();
    if (lite == NULL) {
        _dm_api_unlock();
        return STATE_SYS_DEPEND_MALLOC;
    }

    res = dm_mgr_deprecated_assemble_service_output(devid, identifier, identifier_len, lite);
    if (res != SUCCESS_RETURN) {
        lite_cjson_delete(lite);
        _dm_api_unlock();
        return FAIL_RETURN;
    }

    payload = lite_cjson_print_unformatted(lite);
    lite_cjson_delete(lite);
    if (payload == NULL) {
        _dm_api_unlock();
        return STATE_SYS_DEPEND_MALLOC;
    }

    res = dm_mgr_deprecated_upstream_thing_service_response(devid, msgid, code, identifier, identifier_len, payload,
            strlen(payload));

    DM_free(payload);

    _dm_api_unlock();
    return res;
}

#ifdef DEVICE_MODEL_GATEWAY
int iotx_dm_deprecated_subdev_register(_IN_ int devid, _IN_ char device_secret[IOTX_DEVICE_SECRET_LEN + 1])
{
    int res = 0;
    dm_mgr_dev_node_t *search_node = NULL;

    if (devid < 0) {
        return STATE_USER_INPUT_INVALID;
    }

    _dm_api_lock();
    if ((device_secret != NULL) && (strlen(device_secret) > 0) && (strlen(device_secret) < IOTX_DEVICE_SECRET_LEN + 1)) {
        res = dm_mgr_search_device_node_by_devid(devid, (void **)&search_node);
        if (res != SUCCESS_RETURN) {
            _dm_api_unlock();
            return FAIL_RETURN;
        }
        memset(search_node->device_secret, 0, IOTX_DEVICE_SECRET_LEN + 1);
        memcpy(search_node->device_secret, device_secret, strlen(device_secret));
        _dm_api_unlock();
        return SUCCESS_RETURN;
    }

    res = dm_mgr_upstream_thing_sub_register(devid);

    _dm_api_unlock();
    return res;
}
#endif
#endif
