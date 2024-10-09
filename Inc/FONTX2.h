/*
 * FONTX2.h
 *
 *  Created on: May 12, 2024
 *      Author: haru3me
 */

#ifndef FONTX2_INC_FONTX2_H_
#define FONTX2_INC_FONTX2_H_
#include "stdint.h"
#include "string.h"
#include "stdlib.h"

//#define FONTX2_USE_FATFS

#ifdef FONTX2_USE_FATFS
#include "ff.h"
#endif

typedef enum {
	FONTX2_FAIL = 0x00,
	FONTX2_OK	= 0x01,
} FONTX2_status_t;

typedef uint16_t FONTX2_color;

typedef FONTX2_status_t (*FONTX2_read)(void* handle,uint32_t addr,void* data,uint32_t len);

typedef struct {
#ifdef FONTX2_USE_FATFS
	FIL *pFile;
#endif
	FONTX2_read func_read;
	uint8_t font_name[8];
	uint8_t font_width;
	uint8_t font_height;
	uint8_t char_code;
	uint8_t num_codeblock;
	uint8_t* cache_codeblock;
	uint8_t bytes_x;
	uint16_t bytes_char;
	uint8_t* buf;
} FONTX2_handler_t;

#ifdef FONTX2_USE_FATFS
FONTX2_status_t FONTX2_init(FONTX2_handler_t* pHandle, FIL *pFile, uint8_t isCacheCB);
#else
FONTX2_status_t FONTX2_init(FONTX2_handler_t* pHandle, FONTX2_read pRead, uint8_t isCacheCB);
#endif
FONTX2_status_t FONTX2_Wchar(FONTX2_handler_t* pHandle,uint16_t c,
								uint32_t x,uint32_t y,
								FONTX2_color* destbuf,uint16_t bufwidth,FONTX2_color color);
FONTX2_status_t FONTX2_Hchar(FONTX2_handler_t* pHandle,uint8_t c,
								uint32_t x,uint32_t y,
								FONTX2_color* destbuf,uint16_t bufwidth,FONTX2_color color);
#endif /* FONTX2_INC_FONTX2_H_ */
