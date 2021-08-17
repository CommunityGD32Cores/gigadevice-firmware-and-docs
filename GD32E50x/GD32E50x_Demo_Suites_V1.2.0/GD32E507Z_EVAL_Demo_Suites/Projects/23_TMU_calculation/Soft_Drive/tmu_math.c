/*!
    \file    tmu_math.c
    \brief   tmu math function driver

    \version 2020-09-04, V1.0.0, firmware for GD32E50x
    \version 2021-03-31, V1.2.0, demo for GD32E50x
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

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

#include "tmu_math.h"

/*!
    \brief      TMU mode0 math function: x*2pi
    \param[in]  input_x: the input value of x
    \param[out] none
    \retval     the calculate value
*/
float tmu_x_multiply_2pi_math(float input_x)
{
    /* write the value to input data0 register */
    /* the vlaue must meet IEEE 32-Bit Single Precision Floating-Point Format */
    TMU_IDATA0 = *((uint32_t *)(&input_x));
    /* configure TMU use mode0*/
    tmu_mode_set(TMU_MODE0);
    /* enable the TMU interrupt*/
    tmu_interrupt_enable();
    /* start the TMU to calculation */
    tmu_enable();
    /* wait for the calculation finished */
    while(!(TMU_CTL_CFIE & TMU_CTL));
    /* read the TMU calculation value */
    return *((float *)(&TMU_DATA0));
}

/*!
    \brief      TMU mode1 math function: x/2pi
    \param[in]  input_x: the input value of x
    \param[out] none
    \retval     the calculate value
*/
float tmu_x_divide_2pi_math(float input_x)
{
    /* write the value to input data0 register */
    /* the vlaue must meet IEEE 32-Bit Single Precision Floating-Point Format */
    TMU_IDATA0 = *((uint32_t *)(&input_x));
    /* configure TMU use mode1*/
    tmu_mode_set(TMU_MODE1);
    /* enable the TMU interrupt*/
    tmu_interrupt_enable();
    /* start the TMU to calculation */
    tmu_enable();
    /* wait for the calculation finished */
    while(!(TMU_CTL_CFIE & TMU_CTL));
    /* read the TMU calculation value */
    return *((float *)(&TMU_DATA0));
}

/*!
    \brief      TMU mode2 math function: the square root of x
    \param[in]  input_x: the input value of x
    \param[out] none
    \retval     the calculate value
*/
float tmu_square_root_x_math(float input_x)
{
    /* write the value to input data0 register */
    /* the vlaue must meet IEEE 32-Bit Single Precision Floating-Point Format */
    TMU_IDATA0 = *((uint32_t *)(&input_x));
    /* configure TMU use mode2*/
    tmu_mode_set(TMU_MODE2);
    /* enable the TMU interrupt*/
    tmu_interrupt_enable();
    /* start the TMU to calculation */
    tmu_enable();
    /* wait for the calculation finished */
    while(!(TMU_CTL_CFIE & TMU_CTL));
    /* read the TMU calculation value */
    return *((float *)(&TMU_DATA0));
}

/*!
    \brief      TMU mode3 math function: sin(x*2pi)
    \param[in]  input_x: the input value of x
    \param[out] none
    \retval     the calculate value
*/
float tmu_sin_x_math(float input_x)
{
    /* write the value to input data0 register */
    /* the vlaue must meet IEEE 32-Bit Single Precision Floating-Point Format */
    TMU_IDATA0 = *((uint32_t *)(&input_x));
    /* configure TMU use mode3*/
    tmu_mode_set(TMU_MODE3);
    /* enable the TMU interrupt*/
    tmu_interrupt_enable();
    /* start the TMU to calculation */
    tmu_enable();
    /* wait for the calculation finished */
    while(!(TMU_CTL_CFIE & TMU_CTL));
    /* read the TMU calculation value */
    return *((float *)(&TMU_DATA0));
}

/*!
    \brief      TMU mode4 math function: cos(x*2pi)
    \param[in]  input_x: the input value of x
    \param[out] none
    \retval     the calculate value
*/
float tmu_cos_x_math(float input_x)
{
    /* write the value to input data0 register */
    /* the vlaue must meet IEEE 32-Bit Single Precision Floating-Point Format */
    TMU_IDATA0 = *((uint32_t *)(&input_x));
    /* configure TMU use mode4*/
    tmu_mode_set(TMU_MODE4);
    /* enable the TMU interrupt*/
    tmu_interrupt_enable();
    /* start the TMU to calculation */
    tmu_enable();
    /* wait for the calculation finished */
    while(!(TMU_CTL_CFIE & TMU_CTL));
    /* read the TMU calculation value */
    return *((float *)(&TMU_DATA0));
}

/*!
    \brief      TMU mode5 and TMU mode6 math function: arctan(X),X=x/y or X=y/x
    \param[in]  input_x: x-axis values
    \param[in]  input_y: y-axis values
    \param[out] none
    \retval     the angle value
*/
float tmu_arctan_x_y_math(float input_x, float input_y)
{
    float angle1;
    /* write the value to input data0 register */
    /* the vlaue must meet IEEE 32-Bit Single Precision Floating-Point Format */
    TMU_IDATA0 = *((uint32_t *)(&input_x));
    TMU_IDATA1 = *((uint32_t *)(&input_y));
    /* configure TMU use mode6*/
    tmu_mode_set(TMU_MODE6);
    /* enable the TMU interrupt*/
    tmu_interrupt_enable();
    /* start the TMU to calculation */
    tmu_enable();
    /* wait for the calculation finished */
    while(!(TMU_CTL_CFIE & TMU_CTL));
    /* read the TMU calculation value */
    angle1 = *((float *)(&TMU_DATA1));
    /* the mode6 R0 as inputthe mode5 inpu data */
    TMU_IDATA0 = TMU_DATA0;
    /* configure TMU use mode5*/
    tmu_mode_set(TMU_MODE5);
    /* start the TMU to calculation */
    tmu_enable();
    /* wait for the calculation finished */
    while(!(TMU_CTL_CFIE & TMU_CTL));
    angle1 = angle1 + *((float *)(&TMU_DATA0));
    /* return the angle value */
    return ((*((float *)(&angle1)))*360);
}

/*!
    \brief      TMU mode7 math function: x/y
    \param[in]  input_x: the value of input x
    \param[in]  input_y: the value of input y
    \param[out] none
    \retval     the calculate value
*/
float tmu_x_divide_y_math(float input_x, float input_y)
{
    /* write the value to input data0 register */
    /* the vlaue must meet IEEE 32-Bit Single Precision Floating-Point Format */
    TMU_IDATA0 = *((uint32_t *)(&input_x));
    TMU_IDATA1 = *((uint32_t *)(&input_y));
    /* configure TMU use mode7*/
    tmu_mode_set(TMU_MODE7);
    /* enable the TMU interrupt*/
    tmu_interrupt_enable();
    /* start the TMU to calculation */
    tmu_enable();
    /* wait for the calculation finished */
    while(!(TMU_CTL_CFIE & TMU_CTL));
    /* read the TMU calculation value */
    return *((float *)(&TMU_DATA0));
}

/*!
    \brief      TMU mode8 math function: the square root of (x2 + y2)
    \param[in]  input_x: the value of input x
    \param[in]  input_y: the value of input y
    \param[out] none
    \retval     the calculate value
*/
float tmu_square_root_x2_y2_math(float input_x, float input_y)
{
    /* write the value to input data0 register */
    /* the vlaue must meet IEEE 32-Bit Single Precision Floating-Point Format */
    TMU_IDATA0 = *((uint32_t *)(&input_x));
    TMU_IDATA1 = *((uint32_t *)(&input_y));
    /* configure TMU use mode8 */
    tmu_mode_set(TMU_MODE8);
    /* enable the TMU interrupt*/
    tmu_interrupt_enable();
    /* start the TMU to calculation */
    tmu_enable();
    /* wait for the calculation finished */
    while(!(TMU_CTL_CFIE & TMU_CTL));
    /* read the TMU calculation value */
    return *((float *)(&TMU_DATA0));
}
