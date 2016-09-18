#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#include "cpu_speed.h"
#include "graphics.h"

// Enums
enum Directions
{
    IDLE,
    UP,
    DOWN,
    LEFT,
    RIGHT
};

// Structs
struct Snek
{
    unsigned char x;
    unsigned char y;
};

// Global variables
// LED screen is 48x84
#define DELAY 190
#define SNEKMAXSIZE 100
#define SNEKWIDTH 3
#define SCREENWIDTH 84
#define SCREENHEIGHT 48
unsigned char GAME_OVER = 0;
unsigned char LIVES = 5;
unsigned char SCORE = 0;
unsigned char SNEK_CUR_LENGTH = 2;
enum Directions DIRECTION;
struct Snek SNEK[SNEKMAXSIZE];
struct Snek SNEKFOOD;

// Draws snake
void draw_snek_body(unsigned char x, unsigned char y)
{
    unsigned char i, j;
    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 3; j++)
        {
            set_pixel(x + i, y + j, 1);
        }
    }
}

void draw_snek_head(unsigned char x, unsigned char y)
{
    draw_snek_body(x, y);
    set_pixel(x + 1, y + 1, 0);
}

void draw_snek()
{
    draw_snek_head(SNEK[0].x, SNEK[0].y);

    unsigned char i;
    for (i = 1; i < SNEK_CUR_LENGTH; i++)
    {
        draw_snek_body(SNEK[i].x, SNEK[i].y);
    }
}

void reset_snek()
{
    SNEK_CUR_LENGTH = 2;
    for (unsigned char i = 0; i < 2; i++)
    {
        SNEK[i].x = 50 + (i * SNEKWIDTH);
        SNEK[i].y = 30;
    }
}

// Moves snek by 1 move
void move_snek_body()
{
    for (unsigned char i = SNEK_CUR_LENGTH - 1; i > 0; i--)
    {
        SNEK[i] = SNEK[i - 1];
    }
}

// Snek food
void new_snek_food_location(){
}

void draw_snek_food(){
    draw_snek_body(SNEKFOOD.x, SNEKFOOD.y);
}


// Welcome screen
void welcome_screen()
{
    clear_screen();

    draw_string(5, 5, "Kendrick Tan");
    draw_string(5, 15, "n9701583");

    show_screen();
}

// Reset game
void reset()
{
    // Snek moving direction
    DIRECTION = IDLE;

    // Snek position
    reset_snek();
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

    // Reset snek position
    reset();
}

// Show stats
void show_status()
{
    // Pretty display
    char status_char[7] = "   ( )";
    status_char[4] = LIVES + '0';

    if (SCORE < 10)
    {
        status_char[0] = '0';
        status_char[1] = SCORE + '0';
        draw_string(1, 1, status_char);
    }

    else
    {
        status_char[0] = (SCORE / 10) + '0';
        status_char[1] = (SCORE % 10) + '0';
        draw_string(1, 1, status_char);
    }
}

// Update Screen
// All draw functions will be called here
void update_screen()
{
    clear_screen();

    draw_snek();
    draw_snek_food();
    show_status();

    show_screen();
}

// Checks for user input
void get_inputs()
{
    // Left
    if ((PINB >> 1) & 1)
    {
        DIRECTION = LEFT;
    }

    // Right
    if (PIND & 1)
    {
        DIRECTION = RIGHT;
    }

    // Down
    if ((PINB >> 7) & 1)
    {
        DIRECTION = DOWN;
    }

    // Up
    if ((PIND >> 1) & 1)
    {
        DIRECTION = UP;
    }
}

// Updates game engine
void update_game()
{
    move_snek_body();

    // Directions
    if (DIRECTION == UP)
    {
        SNEK[0].y -= SNEKWIDTH;
    }

    if (DIRECTION == DOWN)
    {
        SNEK[0].y += SNEKWIDTH;
    }

    if (DIRECTION == RIGHT)
    {
        SNEK[0].x += SNEKWIDTH;
    }

    if (DIRECTION == LEFT)
    {
        SNEK[0].x -= SNEKWIDTH;
    }

    // Check boundaries
    if (SNEK[0].x < 3){
        SNEK[0].x = SCREENWIDTH-3;
    }
    if (SNEK[0].x > SCREENWIDTH-3){
        SNEK[0].x = 3;
    }
    if (SNEK[0].y < 3){
        SNEK[0].y = SCREENHEIGHT-3;
    }
    if (SNEK[0].y > SCREENHEIGHT-3){
        SNEK[0].y = 3;
    }
}

// main
int main(void)
{
    setup();

    // Welcome screen
    welcome_screen();
    _delay_ms(2000);

    // Game loop
    while (!GAME_OVER)
    {
        // Get Inputs
        get_inputs();

        // Update game engine
        update_game();

        // update screen
        update_screen();

        _delay_ms(DELAY);
    }

    return 0;
}