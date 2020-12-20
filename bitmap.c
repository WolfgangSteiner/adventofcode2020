#include "bitmap.h"
#include "stdlib.h"
#include "util.h"
#include "math.h"
#include <string.h> 
#include <stdio.h>

bitmap_t* bitmap_init(size_t w, size_t h)
{
    bitmap_t* b = calloc(1, sizeof(bitmap_t));
    b->w = w;
    b->h = h;
    b->data = calloc(w * h, sizeof(u8));
    return b;
}

void bitmap_free(bitmap_t* b)
{
    free(b->data);
    free(b);
}


void bitmap_flip_x(bitmap_t* b)
{
    for (size_t y = 0; y < b->h; ++y)
    {
        u8* row_ptr = b->data + y * b->w;
        for (size_t x = 0; x < b->w / 2; ++x)
        {
           swap_u8(row_ptr + x, row_ptr + b->w - x - 1);
        }
    }
}


void bitmap_flip_y(bitmap_t* b)
{
    for (size_t y = 0; y < b->h/2; ++y)
    {
        u8* row_ptr_a = b->data + b->w * y;
        u8* row_ptr_b = b->data + b->w * (b->h - 1 - y);

        for (size_t x = 0; x < b->w; ++x)
        {
            swap_u8(row_ptr_a + x, row_ptr_b + x);
        }
    }
}


void bitmap_flip_diag(bitmap_t* b)
{
    for (size_t y = 0; y < b->h; ++y)
    {
        for (size_t x = 0; x < b->w - y; ++x)
        {
            u8* src_ptr = b->data + y * b->w + x;
            size_t dst_x = b->h - y - 1; 
            size_t dst_y = b->w - x - 1;
            u8* dst_ptr = b->data + dst_y * b->w + dst_x;
            swap_u8(src_ptr, dst_ptr);
        }
    }
}


void bitmap_rotate(bitmap_t* bitmap)
{
    bitmap_flip_diag(bitmap);
    bitmap_flip_x(bitmap);
}

bitmap_t* bitmap_parse(char* str)
{
    size_t width = (size_t)(sqrtf(strlen(str)) + 0.1f);
    bitmap_t* b = bitmap_init(width, width);

    for (size_t i = 0; i < strlen(str); ++i)
    {
        b->data[i] = str[i] == ' ' ? 0 : 1;
    }

    return b;
}

bitmap_t* bitmap_parse_with_size(char* str, size_t w, size_t h)
{
    bitmap_t* b = bitmap_init(w, h);

    for (size_t i = 0; i < strlen(str); ++i)
    {
        b->data[i] = str[i] == ' ' ? 0 : 1;
    }

    return b;
}

void bitmap_parse_line(bitmap_t* bitmap, char* line, size_t y)
{
    assert(bitmap->w == strlen(line));
    assert(y < bitmap->h);
    u8* row_ptr = bitmap->data + bitmap->w * y;
    
    for (size_t x = 0; x < bitmap->w; x++)
    {
        char c = line[x];
        row_ptr[x] = (c == ' ' || c == '.') ? 0 : 1;
    }
} 


void bitmap_print(bitmap_t* b)
{
    for (size_t y = 0; y < b->h; ++y)
    {
        for (size_t x = 0; x < b->w; ++x)
        {
            printf("%c", b->data[y * b->w + x] == 0 ? '.' : '#');
        }
        printf("\n");
    }
}


bool bitmap_equal_str(bitmap_t* b, char* str)
{
    if (b->w * b->h != strlen(str))
    {
        return false;
    }

    for (size_t i = 0; i < strlen(str); ++i)
    {
        u8 val = (str[i] == ' ' || str[i] == '.') ? 0 : 1;
        if (b->data[i] != val) return false;
    }

    return true;
} 


void bitmap_blit(bitmap_t* dst, rect_t dst_rect, bitmap_t* src, rect_t src_rect)
{
    assert(rect_width(dst_rect) == rect_width(src_rect));
    assert(rect_height(dst_rect) == rect_height(src_rect));

    for (size_t y = 0; y < (size_t) rect_height(dst_rect); ++y)
    {
        u8* dst_ptr = dst->data + (y + dst_rect.y1) * dst->w + dst_rect.x1;
        u8* src_ptr = src->data + (y + src_rect.y1) * src->w + src_rect.x1; 
        memcpy(dst_ptr, src_ptr, rect_width(src_rect));
    }
}


u8 bitmap_at(bitmap_t* b, size_t x, size_t y)
{
    return b->data[y * b->w + x];
}


void bitmap_set_pixel(bitmap_t* b, size_t x, size_t y, u8 value)
{
    b->data[y * b->w + x] = value;
}


size_t bitmap_count_active(bitmap_t* b)
{
    size_t result = 0;
    for (size_t y = 0; y < b->h; y++)
    {
        for (size_t x = 0; x < b->w; x++)
        {
            result += bitmap_at(b, x, y);
        }
    }

    return result;
} 


size_t bitmap_apply_filter(bitmap_t* b, bitmap_t* filter, size_t px, size_t py)
{
    size_t result = 0;

    for (size_t y = 0; y < filter->h; y++)
    {
        for (size_t x = 0; x < filter->w; x++)
        {
            result += (bitmap_at(b, px + x, py + y) && bitmap_at(filter, x, y));
        }
    }

    return result;
}


void bitmap_filter_nand(bitmap_t* b, bitmap_t* filter, size_t px, size_t py)
{
    for (size_t y = 0; y < filter->h; y++)
    {
        for (size_t x = 0; x < filter->w; x++)
        {
            if (bitmap_at(b, px + x, py + y) && bitmap_at(filter, x, y))
            {
                bitmap_set_pixel(b, px + x, py + y, 0);
            }
        }
    }
}



