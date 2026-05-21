#include <stdio.h>
#include <stdlib.h>
#include "horiz_seg_test.h"

static int actual_calls = 0;
static int actual_row[256];
static int actual_col[256];
static int actual_r[256];
static int actual_g[256];
static int actual_b[256];

void set_pixel(int row, int col, int r, int g, int b)
{
    if (actual_calls < 256) {
        actual_row[actual_calls] = row;
        actual_col[actual_calls] = col;
        actual_r[actual_calls] = r;
        actual_g[actual_calls] = g;
        actual_b[actual_calls] = b;
    }
    actual_calls++;
}

static void reset_pixel_calls(void)
{
    actual_calls = 0;
}

static void assert_int_equal(const char *label,
                             int expected,
                             int actual)
{
    if (expected != actual) {
        fprintf(stderr,
                "FAIL: %s: expected %d, got %d\n",
                label, expected, actual);
        exit(EXIT_FAILURE);
    }
}

static void assert_pixel(int index,
                         int expected_row,
                         int expected_col,
                         int expected_r,
                         int expected_g,
                         int expected_b)
{
    assert_int_equal("pixel row",
                     expected_row,
                     actual_row[index]);

    assert_int_equal("pixel col",
                     expected_col,
                     actual_col[index]);

    assert_int_equal("pixel r",
                     expected_r,
                     actual_r[index]);

    assert_int_equal("pixel g",
                     expected_g,
                     actual_g[index]);

    assert_int_equal("pixel b",
                     expected_b,
                     actual_b[index]);
}

static void test_segment_one_draws_red_pixels(void)
{
    reset_pixel_calls();

    drawHorizontalSegments(1, 2, 3, 7);

    assert_int_equal("pixel call count", 4, actual_calls);

    for (int i = 0; i < 4; i++) {
        assert_pixel(i, 2, 3 + i, 255, 0, 0);
    }
}

int main(void)
{
    test_segment_one_draws_red_pixels();

    printf("All tests passed.\n");

    return EXIT_SUCCESS;
}