#include <stdlib.h>
#include <stdbool.h>

#define LED 0xFF200000

volatile int pixel_buffer_start; // global variable

short int Buffer1[240][512]; // 240 rows, 512 (320 + padding) columns
short int Buffer2[240][512];

volatile int X, Y;

//struct to store x and y coordinates of spikes' edges
typedef struct {
    int x;
    int y;
} Coordinate;

void wait_for_vsync();
void plot_pixel(int x, int y, short int color);
void clear_screen();
void draw_spike_left (int x, int y, short int color);
void draw_spike_right (int x, int y, short int color);
void display_top_bottom_spikes(short int color);
bool if_collision (int x_coord, int y_coord, Coordinate coordinates[]);

void interrupt_handler(void);
void interrupt_handler(void) {
	int PS2_data = *(PS2_ptr);
	byte1 = byte2;
	byte2 = byte3;
	byte3 = PS2_data & 0xFF;
	if ((byte2 == (char)0xAA) && (byte3 == (char)0x00)) {
		*(PS2_ptr) = 0xF4;
	}
	
	if((byte3==0x29)&&(byte2!=0xF0)&&(byte2!=0x29)) {
		Y = Y-10;
		clear_screen();
		draw_circle(80,60);
		if(forward) {
			X+=5;
			draw_bird(X, Y);
			wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        	pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
		} else {
			X-=5;
			draw_reverse_bird(X, Y);
			wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        	pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
		}
		
	} 
}

int main(void)
{
    int size = 50;
    int index_left = 0;
    int index_right = 0;
    Coordinate coordinates_left[size];
    Coordinate coordinates_right[size];

     volatile int * led_ptr = (int *) LED;
    *(led_ptr) = 0x0;     //turn off all LEDs

    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;

	//*(PS2_ptr+1) = 1;

    int spikes_y[2];        //y coord of spikes
	
	//NIOS2_WRITE_IENABLE(0x80);
	//NIOS2_WRITE_STATUS(1);
    // declare other variables(not shown)
    // initialize location and direction of rectangles(not shown)
	int x = 135;
	int y = 101;
	int a = 80;
	int b = 60;
	X = x;
	Y = y;

    /* set front pixel buffer to Buffer 1 */
    *(pixel_ctrl_ptr + 1) = (int) &Buffer1; // first store the address in the  back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen(); // pixel_buffer_start points to the pixel buffer

    /* set back pixel buffer to Buffer 2 */
    *(pixel_ctrl_ptr + 1) = (int) &Buffer2;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
    clear_screen(); // pixel_buffer_start points to the pixel buffer
	//draw_circle(a,b);

    while (1)
    {

        //code for generating and updating random locations of spikes
        index_right = 0;

        for (int k = 0; k < 2; k++){
            //generate randome y coordinate - with offset so no need to check for in bounds
            spikes_y[k] = rand() % 214;             
        
            //storing coord values to check for collisions 
            for (int var_y = 0; var_y <= 24; var_y++) {
                if (var_y <= 12) {
                    coordinates_right[index_right].x = 319 - var_y;
                    coordinates_right[index_right].y = spikes_y[k] + var_y;
                    //coordinates[index].y = 0 + y;
                } else {
                    coordinates_right[index_right].x = 319 - (24 - var_y);
                    coordinates_right[index_right].y = spikes_y[k]+ var_y;
                    //coordinates[index].y = 24 + y;
                }
                index_right++;
            }                 
        }

		while((X+50)<=319) {        //forward

            plot_pixel(X, Y, 0xF800);

			//forward = true;
			clear_screen();
            display_top_bottom_spikes(0xFF00);
			//draw_circle(a,b);
			//draw_bird(X,Y);

            // code for drawing the random spikes
            for (int j = 0; j < 2; j++){
                //check if in bounds and reflect accordingly
                //draw_spike_left(0, spikes_y[j], 0xFF00);
                draw_spike_right(319, spikes_y[j], 0xFF00);
            }

        	wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        	pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
			X = x = X+5;
			Y = y = Y+5;
			if((Y+38)==239) {
				break;
			}

            //testing
            //if collision, then game end******
            if (if_collision(X, Y, coordinates_right)){            //if collision, turn on all LEDs
                *(led_ptr) = 0xFFFF;
                //*****end game****
            }
            else{
                *(led_ptr) = 0x0;           //otherwise off
            }
            //end of testing
		}
		
		//code for generating and updating random locations of spikes
        index_left = 0;

        for (int k = 0; k < 2; k++){
            //generate randome y coordinate - with offset so no need to check for in bounds
            spikes_y[k] = rand() % 214;             
        
            //storing coord values to check for collisions 
            for (int var_y = 0; var_y <= 24; var_y++) {
                if (var_y <= 12) {
                    coordinates_left[index_left].x = var_y;
                    coordinates_left[index_left].y = spikes_y[k] + var_y;
                    //coordinates[index].y = 0 + y;
                } else {
                    coordinates_left[index_left].x = 24 - var_y;
                    coordinates_left[index_left].y = spikes_y[k]+ var_y;
                    //coordinates[index].y = 24 + y;
                }
                index_left++;
            }                 
        }

		while(X>=0) {       //back

            plot_pixel(X, Y, 0xF800);

			//forward = false;
			if((Y+38)==239) {
				break;
			}
			clear_screen();
            display_top_bottom_spikes(0xFF00);
			//draw_circle(a,b);
			//draw_reverse_bird(X,Y);

            // code for drawing the random spikes
            for (int j = 0; j < 2; j++){
                //check if in bounds and reflect accordingly
                draw_spike_left(0, spikes_y[j], 0xFF00);
                //draw_spike_right(319, spikes_y[j], 0xFF00);
            }

			wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        	pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
			X = x = X-5;
			Y = y = Y+5;
			if((Y+38)==239) {
				break;
			}

            //testing
            //if collision, then game end******
            if (if_collision(X, Y, coordinates_left)){            //if collision, turn on all LEDs
                *(led_ptr) = 0xFFFF;
                //*****end game****
            }
            else{
                *(led_ptr) = 0x0;           //otherwise off
            }
            //end of testing
        }
            
            if((Y+38)==239) {
                break;
            }
	}
}


void wait_for_vsync(){
    volatile int * pixel_ctrl_ptr = (int *) 0xFF203020;
    int status;

    *pixel_ctrl_ptr = 1;

    status = *(pixel_ctrl_ptr + 3);

    while ((status & 0x01) != 0){
        status = *(pixel_ctrl_ptr + 3);
    }
}

void plot_pixel(int x, int y, short int color)
{
    volatile short int *one_pixel_address;  
        one_pixel_address = pixel_buffer_start + (y << 10) + (x << 1);
        *one_pixel_address = color;
}

void clear_screen()
{
    int y, x;
    for (x = 0; x < 320; x++)
    for (y = 0; y < 240; y++)
    plot_pixel (x, y, 0);
}

void draw_spike_left (int x, int y, short int color){            //triangle with base and height = 25

    for (int i = 0; i <= 12; i++) {                          //top half
        for (int j = 0; j <= i; j++){
            plot_pixel(j + x, i + y, color);
        }
    }

    for (int i = 0; i <= 12; i++) {                         //bottom half
        for (int j = 12 - i; j >= 0; j--){
            plot_pixel(j + x, 12 + i + y, color);
        }
    }
}

void draw_spike_right (int x, int y, short int color){            //triangle with base and height = 25

    for (int i = 0; i <= 12; i++) {                          //top half
        for (int j = 0; j <= i; j++){
            plot_pixel(x - j, i + y, color);
        }
    }

    for (int i = 0; i <= 12; i++) {                         //bottom half
        for (int j = 0; j <= 12 - i; j++){
            plot_pixel(x - i, j + y + 12, color);
        }
    }
}

void display_top_bottom_spikes(short int color){

    //draw spikes y = 239 (bottom)
    for (int i = 0; i < 320; i += 25){
        for (int x = 0; x <= 12; x++) {                          //left half
            for (int y = 0; y <= x; y++){
                plot_pixel(x + i, y, color);
            }
        }   

        for (int x = 0; x <= 12; x++) {                         //right half
            for (int y = 12 - x; y >= 0; y--){
                plot_pixel(x + i + 12, y, color);
            }
        }
        
    }

    //draw spikes y = 239 (bottom)
    for (int j = 0; j < 320; j += 25){
        for (int x = 0; x <= 12; x++) {                          //left half
            for (int y = 0 + x; y >= 0; y--){
                plot_pixel(x + j, 239 - y, color);
            }
        }   

       for (int x = 0; x <= 12; x++) {                         //right half
            for (int y = 12 - x; y >= 0; y--){
                plot_pixel(x + j + 12, 239 - y, color);
            }
        } 
    }  
}

bool if_collision (int x_coord, int y_coord, Coordinate coordinates[]){
    bool collided = false;

    //check for left edge
    for (int i_y = 0; i_y < 38; i_y++){
        for (int index = 0; index < 50; index++){
            if (coordinates[index].x == x_coord){
                if (coordinates[index].y == y_coord + i_y){
                    collided = true;
                    break;
                }
            }
        }
        if (collided) {
            break; // Exit outer loop
        }
    }

    //check for top and bottom edge
    for (int i_x = 0; i_x < 50; i_x++){
        for (int index = 0; index < 50; index++){
            if (coordinates[index].y == y_coord || coordinates[index].y == y_coord + 38){
                if (coordinates[index].x == x_coord + i_x){
                    collided = true;
                    break;
                }
            }
        }
        if (collided) {
            break; // Exit outer loop
        }
    }

    return collided;
}
