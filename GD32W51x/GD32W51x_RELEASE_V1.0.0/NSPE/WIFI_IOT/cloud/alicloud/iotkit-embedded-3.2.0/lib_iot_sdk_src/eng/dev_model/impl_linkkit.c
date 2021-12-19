/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "iotx_dm_internal.h"

#if defined(DEVICE_MODEL_ENABLED) && !defined(DEPRECATED_LINKKIT)
#include "dev_model_api.h"
#include "dm_opt.h"

#ifdef LOG_REPORT_TO_CLOUD
    #include "iotx_log_report.h"
#endif

#ifdef INFRA_MEM_STATS
    #include "infra_mem_stats.h"
    #define IMPL_LINKKIT_MALLOC(size)            LITE_malloc(size, MEM_MAGIC, "impl.linkkit")
    #define IMPL_LINKKIT_FREE(ptr)               LITE_free(ptr)
#else
    #define IMPL_LINKKIT_MALLOC(size)            HAL_Malloc(size)
    #define IMPL_LINKKIT_FREE(ptr)               {HAL_Free((void *)ptr);ptr = NULL;}
#endif

#ifdef DEV_BIND_ENABLED
    #include "bind_api.h"
#endif
#if (defined(AWSS_SUPPORT_ZEROCONFIG) &&  !defined(AWSS_DISABLE_REGISTRAR))
    extern void awss_registrar_init(void);
    extern void awss_registrar_deinit(void);
#endif

#define IOTX_LINKKIT_KEY_ID          "id"
#define IOTX_LINKKIT_KEY_CODE        "code"
#define IOTX_LINKKIT_KEY_DEVID       "devid"
#define IOTX_LINKKIT_KEY_SERVICEID   "serviceid"
#define IOTX_LINKKIT_KEY_PROPERTYID  "propertyid"
#define IOTX_LINKKIT_KEY_EVENTID     "eventid"
#define IOTX_LINKKIT_KEY_PAYLOAD     "payload"
#define IOTX_LINKKIT_KEY_CONFIG_ID   "configId"
#define IOTX_LINKKIT_KEY_CONFIG_SIZE "configSize"
#define IOTX_LINKKIT_KEY_GET_TYPE    "getType"
#define IOTX_LINKKIT_KEY_SIGN        "sign"
#define IOTX_LINKKIT_KEY_SIGN_METHOD "signMethod"
#define IOTX_LINKKIT_KEY_URL         "url"
#define IOTX_LINKKIT_KEY_VERSION     "version"
#define IOTX_LINKKIT_KEY_UTC         "utc"
#define IOTX_LINKKIT_KEY_RRPCID      "rrpcid"
#define IOTX_LINKKIT_KEY_CTX         "ctx"
#define IOTX_LINKKIT_KEY_TOPO        "topo"
#define IOTX_LINKKIT_KEY_PRODUCT_KEY "productKey"
#define IOTX_LINKKIT_KEY_TIME        "time"
#define IOTX_LINKKIT_KEY_DATA        "data"
#define IOTX_LINKKIT_KEY_MESSAGE     "message"
#define IOTX_LINKKIT_KEY_MODULE_ID   "module"

#define IOTX_LINKKIT_SYNC_DEFAULT_TIMEOUT_MS    10000
#define IOTX_LINKKIT_RRPC_ID_LEN                25

#if !defined(DEVICE_MODEL_RAWDATA_SOLO)
typedef enum {
    IOTX_SERVICE_REQ_TYPE_RRPC,
    IOTX_SERVICE_REQ_TYPE_GENERAL,
} iotx_service_req_type_t;

typedef struct {
    char rrpc_id[IOTX_LINKKIT_RRPC_ID_LEN + 1];
} service_meta_rrpc_t;

typedef struct {
    void *alcs_ctx;
} service_meta_general_t;

typedef struct {
    int msgid;
    int type;
    union {
        service_meta_rrpc_t rrpc;
        service_meta_general_t general;
    } service_meta;
    uint64_t ctime;
    struct list_head linked_list;
} iotx_service_ctx_node_t;
#endif /* #if !defined(DEVICE_MODEL_RAWDATA_SOLO) */

typedef struct {
    int msgid;
    void *semaphore;
    int code;
    struct list_head linked_list;
} iotx_linkkit_upstream_sync_callback_node_t;

typedef struct {
    void *mutex;
    void *upstream_mutex;
    void *service_list_mutex;
    int service_list_num;
    int is_opened;
    int is_connected;
    int is_yield_running;
    int yield_running;
    struct list_head upstream_sync_callback_list;
    struct list_head downstream_service_list;
} iotx_linkkit_ctx_t;

static iotx_linkkit_ctx_t g_iotx_linkkit_ctx = {0};

#ifdef ALCS_ENABLED
    extern void dm_server_free_context(_IN_ void *ctx);
#endif

static iotx_linkkit_ctx_t *_iotx_linkkit_get_ctx(void)
{
    return &g_iotx_linkkit_ctx;
}

static void _iotx_linkkit_mutex_lock(void)
{
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();
    if (ctx->mutex) {
        HAL_MutexLock(ctx->mutex);
    }
}

static void _iotx_linkkit_mutex_unlock(void)
{
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();
    if (ctx->mutex) {
        HAL_MutexUnlock(ctx->mutex);
    }
}

static int _impl_copy(_IN_ void *input, _IN_ int input_len, _OU_ void **output, _IN_ int output_len)
{
    if (input == NULL || output == NULL || *output != NULL) {
        return STATE_USER_INPUT_INVALID;
    }

    *output = IMPL_LINKKIT_MALLOC(output_len);
    if (*output == NULL) {
        return STATE_SYS_DEPEND_MALLOC;
    }
    memset(*output, 0, output_len);
    memcpy(*output, input, input_len);

    return SUCCESS_RETURN;
}

#if !defined(DEVICE_MODEL_RAWDATA_SOLO)
static void _linkkit_service_list_mutex_lock(void)
{
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();
    if (ctx->service_list_mutex) {
        HAL_MutexLock(ctx->service_list_mutex);
    }
}

static void _linkkit_service_list_mutex_unlock(void)
{
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();
    if (ctx->service_list_mutex) {
        HAL_MutexUnlock(ctx->service_list_mutex);
    }
}

static int _linkkit_service_list_insert(iotx_service_req_type_t type, char *msgid, char *rrpcid, int rrpcid_len,
                                        void *alcs_ctx, void **p_node)
{
    iotx_service_ctx_node_t *insert_node = NULL;
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();

    if (ctx->service_list_num >= CONFIG_SERVICE_LIST_MAXLEN) {
        iotx_state_event(ITE_STATE_DEV_MODEL, STATE_DEV_MODEL_INTERNAL_ERROR, "service list full");
        return STATE_DEV_MODEL_INTERNAL_ERROR;
    }

    insert_node = (iotx_service_ctx_node_t *)IMPL_LINKKIT_MALLOC(sizeof(iotx_service_ctx_node_t));
    if (insert_node == NULL) {
        iotx_state_event(ITE_STATE_DEV_MODEL, STATE_SYS_DEPEND_MALLOC, NULL);
        return STATE_SYS_DEPEND_MALLOC;
    }
    memset(insert_node, 0, sizeof(iotx_service_ctx_node_t));

    infra_str2int(msgid, &insert_node->msgid);
    insert_node->type = type;
    insert_node->ctime = HAL_UptimeMs();

    if (type == IOTX_SERVICE_REQ_TYPE_RRPC) {
        if (rrpcid_len > IOTX_LINKKIT_RRPC_ID_LEN) {
            IMPL_LINKKIT_FREE(insert_node);
            iotx_state_event(ITE_STATE_DEV_MODEL, STATE_DEV_MODEL_RRPCID_TOO_LONG, "rrpcid_len(%d) > %d", rrpcid_len,
                             IOTX_LINKKIT_RRPC_ID_LEN);
            return STATE_DEV_MODEL_RRPCID_TOO_LONG;
        }
        memcpy(insert_node->service_meta.rrpc.rrpc_id, rrpcid, rrpcid_len);
    } else if (type == IOTX_SERVICE_REQ_TYPE_GENERAL) {
        insert_node->service_meta.general.alcs_ctx = alcs_ctx;
    }

    _linkkit_service_list_mutex_lock();
    list_add(&insert_node->linked_list, &ctx->downstream_service_list);
    ctx->service_list_num++;
    _linkkit_service_list_mutex_unlock();

    *p_node = insert_node;
    return SUCCESS_RETURN;
}

static int _linkkit_service_list_search(iotx_service_ctx_node_t *node)
{
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();
    iotx_service_ctx_node_t *search_node;

    _linkkit_service_list_mutex_lock();
    list_for_each_entry(search_node, &ctx->downstream_service_list, linked_list, iotx_service_ctx_node_t) {
        if (search_node == node) {
            _linkkit_service_list_mutex_unlock();
            return SUCCESS_RETURN;
        }
    }
    _linkkit_service_list_mutex_unlock();

    return STATE_DEV_MODEL_SERVICE_CTX_NOT_EXIST;
}

static int _linkkit_service_list_delete(iotx_service_ctx_node_t *node)
{
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();

    if (node->type == IOTX_SERVICE_REQ_TYPE_RRPC) {
        list_del(&node->linked_list);
        IMPL_LINKKIT_FREE(node);
    } else if (node->type == IOTX_SERVICE_REQ_TYPE_GENERAL) {
#ifdef ALCS_ENABLED
        void *alcs_ctx = node->service_meta.general.alcs_ctx;
#endif
        list_del(&node->linked_list);
#ifdef ALCS_ENABLED
        if (alcs_ctx) {
            dm_server_free_context(alcs_ctx);
        }
#endif
        IMPL_LINKKIT_FREE(node);
    }

    if (ctx->service_list_num > 0) {
        ctx->service_list_num--;
    }

    return SUCCESS_RETURN;
}

static int _linkkit_service_list_destroy(void)
{
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();
    iotx_service_ctx_node_t *search_node, *temp;

    _linkkit_service_list_mutex_lock();
    list_for_each_entry_safe(search_node, temp, &ctx->downstream_service_list, linked_list, iotx_service_ctx_node_t) {
        if (search_node->type == IOTX_SERVICE_REQ_TYPE_RRPC ||
            search_node->type == IOTX_SERVICE_REQ_TYPE_GENERAL) {
            list_del(&search_node->linked_list);
            IMPL_LINKKIT_FREE(search_node);
        } else if (search_node->type == IOTX_SERVICE_REQ_TYPE_GENERAL) {
#ifdef ALCS_ENABLED
            void *alcs_ctx = search_node->service_meta.general.alcs_ctx;
#endif
            list_del(&search_node->linked_list);
#ifdef ALCS_ENABLED
            if (alcs_ctx) {
                dm_server_free_context(alcs_ctx);
            }
#endif
            IMPL_LINKKIT_FREE(search_node);
        }
    }
    ctx->service_list_num = 0;
    _linkkit_service_list_mutex_unlock();

    return SUCCESS_RETURN;
}

void iotx_linkkit_service_list_overtime_handle(void)
{
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();
    iotx_service_ctx_node_t *search_node, *temp;
    uint64_t current_time = HAL_UptimeMs();

    _linkkit_service_list_mutex_lock();
    list_for_each_entry_safe(search_node, temp, &ctx->downstream_service_list, linked_list, iotx_service_ctx_node_t) {
        if (current_time < search_node->ctime) {
            search_node->ctime = current_time;
        }
        if (current_time - search_node->ctime >= CONFIG_SERVICE_REQUEST_TIMEOUT) {
            _linkkit_service_list_delete(search_node);
        }
    }
    _linkkit_service_list_mutex_unlock();
}
#endif /* #if !defined(DEVICE_MODEL_RAWDATA_SOLO) */

#ifdef DEVICE_MODEL_GATEWAY
static void _iotx_linkkit_upstream_mutex_lock(void)
{
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();
    if (ctx->upstream_mutex) {
        HAL_MutexLock(ctx->upstream_mutex);
    }
}

static void _iotx_linkkit_upstream_mutex_unlock(void)
{
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();
    if (ctx->upstream_mutex) {
        HAL_MutexUnlock(ctx->upstream_mutex);
    }
}

static int _iotx_linkkit_upstream_sync_callback_list_insert(int msgid, void *semaphore,
        iotx_linkkit_upstream_sync_callback_node_t **node)
{
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();
    iotx_linkkit_upstream_sync_callback_node_t *search_node = NULL;

    list_for_each_entry(search_node, &ctx->upstream_sync_callback_list, linked_list,
                        iotx_linkkit_upstream_sync_callback_node_t) {
        if (search_node->msgid == msgid) {
            return STATE_DEV_MODEL_DUP_UPSTREAM_MSG;
        }
    }

    search_node = IMPL_LINKKIT_MALLOC(sizeof(iotx_linkkit_upstream_sync_callback_node_t));
    if (search_node == NULL) {
        return STATE_SYS_DEPEND_MALLOC;
    }
    memset(search_node, 0, sizeof(iotx_linkkit_upstream_sync_callback_node_t));
    search_node->msgid = msgid;
    search_node->semaphore = semaphore;
    INIT_LIST_HEAD(&search_node->linked_list);

    list_add(&search_node->linked_list, &ctx->upstream_sync_callback_list);
    iotx_state_event(ITE_STATE_DEV_MODEL, STATE_DEV_MODEL_SYNC_REQ_LIST, "sync list insert, msgid: %d", msgid);

    *node = search_node;
    return SUCCESS_RETURN;
}

static int _iotx_linkkit_upstream_sync_callback_list_remove(int msgid)
{
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();
    iotx_linkkit_upstream_sync_callback_node_t *search_node = NULL;

    list_for_each_entry(search_node, &ctx->upstream_sync_callback_list, linked_list,
                        iotx_linkkit_upstream_sync_callback_node_t) {
        if (search_node->msgid == msgid) {
            HAL_SemaphoreDestroy(search_node->semaphore);
            list_del(&search_node->linked_list);
            IMPL_LINKKIT_FREE(search_node);
            return SUCCESS_RETURN;
        }
    }

    iotx_state_event(ITE_STATE_DEV_MODEL, STATE_DEV_MODEL_UPSTREAM_REC_NOT_FOUND, "msgid: %d", msgid);
    return STATE_DEV_MODEL_UPSTREAM_REC_NOT_FOUND;
}

static int _iotx_linkkit_upstream_sync_callback_list_search(int msgid,
        iotx_linkkit_upstream_sync_callback_node_t **node)
{
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();
    iotx_linkkit_upstream_sync_callback_node_t *search_node = NULL;

    list_for_each_entry(search_node, &ctx->upstream_sync_callback_list, linked_list,
                        iotx_linkkit_upstream_sync_callback_node_t) {
        if (search_node->msgid == msgid) {
            *node = search_node;
            return SUCCESS_RETURN;
        }
    }

    iotx_state_event(ITE_STATE_DEV_MODEL, STATE_DEV_MODEL_UPSTREAM_REC_NOT_FOUND, "msgid: %d", msgid);
    return STATE_DEV_MODEL_UPSTREAM_REC_NOT_FOUND;
}

static void _iotx_linkkit_upstream_sync_callback_list_destroy(void)
{
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();
    iotx_linkkit_upstream_sync_callback_node_t *search_node = NULL, *next_node = NULL;

    list_for_each_entry_safe(search_node, next_node, &ctx->upstream_sync_callback_list, linked_list,
                             iotx_linkkit_upstream_sync_callback_node_t) {
        list_del(&search_node->linked_list);
        HAL_SemaphoreDestroy(search_node->semaphore);
        IMPL_LINKKIT_FREE(search_node);
    }
}


static void _iotx_linkkit_upstream_callback_remove(int msgid, int code)
{
    int res = 0;
    iotx_linkkit_upstream_sync_callback_node_t *sync_node = NULL;
    res = _iotx_linkkit_upstream_sync_callback_list_search(msgid, &sync_node);
    if (res == SUCCESS_RETURN) {
        sync_node->code = (code == IOTX_DM_ERR_CODE_SUCCESS) ? (SUCCESS_RETURN) : (STATE_DEV_MODEL_REFUSED_BY_CLOUD);
        iotx_state_event(ITE_STATE_DEV_MODEL, STATE_DEV_MODEL_SYNC_REQ_LIST, "sync list remove, msgid: %d", msgid);
        HAL_SemaphorePost(sync_node->semaphore);
    }
}
#endif

#ifdef LOG_REPORT_TO_CLOUD
    int  report_sample = 0;
#endif

static void _iotx_linkkit_event_callback(iotx_dm_event_types_t type, char *payload)
{
    int res = 0;
    void *callback;
#ifdef LOG_REPORT_TO_CLOUD
    lite_cjson_t msg_id;
#endif
    lite_cjson_t lite, lite_item_id, lite_item_devid, lite_item_serviceid, lite_item_payload, lite_item_ctx;
    lite_cjson_t lite_item_code, lite_item_eventid, lite_item_utc, lite_item_rrpcid, lite_item_topo;
    lite_cjson_t lite_item_pk, lite_item_time;
    lite_cjson_t lite_item_module,  lite_item_version, lite_item_configid, lite_item_configsize, lite_item_gettype,
                 lite_item_sign,
                 lite_item_signmethod, lite_item_url, lite_item_data, lite_item_message;

    iotx_state_event(ITE_STATE_DEV_MODEL, STATE_DEV_MODEL_ALINK_PROT_EVENT, "alink event type: %d", type);

    if (payload) {
        res = dm_utils_json_parse(payload, strlen(payload), cJSON_Invalid, &lite);
        if (res != SUCCESS_RETURN) {
            return;
        }
#ifdef LOG_REPORT_TO_CLOUD
        dm_utils_json_object_item(&lite, "msgid", 5, cJSON_Invalid, &msg_id);
#endif

        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_ID, strlen(IOTX_LINKKIT_KEY_ID), cJSON_Invalid, &lite_item_id);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_DEVID, strlen(IOTX_LINKKIT_KEY_DEVID), cJSON_Invalid,
                                  &lite_item_devid);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_SERVICEID, strlen(IOTX_LINKKIT_KEY_SERVICEID), cJSON_Invalid,
                                  &lite_item_serviceid);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_PAYLOAD, strlen(IOTX_LINKKIT_KEY_PAYLOAD), cJSON_Invalid,
                                  &lite_item_payload);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_CTX, strlen(IOTX_LINKKIT_KEY_CTX), cJSON_Invalid, &lite_item_ctx);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_CODE, strlen(IOTX_LINKKIT_KEY_CODE), cJSON_Invalid, &lite_item_code);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_EVENTID, strlen(IOTX_LINKKIT_KEY_EVENTID), cJSON_Invalid,
                                  &lite_item_eventid);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_UTC, strlen(IOTX_LINKKIT_KEY_UTC), cJSON_Invalid, &lite_item_utc);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_RRPCID, strlen(IOTX_LINKKIT_KEY_RRPCID), cJSON_Invalid,
                                  &lite_item_rrpcid);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_TOPO, strlen(IOTX_LINKKIT_KEY_TOPO), cJSON_Invalid,
                                  &lite_item_topo);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_PRODUCT_KEY, strlen(IOTX_LINKKIT_KEY_PRODUCT_KEY), cJSON_Invalid,
                                  &lite_item_pk);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_TIME, strlen(IOTX_LINKKIT_KEY_TIME), cJSON_Invalid,
                                  &lite_item_time);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_VERSION, strlen(IOTX_LINKKIT_KEY_VERSION), cJSON_Invalid,
                                  &lite_item_version);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_MODULE_ID, strlen(IOTX_LINKKIT_KEY_MODULE_ID), cJSON_Invalid,
                                  &lite_item_module);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_CONFIG_ID, strlen(IOTX_LINKKIT_KEY_CONFIG_ID), cJSON_Invalid,
                                  &lite_item_configid);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_CONFIG_SIZE, strlen(IOTX_LINKKIT_KEY_CONFIG_SIZE), cJSON_Invalid,
                                  &lite_item_configsize);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_GET_TYPE, strlen(IOTX_LINKKIT_KEY_GET_TYPE), cJSON_Invalid,
                                  &lite_item_gettype);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_SIGN, strlen(IOTX_LINKKIT_KEY_SIGN), cJSON_Invalid,
                                  &lite_item_sign);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_SIGN_METHOD, strlen(IOTX_LINKKIT_KEY_SIGN_METHOD), cJSON_Invalid,
                                  &lite_item_signmethod);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_URL, strlen(IOTX_LINKKIT_KEY_URL), cJSON_Invalid,
                                  &lite_item_url);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_DATA, strlen(IOTX_LINKKIT_KEY_DATA), cJSON_Invalid,
                                  &lite_item_data);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_MESSAGE, strlen(IOTX_LINKKIT_KEY_MESSAGE), cJSON_Invalid,
                                  &lite_item_message);
    }

    switch (type) {
        case IOTX_DM_EVENT_CLOUD_CONNECTED: {
            callback = iotx_event_callback(ITE_CONNECT_SUCC);
            if (callback) {
                ((int (*)(void))callback)();
            }
        }
        break;
        case IOTX_DM_EVENT_CLOUD_DISCONNECT: {
            callback = iotx_event_callback(ITE_DISCONNECTED);
            if (callback) {
                ((int (*)(void))callback)();
            }
        }
        break;
        case IOTX_DM_EVENT_INITIALIZED: {
            if (payload == NULL || lite_item_devid.type != cJSON_Number) {
                return;
            }

            iotx_state_event(ITE_STATE_DEV_MODEL, STATE_DEV_MODEL_ALINK_PROT_EVENT, "dev init, devid: %d",
                             lite_item_devid.value_int);

            callback = iotx_event_callback(ITE_INITIALIZE_COMPLETED);
            if (callback) {
                ((int (*)(const int))callback)(lite_item_devid.value_int);
            }
        }
        break;
        case IOTX_DM_EVENT_MODEL_DOWN_RAW: {
            int raw_data_len = 0;
            unsigned char *raw_data = NULL;

            if (payload == NULL || lite_item_devid.type != cJSON_Number || lite_item_payload.type != cJSON_String) {
                return;
            }

            iotx_state_event(ITE_STATE_DEV_MODEL, STATE_DEV_MODEL_ALINK_PROT_EVENT, "down raw, devid: %d, raw: %.*s",
                             lite_item_devid.value_int,
                             lite_item_payload.value_length, lite_item_payload.value);

            raw_data_len = lite_item_payload.value_length / 2;
            raw_data = IMPL_LINKKIT_MALLOC(raw_data_len);
            if (raw_data == NULL) {
                iotx_state_event(ITE_STATE_DEV_MODEL, STATE_SYS_DEPEND_MALLOC, NULL);
                return;
            }
            LITE_hexstr_convert(lite_item_payload.value, lite_item_payload.value_length, raw_data, raw_data_len);

            HEXDUMP_DEBUG(raw_data, raw_data_len);
            callback = iotx_event_callback(ITE_RAWDATA_ARRIVED);
            if (callback) {
                ((int (*)(const int, const unsigned char *, const int))callback)(lite_item_devid.value_int, raw_data, raw_data_len);
            }

            IMPL_LINKKIT_FREE(raw_data);
        }
        break;
        case IOTX_DM_EVENT_MODEL_UP_RAW_REPLY: {
            int raw_data_len = 0;
            unsigned char *raw_data = NULL;

            if (payload == NULL || lite_item_devid.type != cJSON_Number || lite_item_payload.type != cJSON_String) {
                return;
            }

            iotx_state_event(ITE_STATE_DEV_MODEL, STATE_DEV_MODEL_ALINK_PROT_EVENT, "up raw reply, devid: %d, raw: %.*s",
                             lite_item_devid.value_int,
                             lite_item_payload.value_length, lite_item_payload.value);

            raw_data_len = lite_item_payload.value_length / 2;
            raw_data = IMPL_LINKKIT_MALLOC(raw_data_len);
            if (raw_data == NULL) {
                iotx_state_event(ITE_STATE_DEV_MODEL, STATE_SYS_DEPEND_MALLOC, NULL);
                return;
            }
            memset(raw_data, 0, raw_data_len);
            LITE_hexstr_convert(lite_item_payload.value, lite_item_payload.value_length, raw_data, raw_data_len);

            HEXDUMP_DEBUG(raw_data, raw_data_len);

            callback = iotx_event_callback(ITE_RAWDATA_ARRIVED);
            if (callback) {
                ((int (*)(const int, const unsigned char *, const int))callback)(lite_item_devid.value_int, raw_data, raw_data_len);
            }

            IMPL_LINKKIT_FREE(raw_data);
        }
        break;
#if !defined(DEVICE_MODEL_RAWDATA_SOLO)
        case IOTX_DM_EVENT_THING_SERVICE_REQUEST: {
            int response_len = 0;
            char *request = NULL, *response = NULL;

            uintptr_t property_get_ctx_num = 0;
            void *property_get_ctx = NULL;

            if (payload == NULL || lite_item_id.type != cJSON_String || lite_item_devid.type != cJSON_Number ||
                lite_item_serviceid.type != cJSON_String || lite_item_payload.type != cJSON_Object) {
                return;
            }

            iotx_state_event(ITE_STATE_DEV_MODEL, STATE_DEV_MODEL_ALINK_PROT_EVENT,
                             "service req, msgid: %.*s, devid: %d, serviceid: %.*s, payload: %.*s",
                             lite_item_id.value_length, lite_item_id.value,
                             lite_item_devid.value_int,
                             lite_item_serviceid.value_length, lite_item_serviceid.value,
                             lite_item_payload.value_length, lite_item_payload.value);

            LITE_hexstr_convert(lite_item_ctx.value, lite_item_ctx.value_length, (unsigned char *)&property_get_ctx_num,
                                sizeof(uintptr_t));
            property_get_ctx = (void *)property_get_ctx_num;

            request = IMPL_LINKKIT_MALLOC(lite_item_payload.value_length + 1);
            if (request == NULL) {
                iotx_state_event(ITE_STATE_DEV_MODEL, STATE_SYS_DEPEND_MALLOC, NULL);
                return;
            }
            memset(request, 0, lite_item_payload.value_length + 1);
            memcpy(request, lite_item_payload.value, lite_item_payload.value_length);

            callback = iotx_event_callback(ITE_SERVICE_REQUEST);
            if (callback) {
                res = ((int (*)(const int, const char *, const int, const char *, const int, char **,
                                int *))callback)(lite_item_devid.value_int, lite_item_serviceid.value,
                                                 lite_item_serviceid.value_length, request, lite_item_payload.value_length, &response, &response_len);
                if (response != NULL && response_len > 0) {
                    /* service response exist */
                    iotx_dm_error_code_t code = (res == 0) ? (IOTX_DM_ERR_CODE_SUCCESS) : (IOTX_DM_ERR_CODE_REQUEST_ERROR);
                    iotx_dm_send_service_response(lite_item_devid.value_int, lite_item_id.value, lite_item_id.value_length, code,
                                                  lite_item_serviceid.value,
                                                  lite_item_serviceid.value_length,
                                                  response, response_len, property_get_ctx);
                    HAL_Free(response);
                }
            }

            callback = iotx_event_callback(ITE_SERVICE_REQUEST_EXT);
            if (callback) {
                void *service_ctx = NULL;
                _linkkit_service_list_insert(IOTX_SERVICE_REQ_TYPE_GENERAL, lite_item_id.value, NULL, 0, property_get_ctx,
                                             &service_ctx);
                if (service_ctx != NULL) {
                    res = ((int (*)(int, const char *, int, const char *, int, const char *, int,
                                    void *))callback)(lite_item_devid.value_int,
                                                      lite_item_serviceid.value,
                                                      lite_item_serviceid.value_length, lite_item_id.value, lite_item_id.value_length, request,
                                                      lite_item_payload.value_length, service_ctx);
                }
            }

            IMPL_LINKKIT_FREE(request);
        }
        break;
        case IOTX_DM_EVENT_PROPERTY_SET: {
            char *property_payload = NULL;

            if (payload == NULL || lite_item_devid.type != cJSON_Number || lite_item_payload.type != cJSON_Object) {
                return;
            }

            iotx_state_event(ITE_STATE_DEV_MODEL, STATE_DEV_MODEL_ALINK_PROT_EVENT, "property set, devid: %d, payload: %.*s",
                             lite_item_devid.value_int,
                             lite_item_payload.value_length, lite_item_payload.value);

            property_payload = IMPL_LINKKIT_MALLOC(lite_item_payload.value_length + 1);
            if (property_payload == NULL) {
                iotx_state_event(ITE_STATE_DEV_MODEL, STATE_SYS_DEPEND_MALLOC, NULL);
                return;
            }
            memset(property_payload, 0, lite_item_payload.value_length + 1);
            memcpy(property_payload, lite_item_payload.value, lite_item_payload.value_length);
#ifdef LOG_REPORT_TO_CLOUD
            if (SUCCESS_RETURN == check_target_msg(msg_id.value, msg_id.value_length)) {
                report_sample = 1;
                send_permance_info(msg_id.value, msg_id.value_length, "3", 1);
            }
#endif
            callback = iotx_event_callback(ITE_PROPERTY_SET);
            if (callback) {
                ((int (*)(const int, const char *, const int))callback)(lite_item_devid.value_int, property_payload,
                        lite_item_payload.value_length);
            }
#ifdef LOG_REPORT_TO_CLOUD
            if (1 == report_sample) {
                send_permance_info(NULL, 0, "5", 2);
                report_sample = 0;
            }
#endif

            IMPL_LINKKIT_FREE(property_payload);
        }
        break;
#ifdef DEVICE_MODEL_SHADOW
        case IOTX_DM_EVENT_PROPERTY_DESIRED_GET_REPLY: {
            char *property_data = NULL;
            lite_cjson_t lite_item_data;

            memset(&lite_item_data, 0, sizeof(lite_cjson_t));
            dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_DATA, strlen(IOTX_LINKKIT_KEY_DATA), cJSON_Invalid,
                                      &lite_item_data);
            if (payload == NULL || lite_item_data.type != cJSON_Object) {
                return;
            }

            iotx_state_event(ITE_STATE_DEV_MODEL, STATE_DEV_MODEL_ALINK_PROT_EVENT, "desired get, data: %.*s",
                             lite_item_data.value_length, lite_item_data.value);

            property_data = IMPL_LINKKIT_MALLOC(lite_item_data.value_length + 1);
            if (property_data == NULL) {
                iotx_state_event(ITE_STATE_DEV_MODEL, STATE_SYS_DEPEND_MALLOC, NULL);
                return;
            }
            memset(property_data, 0, lite_item_data.value_length + 1);
            memcpy(property_data, lite_item_data.value, lite_item_data.value_length);

            callback = iotx_event_callback(ITE_PROPERTY_DESIRED_GET_REPLY);
            if (callback) {
                ((int (*)(const char *, const int))callback)(property_data,
                        lite_item_data.value_length);
            }

            IMPL_LINKKIT_FREE(property_data);
        }
        break;
#endif
        case IOTX_DM_EVENT_PROPERTY_GET: {
            int response_len = 0;
            char *request = NULL, *response = NULL;
            uintptr_t property_get_ctx_num = 0;
            void *property_get_ctx = NULL;

            if (payload == NULL || lite_item_id.type != cJSON_String || lite_item_devid.type != cJSON_Number ||
                lite_item_payload.type != cJSON_Array) {
                return;
            }

            iotx_state_event(ITE_STATE_DEV_MODEL, STATE_DEV_MODEL_ALINK_PROT_EVENT,
                             "property get, msgid: %.*s, devid: %d, payload: %.*s",
                             lite_item_id.value_length, lite_item_id.value,
                             lite_item_devid.value_int,
                             lite_item_payload.value_length, lite_item_payload.value);

            LITE_hexstr_convert(lite_item_ctx.value, lite_item_ctx.value_length, (unsigned char *)&property_get_ctx_num,
                                sizeof(uintptr_t));
            property_get_ctx = (void *)property_get_ctx_num;

            request = IMPL_LINKKIT_MALLOC(lite_item_payload.value_length + 1);
            if (request == NULL) {
                iotx_state_event(ITE_STATE_DEV_MODEL, STATE_SYS_DEPEND_MALLOC, NULL);
                return;
            }
            memset(request, 0, lite_item_payload.value_length + 1);
            memcpy(request, lite_item_payload.value, lite_item_payload.value_length);

            callback = iotx_event_callback(ITE_PROPERTY_GET);
            if (callback) {
                res = ((int (*)(const int, const char *, const int, char **, int *))callback)(lite_item_devid.value_int, request,
                        lite_item_payload.value_length, &response, &response_len);

                if (response != NULL && response_len > 0) {
                    /* property get response exist */
                    iotx_dm_error_code_t code = (res == 0) ? (IOTX_DM_ERR_CODE_SUCCESS) : (IOTX_DM_ERR_CODE_REQUEST_ERROR);
                    iotx_dm_send_property_get_response(lite_item_devid.value_int, lite_item_id.value, lite_item_id.value_length, code,
                                                       response, response_len, property_get_ctx);
                    HAL_Free(response);
                }
            }

            IMPL_LINKKIT_FREE(request);
        }
        break;
        case IOTX_DM_EVENT_EVENT_PROPERTY_POST_REPLY:
        case IOTX_DM_EVENT_DEVICEINFO_UPDATE_REPLY:
#ifdef DEVICE_MODEL_SHADOW
        case IOTX_DM_EVENT_PROPERTY_DESIRED_DELETE_REPLY:
#endif
        case IOTX_DM_EVENT_DEVICEINFO_DELETE_REPLY: {
            char *user_payload = NULL;
            int user_payload_length = 0;

            if (payload == NULL || lite_item_id.type != cJSON_Number || lite_item_code.type != cJSON_Number
                || lite_item_devid.type != cJSON_Number) {
                return;
            }
            iotx_state_event(ITE_STATE_DEV_MODEL, STATE_DEV_MODEL_ALINK_PROT_EVENT, "deviceinfo delete post reply, code: %d",
                             lite_item_code.value_int);

            if (lite_item_payload.type == cJSON_Object && lite_item_payload.value_length > 0) {
                user_payload = IMPL_LINKKIT_MALLOC(lite_item_payload.value_length + 1);
                if (user_payload == NULL) {
                    iotx_state_event(ITE_STATE_DEV_MODEL, STATE_SYS_DEPEND_MALLOC, NULL);
                    return;
                }
                memset(user_payload, 0, lite_item_payload.value_length + 1);
                memcpy(user_payload, lite_item_payload.value, lite_item_payload.value_length);
                user_payload_length = lite_item_payload.value_length;
            }

            callback = iotx_event_callback(ITE_REPORT_REPLY);
            if (callback) {
                ((int (*)(const int, const int, const int, const char *, const int))callback)(lite_item_devid.value_int,
                        lite_item_id.value_int, lite_item_code.value_int, user_payload,
                        user_payload_length);
            }

            if (user_payload) {
                IMPL_LINKKIT_FREE(user_payload);
            }
        }
        break;
        case IOTX_DM_EVENT_EVENT_SPECIFIC_POST_REPLY: {
            char *user_eventid = NULL;
            char *user_payload = NULL;

            if (payload == NULL || lite_item_id.type != cJSON_Number || lite_item_code.type != cJSON_Number ||
                lite_item_devid.type != cJSON_Number || lite_item_eventid.type != cJSON_String
                || lite_item_payload.type != cJSON_String) {
                return;
            }

            iotx_state_event(ITE_STATE_DEV_MODEL, STATE_DEV_MODEL_ALINK_PROT_EVENT, "event post reply, eventID: %.*s",
                             lite_item_eventid.value_length, lite_item_eventid.value);

            user_eventid = IMPL_LINKKIT_MALLOC(lite_item_eventid.value_length + 1);
            if (user_eventid == NULL) {
                iotx_state_event(ITE_STATE_DEV_MODEL, STATE_SYS_DEPEND_MALLOC, NULL);
                return;
            }
            memset(user_eventid, 0, lite_item_eventid.value_length + 1);
            memcpy(user_eventid, lite_item_eventid.value, lite_item_eventid.value_length);

            user_payload = IMPL_LINKKIT_MALLOC(lite_item_payload.value_length + 1);
            if (user_payload == NULL) {
                iotx_state_event(ITE_STATE_DEV_MODEL, STATE_SYS_DEPEND_MALLOC, NULL);
                IMPL_LINKKIT_FREE(user_eventid);
                return;
            }
            memset(user_payload, 0, lite_item_payload.value_length + 1);
            memcpy(user_payload, lite_item_payload.value, lite_item_payload.value_length);


            callback = iotx_event_callback(ITE_TRIGGER_EVENT_REPLY);
            if (callback) {
                ((int (*)(const int, const int, const int, const char *, const int, const char *,
                          const int))callback)(lite_item_devid.value_int,
                                               lite_item_id.value_int, lite_item_code.value_int,
                                               user_eventid, lite_item_eventid.value_length, user_payload, lite_item_payload.value_length);
            }

            IMPL_LINKKIT_FREE(user_eventid);
            IMPL_LINKKIT_FREE(user_payload);
        }
        break;
        case IOTX_DM_EVENT_NTP_RESPONSE: {
            char *utc_payload = NULL;

            if (payload == NULL || lite_item_utc.type != cJSON_String) {
                return;
            }

            iotx_state_event(ITE_STATE_DEV_MODEL, STATE_DEV_MODEL_ALINK_PROT_EVENT, "utc: %.*s",
                             lite_item_utc.value_length, lite_item_utc.value);

            utc_payload = IMPL_LINKKIT_MALLOC(lite_item_utc.value_length + 1);
            if (utc_payload == NULL) {
                iotx_state_event(ITE_STATE_DEV_MODEL, STATE_SYS_DEPEND_MALLOC, NULL);
                return;
            }
            memset(utc_payload, 0, lite_item_utc.value_length + 1);
            memcpy(utc_payload, lite_item_utc.value, lite_item_utc.value_length);

            callback = iotx_event_callback(ITE_TIMESTAMP_REPLY);
            if (callback) {
                ((int (*)(const char *))callback)(utc_payload);
            }

            IMPL_LINKKIT_FREE(utc_payload);
        }
        break;
        case IOTX_DM_EVENT_RRPC_REQUEST: {
            int rrpc_response_len = 0;
            char *rrpc_request = NULL, *rrpc_response = NULL;

            if (payload == NULL || lite_item_id.type != cJSON_String || lite_item_devid.type != cJSON_Number ||
                lite_item_serviceid.type != cJSON_String || lite_item_rrpcid.type != cJSON_String
                || lite_item_payload.type != cJSON_Object) {
                return;
            }

            iotx_state_event(ITE_STATE_DEV_MODEL, STATE_DEV_MODEL_ALINK_PROT_EVENT, "rrpc request received, payload: %.*s",
                             lite_item_payload.value_length, lite_item_payload.value);

            rrpc_request = IMPL_LINKKIT_MALLOC(lite_item_payload.value_length + 1);
            if (rrpc_request == NULL) {
                iotx_state_event(ITE_STATE_DEV_MODEL, STATE_SYS_DEPEND_MALLOC, NULL);
                return;
            }
            memset(rrpc_request, 0, lite_item_payload.value_length + 1);
            memcpy(rrpc_request, lite_item_payload.value, lite_item_payload.value_length);

            callback = iotx_event_callback(ITE_SERVICE_REQUEST);
            if (callback) {
                res = ((int (*)(const int, const char *, const int, const char *, const int, char **,
                                int *))callback)(lite_item_devid.value_int, lite_item_serviceid.value,
                                                 lite_item_serviceid.value_length,
                                                 rrpc_request, lite_item_payload.value_length, &rrpc_response, &rrpc_response_len);
                if (rrpc_response != NULL && rrpc_response_len > 0) {
                    iotx_dm_error_code_t code = (res == 0) ? (IOTX_DM_ERR_CODE_SUCCESS) : (IOTX_DM_ERR_CODE_REQUEST_ERROR);
                    iotx_dm_send_rrpc_response(lite_item_devid.value_int, lite_item_id.value, lite_item_id.value_length, code,
                                               lite_item_rrpcid.value,
                                               lite_item_rrpcid.value_length,
                                               rrpc_response, rrpc_response_len);
                    HAL_Free(rrpc_response);
                }
            }

            callback = iotx_event_callback(ITE_SERVICE_REQUEST_EXT);
            if (callback) {
                void *service_ctx = NULL;
                _linkkit_service_list_insert(IOTX_SERVICE_REQ_TYPE_RRPC, lite_item_id.value, lite_item_rrpcid.value,
                                             lite_item_rrpcid.value_length, NULL, &service_ctx);
                if (service_ctx != NULL) {
                    res = ((int (*)(int, const char *, int, const char *, int, const char *, int,
                                    void *))callback)(lite_item_devid.value_int,
                                                      lite_item_serviceid.value,
                                                      lite_item_serviceid.value_length, lite_item_id.value, lite_item_id.value_length, rrpc_request,
                                                      lite_item_payload.value_length, service_ctx);
                }
            }

            IMPL_LINKKIT_FREE(rrpc_request);
        }
        break;
#endif
        case IOTX_DM_EVENT_FOTA_NEW_FIRMWARE: {
            char *version = NULL;
            char *module = NULL;

            if (payload == NULL || lite_item_version.type != cJSON_String) {
                return;
            }

            if (payload != NULL && lite_item_module.type == cJSON_String) {
                module = IMPL_LINKKIT_MALLOC(lite_item_module.value_length + 1);
                if (module == NULL) {
                    return;
                }
                memset(module, 0, lite_item_module.value_length + 1);
                memcpy(module, lite_item_module.value, lite_item_module.value_length);
            }

            iotx_state_event(ITE_STATE_DEV_MODEL, STATE_DEV_MODEL_ALINK_PROT_EVENT, "new fota information received, %.*s",
                             lite_item_version.value_length, lite_item_version.value);

            version = IMPL_LINKKIT_MALLOC(lite_item_version.value_length + 1);
            if (version == NULL) {
                return;
            }
            memset(version, 0, lite_item_version.value_length + 1);
            memcpy(version, lite_item_version.value, lite_item_version.value_length);

            if (NULL == module) {
                callback = iotx_event_callback(ITE_FOTA);
                if (callback) {
                    ((int (*)(const int, const char *))callback)(0, version);
                }
            } else {
                callback = iotx_event_callback(ITE_FOTA_MODULE);
                if (callback) {
                    ((int (*)(const int, const char *, const char *))callback)(0, version, module);
                }
            }

            if (version) {
                IMPL_LINKKIT_FREE(version);
            }

            if (module) {
                IMPL_LINKKIT_FREE(module);
            }


        }
        break;
        case IOTX_DM_EVENT_COTA_NEW_CONFIG: {
            char *config_id = NULL, *get_type = NULL, *sign = NULL, *sign_method = NULL, *url = NULL;

            if (payload == NULL || lite_item_configid.type != cJSON_String || lite_item_configsize.type != cJSON_Number ||
                lite_item_gettype.type != cJSON_String || lite_item_sign.type != cJSON_String
                || lite_item_signmethod.type != cJSON_String ||
                lite_item_url.type != cJSON_String) {
                return;
            }

            iotx_state_event(ITE_STATE_DEV_MODEL, STATE_DEV_MODEL_ALINK_PROT_EVENT, "new cota information received");

            _impl_copy(lite_item_configid.value, lite_item_configid.value_length, (void **)&config_id,
                       lite_item_configid.value_length + 1);
            _impl_copy(lite_item_gettype.value, lite_item_gettype.value_length, (void **)&get_type,
                       lite_item_gettype.value_length + 1);
            _impl_copy(lite_item_sign.value, lite_item_sign.value_length, (void **)&sign, lite_item_sign.value_length + 1);
            _impl_copy(lite_item_signmethod.value, lite_item_signmethod.value_length, (void **)&sign_method,
                       lite_item_signmethod.value_length + 1);
            _impl_copy(lite_item_url.value, lite_item_url.value_length, (void **)&url, lite_item_url.value_length + 1);

            if (config_id == NULL || get_type == NULL || sign == NULL || sign_method == NULL || url == NULL) {
                if (config_id) {
                    IMPL_LINKKIT_FREE(config_id);
                }
                if (get_type) {
                    IMPL_LINKKIT_FREE(get_type);
                }
                if (sign) {
                    IMPL_LINKKIT_FREE(sign);
                }
                if (sign_method) {
                    IMPL_LINKKIT_FREE(sign_method);
                }
                if (url) {
                    IMPL_LINKKIT_FREE(url);
                }
                return;
            }

            callback = iotx_event_callback(ITE_COTA);
            if (callback) {
                ((int (*)(const int, const char *, int, const char *, const char *, const char *, const char *))callback)(0, config_id,
                        lite_item_configsize.value_int, get_type, sign, sign_method, url);
            }

            if (config_id) {
                IMPL_LINKKIT_FREE(config_id);
            }
            if (get_type) {
                IMPL_LINKKIT_FREE(get_type);
            }
            if (sign) {
                IMPL_LINKKIT_FREE(sign);
            }
            if (sign_method) {
                IMPL_LINKKIT_FREE(sign_method);
            }
            if (url) {
                IMPL_LINKKIT_FREE(url);
            }
        }
        break;
        case IOTX_DM_EVENT_CLOUD_ERROR: {
            char *err_data = NULL;
            char *err_detail = NULL;

            if (payload == NULL) {
                return;
            }
            if (payload == NULL || lite_item_code.type != cJSON_Number) {
                return;
            }

            err_data = IMPL_LINKKIT_MALLOC(lite_item_data.value_length + 1);
            if (err_data == NULL) {
                iotx_state_event(ITE_STATE_DEV_MODEL, STATE_SYS_DEPEND_MALLOC, NULL);
                return;
            }

            memset(err_data, 0, lite_item_data.value_length + 1);
            memcpy(err_data, lite_item_data.value, lite_item_data.value_length);

            err_detail = IMPL_LINKKIT_MALLOC(lite_item_message.value_length + 1);
            if (err_detail == NULL) {
                IMPL_LINKKIT_FREE(err_data);
                iotx_state_event(ITE_STATE_DEV_MODEL, STATE_SYS_DEPEND_MALLOC, NULL);
                return;
            }

            memset(err_detail, 0, lite_item_message.value_length + 1);
            memcpy(err_detail, lite_item_message.value, lite_item_message.value_length);

            callback = iotx_event_callback(ITE_CLOUD_ERROR);
            if (callback) {
                ((int (*)(int, const char *, const char *))callback)(lite_item_code.value_int, err_data, err_detail);
            }
            IMPL_LINKKIT_FREE(err_data);
            IMPL_LINKKIT_FREE(err_detail);
        }
        break;
#ifdef DEVICE_MODEL_GATEWAY
        case IOTX_DM_EVENT_TOPO_GET_REPLY: {
            char *topo_list = NULL;

            if (payload == NULL || lite_item_id.type != cJSON_Number || lite_item_devid.type != cJSON_Number ||
                lite_item_code.type != cJSON_Number || lite_item_topo.type != cJSON_Array) {
                return;
            }

            iotx_state_event(ITE_STATE_DEV_MODEL, STATE_DEV_MODEL_ALINK_PROT_EVENT,
                             "topic get reply, msgid = %d, devid = %d, code = %d",
                             lite_item_id.value_int,
                             lite_item_devid.value_int,
                             lite_item_code.value_int);

            topo_list = IMPL_LINKKIT_MALLOC(lite_item_topo.value_length + 1);
            if (topo_list == NULL) {
                iotx_state_event(ITE_STATE_DEV_MODEL, STATE_SYS_DEPEND_MALLOC, NULL);
                return;
            }
            memset(topo_list, 0, lite_item_topo.value_length + 1);
            memcpy(topo_list, lite_item_topo.value, lite_item_topo.value_length);

            callback = iotx_event_callback(ITE_TOPOLIST_REPLY);
            if (callback) {
                ((int (*)(const int, const int, const int, const char *, const int))callback)(lite_item_devid.value_int,
                        lite_item_id.value_int,
                        lite_item_code.value_int, topo_list, lite_item_topo.value_length);
            }

            IMPL_LINKKIT_FREE(topo_list);
        }
        break;
        case IOTX_DM_EVENT_TOPO_DELETE_REPLY:
        case IOTX_DM_EVENT_TOPO_ADD_REPLY:
        case IOTX_DM_EVENT_SUBDEV_REGISTER_REPLY:
        case IOTX_DM_EVENT_PROXY_PRODUCT_REGISTER_REPLY:
        case IOTX_DM_EVENT_COMBINE_LOGIN_REPLY:
        case IOTX_DM_EVENT_COMBINE_LOGOUT_REPLY: {
            if (payload == NULL || lite_item_id.type != cJSON_Number || lite_item_devid.type != cJSON_Number ||
                lite_item_code.type != cJSON_Number) {
                return;
            }
            iotx_state_event(ITE_STATE_DEV_MODEL, STATE_DEV_MODEL_ALINK_PROT_EVENT,
                             "login/logout reply, msgid = %d, devid = %d, code = %d",
                             lite_item_id.value_int,
                             lite_item_devid.value_int,
                             lite_item_code.value_int);

            _iotx_linkkit_upstream_mutex_lock();
            _iotx_linkkit_upstream_callback_remove(lite_item_id.value_int, lite_item_code.value_int);
            _iotx_linkkit_upstream_mutex_unlock();
        }
        break;
        case IOTX_DM_EVENT_GATEWAY_PERMIT: {
            char *product_key = "";

            if (payload == NULL || lite_item_time.type != cJSON_Number) {
                return;
            }

            if (lite_item_pk.type == cJSON_String) {
                product_key = IMPL_LINKKIT_MALLOC(lite_item_pk.value_length + 1);
                if (product_key == NULL) {
                    iotx_state_event(ITE_STATE_DEV_MODEL, STATE_SYS_DEPEND_MALLOC, NULL);
                    return;
                }
                memset(product_key, 0, lite_item_pk.value_length + 1);
                memcpy(product_key, lite_item_pk.value, lite_item_pk.value_length);
            }

            iotx_state_event(ITE_STATE_DEV_MODEL, STATE_DEV_MODEL_ALINK_PROT_EVENT, "permitJoin recv, pk = %.*s, time = %d",
                             lite_item_pk.value_length, lite_item_pk.value,
                             lite_item_time.value_int);

            callback = iotx_event_callback(ITE_PERMIT_JOIN);
            if (callback) {
                ((int (*)(const char *, int))callback)((const char *)product_key, (const int)lite_item_time.value_int);
            }

            if (lite_item_pk.type == cJSON_String) {
                IMPL_LINKKIT_FREE(product_key);
            }
        }
        break;
#endif
        default: {
        }
        break;
    }
}

static int _iotx_linkkit_master_open(iotx_linkkit_dev_meta_info_t *meta_info)
{
    int res = 0;
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();

    if (ctx->is_opened) {
        return STATE_DEV_MODEL_MASTER_ALREADY_OPEN;
    }
    ctx->is_opened = 1;

    IOT_Ioctl(IOTX_IOCTL_SET_PRODUCT_KEY, meta_info->product_key);
    IOT_Ioctl(IOTX_IOCTL_SET_PRODUCT_SECRET, meta_info->product_secret);
    IOT_Ioctl(IOTX_IOCTL_SET_DEVICE_NAME, meta_info->device_name);
    IOT_Ioctl(IOTX_IOCTL_SET_DEVICE_SECRET, meta_info->device_secret);

    /* Create Mutex */
    ctx->mutex = HAL_MutexCreate();
    if (ctx->mutex == NULL) {
        ctx->is_opened = 0;
        return STATE_SYS_DEPEND_MUTEX_CREATE;
    }

#ifdef DEVICE_MODEL_GATEWAY
    ctx->upstream_mutex = HAL_MutexCreate();
    if (ctx->upstream_mutex == NULL) {
        HAL_MutexDestroy(ctx->mutex);
        ctx->is_opened = 0;
        return STATE_SYS_DEPEND_MUTEX_CREATE;
    }
#endif

#if !defined(DEVICE_MODEL_RAWDATA_SOLO)
    ctx->service_list_mutex = HAL_MutexCreate();
    if (ctx->service_list_mutex == NULL) {
        HAL_MutexDestroy(ctx->mutex);
        HAL_MutexDestroy(ctx->upstream_mutex);
        ctx->is_opened = 0;
        return STATE_SYS_DEPEND_MUTEX_CREATE;
    }
    ctx->service_list_num = 0;
#endif /* #if !defined(DEVICE_MODEL_RAWDATA_SOLO) */

    res = iotx_dm_open();
    if (res != SUCCESS_RETURN) {
#ifdef DEVICE_MODEL_GATEWAY
        HAL_MutexDestroy(ctx->upstream_mutex);
#endif
#if !defined(DEVICE_MODEL_RAWDATA_SOLO)
        HAL_MutexDestroy(ctx->service_list_mutex);
#endif /* #if !defined(DEVICE_MODEL_RAWDATA_SOLO) */
        HAL_MutexDestroy(ctx->mutex);
        ctx->is_opened = 0;
        return res;
    }

    INIT_LIST_HEAD(&ctx->upstream_sync_callback_list);
    INIT_LIST_HEAD(&ctx->downstream_service_list);

    return SUCCESS_RETURN;
}

#ifdef DEVICE_MODEL_GATEWAY
static int _iotx_linkkit_slave_open(iotx_linkkit_dev_meta_info_t *meta_info)
{
    int res, devid;
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();

    if (!ctx->is_opened) {
        return STATE_DEV_MODEL_MASTER_NOT_OPEN_YET;
    }

    res = iotx_dm_subdev_create(meta_info->product_key, meta_info->product_secret, meta_info->device_name,
                                meta_info->device_secret, &devid);
    if (res < 0) {
        return res;
    } else {
        return devid;
    }
}
#endif

static int _iotx_linkkit_master_connect(void)
{
    int res = 0;
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();
    iotx_dm_init_params_t dm_init_params;
    iotx_dm_event_types_t type;

    if (ctx->is_connected) {
        return STATE_DEV_MODEL_MASTER_ALREADY_CONNECT;
    }
    ctx->is_connected = 1;

    memset(&dm_init_params, 0, sizeof(iotx_dm_init_params_t));
    dm_init_params.event_callback = _iotx_linkkit_event_callback;

    res = iotx_dm_connect(&dm_init_params);
    if (res != SUCCESS_RETURN) {
        ctx->is_connected = 0;
        return res;
    }

    res = iotx_dm_subscribe(IOTX_DM_LOCAL_NODE_DEVID);
    if (res != SUCCESS_RETURN) {
        ctx->is_connected = 0;
        return res;
    }

    type = IOTX_DM_EVENT_INITIALIZED;
    _iotx_linkkit_event_callback(type, "{\"devid\":0}");

#if (defined(AWSS_SUPPORT_ZEROCONFIG) &&  !defined(AWSS_DISABLE_REGISTRAR))
    awss_registrar_init();
#endif
#ifdef DEV_BIND_ENABLED
    IOT_Bind_Start(NULL, NULL);
#endif

    ctx->yield_running = 1;
    return SUCCESS_RETURN;
}

#ifdef DEVICE_MODEL_GATEWAY
static int _iotx_linkkit_slave_connect(int devid)
{
    int res = 0, msgid = 0, code = 0;
    int proxy_product_register = 0;
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();
    iotx_linkkit_upstream_sync_callback_node_t *node = NULL;
    void *semaphore = NULL;

    if (ctx->is_connected == 0) {
        return STATE_DEV_MODEL_MASTER_NOT_CONNECT_YET;
    }

    if (devid <= 0) {
        return STATE_USER_INPUT_DEVID;
    }

    res = iotx_dm_get_opt(DM_OPT_PROXY_PRODUCT_REGISTER, (void *)&proxy_product_register);
    if (res < SUCCESS_RETURN) {
        return res;
    }

    /* Subdev Register */
    if (proxy_product_register) {
        res = iotx_dm_subdev_proxy_register(devid);
        if (res < SUCCESS_RETURN) {
            return res;
        }
    } else {
        res = iotx_dm_subdev_register(devid);
        if (res < SUCCESS_RETURN) {
            return res;
        }
    }

    if (res > SUCCESS_RETURN) {
        semaphore = HAL_SemaphoreCreate();
        if (semaphore == NULL) {
            return STATE_SYS_DEPEND_SEMAPHORE_CREATE;
        }

        msgid = res;

        _iotx_linkkit_upstream_mutex_lock();
        res = _iotx_linkkit_upstream_sync_callback_list_insert(msgid, semaphore, &node);
        if (res != SUCCESS_RETURN) {
            HAL_SemaphoreDestroy(semaphore);
            _iotx_linkkit_upstream_mutex_unlock();
            return res;
        }
        _iotx_linkkit_upstream_mutex_unlock();

        res = HAL_SemaphoreWait(semaphore, IOTX_LINKKIT_SYNC_DEFAULT_TIMEOUT_MS);
        if (res < SUCCESS_RETURN) {
            _iotx_linkkit_upstream_mutex_lock();
            _iotx_linkkit_upstream_sync_callback_list_remove(msgid);
            _iotx_linkkit_upstream_mutex_unlock();
            return STATE_SYS_DEPEND_SEMAPHORE_WAIT;
        }

        _iotx_linkkit_upstream_mutex_lock();
        code = node->code;
        _iotx_linkkit_upstream_sync_callback_list_remove(msgid);
        if (code != SUCCESS_RETURN) {
            _iotx_linkkit_upstream_mutex_unlock();
            iotx_state_event(ITE_STATE_DEV_MODEL, STATE_DEV_MODEL_REFUSED_BY_CLOUD, "refuse register for devid: %d", devid);
            return STATE_DEV_MODEL_REFUSED_BY_CLOUD;
        }
        _iotx_linkkit_upstream_mutex_unlock();
    }

    /* Subdev Add Topo */
    res = iotx_dm_subdev_topo_add(devid);
    if (res < SUCCESS_RETURN) {
        return res;
    }
    semaphore = HAL_SemaphoreCreate();
    if (semaphore == NULL) {
        return STATE_SYS_DEPEND_SEMAPHORE_CREATE;
    }

    msgid = res;
    _iotx_linkkit_upstream_mutex_lock();
    res = _iotx_linkkit_upstream_sync_callback_list_insert(msgid, semaphore, &node);
    if (res != SUCCESS_RETURN) {
        HAL_SemaphoreDestroy(semaphore);
        _iotx_linkkit_upstream_mutex_unlock();
        return res;
    }
    _iotx_linkkit_upstream_mutex_unlock();

    res = HAL_SemaphoreWait(semaphore, IOTX_LINKKIT_SYNC_DEFAULT_TIMEOUT_MS);
    if (res < SUCCESS_RETURN) {
        _iotx_linkkit_upstream_mutex_lock();
        _iotx_linkkit_upstream_sync_callback_list_remove(msgid);
        _iotx_linkkit_upstream_mutex_unlock();
        return STATE_SYS_DEPEND_SEMAPHORE_WAIT;
    }

    _iotx_linkkit_upstream_mutex_lock();
    code = node->code;
    _iotx_linkkit_upstream_sync_callback_list_remove(msgid);
    if (code != SUCCESS_RETURN) {
        _iotx_linkkit_upstream_mutex_unlock();
        iotx_state_event(ITE_STATE_DEV_MODEL, STATE_DEV_MODEL_REFUSED_BY_CLOUD, "refuse to add topo for devid: %d", devid);
        return STATE_DEV_MODEL_REFUSED_BY_CLOUD;
    }
    _iotx_linkkit_upstream_mutex_unlock();

    return SUCCESS_RETURN;
}

static int _iotx_linkkit_subdev_delete_topo(int devid)
{
    int res = 0, msgid = 0, code = 0;
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();
    iotx_linkkit_upstream_sync_callback_node_t *node = NULL;
    void *semaphore = NULL;

    if (ctx->is_connected == 0) {
        return STATE_DEV_MODEL_MASTER_NOT_CONNECT_YET;
    }

    if (devid <= 0) {
        return STATE_USER_INPUT_DEVID;
    }

    /* Subdev Delete Topo */
    res = iotx_dm_subdev_topo_del(devid);
    if (res < SUCCESS_RETURN) {
        return res;
    }
    msgid = res;

    semaphore = HAL_SemaphoreCreate();
    if (semaphore == NULL) {
        return STATE_SYS_DEPEND_SEMAPHORE_CREATE;
    }

    _iotx_linkkit_upstream_mutex_lock();
    res = _iotx_linkkit_upstream_sync_callback_list_insert(msgid, semaphore, &node);
    if (res != SUCCESS_RETURN) {
        HAL_SemaphoreDestroy(semaphore);
        _iotx_linkkit_upstream_mutex_unlock();
        return res;
    }
    _iotx_linkkit_upstream_mutex_unlock();

    res = HAL_SemaphoreWait(semaphore, IOTX_LINKKIT_SYNC_DEFAULT_TIMEOUT_MS);
    if (res < SUCCESS_RETURN) {
        _iotx_linkkit_upstream_mutex_lock();
        _iotx_linkkit_upstream_sync_callback_list_remove(msgid);
        _iotx_linkkit_upstream_mutex_unlock();
        return STATE_SYS_DEPEND_SEMAPHORE_WAIT;
    }

    _iotx_linkkit_upstream_mutex_lock();
    code = node->code;
    _iotx_linkkit_upstream_sync_callback_list_remove(msgid);
    if (code != SUCCESS_RETURN) {
        _iotx_linkkit_upstream_mutex_unlock();
        iotx_state_event(ITE_STATE_DEV_MODEL, STATE_DEV_MODEL_REFUSED_BY_CLOUD, "refuse to del topo for devid: %d", devid);
        return STATE_DEV_MODEL_REFUSED_BY_CLOUD;
    }
    _iotx_linkkit_upstream_mutex_unlock();

    return SUCCESS_RETURN;
}
#endif

static int _iotx_linkkit_master_close(void)
{
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();

    ctx->yield_running = 0;
    if (ctx->is_yield_running) {
        return STATE_DEV_MODEL_YIELD_RUNNING;
    }
    _iotx_linkkit_mutex_lock();
    if (ctx->is_opened == 0) {
        _iotx_linkkit_mutex_unlock();
        return STATE_DEV_MODEL_MASTER_NOT_OPEN_YET;
    }
    ctx->is_opened = 0;

    iotx_dm_close();
#ifdef DEVICE_MODEL_GATEWAY
    _iotx_linkkit_upstream_sync_callback_list_destroy();
    HAL_MutexDestroy(ctx->upstream_mutex);
#endif
    _iotx_linkkit_mutex_unlock();
    HAL_MutexDestroy(ctx->mutex);
#if !defined(DEVICE_MODEL_RAWDATA_SOLO)
    _linkkit_service_list_destroy();
    HAL_MutexDestroy(ctx->service_list_mutex);
#endif
    memset(ctx, 0, sizeof(iotx_linkkit_ctx_t));

#if (defined(AWSS_SUPPORT_ZEROCONFIG) && !defined(AWSS_DISABLE_REGISTRAR))
    awss_registrar_deinit();
#endif

#ifdef DEV_BIND_ENABLED
    IOT_Bind_Stop();
#endif
    return SUCCESS_RETURN;
}

#ifdef DEVICE_MODEL_GATEWAY
static int _iotx_linkkit_slave_close(int devid)
{
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();

    _iotx_linkkit_mutex_lock();
    if (ctx->is_opened == 0) {
        _iotx_linkkit_mutex_unlock();
        return STATE_DEV_MODEL_MASTER_NOT_OPEN_YET;
    }

    /* Release Subdev Resources */
    iotx_dm_subdev_destroy(devid);

    _iotx_linkkit_mutex_unlock();

    return SUCCESS_RETURN;
}
#endif

int IOT_Linkkit_Open(iotx_linkkit_dev_type_t dev_type, iotx_linkkit_dev_meta_info_t *meta_info)
{
    int res = 0;

    if (meta_info == NULL) {
        return STATE_USER_INPUT_META_INFO;
    }

    switch (dev_type) {
        case IOTX_LINKKIT_DEV_TYPE_MASTER: {
            res = _iotx_linkkit_master_open(meta_info);
            if (res == SUCCESS_RETURN) {
                res = IOTX_DM_LOCAL_NODE_DEVID;
            }
        }
        break;
        case IOTX_LINKKIT_DEV_TYPE_SLAVE: {
#ifdef DEVICE_MODEL_GATEWAY
            res = _iotx_linkkit_slave_open(meta_info);
#else
            res = STATE_DEV_MODEL_GATEWAY_NOT_ENABLED;
#endif
        }
        break;
        default: {
            res = STATE_USER_INPUT_DEVICE_TYPE;
        }
        break;
    }

    return res;
}

int IOT_Linkkit_Connect(int devid)
{
    int res = 0;
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();

    if (devid < 0) {
        return STATE_USER_INPUT_INVALID;
    }

    if (ctx->is_opened == 0) {
        return STATE_DEV_MODEL_MASTER_NOT_OPEN_YET;
    }

    _iotx_linkkit_mutex_lock();

    if (devid == IOTX_DM_LOCAL_NODE_DEVID) {
        res = _iotx_linkkit_master_connect();
    } else {
#ifdef DEVICE_MODEL_GATEWAY
        res = _iotx_linkkit_slave_connect(devid);
#else
        res = STATE_DEV_MODEL_GATEWAY_NOT_ENABLED;
#endif
    }
    _iotx_linkkit_mutex_unlock();

    return res;
}

int IOT_Linkkit_Yield(int timeout_ms)
{
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();
    int res = 0;

    if (ctx->yield_running == 0) {
        HAL_SleepMs(timeout_ms);
        return STATE_DEV_MODEL_YIELD_STOPPED;
    }

    ctx->is_yield_running = 1;
    if (timeout_ms <= 0) {
        ctx->is_yield_running = 0;
        return STATE_USER_INPUT_INVALID;
    }

    if (ctx->is_opened == 0 || ctx->is_connected == 0) {
        ctx->is_yield_running = 0;
        return STATE_DEV_MODEL_MASTER_NOT_CONNECT_YET;
    }

    res = iotx_dm_yield(timeout_ms);
    iotx_dm_dispatch();

#ifdef DEV_BIND_ENABLED
    IOT_Bind_Yield();
#endif
    ctx->is_yield_running = 0;

    return res;
}

int IOT_Linkkit_Close(int devid)
{
    int res = 0;

    if (devid < 0) {
        return STATE_USER_INPUT_DEVID;
    }

    if (devid == IOTX_DM_LOCAL_NODE_DEVID) {
        res = _iotx_linkkit_master_close();
    } else {
#ifdef DEVICE_MODEL_GATEWAY
        res = _iotx_linkkit_slave_close(devid);
#else
        res = STATE_DEV_MODEL_GATEWAY_NOT_ENABLED;
#endif
    }

    return res;
}

#ifdef DEVICE_MODEL_GATEWAY
static int _iotx_linkkit_subdev_login(int devid)
{
    int res = 0, msgid = 0, code = 0;
    iotx_linkkit_upstream_sync_callback_node_t *node = NULL;
    void *semaphore = NULL;
    void *callback = NULL;

    res = iotx_dm_subdev_login(devid);
    if (res < SUCCESS_RETURN) {
        return res;
    }

    msgid = res;
    semaphore = HAL_SemaphoreCreate();
    if (semaphore == NULL) {
        return STATE_SYS_DEPEND_SEMAPHORE_CREATE;
    }

    _iotx_linkkit_upstream_mutex_lock();
    res = _iotx_linkkit_upstream_sync_callback_list_insert(msgid, semaphore, &node);
    if (res != SUCCESS_RETURN) {
        HAL_SemaphoreDestroy(semaphore);
        _iotx_linkkit_upstream_mutex_unlock();
        return res;
    }
    _iotx_linkkit_upstream_mutex_unlock();

    res = HAL_SemaphoreWait(semaphore, IOTX_LINKKIT_SYNC_DEFAULT_TIMEOUT_MS);
    if (res < SUCCESS_RETURN) {
        _iotx_linkkit_upstream_mutex_lock();
        _iotx_linkkit_upstream_sync_callback_list_remove(msgid);
        _iotx_linkkit_upstream_mutex_unlock();
        return res;
    }

    _iotx_linkkit_upstream_mutex_lock();
    code = node->code;
    _iotx_linkkit_upstream_sync_callback_list_remove(msgid);
    if (code != SUCCESS_RETURN) {
        _iotx_linkkit_upstream_mutex_unlock();
        iotx_state_event(ITE_STATE_DEV_MODEL, STATE_DEV_MODEL_REFUSED_BY_CLOUD, "refuse to login for devid: %d", devid);
        return STATE_DEV_MODEL_REFUSED_BY_CLOUD;
    }
    _iotx_linkkit_upstream_mutex_unlock();

    res = iotx_dm_subscribe(devid);
    if (res != SUCCESS_RETURN) {
        return res;
    }

    callback = iotx_event_callback(ITE_INITIALIZE_COMPLETED);
    if (callback) {
        ((int (*)(const int))callback)(devid);
    }

    return SUCCESS_RETURN;
}

static int _iotx_linkkit_subdev_logout(int devid)
{
    int res = 0, msgid = 0, code = 0;
    iotx_linkkit_upstream_sync_callback_node_t *node = NULL;
    void *semaphore = NULL;

    res = iotx_dm_subdev_logout(devid);
    if (res < SUCCESS_RETURN) {
        return res;
    }

    msgid = res;
    semaphore = HAL_SemaphoreCreate();
    if (semaphore == NULL) {
        return STATE_SYS_DEPEND_SEMAPHORE_CREATE;
    }

    _iotx_linkkit_upstream_mutex_lock();
    res = _iotx_linkkit_upstream_sync_callback_list_insert(msgid, semaphore, &node);
    if (res != SUCCESS_RETURN) {
        HAL_SemaphoreDestroy(semaphore);
        _iotx_linkkit_upstream_mutex_unlock();
        return res;
    }
    _iotx_linkkit_upstream_mutex_unlock();

    res = HAL_SemaphoreWait(semaphore, IOTX_LINKKIT_SYNC_DEFAULT_TIMEOUT_MS);
    if (res < SUCCESS_RETURN) {
        _iotx_linkkit_upstream_mutex_lock();
        _iotx_linkkit_upstream_sync_callback_list_remove(msgid);
        _iotx_linkkit_upstream_mutex_unlock();
        return STATE_SYS_DEPEND_SEMAPHORE_WAIT;
    }

    _iotx_linkkit_upstream_mutex_lock();
    code = node->code;
    _iotx_linkkit_upstream_sync_callback_list_remove(msgid);
    if (code != SUCCESS_RETURN) {
        _iotx_linkkit_upstream_mutex_unlock();
        iotx_state_event(ITE_STATE_DEV_MODEL, STATE_DEV_MODEL_REFUSED_BY_CLOUD, "refuse to logout for devid: %d", devid);
        return STATE_DEV_MODEL_REFUSED_BY_CLOUD;
    }
    _iotx_linkkit_upstream_mutex_unlock();

    return SUCCESS_RETURN;
}
#endif

int IOT_Linkkit_Report(int devid, iotx_linkkit_msg_type_t msg_type, unsigned char *payload, int payload_len)
{
    int res = 0;
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();

    if (devid < 0) {
        return STATE_USER_INPUT_DEVID;
    }

    if (msg_type >= IOTX_LINKKIT_MSG_MAX) {
        return STATE_USER_INPUT_MSG_TYPE;
    }

    if (ctx->is_opened == 0 || ctx->is_connected == 0) {
        return STATE_DEV_MODEL_MASTER_NOT_CONNECT_YET;
    }

    switch (msg_type) {
#if !defined(DEVICE_MODEL_RAWDATA_SOLO)
        case ITM_MSG_POST_PROPERTY: {
            if (payload == NULL || payload_len <= 0) {
                return STATE_USER_INPUT_INVALID;
            }
            res = iotx_dm_post_property(devid, (char *)payload, payload_len);
#ifdef LOG_REPORT_TO_CLOUD
            if (1 == report_sample) {
                send_permance_info(NULL, 0, "4", 1);
            }
#endif
        }
        break;
#ifdef DEVICE_MODEL_SHADOW
        case ITM_MSG_PROPERTY_DESIRED_GET: {
            if (payload == NULL || payload_len <= 0) {
                return STATE_USER_INPUT_INVALID;
            }
            res = iotx_dm_property_desired_get(devid, (char *)payload, payload_len);
        }
        break;
        case ITM_MSG_PROPERTY_DESIRED_DELETE: {
            if (payload == NULL || payload_len <= 0) {
                return STATE_USER_INPUT_INVALID;
            }
            res = iotx_dm_property_desired_delete(devid, (char *)payload, payload_len);
        }
        break;
#endif
        case ITM_MSG_DEVICEINFO_UPDATE: {
            if (payload == NULL || payload_len <= 0) {
                return STATE_USER_INPUT_INVALID;
            }
            res = iotx_dm_deviceinfo_update(devid, (char *)payload, payload_len);
        }
        break;
        case ITM_MSG_DEVICEINFO_DELETE: {
            if (payload == NULL || payload_len <= 0) {
                return STATE_USER_INPUT_INVALID;
            }
            res = iotx_dm_deviceinfo_delete(devid, (char *)payload, payload_len);
        }
        break;
#endif
        case ITM_MSG_POST_RAW_DATA: {
            if (payload == NULL || payload_len <= 0) {
                return STATE_USER_INPUT_INVALID;
            }
            res = iotx_dm_post_rawdata(devid, (char *)payload, payload_len);
        }
        break;
        case ITM_MSG_LOGIN: {
#ifdef DEVICE_MODEL_GATEWAY
            res = _iotx_linkkit_subdev_login(devid);
#else
            res = STATE_DEV_MODEL_GATEWAY_NOT_ENABLED;
#endif
        }
        break;
        case ITM_MSG_LOGOUT: {
#ifdef DEVICE_MODEL_GATEWAY
            res = _iotx_linkkit_subdev_logout(devid);
#else
            res = STATE_DEV_MODEL_GATEWAY_NOT_ENABLED;
#endif
        }
        break;
        case ITM_MSG_DELETE_TOPO: {
#ifdef DEVICE_MODEL_GATEWAY
            res = _iotx_linkkit_subdev_delete_topo(devid);
#else
            res = STATE_DEV_MODEL_GATEWAY_NOT_ENABLED;
#endif
        }
        break;
#ifdef DEVICE_MODEL_GATEWAY
#ifdef DEVICE_MODEL_SUBDEV_OTA
        case ITM_MSG_REPORT_SUBDEV_FIRMWARE_VERSION: {
            res = iotx_dm_send_firmware_version(devid, (const char *)payload);
        }
        break;
#endif
#endif
#ifdef DEVICE_HISTORY_POST
        case ITM_MSG_POST_HISTORY_DATA: {
            if (payload == NULL || payload_len <= 0) {
                return STATE_USER_INPUT_INVALID;
            }
            res = iotx_dm_post_history(devid, (char *)payload, payload_len);
        }
        break;
#endif /* #ifdef DEVICE_HISTORY_POST */
        default: {
            res = STATE_USER_INPUT_MSG_TYPE;
        }
        break;
    }
    return res;
}

int IOT_Linkkit_Query(int devid, iotx_linkkit_msg_type_t msg_type, unsigned char *payload, int payload_len)
{
    int res = 0;
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();

    if (devid < 0 || msg_type >= IOTX_LINKKIT_MSG_MAX) {
        return STATE_USER_INPUT_INVALID;
    }

    if (ctx->is_opened == 0 || ctx->is_connected == 0) {
        return STATE_DEV_MODEL_MASTER_NOT_CONNECT_YET;
    }

    switch (msg_type) {
#if !defined(DEVICE_MODEL_RAWDATA_SOLO)
        case ITM_MSG_QUERY_TIMESTAMP: {
            res = iotx_dm_qurey_ntp();
        }
        break;
#endif
        case ITM_MSG_QUERY_TOPOLIST: {
#ifdef DEVICE_MODEL_GATEWAY
            res = iotx_dm_query_topo_list();
#else
            res = STATE_DEV_MODEL_GATEWAY_NOT_ENABLED;
#endif
        }
        break;
        case ITM_MSG_QUERY_FOTA_DATA: {
            res = iotx_dm_fota_perform_sync((char *)payload, payload_len);
        }
        break;
        case ITM_MSG_QUERY_COTA_DATA: {
            res = iotx_dm_cota_perform_sync((char *)payload, payload_len);
        }
        break;
        case ITM_MSG_REQUEST_COTA: {
            res = iotx_dm_cota_get_config("product", "file", "");
        }
        break;
        case ITM_MSG_REQUEST_FOTA_IMAGE: {
            res = iotx_dm_fota_request_image((const char *)payload, payload_len);
        }
        break;
        default: {
            res = STATE_USER_INPUT_MSG_TYPE;
        }
        break;
    }

    return res;
}

int IOT_Linkkit_TriggerEvent(int devid, char *eventid, int eventid_len, char *payload, int payload_len)
{
#if !defined(DEVICE_MODEL_RAWDATA_SOLO)
    int res = 0;
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();

    if (devid < 0 || eventid == NULL || eventid_len <= 0 || payload == NULL || payload_len <= 0) {
        return STATE_USER_INPUT_INVALID;
    }

    if (ctx->is_opened == 0 || ctx->is_connected == 0) {
        return STATE_DEV_MODEL_MASTER_NOT_CONNECT_YET;
    }

    res = iotx_dm_post_event(devid, eventid, eventid_len, payload, payload_len);

    return res;
#else
    return STATE_DEV_MODEL_IN_RAWDATA_SOLO;
#endif
}

int IOT_Linkkit_AnswerService(int devid, char *serviceid, int serviceid_len, char *payload, int payload_len,
                              void *p_service_ctx)
{
#if !defined(DEVICE_MODEL_RAWDATA_SOLO)
    int res = 0;
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();

    char msgid[11] = {0};
    iotx_service_ctx_node_t *service_ctx = (iotx_service_ctx_node_t *)p_service_ctx;

    if (devid < 0 || serviceid == NULL || serviceid_len == 0
        || payload == NULL || payload_len == 0 || service_ctx == NULL) {
        return STATE_USER_INPUT_INVALID;
    }

    if (ctx->is_opened == 0 || ctx->is_connected == 0) {
        return STATE_DEV_MODEL_MASTER_NOT_CONNECT_YET;
    }

    /* check if service ctx exist */
    if (_linkkit_service_list_search(service_ctx) != SUCCESS_RETURN) {
        return STATE_DEV_MODEL_SERVICE_CTX_NOT_EXIST;
    }

    /* msgid int to string */
    infra_int2str(service_ctx->msgid, msgid);

    /* rrpc service response */
    if (service_ctx->type == IOTX_SERVICE_REQ_TYPE_RRPC) {
        char *rrpc_id = service_ctx->service_meta.rrpc.rrpc_id;
        res = iotx_dm_send_rrpc_response(devid, msgid, strlen(msgid), IOTX_DM_ERR_CODE_SUCCESS, rrpc_id, strlen(rrpc_id),
                                         payload, payload_len);
    } else if (service_ctx->type == IOTX_SERVICE_REQ_TYPE_GENERAL) { /* alcs/normal service response */
        void *alcs_ctx = service_ctx->service_meta.general.alcs_ctx;
        res = iotx_dm_send_service_response(devid, msgid, strlen(msgid), IOTX_DM_ERR_CODE_SUCCESS, serviceid, serviceid_len,
                                            payload, payload_len, alcs_ctx);
    } else {
        res = STATE_DEV_MODEL_INTERNAL_ERROR;
    }

    _linkkit_service_list_mutex_lock();
    _linkkit_service_list_delete(service_ctx);
    _linkkit_service_list_mutex_unlock();

    return res;
#else
    return STATE_DEV_MODEL_IN_RAWDATA_SOLO;
#endif
}

#ifdef DEVICE_MODEL_GATEWAY
int iot_linkkit_subdev_query_id(char product_key[IOTX_PRODUCT_KEY_LEN + 1], char device_name[IOTX_DEVICE_NAME_LEN + 1])
{
    int res = -1;
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();

    if (ctx->is_opened == 0) {
        return STATE_DEV_MODEL_MASTER_NOT_OPEN_YET;
    }

    iotx_dm_subdev_query(product_key, device_name, &res);
    return res;
}

int IOCTL_FUNC(IOTX_IOCTL_QUERY_DEVID, void *data)
{
    int res;
    iotx_dev_meta_info_t *dev_info = (iotx_dev_meta_info_t *)data;
    if (data == NULL) {
        return FAIL_RETURN;
    }

    res = iot_linkkit_subdev_query_id(dev_info->product_key, dev_info->device_name);
    return res;
}

#ifdef DEVICE_MODEL_SUBDEV_OTA
int IOCTL_FUNC(IOTX_IOCTL_SET_OTA_DEV_ID, void *data)
{
    int res;
    int devid;
    if (data == NULL) {
        return FAIL_RETURN;
    }

    devid = *(int *)(data);
    if (devid < 0) {
        res = STATE_USER_INPUT_DEVID;
    } else {
        res = iotx_dm_ota_switch_device(devid);
    }
    return res;
}
#endif
#endif /* #ifdef DEVICE_MODEL_GATEWAY */

int IOCTL_FUNC(IOTX_IOCTL_RECV_PROP_REPLY, void *data)
{
    int res;
    if (data == NULL) {
        return FAIL_RETURN;
    }

    res = iotx_dm_set_opt(IMPL_LINKKIT_IOCTL_SWITCH_EVENT_POST_REPLY, data);
    return res;
}

int IOCTL_FUNC(IOTX_IOCTL_SEND_PROP_SET_REPLY, void *data)
{
    int res;
    if (data == NULL) {
        return FAIL_RETURN;
    }
    res = iotx_dm_set_opt(IMPL_LINKKIT_IOCTL_SWITCH_PROPERTY_SET_REPLY, data);
    return res;
}

int IOCTL_FUNC(IOTX_IOCTL_FOTA_TIMEOUT_MS, void *data)
{
    int res;
    if (data == NULL) {
        return FAIL_RETURN;
    }
    res = iotx_dm_set_opt(DM_OPT_FOTA_RETRY_TIMEOUT_MS, data);
    return res;
}

int IOCTL_FUNC(IOTX_IOCTL_SET_PROXY_REGISTER, void *data)
{
    int res;
    if (data == NULL) {
        return FAIL_RETURN;
    }
    res = iotx_dm_set_opt(DM_OPT_PROXY_PRODUCT_REGISTER, data);
    return res;
}

int IOCTL_FUNC(IOTX_IOCTL_SUB_USER_TOPIC, void *data)
{
    int res;
    iotx_user_subscribe_context *context = (iotx_user_subscribe_context *) data;
    if (data == NULL) {
        return FAIL_RETURN;
    }

    res = iotx_dm_subscribe_user_topic((char *)context->topic, (void *)context->callback);
    return res;
}
#endif
