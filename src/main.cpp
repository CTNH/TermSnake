#include <cstdio>		// printf, getchar
#include <cstdlib>		// malloc
#include <cstring>		// strlen
#include <ctime>		// time() for srand()
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

#include "ANSI_UI.h"
#include "SnakeGame.h"

using namespace std;

#define COLOR_NONE	"\e[0m"
#define BG_RED		"\e[48;5;196m"
#define BG_GREEN	"\e[48;5;82m"


// Print a string at x and y position
void xyPrint(int x, int y, char*) {}


int main (int argc, char *argv[]) {
	int offsetX = 10, offsetY = 1;
	ANSI_UI aui;
	aui.init();

	int fieldWidth = 21, fieldHeight = 21, startPos = 215, snakeLength = 3;
	// fieldWidth = 2, fieldHeight = 2, startPos=2, snakeLength=2;
	// fieldWidth = 25, fieldHeight = 20, startPos=253, snakeLength=8;
	
	char* snakeString = (char*) "_|";

	aui.gotoxy(1+offsetX, 1+offsetY);
	printf("\e[48;5;246m");
	for (int i=0; i<fieldWidth+2; i++) {
		printf("  ");
	}
	printf("%s", COLOR_NONE);
	aui.gotoxy(1 + offsetX, fieldHeight + 2 + offsetY);
	printf("\e[48;5;246m");
	for (int i=0; i<fieldWidth+2; i++) {
		printf("  ");
	}
	printf("%s", COLOR_NONE);
	for (int i=2+offsetY; i<fieldHeight+2+offsetY; i++) {
		aui.gotoxy(1+offsetX, i);
		printf("\e[48;5;245m  %s", COLOR_NONE);
		aui.gotoxy(fieldWidth*2+3+offsetX, i);
		printf("\e[48;5;245m  %s", COLOR_NONE);
	}

	// Create the player instance
	Snake* player = new Snake(fieldWidth, fieldHeight, offsetX, offsetY, startPos, snakeLength, Snake::Direction::RIGHT);
	// Draw the snake
	aui.gotoxy(((startPos % fieldWidth) * 2)+3 + offsetX, startPos/fieldWidth+2 + offsetY);
	printf("%s", BG_GREEN);
	for (int i=0; i<snakeLength; i++) {
		printf("%s", snakeString);
	}
	printf("%s", COLOR_NONE);

	aui.inputBuffering(false);
	int color = 0, kp = 0;
	char c;
	int fps = 120;
	int speed = 13;		// Update per n frames
	int loopCount = 0;

	Snake::Direction direction = Snake::Direction::RIGHT;		// Needed to prevent turning 180
	while (c != 'q') {
		c = getchar();

		Snake::Direction currentDirection = player -> getDirection();
		switch (c) {
			case 'w':
			case 'k':
				if (currentDirection != Snake::Direction::DOWN)
					direction = Snake::Direction::UP;
				break;
			case 's':
			case 'j':
				if (currentDirection != Snake::Direction::UP)
					direction = Snake::Direction::DOWN;
				break;
			case 'a':
			case 'h':
				if (currentDirection != Snake::Direction::RIGHT)
					direction = Snake::Direction::LEFT;
				break;
			case 'd':
			case 'l':
				if (currentDirection != Snake::Direction::LEFT)
					direction = Snake::Direction::RIGHT;
				break;
			case ' ':	// Pause
				aui.inputBuffering(true);
				getchar();
				aui.inputBuffering(false);
				break;
			default:
				break;
		}

		usleep(1000000 / fps);	// 60 'updates' per second

		loopCount = (loopCount+1) % speed;
		if (loopCount == (speed - 1)) {
			int oldTailPos = player -> getTailPos();
			switch (player -> move(direction)) {
				case Snake::SnakeState::DEAD: {
						char* deathMsg = (char*) "You died!";
						aui.gotoxy(5+offsetX + fieldWidth*2 - strlen(deathMsg), fieldHeight + 3 + offsetY);
						printf("%s", deathMsg);
						c = 'q';
					}
					break;
				case Snake::SnakeState::MOVE:
					aui.gotoxy(((oldTailPos % fieldWidth+1) * 2)+1+offsetX, oldTailPos/fieldWidth+1 +1 +offsetY);
					printf("  ");
					snakeLength--;
				case Snake::SnakeState::GROW:
					snakeLength++;
					aui.gotoxy(((player->getHeadPos() % fieldWidth) * 2)+3+offsetX, player->getHeadPos()/fieldWidth+1 +1 +offsetY);
					printf("%s%s%s", BG_GREEN, snakeString, COLOR_NONE);
					aui.gotoxy(1 + offsetX, fieldHeight + 3 + offsetY);
					printf("Length %d", snakeLength);
					break;
			}
		}
	}
	free(player);
	aui.inputBuffering(true);
	while (getchar() != 'q') ;


	aui.end();
	return 0;
}

