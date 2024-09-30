#ifndef SNAKE_GAME_H
#define SNAKE_GAME_H

#include <cstdint>	// uint8_t
#include <vector>
#include <unordered_set>
#include <queue>
using namespace std;

#include "ANSI_UI.h"

class Snake : public ANSI_UI {
	public:
		enum class Direction : uint8_t {
			UP    = 0,
			DOWN  = 1,
			LEFT  = 2,
			RIGHT = 3
		};
		enum class SnakeState : uint8_t {
			DEAD = 0,
			MOVE = 1,
			GROW = 2
		};

	private:
		static const char* SNAKE_CELL;
		static const char* FOOD_CELL;

		int fieldOffsetX, fieldOffsetY;

		int fieldWidth, fieldHeight;
		vector<uint8_t> field;
		int headpos, tailpos;
		int ate;		// Food eaten to be consumed
		unordered_set<int> food;

		Direction direction;		// Head direction
		queue<Direction> body;	// Does not keep track of head

		bool growCheck();
		bool grow();
		void findFood();
	public:
		// Constructor
		Snake(int fieldWidth, int fieldHeight, int fieldOffsetX, int fieldOffsetY, int startPos, int startLength, Direction direction);

		// Updates snake position
		SnakeState move(Direction direction);

		// Getters
		Direction getDirection();
		int getHeadPos();
		int getTailPos();
};

#endif // !SNAKE_GAME_H

