#include <stdio.h>
#include "camera_x11.h"
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include "trackRuby.h"


int main() {

    if (start_camera() != 0) {
        printf("Failed to start camera\n");
        return 1;
    }

    take_picture();

    trackRuby(false,false,false,-1,-1);

    display_picture();
    
    return 0;
}


