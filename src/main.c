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
#define STARTSCREENHEIGHT 12
#define STARTSCREENWIDTH 3
#define SCREENWIDTH 84
#define SCREENHEIGHT 48
unsigned char GAME_OVER = 0;
unsigned char LIVES = 5;
unsigned char SCORE = 0;
unsigned char SNEK_CUR_LENGTH = 2;
int seed_time = 42;
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
    for (unsigned char i = 0; i < SNEK_CUR_LENGTH; i++)
    {
        SNEK[i].x = 48 + (i * SNEKWIDTH);
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

// Checks if snake self collided
unsigned char snek_suicide(){
    for(unsigned char i = 1; i < SNEK_CUR_LENGTH; i++){
        if (SNEK[0].x == SNEK[i].x && SNEK[0].y == SNEK[i].y){
            return 1;
        }
    }
    return 0;
}

// Checks if newly spawn food is in snek
// can't have that can we
unsigned char is_food_in_snek(){
    for (unsigned char i = 0; i < SNEK_CUR_LENGTH; i++){
        if (SNEK[0].x == SNEKFOOD.x && SNEK[0].y == SNEKFOOD.y){
            return 1;
        }
    } 
    return 0;
}

// Snek food
void new_snek_food_location()
{
    srand(seed_time);
    SNEKFOOD.x = (rand() % SCREENWIDTH) + STARTSCREENWIDTH;
    SNEKFOOD.y = (rand() % SCREENHEIGHT) + STARTSCREENHEIGHT;

    // Want multiples of 3
    SNEKFOOD.y -= SNEKFOOD.y % 3;
    SNEKFOOD.x -= SNEKFOOD.x % 3;
}

void draw_snek_food()
{
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

    // Snek food
    new_snek_food_location();
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
    char status_char[9] = "S:   L: ";
    status_char[7] = LIVES + '0';

    if (SCORE < 10)
    {
        status_char[2] = '0';
        status_char[3] = SCORE + '0';
        draw_string(1, 1, status_char);
    }

    else
    {
        status_char[2] = (SCORE / 10) + '0';
        status_char[3] = (SCORE % 10) + '0';
        draw_string(1, 1, status_char);
    }

    draw_line(0, 10, SCREENWIDTH, 10);
}

// Update Screen
// All draw functions will be called here
void update_screen()
{
    clear_screen();

    draw_snek_food();
    draw_snek();
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
    // Check snek food
    if (SNEKFOOD.x == SNEK[0].x && SNEK[0].y == SNEKFOOD.y)
    {

        SCORE += 1;
        SNEK_CUR_LENGTH += 1;
        new_snek_food_location();
    
        //check new snek food location
        while(is_food_in_snek() || SNEKFOOD.x < STARTSCREENWIDTH || SNEKFOOD.x > SCREENWIDTH-3 || SNEKFOOD.y < STARTSCREENHEIGHT || SNEKFOOD.y > SCREENHEIGHT -3){
            seed_time += 1;
            new_snek_food_location();
        }
    }

    // Check snek collide
    if (snek_suicide()){
        LIVES -= 1;
        DIRECTION = IDLE;
        reset();
        return;
    }

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
    // +3 because can't be 0'
    if (SNEK[0].x < STARTSCREENWIDTH)
    {
        SNEK[0].x = SCREENWIDTH - 3;
    }
    if (SNEK[0].x > SCREENWIDTH - 3)
    {
        SNEK[0].x = STARTSCREENWIDTH;
    }

    if (SNEK[0].y < STARTSCREENHEIGHT)
    {
        SNEK[0].y = SCREENHEIGHT - 3;
    }
    if (SNEK[0].y > SCREENHEIGHT - 3)
    {
        SNEK[0].y = STARTSCREENHEIGHT;
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
        if (DIRECTION != IDLE){
            update_game();
        }

        // update screen
        update_screen();

        _delay_ms(DELAY);
        seed_time += 1;
    }

    return 0;
}