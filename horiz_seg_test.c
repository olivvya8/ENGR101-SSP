#include "horiz_seg_test.h"

void set_pixel(int row, int col, int r, int g, int b);

void drawHorizontalSegments(int segment, int row,
                            int startColumn, int endColumn)
{
    if (segment == 1) {
        for (int i = startColumn; i < endColumn; i++) {
            set_pixel(row, i, 255, 0, 0);
        }
    }
}