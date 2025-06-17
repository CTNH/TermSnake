#include "SnakeGame.h"

#include <cstdio>	// stdout, fputs
#include <cstdlib>	// srand
#include <cstring>	// strlen
#include <ctime>	// time

const char* Snake:: WALL_CELL = "\e[48;5;245m  \e[0m";
const char* Snake:: SNAKE_CELL = "\e[48;5;77m--\e[0m";
const char* Snake:: FOOD_CELL = "\e[48;5;196m  \e[0m";

bool Snake:: growCheck() {
	bool outBounds;		// Whether new head is out of bounds
	int newHeadPos;		// New head position if grew
	switch (direction) {
		case Direction::UP:
			outBounds = headpos < fieldWidth;
			newHeadPos = headpos - fieldWidth;
			break;
		case Direction::DOWN:
			outBounds = headpos > (fieldWidth * (fieldHeight - 1) - 1);
			newHeadPos = headpos + fieldWidth;
			break;
		case Direction::LEFT:
			outBounds = (headpos % fieldWidth) == 0;
			newHeadPos = headpos - 1;
			break;
		case Direction::RIGHT:
			outBounds = (headpos % fieldWidth) == (fieldWidth - 1);
			newHeadPos = headpos + 1;
			break;
	}
	if (outBounds or ((field[newHeadPos/8] >> (newHeadPos % 8)) & 1))
		return false;

	return true;
}

bool Snake:: grow() {
	if (!growCheck())
		return false;
	body.push(direction);
	field[headpos/8] |= 1 << (headpos % 8);
	switch (direction) {
		case Direction::UP:
			headpos -= fieldWidth;
			break;
		case Direction::DOWN:
			headpos += fieldWidth;
			break;
		case Direction::LEFT:
			headpos--;
			break;
		case Direction::RIGHT:
			headpos++;
			break;
	}
	drawCell((char*) SNAKE_CELL, headpos);
	return true;
}

// Creates food on the map
void Snake:: findFood() {
	srand(time(0));
	int newFood;
	// Continue to generate food until space found
	while (1) {
		newFood = rand() % (fieldWidth * fieldHeight);
		// Check if new food collides with body
		if ((field[newFood/8] >> (newFood % 8)) & 1)
			continue;
		// Food right under head
		if (newFood == headpos)
			continue;
		// Food already exists in list
		if ((food.insert(newFood)).second != true) {
			continue;
		}
		break;
	}
	drawCell((char*) FOOD_CELL, newFood);
}

// Constructor
Snake:: Snake() {
	Snake(21, 21, 1, 1, 213, 3, Direction::RIGHT);
}
Snake:: Snake(int fieldWidth, int fieldHeight, int fieldOffsetX, int fieldOffsetY, int startPos, int startLength, Direction startDirection) {
	this -> fieldWidth = fieldWidth;
	this -> fieldHeight = fieldHeight;
	this -> fieldOffsetX = fieldOffsetX;
	this -> fieldOffsetY = fieldOffsetY;
	// Ensures snake is valid
	if (startLength < 1) {
		startLength = 1;
	}

	bool loop = 1;
	while (loop) {
		// Create field with every space on field as a bit
		vector<uint8_t> field((fieldHeight * fieldWidth) / 8 + (((fieldWidth * fieldHeight) % 8) > 0), 0);
		this -> field = field;

		length = startLength;

		body = queue<Direction>();	// New body

		direction = startDirection;
		headpos = startPos;
		tailpos = startPos;

		ate = 0;
		food.clear();
		loop = start();
	}
}

// Updates snake position
Snake::SnakeState Snake:: move(Direction direction) {
	SnakeState result = SnakeState::MOVE;
	int oldTailPos = tailpos;
	// Consumes ate food to grow
	if (ate > 0) {
		ate--;
		length++;
		result = SnakeState::GROW;
	}
	// No food to grow; remove tail
	else {
		// Clear the bit for tail in field
		field[tailpos/8] &= ~(1 << (tailpos % 8));
		// Must remove tail before food found but after if grow to not remove if dead
		drawCell((char*) "  ", oldTailPos);
		// Get new tail position by direction of current tail
		switch (body.front()) {
			case Direction::UP:
				tailpos -= fieldWidth;
				break;
			case Direction::DOWN:
				tailpos += fieldWidth;
				break;
			case Direction::LEFT:
				tailpos--;
				break;
			case Direction::RIGHT:
				tailpos++;
				break;
		}
		// Remove tail from body
		body.pop();
	}
	// Attempt to grow
	this -> direction = direction;
	switch (direction) {
		case Direction::UP:
			drawCell((char*) "\e[38;5;233;48;5;82m/\\\e[0m", headpos);
			break;
		case Direction::DOWN:
			drawCell((char*) "\e[38;5;233;48;5;82m\\/\e[0m", headpos);
			break;
		case Direction::LEFT:
			drawCell((char*) "\e[38;5;233;48;5;82m< \e[0m", headpos);
			break;
		case Direction::RIGHT:
			drawCell((char*) "\e[38;5;233;48;5;82m >\e[0m", headpos);
			break;
	}
	// Grow is unsuccessful
	if (!grow()) {
		// Redraw snake cell
		drawCell((char*) "\e[48;5;82m  \e[0m", oldTailPos);
		return SnakeState::DEAD;
	}
	// Eat food if exist
	if (food.find(headpos) != food.end()) {		// If head position is in list
		food.erase(headpos);
		ate++;
		findFood();		// Replenish food
	}
	return result;
}

void Snake:: drawCell(char* cell, int pos) {
	gotoxy(((pos % fieldWidth) * 2) + 3 + fieldOffsetX, pos/fieldWidth + 2 + fieldOffsetY);
	fputs(cell, stdout);
}

// Handles UI and gameLoop
bool Snake:: start() {
	init();

	// Assign initial direction
	int startLength = 3;
	drawCell((char*) SNAKE_CELL, tailpos);
	for (int i=0; i<startLength-1; i++) {
		grow();
	}

	// Generate food
	findFood();

	// Draw walls
	// Top Wall
	gotoxy(1+fieldOffsetX, 1+fieldOffsetY);
	for (int i=0; i<fieldWidth+2; i++) {
		fputs(WALL_CELL, stdout);
	}
	// Bottom Wall
	gotoxy(1 + fieldOffsetX, fieldHeight + 2 + fieldOffsetY);
	for (int i=0; i<fieldWidth+2; i++) {
		fputs(WALL_CELL, stdout);
	}
	// Side Walls
	for (int i=2+fieldOffsetY; i<fieldHeight+2+fieldOffsetY; i++) {
		gotoxy(1 + fieldOffsetX, i);
		fputs(WALL_CELL, stdout);
		gotoxy(fieldWidth * 2 + 3 + fieldOffsetX, i);
		fputs(WALL_CELL, stdout);
	}

	inputBlock(false);

	// Game gameplay loop
	int ups = 160;		// Updates per second
	int speed = 15;		// Update per n frames
	int loopCount = 0;

	Direction nextDirection = direction;	// Used to prevent 180 turns
	char c;		// To store input
	while (c != 'q') {
		c = getchar();

		switch (c) {
			case 'w':
			case 'k':
				if (direction != Direction::DOWN)
					nextDirection = Direction::UP;
				break;
			case 's':
			case 'j':
				if (direction != Direction::UP)
					nextDirection = Direction::DOWN;
				break;
			case 'a':
			case 'h':
				if (direction != Direction::RIGHT)
					nextDirection = Direction::LEFT;
				break;
			case 'd':
			case 'l':
				if (direction != Direction::LEFT)
					nextDirection = Direction::RIGHT;
				break;
			// Pause
			case ' ':
				inputBlock(true);
				gotoxy(5+fieldOffsetX + fieldWidth*2 - 6, fieldHeight + 3 + fieldOffsetY);
				fputs("Paused", stdout);
				getchar();
				gotoxy(5+fieldOffsetX + fieldWidth*2 - 6, fieldHeight + 3 + fieldOffsetY);
				fputs("      ", stdout);
				inputBlock(false);
				break;
			// Arrows
			case 27:
				getchar();	// 91
				switch (getchar()) {
					case 65:
						if (direction != Direction::DOWN)
							nextDirection = Direction::UP;
						break;
					case 66:
				if (direction != Direction::UP)
					nextDirection = Direction::DOWN;
						break;
					case 67:
				if (direction != Direction::LEFT)
					nextDirection = Direction::RIGHT;
						break;
					case 68:
				if (direction != Direction::RIGHT)
					nextDirection = Direction::LEFT;
						break;
				}
				break;
			default:
				break;
		}

		usleep(1000000 / ups);

		// loopCount = (loopCount + 1) % speed;
		//if (loopCount == (speed - 1)) {
		loopCount++;
		if (loopCount == speed) {
			loopCount = 0;

			int oldTailPos = tailpos;
			switch (move(nextDirection)) {
				case SnakeState::MOVE:
				case SnakeState::GROW:
					// drawCell((char*) SNAKE_CELL, headpos);
					gotoxy(1 + fieldOffsetX, fieldHeight + 3 + fieldOffsetY);
					printf("Length %d", length);
					break;
				case SnakeState::DEAD:
					char* deathMsg = (char*) "You died!";
					gotoxy(5+fieldOffsetX + fieldWidth*2 - strlen(deathMsg), fieldHeight + 3 + fieldOffsetY);
					fputs(deathMsg, stdout);
					c = 'q';
					break;
			}
		}
	}

	inputBlock(true);

	gotoxy(1 + fieldOffsetX, fieldHeight + 4 + fieldOffsetY);
	fputs("Press r to restart, q to quit", stdout);
	while (1) {
		c = getchar();
		if (c == 'q' or c == 'r')
			break;
	}

	end();

	if (c == 'r')
		return true;
	return false;
}

