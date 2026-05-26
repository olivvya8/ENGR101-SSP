void changeStartDisplay(bool present){
	if(present){
		for(int row = 0; row < 30; row ++){
			for(int col = 0; col < 50; col ++){
				set_pixel(row, col, 0, 255, 0);
			}
		}
	}
	else{
		for(int r = 0; r < 480; r ++){
			for(int c = 0; c < 640; c ++){
				set_pixel(r, c, 255, 0, 0);
			}
		}
	}
	display_picture();
}