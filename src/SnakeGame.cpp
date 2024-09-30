#include "SnakeGame.h"

#include <cstdio>	// stdout, fputs
#include <cstdlib>	// srand
#include <ctime>	// time

const char* Snake:: SNAKE_CELL = "\e[48;5;82m_|\e[0m";
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
	gotoxy(((newFood % fieldWidth) * 2) + 3 + fieldOffsetX, newFood / fieldWidth + 2 + fieldOffsetY);
	fputs(FOOD_CELL, stdout);
}

// Constructor
// Snake:: Snake(int fieldWidth, int fieldHeight, int startPos, int startLength, Direction direction) {
Snake:: Snake(int fieldWidth, int fieldHeight, int fieldOffsetX, int fieldOffsetY, int startPos, int startLength, Direction direction) {
	this -> fieldWidth = fieldWidth;
	this -> fieldHeight = fieldHeight;
	this -> fieldOffsetX = fieldOffsetX;
	this -> fieldOffsetY = fieldOffsetY;
	// Create field with every space on field as a bit
	vector<uint8_t> field((fieldHeight * fieldWidth) / 8 + (((fieldWidth * fieldHeight) % 8) > 0), 0);
	this -> field = field;
	// Ensures snake is valid
	if (startLength < 1) {
		startLength = 1;
	}

	// Assign initial direction
	this -> direction = direction;
	headpos = startPos;
	for (int i=0; i<startLength-1; i++) {
		grow();
	}
	tailpos = startPos;

	// Generate food
	findFood();
}

Snake::Direction Snake:: getDirection() {
	return direction;
}
int Snake:: getHeadPos() {
	return headpos;
}
int Snake:: getTailPos() {
	return tailpos;
}

// Updates snake position
Snake::SnakeState Snake:: move(Direction direction) {
	SnakeState result = SnakeState::MOVE;
	// Consumes ate food to grow
	if (ate > 0) {
		ate--;
		result = SnakeState::GROW;
	}
	// No food to grow; remove tail
	else {
		// Clear the bit for tail in field
		field[tailpos/8] &= ~(1 << (tailpos % 8));
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
	// Grow is unsuccessful
	if (!grow()) {
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

