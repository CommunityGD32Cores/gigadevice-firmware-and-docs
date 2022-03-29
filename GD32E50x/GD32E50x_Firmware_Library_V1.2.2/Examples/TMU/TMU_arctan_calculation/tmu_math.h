/*!
    \file    tmu_math.h
    \brief   definitions for the tmu math

    \version 2020-09-30, V1.0.0, firmware for GD32E50x
    \version 2021-03-23, V1.2.0, firmware for GD32E50x
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

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

#ifndef TMU_MATH_H
#define TMU_MATH_H

#include "gd32e50x.h"

/* function declarations */
/* TMU mode0 math function: x*2pi */
float tmu_x_multiply_2pi_math(float input_x);
/* TMU mode1 math function: x/2pi */
float tmu_x_divide_2pi_math(float input_x);
/* TMU mode2 math function: the square root of x */
float tmu_square_root_x_math(float input_x);
/* TMU mode3 math function: sin(x*2pi) */
float tmu_sin_x_math(float input_x);
/* TMU mode4 math function: cos(x*2pi) */
float tmu_cos_x_math(float input_x);
/* TMU mode5 and TMU mode6 math function: arctan(X),X=x/y or X=y/x */
float tmu_arctan_x_y_math(float input_x, float input_y);
/* TMU mode7 math function: x/y */
float tmu_x_divide_y_math(float input_x, float input_y);
/* TMU mode8 math function: the square root of (x2 + y2) */
float tmu_square_root_x2_y2_math(float input_x, float input_y);

#endif /* TMU_MATH_H */
