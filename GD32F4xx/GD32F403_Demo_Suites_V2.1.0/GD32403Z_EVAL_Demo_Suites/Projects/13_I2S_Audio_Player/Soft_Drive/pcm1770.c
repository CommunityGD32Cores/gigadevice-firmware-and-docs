/*!
    \file  pcm1770.c
    \brief the read and write function file

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

#include "pcm1770.h"
#include "spi_i2s.h"
#include "test_wave_data.h"
#include <stdio.h>

/* audio frequency value */
uint16_t i2s_audio_freq    = I2S_AUDIOSAMPLE_8K;
uint8_t audio_header_index = 0;
uint8_t *audio_file_header = 0;

/* audio codec variables */
wave_file_struct  wave_format;
__IO uint32_t audio_file_address = 0; 
uint32_t audio_data_length = countof(wave_test_data);
uint32_t data_start_addr = 0x0;
__IO uint32_t audio_data_index = 0;
uint8_t audio_play_status = STOPPED;
static uint32_t audio_replay_remain_count = 0xFFFF;
static __IO uint32_t audio_replay_count = 0xFFFF;
static __IO uint32_t monovar = 0;

static uint32_t audio_read_unit(uint8_t bytes_num, endianness_enum bytes_endian);

/*!
    \brief      .wav audio file format parsing, and check the audio file header information is correct
    \param[in]  audio_file_header_address: audio file address
    \param[out] none
    \retval     error code
*/
errorcode_enum audio_format_parsing(uint8_t* audio_file_header_address)
{
    uint32_t extra_format_bytes = 0;

    /* set audio file header address */
    audio_file_header = audio_file_header_address;

    /* init the audio_header_index */
    audio_header_index = 0;

    /* read the chunkid, and it must be 'RIFF' */
    if(AUDIO_CHUNKID != audio_read_unit(4, BIG_ENDIAN)){
        return(UNVALID_RIFF_ID);
    }
    
    /* read the audio file length */
    wave_format.riff_chunk_size = audio_read_unit(4, LITTLE_ENDIAN);

    /* read the audio file format, and it must be 'WAVE' */
    if(AUDIO_FILEFORMAT != audio_read_unit(4, BIG_ENDIAN)){
        return(UNVALID_WAVE_FORMAT);
    }
    
    /* read the format chunk, and it must be 'fmt' */
    if(AUDIO_FORMATID != audio_read_unit(4, BIG_ENDIAN)){
        return(UNVALID_FORMATCHUNK_ID);
    }
    
    /* read the length of the 'fmt' data, and it must be 0x10 */
    if(AUDIO_FORMATLEN != audio_read_unit(4, LITTLE_ENDIAN)){
        extra_format_bytes = 1;
    }
    
    /* read the audio format, and it must be 0x01 (PCM) */
    wave_format.format_tag = audio_read_unit(2, LITTLE_ENDIAN);
    if(AUDIO_WAVE_FORMAT_PCM != wave_format.format_tag){
        return(UNSUPPORETD_FORMATTAG);    
    }
    
    /* read the number of channels: 0x02->Stereo and 0x01->Mono */
    wave_format.num_channels = audio_read_unit(2, LITTLE_ENDIAN);

    /* read the sample rate */
    wave_format.sample_rate = audio_read_unit(4, LITTLE_ENDIAN);

    /* read the byte rate */
    wave_format.byte_rate = audio_read_unit(4, LITTLE_ENDIAN);

    /* read the block alignment */
    wave_format.block_align = audio_read_unit(2, LITTLE_ENDIAN);

    /* read the number of bits per sample */
    wave_format.bits_per_sample = audio_read_unit(2, LITTLE_ENDIAN);
    if(AUDIO_BIT_PER_SAMPLE_16 != wave_format.bits_per_sample){
        return(UNSUPPORETD_BITS_PER_SAMPLE);
    }

    /* if there are extra format bytes, these bytes will be defined in "Fact Chunk" */
    if(1 == extra_format_bytes){
        /* read the extra format bytes, and it must be 0x00 */
        if(0x00 != audio_read_unit(2, LITTLE_ENDIAN)){
            return(UNSUPPORETD_EXTRAFORMATBYTES);
        }
        /* read the fact chunk, and it must be 'fact' */
        if(AUDIO_FACTID != audio_read_unit(4, BIG_ENDIAN)){
            return(UNVALID_FACTCHUNK_ID);
        }
        /* read fact chunk data size and set the index to start reading after the header end */
        audio_header_index += audio_read_unit(4, LITTLE_ENDIAN);
    }
    
    /* read the data chunk, and it must be 'data' */
    if(AUDIO_DATAID != audio_read_unit(4, BIG_ENDIAN)){
        return(UNVALID_DATACHUNK_ID);
    }
    /* read the size of sample data */
    wave_format.data_size = audio_read_unit(4, LITTLE_ENDIAN);
    /* set first address of the effective audio data */
    data_start_addr += audio_header_index;

    return(VALID_WAVE_FILE);
}

/*!
    \brief      read some bytes from the audio file header in big or little endian
    \param[in]  bytes_num: number of bytes to read
    \param[in]  bytes_endian: specifie the bytes in big or little endian
    \param[out] none
    \retval     data to read
*/
static uint32_t audio_read_unit(uint8_t bytes_num, endianness_enum bytes_endian)
{
    uint32_t i = 0;
    uint32_t temp = 0;

    if(LITTLE_ENDIAN == bytes_endian){
        for(i = 0; i < bytes_num; i++){
            temp |= audio_file_header[audio_header_index++] << (i * 8);
        }
    }else{
        for(i = bytes_num; i != 0; i--){
            temp |= audio_file_header[audio_header_index++] << ((i-1) * 8);
        }
    }
    return temp;
}

/*!
    \brief      configure audio replay 
    \param[in]  replay_count: audio file replay count, and if replay_count = 0, audio file paly infinite loop
    \param[out] none
    \retval     none
*/
void pcm1770_replay_config(uint32_t replay_count)
{ 
    /* set audio replay count by user */
    audio_replay_count = replay_count;

    /* audio replay remaining count (if audio_replay_count != 0) */
    audio_replay_remain_count = replay_count;
}

/*!
    \brief      initialize SPI, I2S and audio file
    \param[in]  address: audio file first address
    \param[out] none
    \retval     AUDIO_OK and AUDIO_FAIL
*/
uint8_t pcm1770_init(uint32_t address)
{ 
    /* set the audio file address */
    audio_file_address = (uint32_t) address;
    
    /* if replay_count = 0, audio file paly infinite loop */
    audio_replay_count = 0;
    
    /* audio file init */ 
    if(AUDIO_FAIL == audio_init()){
        return AUDIO_FAIL;
    }

    /* configure the I2S1 and SPI0 GPIO */
    spi_i2s_gpio_config();
    /* configure the I2S1 and SPI0 */
    spi_i2s_config(I2S_STANDARD, I2S_MCLKOUTPUT, i2s_audio_freq);

    return AUDIO_OK; 
}

/*!
    \brief      send the audio data
    \param[in]  none
    \param[out] none
    \retval     none
*/
void pcm1770_send_data(void)
{
    /* send the data read from the memory */
    spi_i2s_data_transmit(SPI1, (audio_read_half_word(audio_data_index + data_start_addr)));

    /* increment the index */
    audio_data_index_inc(wave_format.num_channels); 
}

/*!
    \brief      play the audio file
    \param[in]  none
    \param[out] none
    \retval     the address offset of palying the audio file
*/
uint32_t pcm1770_audio_play()
{   
    /* set playing status as PLAYING or PLAYING_MUTE */
    if((audio_play_status >> 2) == 1){
        audio_set_play_status(PLAYING_MUTE);
    }else{
        audio_set_play_status(PLAYING);
    }

    /* enable the I2S1 TBE interrupt */ 
    spi_i2s_interrupt_enable(SPI1, SPI_I2S_INT_TBE);
    
    return audio_data_index;
}

/*!
    \brief      pause the audio file
    \param[in]  none
    \param[out] none
    \retval     the address offset of paused the audio file
*/
uint32_t pcm1770_audio_file_paused()
{   
    /* set playing status as PAUSED or PAUSED_MUTE */
    if(1 == (audio_play_status >> 2)){
        audio_set_play_status(PAUSED_MUTE);
    }else{
        audio_set_play_status(PAUSED);
    }
    /* disable the I2S1 TBE interrupt */ 
    spi_i2s_interrupt_disable(SPI1, SPI_I2S_INT_TBE);
    
    return audio_data_index;
}

/*!
    \brief      increment the audio_data_index variable
    \param[in]  inc_num: number of increment the AudioDataIndex variable
    \param[out] none
    \retval     none
*/
void audio_data_index_inc(uint32_t inc_num)
{ 
    audio_data_index += (uint32_t)inc_num;
    
    if(audio_data_index >= audio_data_length){
        audio_replay_count_dec();
    }
}

/*!
    \brief      decrease audio file repaly count
    \param[in]  none
    \param[out] none
    \retval     none
*/
void audio_replay_count_dec(void)
{ 
    if(audio_replay_count != 0){
        audio_replay_remain_count--;

        if(0 == audio_replay_remain_count){
            /* set playing status as STOPPED or STOPPED_MUTE */
            if(1 == (audio_play_status >> 2)){
                audio_set_play_status(STOPPED_MUTE);
            }else{
                audio_set_play_status(STOPPED);
            }
            /* disable the I2S1 TBE interrupt */ 
            spi_i2s_interrupt_disable(SPI1, SPI_I2S_INT_TBE);
        }
    }   
}

/*!
    \brief      enable low power behavior when the mcu is in debug mode
    \param[in]  play_status: the audio file play staus
                this parameter can be any combination of the following values:
      \arg        PLAYING: the audio is playing
      \arg        STOPPED: the audio is stopped
      \arg        PAUSED: the audio is paused
      \arg        PLAYING_MUTE: the audio is playing and mute
      \arg        STOPPED_MUTE: the audio is stopped and mute
      \arg        PAUSED_MUTE: the audio is paused and mute
    \param[out] none
    \retval     the audio file play staus
*/
uint8_t audio_set_play_status(uint8_t play_status)
{ 
    audio_play_status = play_status;
   
    return audio_play_status;
}

/*!
    \brief      get the audio file play staus
    \param[in]  none
    \param[out] none
    \retval     the audio file play staus
*/
uint8_t audio_get_play_status(void)
{    
    return audio_play_status;
}

/*!
    \brief      read one half word from the audio file
    \param[in]  address_offset: the adress offset.
    \param[out] none
    \retval     data to read from the audio file
*/
uint16_t audio_read_half_word(uint32_t address_offset)
{ 
    uint32_t value = 0U;
    
    if((audio_file_address + address_offset ) >= AUDIO_FILE_ADDRESS_END){
        audio_data_index = 0;
    }

    /* the left channel is to be sent */
    if(0U == monovar ){
        value =  (*(__IO uint16_t *) (audio_file_address + address_offset));
        /* increment the monovar variable if the audio file is in mono format */   
        if(AUDIO_CHANNEL_MONO == wave_format.num_channels){
            /* increment the monovar variable */
            monovar++;
        }
        return value;
    }else{
        /* right channel to be sent in mono format */
        /* reset the monovar variable */
        monovar = 0U;
        /* return the previous read data in mono format */
        return value;
    }
}

/*!
    \brief      audio file init and set the audio file length and sample_rate
    \param[in]  none
    \param[out] none
    \retval     AUDIO_OK and AUDIO_FAIL
*/
uint8_t audio_init(void)
{ 
    /* check if the audio file is a valid .wav file */
    if(VALID_WAVE_FILE == audio_format_parsing((uint8_t*)(audio_file_address))){
        /* set the audio frequency */
        i2s_audio_freq = (uint16_t)wave_format.sample_rate;
        return AUDIO_OK;
    }
    return AUDIO_FAIL; 
}

/*!
    \brief      send software control data to pcm1770 register by SPI0
    \param[in]  reg: the address of pcm1770 register
    \param[in]  data: control data to send
    \param[out] none
    \retval     none
*/
void pcm1770_send_ctrl_data(uint8_t reg, uint8_t data)
{
    uint16_t reg_data, i;

    /* send software control data to pcm1770 register */
    reg_data = (0x00FF & reg);
    reg_data = reg_data << 8;
    reg_data = reg_data | data;
    /* chip select low */
    PCM1770_CS_LOW();
    /* SPI send data */
    spi_send_data(reg_data);
    /* MLH >= 20ns*/
    for(i=0; i<0xFFU; i++);
    /* chip select high */
    PCM1770_CS_HIGH();
}

/*!
    \brief      SPI0 sneds data
    \param[in]  data: data to send
    \param[out] none
    \retval     none
*/
void spi_send_data(uint16_t data)
{
    /* loop while DTR register in not emplty */
    while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE));

    /* send byte through the SPI0 */
    spi_i2s_data_transmit(SPI0, data);
}
