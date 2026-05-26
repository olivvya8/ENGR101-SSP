#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

// =====================================================
// THRESHOLDS
// =====================================================

// --- Dark/noise rejection ---
const double MIN_TOTAL_BRIGHTNESS = 0.10;

// --- White/grey glare rejection ---
const double BRIGHT_GLARE_TOTAL = 2.70;
const double MAX_GLARE_CHROMA = 0.05;

// --- Yellow/orange rejection ---
const double MAX_GREEN_RED_RATIO = 0.78;


// =====================================================
// BRIGHT LIGHT THRESHOLDS
// =====================================================

const double BRIGHT_TOTAL_THRESHOLD = 1.55;

const double BRIGHT_MIN_R_RATIO = 0.40;

const double BRIGHT_MIN_RG_DELTA = 0.16;
const double BRIGHT_MIN_RB_DELTA = 0.16;

const double BRIGHT_MIN_CHROMA = 0.18;


// =====================================================
// NORMAL LIGHT THRESHOLDS
// =====================================================

const double NORMAL_TOTAL_THRESHOLD = 0.60;

const double NORMAL_MIN_R_RATIO = 0.50;

const double NORMAL_MIN_RG_DELTA = 0.12;
const double NORMAL_MIN_RB_DELTA = 0.10;

const double NORMAL_MIN_CHROMA = 0.12;


// =====================================================
// DIM LIGHT THRESHOLDS
// =====================================================

const double DIM_MIN_R_RATIO = 0.62;

const double DIM_MIN_RG_DELTA = 0.06;
const double DIM_MIN_RB_DELTA = 0.06;

const double DIM_MIN_CHROMA = 0.06;

const int highDisplacement = 50;
const int smallDisplacement = 16;

void trackRuby(bool *rubyDetected, bool *rubyMoved, bool *attemptedTheft, int *prevCenterRow, int *prevCenterCol)
{
    static int motionCount = 0;

    int redCount = 0;
    bool valid = false;
    long totalRow = 0;
    long totalCol = 0;
    

    
    for (int row = 0; row < 480; row++) {

        for (int col = 0; col < 640; col++) {

            valid = false;

            uchar r, g, b;

            get_pixel(row, col, &r, &g, &b);

            
				// ---------- Brightness ----------
			double total = ((double)r + (double)g + (double)b) / 255.0;
			double scaledR = (double)r / 255.0;
			double scaledG = (double)g / 255.0;
			double scaledB = (double)b / 255.0;

					// Reject near-black pixels/noise
			if (total <= MIN_TOTAL_BRIGHTNESS){
				continue;
			}
				


			// ---------- Basic colour metrics ----------
		double maxVal = scaledR;
		if (scaledG > maxVal) maxVal = scaledG;
		if (scaledB > maxVal) maxVal = scaledB;

		double minVal = scaledR;
		if (scaledG < minVal) minVal = scaledG;
		if (scaledB < minVal) minVal = scaledB;
			double chroma = maxVal - minVal;

			double rRatio = scaledR / (total + 0.000001);

			double rgDifference = scaledR - scaledG;
			double rbDifference = scaledR - scaledB;


			// ---------- Reject white/grey glare ----------
			if (total > BRIGHT_GLARE_TOTAL && chroma < MAX_GLARE_CHROMA){

				continue;
				
			}


			// ---------- Red must dominate ----------
			if (scaledR <= scaledG || scaledR <= scaledB)
				continue;


			// ---------- Reject yellow/orange ----------
			if (scaledG > scaledR * MAX_GREEN_RED_RATIO)
				continue;


			// =================================================
			// BRIGHT LIGHTING
			// =================================================
			
            if (total >= BRIGHT_TOTAL_THRESHOLD && rRatio >= BRIGHT_MIN_R_RATIO &&	rgDifference >= BRIGHT_MIN_RG_DELTA &&
                rbDifference >= BRIGHT_MIN_RB_DELTA &&
                chroma >= BRIGHT_MIN_CHROMA)
            {
                valid = true;
            }
			

			// =================================================
			// NORMAL LIGHTING
			// =================================================
			
            else if (total >= NORMAL_TOTAL_THRESHOLD && rRatio >= NORMAL_MIN_R_RATIO &&
                rgDifference >= NORMAL_MIN_RG_DELTA &&
                rbDifference >= NORMAL_MIN_RB_DELTA &&
                chroma >= NORMAL_MIN_CHROMA)
            {
                valid = true;
            }
			

			// =================================================
			// DIM LIGHTING
			// =================================================
			
            else if (rRatio >= DIM_MIN_R_RATIO &&
                rgDifference >= DIM_MIN_RG_DELTA &&
                rbDifference >= DIM_MIN_RB_DELTA &&
                chroma >= DIM_MIN_CHROMA)
            {
                valid = true;
            }
			

            if(valid){
                redCount ++;
                totalRow += row;
                totalCol += col;
            }

		}
    }

	

    // -------- Ruby detection --------
    if (redCount > 2000) {

        *rubyDetected = true;
        *attemptedTheft = false;

        // -------- Find center --------
        int centerRow = totalRow / redCount;
        int centerCol = totalCol / redCount;

       
        // -------- Motion detection --------
        if (*prevCenterRow != -1) {

			//start = false;
            int dx = centerRow - *prevCenterRow;
            int dy = centerCol - *prevCenterCol;

            long disp = dx*dx + dy*dy;

            
            if(disp > highDisplacement){
                *rubyMoved = true;
                *attemptedTheft = true;
            }
			else if(disp > smallDisplacement){
				motionCount ++;
				
			}
			else{
				motionCount = 0; //ruby is still/center has hardly changed
			}
        }


        if(motionCount >= 3){
            *rubyMoved = true;
            *attemptedTheft = true;
        }

        *prevCenterRow = centerRow;
        *prevCenterCol = centerCol;

        

    }

    else {

        *rubyDetected = false;
        *attemptedTheft = true;

        

        printf("Attempted theft\n");
    }
}