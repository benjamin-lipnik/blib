#ifndef PPMLOADER_H
#define PPMLOADER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "assetloader.h"

typedef struct{	
	uint32_t width;
	uint32_t height;
	uint8_t* data;
}PPM_Image;

PPM_Image* il_load(const char* filename);
void il_img_free(PPM_Image* img);
PPM_Image* il_img_create(uint32_t width, uint32_t height);

#ifdef PPMLOADER_IMPLEMENTATION

uint8_t read_header_field(char* out_buffer, void* file_buffer, int* file_index, int max_len) {
	while(--max_len) {
		char c = ((uint8_t*)file_buffer)[(*file_index)++];
		if(c == '\n') {
			*out_buffer = 0;
			return 1;
		}
		*out_buffer++ = c;
	}
	return 0;
}

PPM_Image* il_load(const char* filename) {
	long file_size;
	uint8_t* file_buffer = load_asset((char*)filename, &file_size);
	if(!file_buffer)
		return NULL;

	char buff[256] = {0};
	PPM_Image* img = NULL;
	int vals_read = 0;
	int vals[3] = {0};

	int file_index = 0;

	if(!read_header_field(buff, file_buffer, &file_index, 255))
		goto il_load_error;
	if(strncmp(buff, "P6", 2)) {
		printf("Wrong format.\n");
		goto il_load_error;
	}

	while(vals_read != 3) {
		if(!read_header_field(buff, file_buffer, &file_index, 255))
			goto il_load_error;

		if(buff[0] == '#') { //comentar 
			//printf("%s\n", buff);
			continue;
		}

		switch(vals_read) {
			case 0:
				if(sscanf(buff, "%d %d", vals, vals+1) != 2) {
					printf("Format error 0.\n");
					goto il_load_error;
				}
				vals_read += 2;
				break;
			case 2:
				if(!sscanf(buff, "%d", vals+2)) {
					printf("Format error 1.\n");
					goto il_load_error;
				}
				vals_read += 1;
				break;
		}
	}

	if(vals[2] != 255) {
		printf("Format error 2\n");
		goto il_load_error;
	}

	img = (PPM_Image*)malloc(sizeof(PPM_Image));
	img->width = vals[0];
	img->height = vals[1];
	img->data = (uint8_t*)malloc(3*img->width*img->height);

	//fread(img->data, 3, img->width*img->height, fh);
	memcpy(img->data, file_buffer + file_index, img->width*img->height*3);
	free(file_buffer);

	return img;
	il_load_error:
		free(file_buffer);
		return NULL;
}

PPM_Image* il_img_create(uint32_t width, uint32_t height) {
	PPM_Image* img = (PPM_Image*)malloc(sizeof(PPM_Image));
	img->width = width;
	img->height = height;
	img->data = (uint8_t*)malloc(3*width*height);
	return img;
}

void il_img_free(PPM_Image* img) {
	free(img->data);
	free(img);
}

#endif
#endif
