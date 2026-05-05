 /*
 * The code below is meant to detect whether the Red Ruby
 * is present. The starategy implemented by the code is not very effective. 
 * Study the code so that you understand what the strategy is and how 
 * it is implemented. Then design and implement a better startegy.
 * 
 * */



#include <stdio.h>
#include "camera_x11.h"

//using namespace std;

int main() {
	
	 if (start_camera() != 0) {
        printf("Failed to start camera\n");
        return 1;
    }
  //int err = init(0);
  //cout<<"Error: "<<err<<endl;
  //open_screen_stream();
  
  // make 1000 runs  
  for (int countrun = 0; countrun < 1000; countrun++) {
	take_picture();
	display_picture();
	int totRed = 0;
	int totInt = 0;
	double redness = 0.0;
	int redCount = 0;
	Boolean rubyDetected = false;
    // for all pixels in latest image
    for (int row = 0 ; row < 240 ; row++) {	
		for (int col = 0; col < 320; col++) {
			uchar r, g ,b ;
			get_pixel(row, col, &r, &g ,&b);
			// Check if red pixel
			if (r > 150 && r > g + 50 && r > b + 50) {
				redCount ++
				if (redCount > 200) {
					printf("Ruby detected\n");
					rubyDetected = true;
					break;
				}
			}
			//totRed = totRed + r;
			//totInt = totInt + (r+g+b)/3;
			//redness = (double)totRed/(3.0*(double)totInt);
		}
	}

	printf(" countrun: =%d\n",countrun);
	//cout<<" Total red: "<<totRed<<endl;
	//cout<<", Total intensity: "<<totInt<<endl;
	//cout<<", redness : "<<redness<<endl<<endl;
	//sleep1(1000); // slow down a bit to make display easier
	
  }  
  //close_screen_stream();
  return 0;
}

int checkForRuby(Boolean rubyDetected) {
	if (!rubyDetected) {
		for (int row = 0 ; row < 240 ; row++) {		// Loop through all X pixels
			for (int col = 0; col < 320; col++) {	// Loop through all Y pixels
				set_pixel(row, col, 255, 0, 0); 	// Set every pixel to red (screen alert)
			}
		}
	} else {
		for (int row = 10 ; row < 40 ; row++) {		// Loop through rectangle X pixels
			for (int col = 10; col < 40; col++) {	// Loop through rectangle Y pixels
				set_pixel(row, col, 0, 255, 0); 	// Draw green rectangle
			}
		}
	}
	return 0;
}
