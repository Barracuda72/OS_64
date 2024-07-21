/*
 * vesa.h
 *
 * Простой видеодрайвер VESA-режима
 *
 */

#ifndef __VESA_H__
#define __VESA_H__

#include <stdint.h>

// Логический адрес начала буфера кадров
#define VESA_FB_ADDR ((void*)0xFFFFFFFFB0000000)

#pragma pack(1)
typedef struct __attribute__((packed))
{
    uint16_t mode_attr;
    uint8_t win_attr[2];
    uint16_t win_grain;
    uint16_t win_size;
    uint16_t win_seg[2];
    uint32_t win_scheme;
    uint16_t logical_scan;

    uint16_t h_res;
    uint16_t v_res;
    uint8_t char_width;
    uint8_t char_height;
    uint8_t memory_planes;
    uint8_t bpp;
    uint8_t banks;
    uint8_t memory_layout;
    uint8_t bank_size;
    uint8_t image_pages;
    uint8_t page_function;

    uint8_t rmask;
    uint8_t rpos;
    uint8_t gmask;
    uint8_t gpos;
    uint8_t bmask;
    uint8_t bpos;
    uint8_t resv_mask;
    uint8_t resv_pos;
    uint8_t dcm_info;

    uint8_t *lfb_ptr;   /* Linear frame buffer address */
    uint8_t *offscreen_ptr; /* Offscreen memory address */
    uint16_t offscreen_size;

    uint8_t reserved[206];
} vesa_mode_info_t;
#pragma pack()

void vesa_init(vesa_mode_info_t *vi);

#endif // __VESA_H__
