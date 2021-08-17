/*!
    \file  main.h
    \brief the header file of main
*/
/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

#ifndef MAIN_H
#define MAIN_H

/* erase fmc pages from FMC_WRITE_START_ADDR to FMC_WRITE_END_ADDR */
void fmc_erase_pages(void);
/* program fmc word by word from FMC_WRITE_START_ADDR to FMC_WRITE_END_ADDR */
void fmc_program(void);
/* check fmc erase result */
void fmc_erase_pages_check(void);
/* check fmc program result */
void fmc_program_check(void);

#endif
