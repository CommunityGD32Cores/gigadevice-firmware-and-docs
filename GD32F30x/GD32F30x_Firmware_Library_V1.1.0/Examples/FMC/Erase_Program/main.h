/*!
    \file  main.h
    \brief the header file of main
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.1, firmware for GD32F30x
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

