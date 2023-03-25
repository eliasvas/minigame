#ifndef MQOI_H
#define MQOI_H
#include "base.h"
//this is gooooood https://www.gingerbill.org/series/memory-allocation-strategies/

#define MQOI_OP_INDEX  0x00 /* 00xxxxxx */
#define MQOI_OP_DIFF   0x40 /* 01xxxxxx */
#define MQOI_OP_LUMA   0x80 /* 10xxxxxx */
#define MQOI_OP_RUN    0xc0 /* 11xxxxxx */
#define MQOI_OP_RGB    0xfe /* 11111110 */
#define MQOI_OP_RGBA   0xff /* 11111111 */
#define MQOI_MASK_2    0xc0 /* 11000000 */


static const u8 qoi_padding[8] = {0,0,0,0,0,0,0,1};

typedef struct {
	u32 width, height;
	u8 channels, colorspace;
}mqoiDesc;

typedef union {
	struct {u8 r,g,b,a;};
	u32 v;
}mqoiPixel;

static inline u8 mqoi_hash(mqoiPixel p){
	return (p.r * 3 + p.g * 5 + p.b * 7 + p.a * 11) % 64;
}

static inline u32 mqoi_read_32(const u8 *bytes, int *p) {
	u8 a = bytes[(*p)++];
	u8 b = bytes[(*p)++];
	u8 c = bytes[(*p)++];
	u8 d = bytes[(*p)++];
	return a << 24 | b << 16 | c << 8 | d;
}

static inline void *qoi_decode(void *data, u32 data_size, mqoiDesc *desc){
	if (data == NULL || desc == NULL || desc->colorspace > 1){
		return NULL;
	}
	u32 p = 0;
	const u8 *bytes = (const u8*)data;
	u32 header_magic = mqoi_read_32(bytes, &p);
	desc->width = mqoi_read_32(bytes, &p);
	desc->height = mqoi_read_32(bytes, &p);
	desc->channels = bytes[p++];
	desc->colorspace = bytes[p++];
	if (desc->width == 0 || desc->height == 0 || desc->channels < 3 || desc->channels >4 || desc->colorspace > 1){
		return NULL;
	}




	u32 pcount = desc->width *desc->height * desc->channels;
	u8 *pdata = MALLOC(pcount);
	if (pdata == NULL){
		return NULL;
	}

	u32 index[64];
	MEMZERO_ARRAY(index);
	mqoiPixel prev_pixel = {0};
	prev_pixel.a = 255;
	mqoiPixel pixel = prev_pixel;
	u32 run = 0;


	u32 chunks_len = data_size - (int)sizeof(qoi_padding);
	for (u32 px_pos = 0; px_pos < pcount; px_pos+=desc->channels){
		if (run > 0){
			--run;
		}else if (p < chunks_len) {
			u32 b1 = bytes[p++];

			if (b1 == MQOI_OP_RGB) {
				pixel.r = bytes[p++];
				pixel.g = bytes[p++];
				pixel.b = bytes[p++];
			}
			else if (b1 == MQOI_OP_RGBA) {
				pixel.r = bytes[p++];
				pixel.g = bytes[p++];
				pixel.b = bytes[p++];
				pixel.a = bytes[p++];
			}
			else if ((b1 & MQOI_MASK_2) == MQOI_OP_INDEX) {
				pixel.v = index[b1];
			}
			else if ((b1 & MQOI_MASK_2) == MQOI_OP_DIFF) {
				pixel.r += ((b1 >> 4) & 0x03) - 2;
				pixel.g += ((b1 >> 2) & 0x03) - 2;
				pixel.b += ( b1       & 0x03) - 2;
			}
			else if ((b1 & MQOI_MASK_2) == MQOI_OP_LUMA) {
				int b2 = bytes[p++];
				int vg = (b1 & 0x3f) - 32;
				pixel.r += vg - 8 + ((b2 >> 4) & 0x0f);
				pixel.g += vg;
				pixel.b += vg - 8 +  (b2       & 0x0f);
			}
			else if ((b1 & MQOI_MASK_2) == MQOI_OP_RUN) {
				run = (b1 & 0x3f);
			}

			index[mqoi_hash(pixel) % 64] = pixel.v;
		}
		
		
		pdata[px_pos + 0] = pixel.r;
		pdata[px_pos + 1] = pixel.g;
		pdata[px_pos + 2] = pixel.b;
		if (desc->channels == 4){
			pdata[px_pos + 3] = pixel.a;
		}

	}


	/*
	for (u32 i = 0; i < desc->width * desc->height * desc->channels; i += channels){
		pdata[i + 0] = 255*i/(f32)(desc->width * desc->height * channels);
		pdata[i + 1] = 255*i/(f32)(desc->width * desc->height * channels);
		pdata[i + 2] = 255*i/(f32)(desc->width * desc->height * channels);
		pdata[i + 3] = 255;
	}
	*/
	return pdata;
}


static inline void *mqoi_load(char *filename, mqoiDesc *desc){
	FILE *f = fopen(filename, "rb");
	u32 size, bytes_read;
	void *pixels, *data;

	if (!f) {
		return NULL;
	}

	fseek(f, 0, SEEK_END);
	size = ftell(f);
	if (size <= 0) {
		fclose(f);
		return NULL;
	}
	fseek(f, 0, SEEK_SET);

	data = MALLOC(size);
	if (!data) {
		fclose(f);
		return NULL;
	}

	bytes_read = fread(data, 1, size, f);
	fclose(f);

	pixels = qoi_decode(data, bytes_read, desc);
	FREE(data);
	return pixels;
}
#endif