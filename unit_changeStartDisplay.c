#include <stdio.h>
#include <stdbool.h>
#include "camera_x11.h"
#include "changeStartDisplay.h"


int main() {
    int passed = 0;

    if (start_camera() != 0) {
        printf("Failed to start camera\n");
        return 1;
    }

    for (int i=0;i<50;i++) {

        take_picture();

        changeStartDisplay(true);

        display_picture();
    }
    passed++;
    sleep(1);

    for (int i=0;i<50;i++) {

        take_picture();

        changeStartDisplay(false);

        display_picture();
    }
    passed++;
    sleep(1);

    printf("Tests passed: %d\n", passed);
    return 0;
}

    

