#include <cstdio>
#include <termios.h>

// Initialize interface
void ginit() {
	struct termios t;
	tcgetattr(fileno(stdin), &t);
	t.c_lflag &= ~ICANON;
	t.c_lflag &= ~ECHO;
	tcsetattr(fileno(stdin), TCSANOW, &t);

	printf("\e[?1049h");	// Alternative screen buffer
	printf("\e[?25l");		// Hide cursor
}
// Terminate interface
void gend() {
	printf("\e[?25h");		// Show cursor
	printf("\e[?1049l");	// Alternative screen buffer

	struct termios t;
	tcgetattr(fileno(stdin), &t);
	t.c_lflag |= ICANON;
	t.c_lflag |= ECHO;
	tcsetattr(fileno(stdin), TCSANOW, &t);
}
// Move cursor to x and y positions
void gotoxy(int x, int y) {
	printf("\033[%d;%dH", y, x);
}


int main (int argc, char *argv[]) {
	ginit();

	gotoxy(80, 16);
	printf("\e[48;5;82m  ");
	printf("\e[0m");
	printf("Hello World");

	while (1) {
		if (getchar() == 'q')
			break;
	}

	gend();
	return 0;
}

