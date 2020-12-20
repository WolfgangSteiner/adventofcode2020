#ifndef BITMAP_H_
#define BITMAP_H_
#include <stddef.h>
#include "basetypes.h" 
#include "rect.h"

typedef struct
{
    size_t w;
    size_t h;
    u8* data;
} bitmap_t;


bitmap_t* bitmap_init(size_t w, size_t h);
void bitmap_free(bitmap_t* b);
void bitmap_flip_x(bitmap_t* bitmap);
void bitmap_flip_y(bitmap_t* bitmap);
void bitmap_flip_diag(bitmap_t* bitmap);
void bitmap_rotate(bitmap_t* bitmap);
bitmap_t* bitmap_parse(char* str);
bitmap_t* bitmap_parse_with_size(char* str, size_t w, size_t h);
void bitmap_parse_line(bitmap_t* bitmap, char* line, size_t y);
void bitmap_print(bitmap_t* b);
bool bitmap_equal_str(bitmap_t* b, char* str);
void bitmap_blit(bitmap_t* dst, rect_t dst_rect, bitmap_t* src, rect_t src_rect);
size_t bitmap_apply_filter(bitmap_t* b, bitmap_t* filter, size_t px, size_t py);
u8 bitmap_at(bitmap_t* b, size_t x, size_t y);
void bitmap_set_pixel(bitmap_t* b, size_t x, size_t y, u8 value);
size_t bitmap_count_active(bitmap_t* b);
void bitmap_filter_nand(bitmap_t* b, bitmap_t* filter, size_t x, size_t y);
#endif
