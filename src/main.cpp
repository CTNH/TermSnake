#include <cstdint>		// uint8_t
#include <cstdio>		// printf, getchar
#include <cstdlib>		// malloc
#include <queue>		// queue
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

#define COLOR_NONE	"\e[0m"
#define BG_RED		"\e[48;5;196m"
#define BG_GREEN	"\e[48;5;82m"

enum class SnakeState : uint8_t {
	DEAD = 0,
	MOVE = 1,
	GROW = 2
};

class Snake {
	private:
		int fieldWidth, fieldHeight;
		uint8_t* field;
		int headpos, tailpos;
		int food;		// Food eaten

		// 0 - Up
		// 1 - Down
		// 2 - Left
		// 3 - Right
		uint8_t direction;		// Head direction
		queue<uint8_t> body;	// Does not keep track of head

		bool growCheck() {
			bool outBounds;		// Whether new head is out of bounds
			int newHeadPos;		// New head position if grew
			switch (direction) {
				case 0:
					outBounds = headpos < fieldWidth;
					newHeadPos = -fieldHeight;
					break;
				case 1:
					outBounds = headpos > (fieldWidth * (fieldHeight - 1));
					newHeadPos = fieldHeight;
					break;
				case 2:
					outBounds = headpos % fieldWidth;
					newHeadPos = -1;
					break;
				case 3:
					outBounds = (headpos % fieldWidth) == (fieldWidth - 1);
					newHeadPos = 1;
					break;
			}
			if (outBounds or ((field[newHeadPos/8] >> (newHeadPos % 8)) & 1))
				return false;

			return true;
		}

		bool grow() {
			if (!growCheck())
				return false;
			body.push(direction);
			switch (direction) {
				case 0:
					headpos -= fieldHeight;
					break;
				case 1:
					headpos += fieldHeight;
					break;
				case 2:
					headpos--;
					break;
				case 3:
					headpos++;
					break;
			}
			field[headpos/8] |= 1 << (headpos % 8);
			return true;
		}

	public:
		// Constructor
		Snake(int fieldWidth, int fieldHeight, int startPos, int startLength) {
			this -> fieldWidth = fieldWidth;
			this -> fieldHeight = fieldHeight;
			field = (uint8_t*) malloc((fieldHeight * fieldWidth) / 8 + (((fieldWidth * fieldHeight) % 8) > 0));

			if (startLength < 1) {
				startLength = 1;
			}
			direction = 3;
			headpos = startPos;
			for (int i=0; i<startLength; i++) {
				grow();
			}
			tailpos = startPos;
		}
		
		// Getters for head and tail positions
		int getHeadPos() {
			return headpos;
		}
		int getTailPos() {
			return tailpos;
		}

		// Updates snake position
		SnakeState move() {
			if (!grow()) {
				return SnakeState::DEAD;
			}
			// Consumes food to grow
			if (food > 0) {
				food--;
				return SnakeState::GROW;
			}
			// No food to grow; remove tail
			// Clear the bit for the tail
			field[headpos/8] &= ~(1 << (headpos % 8));
			switch (body.front()) {
				case 0:
					tailpos -= fieldHeight;
					break;
				case 1:
					tailpos += fieldHeight;
					break;
				case 2:
					tailpos--;
					break;
				case 3:
					tailpos++;
					break;
			}
			body.pop();
			return SnakeState::MOVE;
		}
};

// Initialize interface
void ginit() {
	// Disable buffering
	struct termios t;
	tcgetattr(STDIN_FILENO, &t);
	t.c_lflag &= ~ICANON;	// Disable canonical mode
	t.c_lflag &= ~ECHO;		// Disable echo
	tcsetattr(STDIN_FILENO, TCSANOW, &t);

	printf("\e[?1049h");	// Alternative screen buffer
	printf("\e[?25l");		// Hide cursor
}
// Terminate interface
void gend() {
	printf("\e[?25h");		// Show cursor
	printf("\e[?1049l");	// Alternative screen buffer

	// Enable buffering
	struct termios t;
	tcgetattr(STDIN_FILENO, &t);
	t.c_lflag |= ICANON;	// Enable canonical mode
	t.c_lflag |= ECHO;		// Enable echo
	tcsetattr(STDIN_FILENO, TCSANOW, &t);
}
// Move cursor to x and y positions
void gotoxy(int x, int y) {
	printf("\033[%d;%dH", y, x);
}

// Enables / Disables input blocking
void inputBlocking(bool enable) {
	int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
	if (enable)
		fcntl(STDIN_FILENO, F_SETFL, flags | ~O_NONBLOCK);
	else
		fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}

int main (int argc, char *argv[]) {
	ginit();

	// gotoxy(100, 16);
	// printf("%s  %s", BG_GREEN, COLOR_NONE);

	int fieldWidth = 30, fieldHeight = 30, startPos = 38, startLength = 3;

	Snake* player = new Snake(fieldWidth, fieldHeight, startPos, startLength);
	gotoxy(((startPos % fieldWidth) * 2)+2, startPos/fieldWidth+1);
	printf("%s", BG_GREEN);
	for (int i=0; i<startLength; i++) {
		printf("  ");
	}
	printf("%s", COLOR_NONE);

	inputBlocking(false);
	int color = 0, kp = 0;
	char c;
	int fps = 60;
	int speed = 30;		// Update per n frames
	int loopCount = 0;
	while (c != 'q') {
		c = getchar();

		/*
		switch (c) {
			case ' ' ... '~':
				kp++;
				gotoxy(100, 22);
				printf("\e[0m%c %d", c, kp);
				break;
			default:
				break;
		}
		*/

		// gotoxy(100, 20);
		// printf("\e[48;5;%dm        ", color);
		// color = (color+1) % 256;

		usleep(1000000 / fps);	// 60 frames per second

		loopCount = (loopCount+1) % speed;
		if (loopCount == (speed - 1)) {
			int oldTailPos = player -> getTailPos();
			switch (player -> move()) {
				case SnakeState::DEAD:
					c = 'q';
					break;
				case SnakeState::MOVE:
					gotoxy(((oldTailPos % fieldWidth + 1) * 2), oldTailPos/fieldWidth+1);
					printf("  ");
				case SnakeState::GROW:
					gotoxy(((player->getHeadPos() % fieldWidth) * 2), player->getHeadPos()/fieldWidth+1);
					printf("%s  %s", BG_GREEN, COLOR_NONE);
					break;
			}
		}

		gotoxy(1, 50);
		printf("%d\t%d", player->getHeadPos(), player->getTailPos());
		// usleep(10000000);
	}
	inputBlocking(true);

	gotoxy(1, 1);
	printf("You died!");

	gend();
	return 0;
}

