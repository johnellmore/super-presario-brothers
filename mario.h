/* 
 * File: mario.h
 * -----------------------------------------------------
 * This file contains collision detection utilities and other very useful 
 * tools for grid-based platform games.
 */
 
#ifndef _mario_h
#define _mario_h

/* GLOBAL VARIABLES */

// pixel size of each grid square
const int gridCellW = 16;
const int gridCellH = 16;

// number of grids on the canvas
const int gridW = 500;
const int gridH = 14;

// width of the window in grid cells
const int windowGridW = 36;

// pixel width of the player
const int playerW = 16;
const int playerH = 16;

// the player's pixel coordinates - represented at the base middle of the character
int playerX;
int playerY;

// the width of the entire level in pixels
int levelW = 0;

// the current location of the viewport
int viewportX = 0;

// gravitational constant - adjust to modify the pull of gravity
const double gravity = 0.5;

// jump height - in frames
const int jumpHeight = 17;

// spring jump height in frames
const int springJumpHeight = 22;

// how many "frames" that the player has been falling
int verticalForce = 0;

// define our level content
int levelNum = 0;

// the level map, made up of chars
char level [gridH][gridW];

// the screen that the player is currently on.
char screen = 'M';

// maximum allowable number of stars on any level
const int maxStars = 50;

// the current score that the player has
int totalStars = 0;

// the current score that the player has
int stars = 0;

// prevents the pause menu from flashing on and off
bool pausePressed = false;

// if the user has passed through a checkpoint on this level
bool checkpointExists = false;

// the location of said checkpoint in level array
int checkpoint = 0;

// the stars that have been collected at the checkpoint
int checkpointStars[maxStars];

// stars that have already been collected
int starsCollected[maxStars];

// EXTRA KEY CODES
const int KEY_ENTER = 0xD;
const int KEY_SPACE = 0x20;

/*
 * Function: IfPlayerCanMove
 * Usage: IfPlayerCanMove(D_UP);
 * ------------------------------------------
 * Performs collision detection to determine whether the player is allowed 
 * to move in the given direction.
 */
const int D_UP    = 1;
const int D_RIGHT = 2;
const int D_DOWN  = 3;
const int D_LEFT  = 4;
bool IfPlayerCanMove (int direction);

/*
 * Function: HitObject
 * Usage: HitObject(x, y);
 * ------------------------------------------
 * Determines what object the player has hit, then performs a certain action 
 * based on the type of that object. It returns a boolean value which 
 * determines whether the user can continue moving in that direction. Actions 
 * are as follows:
 * - {space} is air, it does not impede the player or do anything
 * - B, C, G, and Q are different solid objects. They stop the player
 * - X kills the player
 * - S gives the player 100 points and does not impede the player
 */
bool HitObject(int x, int y);

/*
 * Function: ShowScore
 * Usage: ShowScore();
 * ------------------------------------------
 * Prints the player's score to the console
 */
void ShowScore();

/*
 * Function: MovePlayer
 * Usage: MovePlayer(-2, 0);
 * ------------------------------------------
 * Moves the player a certain number of pixels. Also handles errors if the 
 * player tries to jump off a cliff or otherwise tries to leave the map.
 */
void MovePlayer (int changeX, int changeY, bool newLocation = false);

#endif
