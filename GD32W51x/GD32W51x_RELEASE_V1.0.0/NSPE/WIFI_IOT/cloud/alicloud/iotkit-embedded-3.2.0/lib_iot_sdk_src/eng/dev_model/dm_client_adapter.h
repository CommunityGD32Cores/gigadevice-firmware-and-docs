#ifndef _DM_CLIENT_ADAPTER_H_
#define _DM_CLIENT_ADAPTER_H_

typedef struct {
    int fd;
    void *callback;
} dm_client_ctx_t;

int dm_client_open(void);
int dm_client_connect(int timeout_ms);
int dm_client_close(void);
int dm_client_subscribe(char *uri, iotx_cm_data_handle_cb callback, void *context);
int dm_client_unsubscribe(char *uri);
int dm_client_publish(char *uri, unsigned char *payload, int payload_len, iotx_cm_data_handle_cb callback);
int dm_client_yield(unsigned int timeout);
void dm_client_user_sub_request(int fd, const char *topic, const char *payload, unsigned int payload_len,
                                void *context);
#endif
