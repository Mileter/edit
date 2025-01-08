/* 
   ui.cpp --- handles all ui for the editor. I expect this to be the
   chunkiest.

   Copyright 2024 Miles R. Chang

   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the
   “Software”), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to permit
   persons to whom the Software is furnished to do so, subject to the
   following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.

   Except as contained in this notice, the name of Miles R. Chang shall not be 
   used in advertising or otherwise to promote the sale, use or other dealings 
   in this Software without prior written authorization from Miles R. Chang. */

// REVIEW ~285

#include "edit.h"

// extra getch macros
#define CTRL(x) ((x) & 0x1f)
// KEY_F(n) is already defined

size_t scr_max_y;		// max width and height of screen
size_t scr_max_x;
bool console_color = false;	// does the CONSOLE have color support

WINDOW *textArea = NULL;	// initialize to NULL for safety
WINDOW *menuBar = NULL;
WINDOW *statusBar = NULL;

// hidden overload
void display_status(WINDOW * win, std::string message)
{
	werase(statusBar);

#if HAVE_COLOR
	if (console_color)
		wbkgd(statusBar, COLOR_PAIR(COLOR_PAIR_MENU_BAR));
#endif

	int x = getmaxx(win);	// max legnth

	for (int i = 0; i < x && i < (int)message.size(); i++)
	{
		mvwaddch(win, 0, i, message[i]);
	}

	wrefresh(win);
}

void display_status(std::string message)
{
	display_status(statusBar, message);
}

void init_curs()
{
	initscr();
#if HAVE_COLOR

#ifdef FORCE_COLOR_ON
	console_color = true;
#else
	console_color = has_colors();	// is color supported?
#endif

	if (console_color)
	{
		start_color();
		init_pair(COLOR_PAIR_ERR, COLOR_WHITE, COLOR_RED);
		init_pair(COLOR_PAIR_WARN, COLOR_BLACK, COLOR_YELLOW);

		init_pair(COLOR_PAIR_NORMAL, COLOR_WHITE, COLOR_BLACK);
		init_pair(COLOR_PAIR_SELECTED, COLOR_BLACK, COLOR_WHITE);

		init_pair(COLOR_PAIR_MENU_BAR, COLOR_WHITE, COLOR_BLUE);
		init_pair(COLOR_PAIR_MENU_PRESS, COLOR_BLACK, COLOR_WHITE);
	}
#endif

	nonl();

	getmaxyx(stdscr, scr_max_y, scr_max_x);

	menuBar = newwin(1, scr_max_x, 0, 0);
	textArea = newwin(scr_max_y - 2, scr_max_x, 1, 0);
	statusBar = newwin(1, scr_max_x, scr_max_y - 1, 0);

#if HAVE_COLOR
	if (console_color)
	{
		wbkgd(menuBar, COLOR_PAIR(COLOR_PAIR_MENU_BAR));
		wbkgd(textArea, COLOR_PAIR(COLOR_PAIR_NORMAL));
		wbkgd(statusBar, COLOR_PAIR(COLOR_PAIR_MENU_BAR));
	}
#endif

	noecho();

	wrefresh(menuBar);
	wrefresh(textArea);
	wrefresh(statusBar);
}

// Display the buffer
void
display_buffer(WINDOW * win, std::vector < std::string > buffer,
	       size_t offset_x, size_t offset_y)
{
	// Get the size of the window
	int max_y, max_x;
	getmaxyx(win, max_y, max_x);	// max_y is the number of rows, max_x
	// is
	// the number of columns

	// Loop through the buffer and display the content starting from the
	// offset
	for (size_t y = offset_y; y < buffer.size() && y < offset_y + max_y;
	     ++y)
	{
		// For each line in the window, print the corresponding
		// portion of the 
		// buffer
		std::string line = buffer[y];
		for (size_t x = offset_x;
		     x < line.size() && x < offset_x + max_x; ++x)
		{
			// Print each character
			mvwaddch(win, y - offset_y, x - offset_x, line[x]);
		}
	}

	// Refresh the window to display the content
	wrefresh(win);
}

void uninit_curs()
{
	if (textArea != NULL)
		delwin(textArea);
	if (menuBar != NULL)
		delwin(menuBar);
	if (statusBar != NULL)
		delwin(statusBar);
	endwin();
}

size_t cursor_x = 0;
size_t cursor_y = 0;
size_t offset_x = 0;
size_t offset_y = 0;

void extrnal_refresh_ui()
{
	refresh();		// refresh stdscr

	werase(menuBar);
	werase(textArea);

	menu_interact(menuBar, filename, true);
	// void display_menu(WINDOW *win, std::vector<std::string> menu_items,
	// size_t highlight, int pair_normal, int pair_selected)

	display_buffer(textArea, filebuf, offset_x, offset_y);

	keypad(textArea, true);

	wrefresh(menuBar);
	wrefresh(textArea);
}

bool mainloop()			// return false to quit
{
	int max_y, max_x;
	getmaxyx(textArea, max_y, max_x);

	if (cursor_x - offset_x <= 2)
	{
		if (offset_x - 1 < offset_x)	// underflow protection
			offset_x--;
	}
	else if ((int)(cursor_x - offset_x) >= max_x - 2)
	{
		if (offset_x + 1 > offset_x)	// overflow protection
			offset_x++;
	}

	if (cursor_y - offset_y <= 2)
	{
		if (offset_y - 1 < offset_y)	// underflow protection
			offset_y--;
	}
	else if ((int)(cursor_y - offset_y) >= max_y - 2)
	{
		if (offset_y + 1 > offset_y)	// overflow protection
			offset_y++;
	}

	werase(menuBar);
	werase(textArea);
	werase(statusBar);

	menu_interact(menuBar, filename, true);
	// void display_menu(WINDOW *win, std::vector<std::string> menu_items,
	// size_t highlight, int pair_normal, int pair_selected)

	display_buffer(textArea, filebuf, offset_x, offset_y);
	display_status(statusBar,
		       (std::string) " Ln " + std::to_string(cursor_y + 1) +
		       ", Col " + std::to_string(cursor_x + 1) + " | lines: " +
		       std::to_string(filebuf.size()) +
		       " | Press ESC to access to menu bar.");

	keypad(textArea, true);

	wrefresh(menuBar);
	wrefresh(textArea);
	wrefresh(statusBar);

	int ch = mvwgetch(textArea, cursor_y - offset_y, cursor_x - offset_x);

	if (ch == ERR)
	{
		return !show_fatal("ERR received as an input",
				   "FATAL ERROR. The program will exit, once this box is closed.\n\n\
		The input ERR was unexpectedly recieved. \
		THIS CONDITION WILL RESET THE CURSOR TO (0, 0).");
		cursor_x = 0;
		cursor_y = 0;
	}

	if (ch == 27)		// for now, until menu bar implemented, quit
		// test build
	{
		// show_norm("Not implemented yet!", 
		// "Menu bar is not implemented yet. For now, it just closes
		// the
		// program directly.");
		return menu_interact(menuBar, filename);
	}

	// add editor logic
	if (ch == KEY_UP)
	{
		if (cursor_y > 0)
		{
			cursor_y--;
			if (cursor_x >= filebuf[cursor_y].size() - 1)
				cursor_x = filebuf[cursor_y].size() - 1;
		}
		return true;
	}
	if (ch == KEY_DOWN)
	{
		if (cursor_y < filebuf.size() - 1)
		{
			cursor_y++;
			if (cursor_x >= filebuf[cursor_y].size() - 1)
				cursor_x = filebuf[cursor_y].size() - 1;
		}
		return true;
	}
	if (ch == KEY_LEFT)
	{
		if (cursor_x > 0)
			cursor_x--;
		return true;
	}
	if (ch == KEY_RIGHT)
	{
		if (cursor_x < filebuf[cursor_y].size() - 1)
			cursor_x++;
		return true;
	}
	if (ch == '\r')
	{
		ch = '\n';	// handle as newline, because Carridge Return
		// by itself is illegal.
	}
	if (ch == '\n')		// Enter key (newline)
	{
		// Handle newline
		filebuf.insert(filebuf.begin() + cursor_y + 1, " ");

		cursor_y++;
		cursor_x = 0;	// This can be changed later.
		return true;
	}
	if (ch == 8)
	{
		// Handle backspace logic
		if (cursor_x > 0)
		{
			filebuf[cursor_y].erase(cursor_x - 1, 1);
			cursor_x--;
		}
		else if (cursor_y > 0)	// Handle delete line
		{
			// show_err("Not implemented yet!",
			// "Deleting newlines not implemented yet! Please wait 
			// 
			// 
			// 
			// 
			// 
			// 
			// 
			// for 1.0 for 
			// 
			// 
			// 
			// 
			// 
			// 
			// 
			// 
			// 
			// 
			// this feature.");
			filebuf[cursor_y -
				1].insert(filebuf[cursor_y - 1].size() - 1,
					  filebuf[cursor_y]);
			filebuf.erase(filebuf.begin() + cursor_y);
		}
		return true;
	}

	// otherwise
	// Insert regular character input
	std::string unctrl_ch = std::string(unctrl(ch));
	filebuf[cursor_y].insert(cursor_x, unctrl_ch);
	cursor_x += unctrl_ch.size();
	return true;
}
