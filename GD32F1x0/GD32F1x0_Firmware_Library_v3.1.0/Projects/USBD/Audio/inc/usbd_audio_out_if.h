/*!
    \file  usbd_audio_out_if.h
    \brief audio Out (playback) interface header file
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=5)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=5)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=5)
*/

#ifndef USBD_AUDIO_OUT_IF_H
#define USBD_AUDIO_OUT_IF_H

#include "gd32f150R_audio_codec.h"

/* audio Commands enmueration */
typedef enum
{
    AUDIO_CMD_PLAY = 1,
    AUDIO_CMD_PAUSE,
    AUDIO_CMD_STOP,
}audio_cmd_enum;

/* mute commands */
#define AUDIO_MUTE                      0x01U
#define AUDIO_UNMUTE                    0x00U

/* functions return value */
#define AUDIO_OK                        0x00U
#define AUDIO_FAIL                      0xFFU

/* audio Machine States */
#define AUDIO_STATE_INACTIVE            0x00U
#define AUDIO_STATE_ACTIVE              0x01U
#define AUDIO_STATE_PLAYING             0x02U
#define AUDIO_STATE_PAUSED              0x03U
#define AUDIO_STATE_STOPPED             0x04U
#define AUDIO_STATE_ERROR               0x05U

typedef struct _audio_fops
{
    uint8_t  (*audio_init)        (uint32_t audio_freq, uint32_t volume, uint32_t options);
    uint8_t  (*audio_deinit)      (uint32_t options);
    uint8_t  (*audio_cmd)         (uint8_t* pbuf, uint32_t size, uint8_t cmd);
    uint8_t  (*audio_volume_ctl)  (uint8_t vol);
    uint8_t  (*audio_mute_ctl)    (uint8_t cmd);
    uint8_t  (*audio_periodic_tc) (uint8_t cmd);
    uint8_t  (*audio_state_get)   (void);
}audio_fops_struct;

extern audio_fops_struct  audio_out_fops;

#endif  /* USBD_AUDIO_OUT_IF_H */
