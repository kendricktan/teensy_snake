#include <avr/io.h>
#include <util/delay.h>

#include "cpu_speed.h"
#include "graphics.h"

void draw_snake_bit(unsigned char x, unsigned char y){
    unsigned char i, j;
    for(i = 0; i < 3; i++){
        for(j = 0; j < 3; j++){
            set_pixel(x+i, x+j, 1);
        }
    }
}

// Setup
void setup()
{
    set_clock_speed(CPU_8MHz);

    //initialise the LCD screen
    lcd_init(LCD_DEFAULT_CONTRAST);

    // Screen = outputs
    // LED = outputs
    DDRB = 0b01111100;
    DDRC = (1 << 6);
    DDRD = (1 << 7);
    DDRF = (1 << 7);

    // Turn off led
    PORTB = 0;
}

// Update Screen
void update_screen(){
    clear_screen();

    draw_snake_bit(5, 5);

    show_screen();
}

int main(void)
{
    setup();

    // Game loop
    update_screen();
    
    return 0;
}