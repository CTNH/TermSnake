#include "ANSI_UI.h"

// Initialize interface
void ANSI_UI:: init() {
	inputBuffer(false);

	printf("\e[?1049h");	// Alternative screen buffer
	printf("\e[?25l");		// Hide cursor
}

// Terminate interface
void ANSI_UI:: end() {
	printf("\e[?25h");		// Show cursor
	printf("\e[?1049l");	// Alternative screen buffer

	inputBuffer(true);
}

// Enables / Disables input blocking
// Requires enter to continue; enabled by default
void ANSI_UI:: inputBuffer(bool enable) {
	struct termios t;
	tcgetattr(STDIN_FILENO, &t);
	// Canonical and echo modes
	if (enable)
		t.c_lflag |= ICANON | ECHO;
	else
		t.c_lflag &= (~ICANON) & (~ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &t);
}
// Enables / Disables input blocking
// Waits for user input to continue; enabled by default
void ANSI_UI:: inputBlock(bool enable) {
	int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
	if (enable)
		fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);
	else
		fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}

// Move cursor to x and y positions on screen
void ANSI_UI:: gotoxy(int x, int y) {
	// 0 & 1 is same
	printf("\033[%d;%dH", y, x);
}
void ANSI_UI:: printxy(int x, int y, char* txt) {
	gotoxy(x, y);
	printf("%s", txt);
}

