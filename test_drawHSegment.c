#include <stdio.h>
#include "camera_x11.h"
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include "drawHorizontalSegments.h"
#include "drawVerticalSegments.h"
#include "changeStartDisplay.h"
#include "displayTime.h"
#include "logTheft.h"
#include "trackRuby.h"

info = localtime(&raw_time);
		int hours = info->tm_hour;
		int minutes = info->tm_min;
		int seconds = info->tm_sec;
		int wday = info->tm_wday;
		int mday = info->tm_mday;
		int month = info->tm_mon + 1;
		int year = info->tm_year + 1900;

int main() {
    drawHorizontalSegments(1,10,10,20);
    drawVerticalSegments(1,10,10,20);
    displayTime("134500");
    logTheft(info);
    changeStartDisplay(true);
    trackRuby(false,false,false,-1,-1);

    return 0;
}


