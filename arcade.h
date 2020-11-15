/* 
 * File: arcade.h
 * -----------------------------------------------------
 * This interface provides access to a set of data types
 * a and simple library of functions that make it possible
 * to create simple 2D arcade games. This interface presents
 * a portable abstraction that can be used with a variety of
 * window systems implemented on different hardware platforms.
 */

#ifndef _arcade_h
#define _arcade_h

#include <string>

/*
 * Typical program form:
 *
 * int main()
 * {
 *     InitArcade("Bounce",320,240,0,0,0,100);
 *     while (IsArcadeActive()) {
 *         if (IsKeyPressed(ESCAPE))
 *             break;
 *         ...
 *         DrawSprite("ball.gif",x,y);
 *         ...
 *     }
 * }
 */

/*
 * Function: InitArcade
 * Usage: InitArcade("Bounce",320,240,0,0,0,100);
 * ----------------------
 * This procedure creates the graphics window and prepares
 * the game environment. The first three numbers set the
 * background color for the window. The pace of the game is
 * determined by the numbers of milliseconds specified as the
 * interval. The call to InitGraphics must precede calls to
 * other functions in this package.
 */
 
void InitArcade(const std::string &name,int width,int height,double r,double g,double b,int interval);

/*
 * Function: IsKeyPressed
 * Usage: if (IsKeyPressed(ESCAPE)) ...
 * ------------------------------------------
 * Determine if a key is currently pressed. One of the following
 * constants should be passed.
 */
 
const int KEY_SHIFT     = 0x10;
const int KEY_CONTROL   = 0x11;
const int KEY_ESCAPE    = 0x1B;
const int KEY_PAGE_UP   = 0x21;
const int KEY_PAGE_DOWN = 0x22;
const int KEY_END       = 0x23;
const int KEY_HOME      = 0x24;
const int KEY_LEFT      = 0x25;
const int KEY_UP        = 0x26;
const int KEY_RIGHT     = 0x27;
const int KEY_DOWN      = 0x28;
const int KEY_INSERT    = 0x2D;
const int KEY_DELETE    = 0x2E;

bool IsKeyPressed(int key);

/*
 * Function: DrawSprite
 * Usage: DrawSprite("ship.gif",x,y);
 * ------------------------------------------
 * Draws an image taken from the file specified at location
 * (x,y). Can only be used between calls to DrawBackground()
 * and Wait().
 */

void DrawSprite(const std::string &name, int x, int y);

/*
 * Function: IsArcadeActive
 * Usage: while (IsArcadeActive()) { ... }
 * ------------------------------------------
 * Determines if the arcade window is still running.
 */

bool IsArcadeActive();


/*
 * Function: UpdateArcade()
 * ------------------------------------------
 * Update arcade visualization.
 */

void UpdateArcade();


#endif
