/*
 * FONTX2_str.h
 *
 *  Created on: May 12, 2024
 *      Author: haru3me
 */

#ifndef FONTX2_INC_FONTX2_STR_H_
#define FONTX2_INC_FONTX2_STR_H_

#include "FONTX2.h"

typedef struct {
	uint8_t space;
	FONTX2_handler_t* hFont;
	FONTX2_handler_t* zFont;
	uint8_t* canvas;
	uint16_t canvas_width;
	uint16_t canvas_height;
	uint16_t cursor_x;
	uint16_t cursor_y;
} FONTX2_str_handler_t;


FONTX2_status_t FONTX2_str_init(FONTX2_str_handler_t* handler, uint8_t space,
								FONTX2_handler_t* hFont, FONTX2_handler_t* zFont,
								uint16_t canvas_width, uint16_t canvas_height,
								uint8_t* pbuf);
FONTX2_status_t FONTX2_str_puts(FONTX2_str_handler_t* handler, char* str, uint8_t color);
#endif /* FONTX2_INC_FONTX2_STR_H_ */
