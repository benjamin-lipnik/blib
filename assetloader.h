#ifndef ASSET_LOADER_H
#define ASSET_LOADER_H

#include <stdio.h>
#include <stdlib.h>

void init_loader(void* args);
void* load_asset(char* filename, long* out_size);

#ifdef ASSET_LOADER_IMPLEMENTATION

#include <stdint.h>
#include <string.h>

#ifdef ANDROID

#include <android_native_app_glue.h>
#include <android/asset_manager.h>

static struct android_app* al_gapp;
void init_loader(void* args) {
	al_gapp = (struct android_app*)args;
}
void* load_asset(char* filename, long* out_size) {
	AAsset * asset = AAssetManager_open( al_gapp->activity->assetManager, filename, AASSET_MODE_BUFFER );
	if(!asset)  {
		return NULL;
	}

	size_t file_length = AAsset_getLength(asset);	
	uint8_t* data = malloc(file_length+1);
	data[file_length] = 0;

	memcpy(data, AAsset_getBuffer(asset), file_length);
	*out_size = file_length;

	AAsset_close(asset);
	return data;
}
#else
	void init_loader(void* args) {}
	void* load_asset(char* fname, long* out_size) {
		char filename[1000];
		sprintf(filename, "assets/%s", fname);
		FILE* fh = fopen(filename, "r");
		if(!fh)
			return NULL;

		long file_size;
		fseek(fh, 0, SEEK_END);
		file_size = ftell(fh);
		fseek(fh, 0, SEEK_SET);

		uint8_t* data = malloc(file_size + 1);
		data[file_size] = 0;
		fread(data, 1, file_size, fh);
		fclose(fh);	

		*out_size = file_size;
		return data;
	}
#endif

#endif
#endif
