#include <cstdio>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

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

	gotoxy(100, 16);
	printf("\e[48;5;82m  ");
	printf("\e[0m");
	gotoxy(100, 18);
	printf("Hello World");

	inputBlocking(false);
	int color = 0, kp = 0;
	char c;
	while (c != 'q') {
		c = getchar();

		switch (c) {
			case ' ' ... '~':
				kp++;
				gotoxy(100, 22);
				printf("\e[0m%c %d", c, kp);
				break;
			default:
				break;
		}

		gotoxy(100, 20);
		printf("\e[48;5;%dm        ", color);
		color = (color+1) % 256;

		usleep(1000000 / 60);	// 60 frames per second
	}
	inputBlocking(true);

	gend();
	return 0;
}

