#include <stdio.h>
#include <unistd.h>
#include "camera_x11.h"
#include "drawHorizontalSegments.h"


int main() {

    if (start_camera() != 0) {
        printf("Failed to start camera\n");
        return 1;
    }

    for (int i=0;i<50;i++) {

        take_picture();

        drawHorizontalSegments(1,10,10,20);

        display_picture();
    }
    sleep(2);

    printf("Test complete\n");
    return 0;
}

