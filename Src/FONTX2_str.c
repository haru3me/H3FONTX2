/*
 * FONTX2_str.c
 *
 *  Created on: May 12, 2024
 *      Author: haru3me
 */

#include "FONTX2_str.h"

FONTX2_status_t FONTX2_str_init(FONTX2_str_handler_t* handler, uint8_t space,
								FONTX2_handler_t* hFont, FONTX2_handler_t* zFont,
								uint16_t canvas_width, uint16_t canvas_height,
								uint8_t* pbuf){
	if(hFont == NULL && zFont == NULL)
		return FONTX2_FAIL;
	if(canvas_width == 0 || canvas_height == 0)
		return FONTX2_FAIL;
	if(pbuf == NULL)
		return FONTX2_FAIL;

	handler->space = space;
	handler->hFont = hFont;
	handler->zFont = zFont;

	handler->canvas = pbuf;
	handler->canvas_width = canvas_width;
	handler->canvas_height = canvas_height;

	handler->cursor_x = 0;
	handler->cursor_y = 0;
	return FONTX2_OK;
}

FONTX2_status_t FONTX2_str_puts(FONTX2_str_handler_t* handler, char* str, uint8_t color){
	uint16_t slen = strlen(str);
	uint32_t ptr = 0;
	uint8_t chr;

	while(ptr<slen){
		chr = *((uint8_t*)(str+ptr));
		if((chr >= 0x81 && chr <= 0x9f) || (chr >= 0xe0 && chr <= 0xfc)){
			//s-jis
			if(handler->zFont == NULL){ //skip
				ptr+=2;
				continue;
			}
			if(handler->zFont->font_width > (handler->canvas_width - handler->cursor_x)){
				handler->cursor_y += handler->zFont->font_height;
				handler->cursor_x = 0;
			}
			if(handler->cursor_y + handler->zFont->font_height > handler->canvas_height){
				break; //no space
			}

			uint16_t zchr = chr << 8 | *((uint8_t*)(str+ptr+1));
			FONTX2_Wchar(handler->zFont, zchr, handler->cursor_x, handler->cursor_y, handler->canvas, handler->canvas_width, color);
			ptr+=2;
			handler->cursor_x += handler->zFont->font_width + handler->space;
		}else{
			//ANK
			if(handler->hFont == NULL){ //skip
				ptr+=1;
				continue;
			}
			if(chr == '\n'){
				handler->cursor_y += handler->hFont->font_height;
				handler->cursor_x = 0;
				ptr+=1;
				continue;
			}
			if(handler->hFont->font_width > (handler->canvas_width - handler->cursor_x)){
				handler->cursor_y += handler->hFont->font_height;
				handler->cursor_x = 0;
			}
			if(handler->cursor_y + handler->hFont->font_height > handler->canvas_height){
				break; //no space
			}

			FONTX2_Hchar(handler->hFont, chr, handler->cursor_x, handler->cursor_y, handler->canvas, handler->canvas_width, color);
			ptr+=1;
			handler->cursor_x += handler->hFont->font_width + handler->space;
		}
	}

	return FONTX2_OK;
}
