/*
 * FONTX2.c
 *
 *  Created on: May 11, 2024
 *      Author: haru3me
 */

#include "FONTX2.h"

#ifdef FONTX2_USE_FATFS
FONTX2_status_t FONTX2_read_FF(void* handle,uint32_t addr, void* data, uint32_t len){
	UINT br;
	FRESULT fres;
	fres = f_lseek(((FONTX2_handler_t*)handle)->pFile, addr);
	if(fres != FR_OK)
		return FONTX2_FAIL;
	fres = f_read(((FONTX2_handler_t*)handle)->pFile, data, len, &br);
	if(fres != FR_OK)
			return FONTX2_FAIL;
	return (br==len)?FONTX2_OK:FONTX2_FAIL;
}
#endif

FONTX2_status_t FONTX2_draw(FONTX2_handler_t* pHandle,uint32_t xoffs,uint32_t yoffs,
							uint8_t* destbuf,uint16_t bufwidth,uint8_t color){
	for(uint8_t y = 0;y < pHandle->font_height;y++){
		uint8_t x = 0;
		for(uint8_t xb = 0;xb < pHandle->bytes_x;xb++){
			uint8_t line = *((uint8_t*)(pHandle->buf + pHandle->bytes_x * y + xb));
			uint8_t shift = pHandle->font_width - x > 8?8:pHandle->font_width - x;
			uint8_t xs = 0;
			while(xs < shift){
				if(line & (1 << (7 - xs)))
					destbuf[bufwidth*(y+yoffs)+(x+xoffs)] = color;
				x++;
				xs++;
			}
		}
	}
	return FONTX2_OK;
}

FONTX2_status_t FONTX2_Hchar(FONTX2_handler_t* pHandle,uint8_t c,
								uint32_t x,uint32_t y,
								uint8_t* destbuf,uint16_t bufwidth,uint8_t color){
	if(pHandle->char_code != 0)
			return FONTX2_FAIL;
	uint32_t offset = 17 + pHandle->bytes_char * c;
	if(pHandle->func_read(pHandle,offset,pHandle->buf,pHandle->bytes_char) == FONTX2_FAIL)
		return FONTX2_FAIL;
	return FONTX2_draw(pHandle, x, y, destbuf, bufwidth, color);
}

FONTX2_status_t FONTX2_Wchar(FONTX2_handler_t* pHandle,uint16_t c,
								uint32_t x,uint32_t y,
								uint8_t* destbuf,uint16_t bufwidth,uint8_t color){
	if(pHandle->char_code != 1)
		return FONTX2_FAIL;

	uint8_t ptr = 0;
	uint16_t cbstart, cbend;
	uint32_t offset, sum = 0;

	//search code block
	if(pHandle->cache_codeblock != NULL){ // cached cb
		while(ptr < pHandle->num_codeblock){
			cbstart = ((uint16_t*)pHandle->cache_codeblock)[2*ptr];
			cbend = ((uint16_t*)pHandle->cache_codeblock)[2*ptr+1];
			if(c >= cbstart && c <= cbend)
				break;
			ptr++;
			sum += cbend - cbstart + 1;
			if(ptr == pHandle->num_codeblock || ptr == 0) //no match in cb
				return FONTX2_FAIL;
		}
	}else{
		uint8_t cbbuf[4];
		while(ptr < pHandle->num_codeblock){
			if(pHandle->func_read(pHandle,18+4*ptr,cbbuf,4) == FONTX2_FAIL)
				return FONTX2_FAIL;


			cbstart = ((uint16_t*)cbbuf)[0];
			cbend =  ((uint16_t*)cbbuf)[1];

			if(c >= cbstart && c <= cbend)
				break;
			ptr++;
			sum += cbend - cbstart + 1;
			if(ptr == pHandle->num_codeblock || ptr == 0) //no match in cb
				return FONTX2_FAIL;
		}
	}

	offset = (18 + 4 * pHandle->num_codeblock) + (sum + (c - cbstart)) * pHandle->bytes_char;
	if(pHandle->func_read(pHandle,offset,pHandle->buf,pHandle->bytes_char) == FONTX2_FAIL)
		return FONTX2_FAIL;

	return FONTX2_draw(pHandle, x, y, destbuf, bufwidth, color);
}

FONTX2_status_t FONTX2_init_common(FONTX2_handler_t* pHandle,uint8_t isCacheCB){
	uint8_t read_buffer[18];
	const uint8_t signature[] = {'F','O','N','T','X','2'};
	FONTX2_status_t result;
	result = pHandle->func_read(pHandle,0,read_buffer,18);
	if(result == FONTX2_FAIL)
		return result;

	for(uint8_t i=0;i < 6;i++){
		if(read_buffer[i] != signature[i])
			return FONTX2_FAIL;
	}

	memcpy(pHandle->font_name,read_buffer+6,8);
	pHandle->font_width = read_buffer[14];
	pHandle->font_height = read_buffer[15];
	pHandle->char_code = read_buffer[16];

	switch(pHandle->char_code){
	case 0: //ANK
		//nothing to do
		break;

	case 1: //s-jis
		pHandle->num_codeblock = read_buffer[17];
		if(isCacheCB){
			pHandle->cache_codeblock = malloc(pHandle->num_codeblock*4); //uint16*2
			if(pHandle->cache_codeblock != NULL){
				result = pHandle->func_read(pHandle,18,pHandle->cache_codeblock,
									pHandle->num_codeblock*4);
				if(result == FONTX2_FAIL){
					free(pHandle->cache_codeblock);
					pHandle->cache_codeblock = NULL;
				}
			}
		}
		break;
	default:
		return FONTX2_FAIL;
	}

	pHandle->bytes_x = (pHandle->font_width+7) / 8;
	pHandle->bytes_char = pHandle->bytes_x * pHandle->font_height;
	pHandle->buf = malloc(pHandle->bytes_char);
	if(pHandle->buf == NULL){
		if(pHandle->cache_codeblock){
			free(pHandle->cache_codeblock);
			pHandle->cache_codeblock = NULL;
		}
		return FONTX2_FAIL;
	}
	return FONTX2_OK;
}

#ifdef FONTX2_USE_FATFS
FONTX2_status_t FONTX2_init(FONTX2_handler_t* pHandle, FIL *pFile, uint8_t isCacheCB){
	memset(pHandle,0x00,sizeof(FONTX2_handler_t));
	pHandle->pFile = pFile;
	pHandle->func_read = FONTX2_read_FF;
	return FONTX2_init_common(pHandle, isCacheCB);
}
#else
FONTX2_status_t FONTX2_init(FONTX2_handler_t* pHandle, FONTX2_read pRead, uint8_t isCacheCB){
	memset(pHandle,0x00,sizeof(FONTX2_handler_t));
	pHandle->func_read = pRead;
	return FONTX2_init_common(pHandle, isCacheCB);
}
#endif

