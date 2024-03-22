void draw_circle(int x, int y) {
	int number = 0;
	for(int i = 0; i<19200; i++) {
		if(i%160==0) {
			y = y+1;
			number = 0;
		}
		
		plot_pixel(x+number,y,Circle[i]);
		number++;
	}
	
}
	
