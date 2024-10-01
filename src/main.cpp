#include "SnakeGame.h"

using namespace std;

int main (int argc, char *argv[]) {
	int offsetX = 10, offsetY = 1;

	int fieldWidth = 21, fieldHeight = 21, startPos = 215, snakeLength = 3;
	// fieldWidth = 2, fieldHeight = 2, startPos=2, snakeLength=2;
	// fieldWidth = 25, fieldHeight = 20, startPos=253, snakeLength=8;
	
	// Snake* game = new Snake(fieldWidth, fieldHeight, offsetX, offsetY, startPos, snakeLength, Snake::Direction::RIGHT);
	Snake game(21, 21, 40, 2, 215, 3, Snake::Direction::RIGHT);

	return 0;
}

