#include "drawHorizontalSegments.h"
#include "drawVerticalSegments.h"

const int values_segments[10][7] = {
    {1, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 0, 0, 0, 0},
    {1, 1, 0, 1, 1, 0, 1},
	{1, 1, 1, 1, 0, 0, 1},
	{0, 1, 1, 0, 0, 1, 1},
	{1, 0, 1, 1, 0, 1, 1},
	{1, 0, 1, 1, 1, 1, 1},
	{1, 1, 1, 0, 0, 0, 0},
	{1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 0, 1, 1}
};

void displayTime(const char *time_str){
	int size = 20;
	int startRow = 40;
	int startCol = 40;
	int spacing = 5;
	int row = startRow;
	int col = startCol;
	for(int i = 0; i < strlen(time_str); i++){
		int digit = time_str[i] - '0';
		drawHorizontalSegments(values_segments[digit][0], row, col, (col+size));
		drawHorizontalSegments(values_segments[digit][3], (row+2*size), col, (col+size));
		drawHorizontalSegments(values_segments[digit][6], (row+size), col, (col+size));

		drawVerticalSegments(values_segments[digit][1], row, (row+size), (col+size));
		drawVerticalSegments(values_segments[digit][2], (row+size), (row+2*size), (col+size));
		drawVerticalSegments(values_segments[digit][4], (row+size), (row+2*size), (col));
		drawVerticalSegments(values_segments[digit][5], row, (row+size), col);

		col = col + size + spacing;

		if(i == 1 || i == 3){
			drawHorizontalSegments(1, (row+size), col, (col+size));
			col = col + size + spacing;
		}
	}
	display_picture();
}