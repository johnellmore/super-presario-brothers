/*
 * CS120
 * John Ellmore
 * This awesome game follows the adventures of Timmy the Presario as he finds
 * himself in a strange new world.
 */

#include <iostream>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include "mario.h"
#include "arcade.h"
using namespace std;

bool IfPlayerCanMove (int direction) {
	// get the coordinates to check
	int checkXi = 0;
	int checkYi = 0;
	if (direction == D_UP) {
		checkYi--;
	} else if (direction == D_RIGHT) {
		checkXi++;
	} else if (direction == D_DOWN) {
		checkYi++;
	} else if (direction == D_LEFT) {
		checkXi--;
	}
	
	// check corners of the player
	int checkX;
	int checkY;
	
	bool canMove = true;
	// top left corner
	if (direction == D_UP || direction == D_LEFT) {
		checkX = checkXi + playerX;
		checkY = checkYi + playerY;
		if (HitObject(checkX, checkY))
			canMove = false;
	}
	// top right corner
	if (direction == D_UP || direction == D_RIGHT) {
		checkX = checkXi + playerX + gridCellW - 1;
		checkY = checkYi + playerY;
		if (HitObject(checkX, checkY))
			canMove = false;
	}
	// bottom right corner
	if (direction == D_DOWN || direction == D_RIGHT) {
		checkX = checkXi + playerX + gridCellW - 1;
		checkY = checkYi + playerY + gridCellH - 1;
		if (HitObject(checkX, checkY))
			canMove = false;
	}
	// bottom left corner
	if (direction == D_DOWN || direction == D_LEFT) {
		checkX = checkXi + playerX;
		checkY = checkYi + playerY + gridCellH - 1;
		if (HitObject(checkX, checkY))
			canMove = false;
	}
	return canMove;
}

bool HitObject(int x, int y) {
	// take the appropriate action when a player hits an obstacle
	int gridX = x / gridCellW;
	int gridY = y / gridCellH;
	if (gridY < 0) {
		return false;
	}
	
	switch (level[gridY][gridX]) {
		// A represents a checkpoint
		case 'A':
		if (checkpoint != (gridY * gridW + gridX)) {
			cout << "  Checkpoint!" << endl;
		}
		checkpointExists = true;
		checkpoint = (gridY * gridW) + gridX;
		for (int index = 0; index < maxStars; index++) {
			checkpointStars[index] = starsCollected[index];
		}
		return false;
		
		// S represents a star, which the user is supposed to acquire
		case 'S':
		stars += 1;
		ShowScore();
		level[gridY][gridX] = ' ';
		
		{
		int lastIndex = 0;
		for (int i = 0; i < maxStars; i++) {
			lastIndex = i;
			if (starsCollected[i] == -1)
				break;
		}
		starsCollected[lastIndex] = gridY * gridW + gridX;
		}
		
		if (stars == totalStars) {
			cout << "You beat the level!" << endl;
			screen = 'W';
		}
		return false;
		
		// B, C, G, and Q represent walls
		case 'B':
		case 'C':
		case 'G':
		case 'Q':
		return true;
		
		// X represents an invisible deadly object, in this case an abyss
		case 'X':
		case 'M':
		screen = 'X';
		return true;
		
		// J is a spring that launches the player higher than they could normally jump
		case 'J':
		verticalForce = -1 * springJumpHeight;
		return true;
		
		default:
		return false;
	}
}

void MovePlayer (int changeX, int changeY, bool newLocation) {
	int newX, newY;
	// set player physical location
	if (newLocation) {
		newX = changeX;
		newY = changeY;
	} else {
		newX = playerX + changeX;
		newY = playerY + changeY;
	}
	
	// set the boundaries
	// +1's are a workaround, I don't really want to figure out why my 
	// character positioning is off by one pixel horizontally
	if (newX > -1 && newX < (levelW - gridCellW + 1)) {
		playerX = newX;
	}
	if (newY < (gridH * gridCellH + gridCellH)) {
		playerY = newY;
	} else {
		screen = 'X';
	}
	
	// set the viewport
	int xIfCentered = (windowGridW * gridCellW / 2) - (gridCellW / 2);
	// left bound of the level
	if (playerX < xIfCentered) {
		viewportX = 0;
	}
	// right bound of the level
	else if (playerX > levelW - xIfCentered - gridCellW) {
		viewportX = levelW - (windowGridW * gridCellW);
	}
	// moving with the player
	else {
		viewportX = playerX - xIfCentered;
	}
}

void ShowScore () {
	cout << "  You have " << stars
		<< " out of " << totalStars << " stars" << endl;
}

int main()
{
	// create the canvas
	InitArcade("Super Presario Brothers", (windowGridW * gridCellW), (gridH * gridCellH), 1, 1, 1, 25);
	
	// position of the clouds
	double dCloudsX = 0;
	
	// whether a level file for the next level exists
	// -1 means haven't checked yet
	int nextLevelExists = -1;
	
	// whether the user is in the middle of a jump or not
	bool jumping = false;
	
	while (IsArcadeActive()) {
		// decide which screen we are going to.
		switch (screen) {
			case 'M': // main screen
				DrawSprite("screens/start.jpg", 0, 0);
				
				// begin the game when the user presses control
				if (IsKeyPressed(KEY_ENTER)) {
					screen = 'L';
				}
			break;
			
			case 'L': // loading screen
				DrawSprite("screens/loading.jpg", 0, 0);
				
				// reset common variables for another level
				viewportX = 0;
				totalStars = 0;
				stars = 0;
				levelW = 0;
				verticalForce = 0;
				
				// temporary player x and y coordinates
				int tempX;
				int tempY;
				
				if (!checkpointExists) {
					for (int i = 0; i < 50; i++) {
						starsCollected[i] = -1;
					}
				}
				
				// determine the name of the level file
				char levelFilename[20];
				sprintf(levelFilename, "levels/level%d.lvl", levelNum);
				
				{
				// Open the level file and process it
				fstream levelFile(levelFilename, ios::in);
				if (levelFile.is_open()) {
					// for every cell in the grid
					for (int line = 0; line < gridH; line++) {
						// keep reading tiles until we hit a newline
						int col = 0;
						while (levelFile.get(level[line][col]) // while the file still has characters
							&& col < gridW) { // and the file isn't too big for our array
							
							if (level[line][col] == 0xA) {
								for (int remaining = col; remaining < gridW; remaining++) {
									level[line][remaining] = 0x0;
								}
								break;
							}
							
							// if we find the player location, set the player to that location, but only
							// if the user has not been through a checkpoint
							if (level[line][col] == 'P' && !checkpointExists) {
								tempX = col * gridCellW;
								tempY = line * gridCellH;
							}
							
							// if it's a star, increase our star total
							else if (level[line][col] == 'S') {
								bool starAlreadyCollected = false;
								if (checkpointExists) {
									for (int i = 0; i < maxStars && checkpointStars[i] != -1; i++) {
										if (checkpointStars[i] == line * gridW + col) {
											starAlreadyCollected = true;
										}
									}
								}
								totalStars++;
								if (starAlreadyCollected) {
									stars++;
									level[line][col] = ' ';
								}
							}
							
							// calculate the level's width
							if ((col + 1) * gridCellW > levelW) {
								levelW = (col + 1) * gridCellW;
							}
							col++;
						}
					}
				levelFile.close();
				}
				}
				
				cout << "Level " << (levelNum + 1) << ":" << endl;
				cout << "  There are " << totalStars << " stars on this level." << endl;
				
				if (checkpointExists) {
					cout << "  Respawning at last checkpoint" << endl;
					tempX = (checkpoint % gridW) * gridCellW;
					tempY = (checkpoint / gridW) * gridCellH;
					ShowScore();
				}
				MovePlayer(tempX, tempY, true);
				
				// now that the level is loaded, start game
				screen = 'G';
			break;
			
			case 'G': { // game
				/* PLAYER MOTION */
				
				// falling
				if (verticalForce == 0 && IfPlayerCanMove(D_DOWN)) {
					verticalForce = 1;
				}
				// jumping
				else if (IsKeyPressed(KEY_UP) && verticalForce == 0 && IfPlayerCanMove(D_UP) && !IfPlayerCanMove(D_DOWN)) {
					// This will basically be handled like inverse gravity
					verticalForce = -1 * jumpHeight;
					jumping = true;
				}
				
				// calculate player gravity
				int travelY = 0;
				if (verticalForce) {
					travelY = (int)(gravity * abs(verticalForce));
				}
				
				// left or right movement to be processed bewtween frames
				int travelX = 0;
				int directionX = 0;
				bool couldMoveHorizontal = true;
				if (IsKeyPressed(KEY_LEFT)) {
					directionX = D_LEFT;
					if (IfPlayerCanMove(D_LEFT)) {
						travelX = 2;
					} else {
						couldMoveHorizontal = false;
					}
				} else if (IsKeyPressed(KEY_RIGHT)) {
					directionX = D_RIGHT;
					if (IfPlayerCanMove(D_RIGHT)) {
						travelX = 2;
					} else {
						couldMoveHorizontal = false;
					}
				}
				
				// perform vertical movement
				for (int moveY = 0; moveY < travelY; moveY++) {
					bool moveSuccessful = false;
					
					// move for jumping
					if (verticalForce < 0 && IfPlayerCanMove(D_UP)) {
						MovePlayer(0, -1);
						moveSuccessful = true;
					}
					// move for falling
					else if (verticalForce > 0 && IfPlayerCanMove(D_DOWN)) {
						MovePlayer(0, 1);
						moveSuccessful = true;
					}
					
					// check to see if the player wants to move horizontally
					if (!couldMoveHorizontal) {
						if (directionX == D_LEFT && IfPlayerCanMove(D_LEFT)) {
							MovePlayer(-2, 0);
							travelX = 0;
							break;
						} else if (directionX == D_RIGHT && IfPlayerCanMove(D_RIGHT)) {
							MovePlayer(2, 0);
							travelX = 0;
							break;
						}
					}
					
					// if there's a collision
					if (!moveSuccessful) {
						verticalForce = -1;
						jumping = false;
						break;
					}
				}
				if (verticalForce) {
					verticalForce++;
				}
				
				// perform horizontal movement
				for (int moveX = 0; moveX < travelX; moveX++) {
					if (verticalForce == 0 && IfPlayerCanMove(D_DOWN) && !jumping) {
						MovePlayer(0, 1);
						break;
					}
					
					// move left
					if (directionX == D_LEFT && IfPlayerCanMove(D_LEFT)) {
						MovePlayer(-1, 0);
					}
					// move right
					else if (directionX == D_RIGHT && IfPlayerCanMove(D_RIGHT)) {
						MovePlayer(1, 0);
					}
					// if there's a collision or no horizontal movement
					else {
						break;
					}
				}
				
				/* DRAW THE LEVEL */
				// draw clouds - they move proportionally to the viewport, as well as
				// on their own
				double cloudsX = fmod(0 - (viewportX / 4) - dCloudsX, 576.0);
				DrawSprite("props/clouds.jpg", (int)cloudsX, 0);
				DrawSprite("props/clouds.jpg", (int)cloudsX + 576, 0);
				dCloudsX += 0.1;
				
				// draw mountains - they move proportionally to the viewport
				double mountainsX = 0 - (viewportX / 2 % 576);
				DrawSprite("props/mountains.gif", (int)mountainsX, 56);
				DrawSprite("props/mountains.gif", (int)mountainsX + 576, 56);
				
				// draw the obstacles
				for (int y = 0; y < gridH; y++) {
					// render only the obstacles that are in our viewport at the moment
					for (int x = (viewportX / gridCellW); level[y][x] != 0xA
						&& x < (viewportX / gridCellW) + windowGridW + 1; x++) {
						switch (level[y][x]) {
							case 'B':
								DrawSprite("sprites/B.bmp", (x * gridCellW) - viewportX, y * gridCellH);
							break;
							case 'C':
								DrawSprite("sprites/C.bmp", (x * gridCellW) - viewportX, y * gridCellH);
							break;
							case 'G':
								DrawSprite("sprites/G.bmp", (x * gridCellW) - viewportX, y * gridCellH);
							break;
							case 'M':
								DrawSprite("sprites/M.gif", (x * gridCellW) - viewportX, y * gridCellH);
							break;
							case 'S':
								DrawSprite("sprites/S.gif", (x * gridCellW) - viewportX, y * gridCellH);
							break;
							case 'J':
								DrawSprite("sprites/J.gif", (x * gridCellW) - viewportX, y * gridCellH);
							break;
						}
					}
				}
				
				// draw the player
				DrawSprite("sprites/timmy.gif", playerX - viewportX, playerY);
				}
			break;
			
			case 'X': // lose screen
				DrawSprite("screens/dead.jpg", 0, 0);
				
				// exit the game
				if (IsKeyPressed(KEY_SPACE)) {
					screen = 'L';
				}
			break;
			
			case 'W': // level win screen
				if (nextLevelExists == -1) {
					char levelNextLevel[20];
					sprintf(levelNextLevel, "levels/level%d.lvl", levelNum + 1);
					
					struct stat fileExists;
					nextLevelExists = (stat(levelNextLevel, &fileExists)) ? 0: 1; 
				}
				
				// level win
				if (nextLevelExists) {
					DrawSprite("screens/win.jpg", 0, 0);
					
					// continue to the next level
					if (IsKeyPressed(KEY_SPACE)) {
						checkpointExists = false;
						checkpoint = 0;
						nextLevelExists = -1;
						levelNum++;
						screen = 'L';
					}
				}
				// game win
				else {
					DrawSprite("screens/gamewin.jpg", 0, 0);
					
					// exit the game
					if (IsKeyPressed(KEY_ENTER)) {
						cout << "You beat the game!";
						exit(0);
					}
				}
			break;
		}
	}
}
