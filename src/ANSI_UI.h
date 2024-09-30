#ifndef ANSI_UI_H
#define ANSI_UI_H

#include <unistd.h>		// STDIN_FILENO
#include <termios.h>	// Buffering (enter)
#include <iostream>		// printf
#include <fcntl.h>		// Buffering (wait)

class ANSI_UI {
	private:
		int fieldWidth, fieldHeight;

		virtual void keypressHandler() {};

	public:
		// Constructor
		ANSI_UI() {}

		// Initialize interface
		void init();
		// Terminate interface
		void end();

		// Enables / Disables input buffering / blocking
		// Waits for user input to continue; enabled by default
		void inputBuffering(bool enable);

		// Move cursor to x and y positions on screen
		void gotoxy(int x, int y);
		void printxy(int x, int y, char* txt);
};

#endif // !ANSI_UI_H

