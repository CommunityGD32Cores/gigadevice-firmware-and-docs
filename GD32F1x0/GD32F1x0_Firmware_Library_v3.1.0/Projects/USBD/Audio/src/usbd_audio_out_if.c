/*!
    \file  usbd_audio_out_if.c
    \brief audio Out (playback) interface functions
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=5)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=5)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=5)
*/

#include "audio_core.h"
#include "usbd_audio_out_if.h"

static uint8_t  init         (uint32_t audiofreq, uint32_t volume, uint32_t options);
static uint8_t  deinit       (uint32_t options);
static uint8_t  audio_cmd     (uint8_t* pbuf, uint32_t size, uint8_t cmd);
static uint8_t  volume_ctl    (uint8_t vol);
static uint8_t  mute_ctl      (uint8_t cmd);
static uint8_t  periodic_tc   (uint8_t cmd);
static uint8_t  get_state     (void);

audio_fops_struct  audio_out_fops = 
{
    init,
    deinit,
    audio_cmd,
    volume_ctl,
    mute_ctl,
    periodic_tc,
    get_state
};

static uint8_t audio_state = AUDIO_STATE_INACTIVE;

/*!
    \brief      initialize and configures all required resources for audio play function
    \param[in]  audio_freq: statrt_up audio frequency
    \param[in]  volume: start_up volume to be set
    \param[in]  options: specific options passed to low layer function
    \param[out] none
    \retval     AUDIO_OK if all operations succeed, AUDIO_FAIL else
*/
static uint8_t  init (uint32_t audio_freq, uint32_t volume, uint32_t options)
{
    static uint32_t initialized = 0;

    /* check if the low layer has already been initialized */
    if (initialized == 0) {
        /* call low layer function */
        if (eval_audio_init(OUTPUT_DEVICE_AUTO, volume, audio_freq) != 0) {
            audio_state = AUDIO_STATE_ERROR;
            return AUDIO_FAIL;
        }

        /* set the Initialization flag to prevent reinitializing the interface again */
        initialized = 1;
    }

    /* update the Audio state machine */
    audio_state = AUDIO_STATE_ACTIVE;

    return AUDIO_OK;
}

/*!
    \brief      free all resources used by low layer and stops audio-play function
    \param[in]  options: specific options passed to low layer function
    \param[out] none
    \retval     AUDIO_OK if all operations succeed, AUDIO_FAIL else
*/
static uint8_t  deinit (uint32_t Options)
{
    /* Update the Audio state machine */
    audio_state = AUDIO_STATE_INACTIVE;

    return AUDIO_OK;
}

/*!
    \brief      play, stop, pause or resume current file
    \param[in]  pbuf: address from which file shoud be played
    \param[in]  size: size of the current buffer/file
    \param[in]  cmd: command to be executed, can be:
      \arg        AUDIO_CMD_PLAY
      \arg        AUDIO_CMD_PAUSE
      \arg        AUDIO_CMD_RESUME
      \arg        AUDIO_CMD_STOP
    \param[out] none
    \retval     AUDIO_OK if all operations succeed, AUDIO_FAIL else
*/
static uint8_t  audio_cmd (uint8_t* pbuf, uint32_t size, uint8_t cmd)
{
    /* check the current state */
    if ((audio_state == AUDIO_STATE_INACTIVE) || (audio_state == AUDIO_STATE_ERROR)) {
        audio_state = AUDIO_STATE_ERROR;

        return AUDIO_FAIL;
    }

    switch (cmd) {
        /* process the play command */
        case AUDIO_CMD_PLAY:
            /* if current state is active or stopped */
            if ((audio_state == AUDIO_STATE_ACTIVE) || \
                (audio_state == AUDIO_STATE_STOPPED) || \
                (audio_state == AUDIO_STATE_PLAYING)) {
                audio_mal_play((uint32_t)pbuf, (size / 2));
                audio_state = AUDIO_STATE_PLAYING;

                return AUDIO_OK;
            } else if (audio_state == AUDIO_STATE_PAUSED) {
                if (eval_audio_pause_resume(AUDIO_RESUME, (uint32_t)pbuf, (size/2)) != 0) {
                    audio_state = AUDIO_STATE_ERROR;

                    return AUDIO_FAIL;
                } else {
                    audio_state = AUDIO_STATE_PLAYING;

                    return AUDIO_OK;
                }
            } else {
                return AUDIO_FAIL;
            }

        /* process the stop command */
        case AUDIO_CMD_STOP:
            if (audio_state != AUDIO_STATE_PLAYING) {
                /* unsupported command */
                return AUDIO_FAIL;
            } else if (eval_audio_stop(CODEC_PDWN_SW) != 0) {
                audio_state = AUDIO_STATE_ERROR;

                return AUDIO_FAIL;
            } else {
                audio_state = AUDIO_STATE_STOPPED;

                return AUDIO_OK;
            }

        /* process the pause command */
        case AUDIO_CMD_PAUSE:
            if (audio_state != AUDIO_STATE_PLAYING) {
                /* unsupported command */
                return AUDIO_FAIL;
            } else if (eval_audio_pause_resume(AUDIO_PAUSE, (uint32_t)pbuf, (size / 2)) != 0) {
                audio_state = AUDIO_STATE_ERROR;

                return AUDIO_FAIL;
            } else {
                audio_state = AUDIO_STATE_PAUSED;

                return AUDIO_OK;
            }

        /* unsupported command */
        default:
            return AUDIO_FAIL;
    }
}

/*!
    \brief      set the volume level
    \param[in]  vol: volume level to be set in % (from 0% to 100%)
    \param[out] none
    \retval     AUDIO_OK if all operations succeed, AUDIO_FAIL else
*/
static uint8_t  volume_ctl (uint8_t vol)
{
    /* call low layer volume setting function */  
    if (eval_audio_volume_ctl(vol) != 0) {
        audio_state = AUDIO_STATE_ERROR;

        return AUDIO_FAIL;
    }

    return AUDIO_OK;
}

/*!
    \brief      mute or unmute the audio current output
    \param[in]  cmd: can be 0 to unmute, or 1 to mute
    \param[out] none
    \retval     AUDIO_OK if all operations succeed, AUDIO_FAIL else
*/
static uint8_t  mute_ctl (uint8_t cmd)
{
    /* call low layer mute setting function */
    if (eval_audio_mute(cmd) != 0) {
        audio_state = AUDIO_STATE_ERROR;

        return AUDIO_FAIL;
    }

    return AUDIO_OK;
}

/*!
    \brief      
    \param[in]  cmd: 
    \param[out] none
    \retval     AUDIO_OK if all operations succeed, AUDIO_FAIL else
*/
static uint8_t  periodic_tc (uint8_t cmd)
{
    return AUDIO_OK;
}

/*!
    \brief      return the current state of the audio machine
    \param[in]  none
    \param[out] none
    \retval     AUDIO_OK if all operations succeed, AUDIO_FAIL else
*/
static uint8_t  get_state (void)
{
    return audio_state;
}
