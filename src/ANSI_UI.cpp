#include "ANSI_UI.h"

// Initialize interface
void ANSI_UI:: init() {
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
void ANSI_UI:: end() {
	printf("\e[?25h");		// Show cursor
	printf("\e[?1049l");	// Alternative screen buffer

	// Enable buffering
	struct termios t;
	tcgetattr(STDIN_FILENO, &t);
	t.c_lflag |= ICANON;	// Enable canonical mode
	t.c_lflag |= ECHO;		// Enable echo
	tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

// Enables / Disables input buffering / blocking
// Waits for user input to continue; enabled by default
void ANSI_UI:: inputBuffering(bool enable) {
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

