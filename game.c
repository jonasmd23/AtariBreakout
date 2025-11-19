#include <stdio.h>
#include <stdlib.h> // rand
#include <inttypes.h> // For PRIu32

#include "hw.h"
#include "lcd.h"
#include "cursor.h"
#include "sound.h"
#include "pin.h"
#include "ball.h"
#include "platform.h"
#include "brick.h"
#include "game.h"
#include "config.h"

// sound support
#include "missileLaunch.h"

// M2: Define stats constants
#define STATS_Y 10
#define SHOTS_X 10
#define IMPACTED_X 200
#define SOUND_VOLUME 50
#define BIG 32

// All missiles
brick_t brick[CONFIG_MAX_TOTAL_BRICKS];

// M2: Declare stats variables
uint32_t bricks = 0;
uint32_t broken = 0;

// Button state tracking for debouncing
static bool pressed = false;



// Initialize the game control logic.
// This function initializes all missiles, planes, stats, etc.
void game_init(void)
{
	// Initialize bricks
	for (uint32_t i = 0; i < CONFIG_MAX_TOTAL_BRICKS; i++)
		brick_init(bricks+i);

	// Initialize other functions
    platform_init();
    ball_init();

	// M2: Initialize stats
	bricks = 0;
	broken = 0;
	
	// Initialize button state
	pressed = false;

	// M2: Set sound volume
	sound_set_volume(SOUND_VOLUME);
}

// Update the game control logic.
// This function calls the missile & plane tick functions, relaunches
// idle enemy missiles, handles button presses, launches player missiles,
// detects collisions, and updates statistics.
void game_tick(void)
{
	// Tick all bricks in one batch
	for (uint32_t i = 0; i < CONFIG_MAX_TOTAL_BRICKS; i++){
		brick_tick(bricks+i);
	}

	// Tick brick and platform
	ball_tick();
    platform_tick();

    // start ball moving

    //start platform moving

    //check for a brick collision

    //check for a wall collision

    //check for a platform collision
	





	// M2: Draw stats
    char text_buffer[BIG];
	// Draw shots count
	snprintf(text_buffer, sizeof(text_buffer), "Bricks: %" PRIu32, bricks);
	lcd_drawString(SHOTS_X, STATS_Y, text_buffer, CONFIG_COLOR_STATUS);
	// Draw impacted count
	snprintf(text_buffer, sizeof(text_buffer), "Broken: %" PRIu32, broken);
	lcd_drawString(IMPACTED_X, STATS_Y, text_buffer, CONFIG_COLOR_STATUS); 

}