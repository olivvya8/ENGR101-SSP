/*
 * The code below is meant to detect whether the Red Ruby
 * is present. The starategy implemented by the code is not very effective. 
 * Study the code so that you understand what the strategy is and how 
 * it is implemented. Then design and implement a better startegy.
 * 
 * */


#include <stdio.h>
#include "camera_x11.h"

#include <stdbool.h>
#include <unistd.h>
#include <math.h>

//#define TESTING
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

//using namespace std;

int checkForRuby(bool rubyDetected) {
	if (rubyDetected == false) {
		for (int row = 0 ; row < 600; row++) {		// Loop through all X pixels
			for (int col = 0; col < 500; col++) {	// Loop through all Y pixels
				//set_pixel(row, col, 250, 0, 0); // Set every pixel to red (screen alert)
				//printf("Ruby not detected :( \n");
			}
		}
		
	} else {
		for (int row = 10 ; row < 40 ; row++) {		// Loop through rectangle X pixels
			for (int col = 10; col < 40; col++) {	// Loop through rectangle Y pixels
				set_pixel(row, col, 0, 255, 0); 	// Draw green rectangle
			}
		}
	}
	display_picture();
	return 0;
}

int detect(bool *rubyDetected, int redCount, long *totalRow, long *totalCol, int *prevCenterRow, int *prevCenterCol, bool *rubyMoved) {

	//printf("Red Count: %d\n", redCount);
	
	if (redCount > 500) {
		*rubyDetected = true;
		// ----------- Find centre -----------
		int centerRow = *totalRow / redCount;
    	int centerCol = *totalCol / redCount;

		if (*prevCenterRow != -1) {
			int xChange = centerRow - *prevCenterRow;
			int yChange = centerCol - *prevCenterCol;

			// Find change in center pos
			long disp = xChange * xChange + yChange * yChange;
			//printf("Displacement: %ld\n", disp);
			if (disp > 100) {    		// Some number between 10 and 20
				printf("Ruby moved\n");
				//*rubyMoved = true;
			} else {
				//printf("Ruby still\n");
				*rubyMoved = false;
			}
		}

		// Update prev row and col
		*prevCenterRow = centerRow;
		*prevCenterCol = centerCol;
	} else {
		*rubyDetected = false;
		printf("No ruby detected\n");
	}

	return *rubyMoved;
}

int main() {
	
	 if (start_camera() != 0) {
        printf("Failed to start camera\n");
        return 1;
    }
  //int err = init(0);
  //cout<<"Error: "<<err<<endl;
  //open_screen_stream();
  
	bool rubyDetected = false;
	bool rubyMoved = false;
	int prevCenterRow = -1;
  	int prevCenterCol = -1;

  // make 1000 runs  
  for (int countrun = 0; countrun < 1000; countrun++) {
	while (!rubyMoved) {
		take_picture();
		display_picture();

		int redCount = 0;

		// -------- Find centre of ruby -------------------
		long totalRow = 0;
		long totalCol = 0;


		// orginal redCount on start up
		// for all pixels in latest image
		for (int row = 0 ; row < 1200 ; row++) {	
			for (int col = 0; col < 420; col++) {
				uchar r, g ,b ;
				get_pixel(row, col, &r, &g ,&b);
				
				// ----------------- NEW COLOUR PROCESSING --------------------
				double normR = r/255; // Normalise values
				double normG = g/255;
				double normB = b/255;

				double max = MAX(normR, MAX(normG,normB));
				double min = MIN(normR, MIN(normG,normB));

				double hue = (60 * (normG-normB)/(max - min)) + 360;
				double lightness = (max + min) / 2;
				//printf("%f\n", hue);

			
				// ------------------------------------------------------------
				
				// Check if red pixel
				int intensity = (int)(r + g + b);
				
				if (intensity > 30 && intensity < 400) {
					 double redRatio = (double)r / intensity;

    				if (redRatio > 0.45 && r > g * 1.3 && r > b * 1.3) {
					//if (r > 100 && r > g + 25 && r > b + 25) {
						redCount ++;		// Increment counter for red px
						totalRow += row;	// Add row
						totalCol += col;	// Add col

						#if defined(TESTING)
						//printf("Red Counter: %d\n", redCount);
						//printf("totalRow: %ld\n", totalRow);
						//printf("TotalCol: %ld\n", totalCol);
						#endif
					}
				}
				//totRed = totRed + r;
				//totInt = totInt + (r+g+b)/3;
				//redness = (double)totRed/(3.0*(double)totInt);
			}
		}
		//printf("Red Count: %d\n", redCount);
		detect(&rubyDetected, redCount, &totalRow, &totalCol, &prevCenterRow, &prevCenterCol, &rubyMoved);

		checkForRuby(rubyDetected);

		//printf(" countrun: =%d\n",countrun);
		//cout<<" Total red: "<<totRed<<endl;
		//cout<<", Total intensity: "<<totInt<<endl;
		//cout<<", redness : "<<redness<<endl<<endl;
		//sleep1(1000); // slow down a bit to make display easier
		
	}
}
  //close_screen_stream();
  return 0;
}


