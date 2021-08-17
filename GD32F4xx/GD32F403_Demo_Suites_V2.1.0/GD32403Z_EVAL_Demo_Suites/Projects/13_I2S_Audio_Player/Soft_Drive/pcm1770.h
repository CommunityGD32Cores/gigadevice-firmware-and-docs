/*!
    \file  pcm1770.h
    \brief the header file of pcm1770.c

    \version 2017-02-10, V1.0.0, demo for GD32F403
    \version 2018-10-10, V1.1.0, demo for GD32F403
    \version 2018-12-25, V2.0.0, demo for GD32F403
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#ifndef __PCM1770_H
#define __PCM1770_H

/* .WAV file format:
 Endian      Offset      Length      Contents
 big         0           4 bytes     'RIFF'             0x52494646
 little      4           4 bytes     <file length - 8>
 big         8           4 bytes     'WAVE'             0x57415645

next, the fmt chunk describes the sample format:
  big         12          4 bytes     'fmt '            0x666D7420
  little      16          4 bytes     0x00000010        Length of the fmt data (16 bytes)
  little      20          2 bytes     0x0001            Format tag: 1 = PCM
  little      22          2 bytes     <channels>        Channels: 1 = mono, 2 = stereo
  little      24          4 bytes     <sample rate>     Samples per second: e.g., 22050
  little      28          4 bytes     <bytes/second>    sample rate * block align
  little      32          2 bytes     <block align>     channels * bits/sample / 8
  little      34          2 bytes     <bits/sample>     8 or 16

finally, the data chunk contains the sample data:
  big         36          4 bytes     'data'            0x64617461
  little      40          4 bytes     <length of the data block>
  little      44          *           <sample data>

*/

#include "gd32f403.h"

/* extern audio file */
extern const char wave_test_data[];

/* audio file information structure */
typedef struct
{
    uint32_t  riff_chunk_size;              /* riff chunk size */
    uint16_t  format_tag;                   /* format tag */
    uint16_t  num_channels;                 /* number of channel */
    uint32_t  sample_rate;                  /* audio sample rate */
    uint32_t  byte_rate;                    /* byte rate */
    uint16_t  block_align;                  /* block align */
    uint16_t  bits_per_sample;              /* bits per sample */
    uint32_t  data_size;                    /* audio data size */
} wave_file_struct;

/* error identification structure */
typedef enum
{
    VALID_WAVE_FILE = 0,                    /* valid wave file */
    UNVALID_RIFF_ID,                        /* unvalid riff id */
    UNVALID_WAVE_FORMAT,                    /* unvalid wave format */
    UNVALID_FORMATCHUNK_ID,                 /* unvalid format chunk id */
    UNSUPPORETD_FORMATTAG,                  /* unsupporetd format tag */
    UNSUPPORETD_NUMBER_OF_CHANNEL,          /* unsupporetd number of channel */
    UNSUPPORETD_SAMPLE_RATE,                /* unsupporetd sample rate */
    UNSUPPORETD_BITS_PER_SAMPLE,            /* unsupporetd bits per sample */
    UNVALID_DATACHUNK_ID,                   /* unvalid data chunk id */
    UNSUPPORETD_EXTRAFORMATBYTES,           /* unsupporetd extra format bytes */
    UNVALID_FACTCHUNK_ID                    /* unvalid fact chunk id */
}errorcode_enum;

/* endianness enum */
typedef enum
{
    LITTLE_ENDIAN,
    BIG_ENDIAN
}endianness_enum;

#define countof(a)                          (sizeof(a) / sizeof(*(a)))

/* audio parsing constants */
/* correspond to the letters 'RIFF' */
#define  AUDIO_CHUNKID                      0x52494646 
/* correspond to the letters 'WAVE' */
#define  AUDIO_FILEFORMAT                   0x57415645 
/* correspond to the letters 'fmt' */
#define  AUDIO_FORMATID                     0x666D7420 
/* correspond to the letters 'data' */
#define  AUDIO_DATAID                       0x64617461
/* correspond to the letters 'fact' */
#define  AUDIO_FACTID                       0x66616374 
/* correspond to the letters 'length of the fmt data' */
#define  AUDIO_FORMATLEN                    0x10

#define  AUDIO_WAVE_FORMAT_PCM              0x01
#define  AUDIO_FORMATCHUNKSIZE              0x10
#define  AUDIO_CHANNEL_MONO                 0x01
#define  AUDIO_CHANNEL_STEREO               0x02

/* bits per sample */
#define  AUDIO_BIT_PER_SAMPLE_8             8
#define  AUDIO_BIT_PER_SAMPLE_16            16

#define  AUDIO_OK                           1
#define  AUDIO_FAIL                         0

#define AUDIO_FILE_ADDRESS                  (uint32_t)wave_test_data
#define AUDIO_FILE_ADDRESS_END              (uint32_t)(wave_test_data + (countof( wave_test_data)))

/* volume control constants */
#define MAX_VOL                             0x3F
#define DEFAULT_VOL                         MAX_VOL

/* I2S configuration parameters */
#define I2S_STANDARD                        I2S_STD_MSB
#define I2S_MCLKOUTPUT                      I2S_MCKOUT_ENABLE 

/* MUTE commands */ 
#define MUTE_ON                             0xFF
#define MUTE_OFF                            0x3F

/* select pcm1770: chipselect pin low  */
#define PCM1770_CS_LOW()                    gpio_bit_reset(GPIOA, GPIO_PIN_4)

/* deselect pcm1770: chip select pin high */
#define PCM1770_CS_HIGH()                   gpio_bit_set(GPIOA, GPIO_PIN_4)

/* play staus structure */
#define PLAYING                             0x00
#define STOPPED                             0x01
#define PAUSED                              0x02
#define PLAYING_MUTE                        0x04
#define STOPPED_MUTE                        0x05
#define PAUSED_MUTE                         0x06

/* .wav audio file format parsing, and check the audio file header information is correct */
errorcode_enum audio_format_parsing(uint8_t* audio_file_header_address);
/* audio file init and set the audio file length and sample_rate */
uint8_t audio_init(void);
/* increment the audio_data_index variable */
void audio_data_index_inc(uint32_t inc_num);
/* decrease audio file repaly count */
void audio_replay_count_dec(void);
/* enable low power behavior when the mcu is in debug mode */
uint8_t audio_set_play_status(uint8_t play_status);
/* get the audio file play staus */
uint8_t audio_get_play_status(void);
/* read one half word from the audio file */
uint16_t audio_read_half_word(uint32_t address_offset);
/* configure audio replay */
void pcm1770_replay_config(uint32_t replay_count);
/* initialize SPI, I2S and audio file */
uint8_t pcm1770_init(uint32_t address);
/* send the audio data */
void pcm1770_send_data(void);
/* play the audio file */
uint32_t pcm1770_audio_play(void);
/* pause the audio file */
uint32_t pcm1770_audio_file_paused(void);
/* send software control data to pcm1770 register by SPI0 */
void pcm1770_send_ctrl_data(uint8_t reg, uint8_t data);
/* SPI0 sneds data */
void spi_send_data(uint16_t data);

#endif /* __PCM1770_H */
