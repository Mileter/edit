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
void display_buffer(WINDOW * win, std::string & buffer, size_t offset_x, size_t offset_y)
{
	// Get the size of the window
	int max_y, max_x;
	getmaxyx(win, max_y, max_x);	// max_y is the number of rows, max_x
	// is
	// the number of columns

	std::vector < std::string > split_buf;

	extractLinesFromBuf(split_buf, buffer, offset_y, scr_max_y);


	// Loop through the buffer and display the content starting from the
	// offset
	for (size_t y = offset_y; y < split_buf.size() && y < offset_y + max_y; ++y)
	{
		// For each line in the window, print the corresponding
		// portion of the 
		// buffer
		std::string line = split_buf[y];
		for (size_t x = offset_x; x < line.size() && x < offset_x + max_x; ++x)
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

	try
	{
		display_buffer(textArea, filebuf, offset_x, offset_y);
	}
	catch(std::runtime_error & r)
	{
		show_fatal("Failed to display buffer", r.what());
		return false;
	}
	display_status(statusBar,
		       (std::string) " Ln " + std::to_string(cursor_y + 1) +
		       ", Col " + std::to_string(cursor_x + 1) +
		       " | length: " + std::to_string(filebuf.size()) + " | Press ESC to access to menu bar.");

	keypad(textArea, true);

	wrefresh(menuBar);
	wrefresh(textArea);
	wrefresh(statusBar);


	curs_set(1);		// set on anyway.
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
		return menu_interact(menuBar, filename);
	}

	// add editor logic
	if (ch == KEY_UP)
	{
		if (cursor_y > 0)
		{
			cursor_y--;
			std::string s;
			extractSingleLineFromBuf(s, filebuf, cursor_y);
			if (cursor_x >= s.size() - 1)
				cursor_x = s.size() - 1;
		}
		return true;
	}
	if (ch == KEY_DOWN)
	{
		cursor_y++;
		std::string s;
		bool res = extractSingleLineFromBuf(s, filebuf, cursor_y + 1);
		
		if (!res)	// on no such line,
			cursor_y--;
		else if (cursor_x >= s.size() - 1)
			cursor_x = s.size() - 1;
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
		std::string s;
		extractSingleLineFromBuf(s, filebuf, cursor_y);
		
		if (cursor_x < s.size())
			cursor_x++;
		return true;
	}
	if (ch == '\r')
	{
		ch = '\n';	// handle as newline... strictly for DOS line
		// feed remainents in windows
	}
	if (ch == '\n')		// Enter key (newline)
	{
		// Handle newline

		if (useCRLF)
			filebuf.insert(getNthDelimWithOffset(filebuf, cursor_y, 0), "\r\n");
		else
			filebuf.insert(getNthDelimWithOffset(filebuf, cursor_y, 0), "\n");

		cursor_y++;
		cursor_x = 0;	// This can be changed later.
		return true;
	}
	if (ch == 8 || ch == 127 || ch == 7 || ch == CTRL('G') || ch == 263)
		// seems that  the  ^G  is  called  when pressing BS
		// also seems that ch is set to 263 when that happens?
		// ncurses does not behave like pdcurses in this regard.
	{
		try
		{
			// Handle backspace logic
			if (cursor_x > 0)
			{
				filebuf.erase(filebuf.begin() + getNthDelimWithOffset(filebuf, cursor_y, cursor_x) - 1);
				cursor_x--;
			}
			else if (cursor_y > 0)	// Handle delete line
			{
				filebuf.erase(filebuf.begin() + getNthDelimWithOffset(filebuf, cursor_y, 0));
				cursor_y--;
			}
			else	// not valid move
			{
				flash();
				beep();
			}
		}
		catch(std::runtime_error & r)
		{
			show_fatal
				("An unexpected error occured while trying to handle event \"backspace\"",
				 "Please report the issue to the issue tracker.\nDETAILS:\n" + (std::string) r.what());
		}
		return true;
	}

	// otherwise
	// Insert regular character input
	std::string unctrl_ch = std::string(unctrl(ch));
	try
	{
		filebuf.insert(getNthDelimWithOffset(filebuf, cursor_y, cursor_x), unctrl_ch);
	}
	catch(std::runtime_error & ex)
	{
		show_fatal
			("Unexpected error occured whilest trying to handle keyboard interupt.",
			 (std::string) ex.what());
		return false;
	}

	cursor_x += unctrl_ch.size();
	return true;
}
