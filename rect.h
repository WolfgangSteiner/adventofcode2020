#ifndef RECT_H_
#define RECT_H_

typedef struct
{
  int x1;
  int y1;
  int x2;
  int y2;
} rect_t;

#define rect_width(RECT) (RECT.x2 - RECT.x1)
#define rect_height(RECT) (RECT.y2 - RECT.y1)

#endif
