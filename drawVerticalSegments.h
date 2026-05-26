void drawVerticalSegments(int segment, int startRow, int endRow, int col){
	if(segment == 1){
		for(int i = startRow; i < endRow; i++){
			set_pixel(i, col, 255, 0, 0);
		}
	}

}