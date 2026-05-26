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


    bool rubyDetected = false;
    bool rubyMoved = false;
    bool attemptedTheft = false;
    int prevR = 5;
    int prevC = 5;
    
    for (int i=0;i<50;i++) {

        take_picture();

        trackRuby(&rubyDetected, &rubyMoved, &attemptedTheft, &prevR, &prevC);

        display_picture();
    }
    
    sleep(1);

    printf("Test complete\n");
    
    return 0;
}


