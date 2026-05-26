#include <stdio.h>
#include "camera_x11.h"
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include "displayTime.h"



int main() {

    if (start_camera() != 0) {
        printf("Failed to start camera\n");
        return 1;
    }

    take_picture();

    info = localtime(&raw_time);

    int hours = info->tm_hour;
    int minutes = info->tm_min;
    int seconds = info->tm_sec;
    int wday = info->tm_wday;
    int mday = info->tm_mday;
    int month = info->tm_mon + 1;
    int year = info->tm_year + 1900;

    char time_str[7];
    char time_str_display[9];
    snprintf(time_str, sizeof(time_str), "%02d%02d%02d", hours, minutes, seconds);
    
    displayTime("134500");

    display_picture();
    
    return 0;
}

