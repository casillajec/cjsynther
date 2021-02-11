#include <ncurses.h>

int main() {
	int ch;

	initscr();
	cbreak();
	noecho();

	printw("sukisukisuki\n");
	refresh();
	int goon = 1;
	while (goon) { 
		ch = getch();
		if (ch == 'x') { goon = 0; }
		mvaddch(1, 0, ch);
		refresh();
	}
	
	endwin();
	return 0;
}
