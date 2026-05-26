#include <stdio.h>
#include <unistd.h>
#include "camera_x11.h"
#include "drawVerticalSegments.h"


int main() {

    if (start_camera() != 0) {
        printf("Failed to start camera\n");
        return 1;
    }

    for (int i=0;i<50;i++) {

        take_picture();

        drawVerticalSegments(1,10,50,10);

        display_picture();
    }
    sleep(2);

    printf("Test complete\n");
    return 0;
}

