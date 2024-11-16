/* 
   diag.cpp --- handles all dialogs (except for those of the editor itself, as 
   in menu bar, text area, status bar).

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

#include "edit.h"


// returns true if the dialog displayed
// returns false if fallback was used

// Function to show fatal error message has 
// FALSE -> [R]isks understood, continue anyway, OR 
// TRUE -> PRESS ANY OTHER KEY TO TERMINATE.
bool show_fatal(const std::string & title, const std::string & message)
{
	WINDOW *errWindow = newwin(scr_max_y - 4, scr_max_x - 4, 2, 2);
	int prev = curs_set(0);

	if (!errWindow)
	{
		std::cerr << "FATAL ERROR (fallback) - " << title << "\n" << message <<
			"\n";
		return false;
	}

	// Enable keypad input to capture arrow keys
	keypad(errWindow, TRUE);

	// Draw box around the message window
	box(errWindow, 0, 0);

	// Set background color for error
#if HAVE_COLOR
	if (console_color)
	{
		wbkgd(errWindow, COLOR_PAIR(COLOR_PAIR_ERR));
	}
#endif

	mvwprintw(errWindow, 0, (scr_max_x - title.size() - 7) / 2,
			  "FATAL ERROR: %s", title.c_str());

	size_t max_message_width = scr_max_x - 4;	// Considering the border (2
	// on each side)
	std::vector < std::string > wrapped_lines;
	wrap_message(message, max_message_width, wrapped_lines);

	size_t total_lines = wrapped_lines.size();
	size_t y_offset = 1;		// Start printing from the second row (after
	// the title)
	size_t view_offset = 0;		// Keeps track of the current scroll position

	// Initial rendering of the wrapped message
	for (size_t i = 0; i < scr_max_y - 6 && i + view_offset < total_lines; ++i)
	{
		mvwprintw(errWindow, i + y_offset, 1, "%s",
				  wrapped_lines[i + view_offset].c_str());
	}

	mvwprintw(errWindow, scr_max_y - 6, (scr_max_x - 73) / 2,
			  "[R]isks understood, continue anyway, OR PRESS ANY OTHER KEY TO TERMINATE.");

	// Allow scrolling with up and down arrows
	wrefresh(errWindow);
	while (true)
	{
		int ch = wgetch(errWindow);

		// Scroll up or down with the arrow keys
		if (ch == ERR);
		else if (ch == KEY_UP && view_offset > 0)
		{
			--view_offset;		// Scroll up
		}
		else if (ch == KEY_DOWN && view_offset + (scr_max_y - 7) < total_lines)
		{
			++view_offset;		// Scroll down
		}
		else if (ch == KEY_UP || ch == KEY_LEFT || ch == KEY_DOWN
				 || ch == KEY_LEFT)
			continue;
		else if (std::tolower(ch) == 'r')
			return false;
		else
		{
			// Any non-arrow key closes the message box
			break;				// Exit on any non-arrow key
		}

		// Re-render the window with the updated view_offset
		werase(errWindow);
		box(errWindow, 0, 0);
		mvwprintw(errWindow, 0, (scr_max_x - title.size() - 7) / 2,
				  "FATAL ERROR: %s", title.c_str());
		for (size_t i = 0; i < scr_max_y - 7 && i + view_offset < total_lines;
			 ++i)
		{
			mvwprintw(errWindow, i + y_offset, 1, "%s",
					  wrapped_lines[i + view_offset].c_str());
		}

		mvwprintw(errWindow, scr_max_y - 6, (scr_max_x - 73) / 2,
				  "[R]isks understood, continue anyway, OR PRESS ANY OTHER KEY TO TERMINATE.");
		wrefresh(errWindow);	// Refresh the window with updated content
	}

	delwin(errWindow);			// Clean up
	curs_set(prev);

	return true;
}


// Function to show an error message
bool show_err(const std::string & title, const std::string & message)
{
	WINDOW *errWindow = newwin(scr_max_y - 4, scr_max_x - 4, 2, 2);
	int prev = curs_set(0);

	if (!errWindow)
	{
		std::cerr << "ERROR (fallback) - " << title << "\n" << message << "\n";
		return false;
	}

	// Enable keypad input to capture arrow keys
	keypad(errWindow, TRUE);

	// Draw box around the message window
	box(errWindow, 0, 0);

	// Set background color for error
#if HAVE_COLOR
	if (console_color)
	{
		wbkgd(errWindow, COLOR_PAIR(COLOR_PAIR_ERR));
	}
#endif

	mvwprintw(errWindow, 0, (scr_max_x - title.size() - 7) / 2, "ERROR: %s",
			  title.c_str());

	size_t max_message_width = scr_max_x - 4;	// Considering the border (2
	// on each side)
	std::vector < std::string > wrapped_lines;
	wrap_message(message, max_message_width, wrapped_lines);

	size_t total_lines = wrapped_lines.size();
	size_t y_offset = 1;		// Start printing from the second row (after
	// the title)
	size_t view_offset = 0;		// Keeps track of the current scroll position

	// Initial rendering of the wrapped message
	for (size_t i = 0; i < scr_max_y - 6 && i + view_offset < total_lines; ++i)
	{
		mvwprintw(errWindow, i + y_offset, 1, "%s",
				  wrapped_lines[i + view_offset].c_str());
	}

	mvwprintw(errWindow, scr_max_y - 6, (scr_max_x - 25) / 2,
			  "Press any key to close...");

	// Allow scrolling with up and down arrows
	wrefresh(errWindow);
	while (true)
	{
		int ch = wgetch(errWindow);

		// Scroll up or down with the arrow keys
		if (ch == ERR);
		else if (ch == KEY_UP && view_offset > 0)
		{
			--view_offset;		// Scroll up
		}
		else if (ch == KEY_DOWN && view_offset + (scr_max_y - 7) < total_lines)
		{
			++view_offset;		// Scroll down
		}
		else if (ch == KEY_UP || ch == KEY_LEFT || ch == KEY_DOWN
				 || ch == KEY_LEFT)
			continue;
		else
		{
			// Any non-arrow key closes the message box
			break;				// Exit on any non-arrow key
		}

		// Re-render the window with the updated view_offset
		werase(errWindow);
		box(errWindow, 0, 0);
		mvwprintw(errWindow, 0, (scr_max_x - title.size() - 7) / 2,
				  "ERROR: %s", title.c_str());
		for (size_t i = 0; i < scr_max_y - 7 && i + view_offset < total_lines;
			 ++i)
		{
			mvwprintw(errWindow, i + y_offset, 1, "%s",
					  wrapped_lines[i + view_offset].c_str());
		}

		mvwprintw(errWindow, scr_max_y - 6, (scr_max_x - 25) / 2,
				  "Press any key to close...");
		wrefresh(errWindow);	// Refresh the window with updated content
	}

	delwin(errWindow);			// Clean up
	curs_set(prev);
	return true;
}

// Function to show a warning message
bool show_warn(const std::string & title, const std::string & message)
{
	WINDOW *warnWindow = newwin(scr_max_y - 4, scr_max_x - 4, 2, 2);
	int prev = curs_set(0);

	if (!warnWindow)
	{
		std::cerr << "ERROR (fallback) - " << title << "\n" << message << "\n";
		return false;
	}

	// Enable keypad input to capture arrow keys
	keypad(warnWindow, TRUE);

	// Draw box around the message window
	box(warnWindow, 0, 0);

	// Set background color for warning
#if HAVE_COLOR
	if (console_color)
	{
		wbkgd(warnWindow, COLOR_PAIR(COLOR_PAIR_WARN));
	}
#endif

	mvwprintw(warnWindow, 0, (scr_max_x - title.size() - 9) / 2,
			  "WARNING: %s", title.c_str());

	size_t max_message_width = scr_max_x - 4;	// Considering the border (2
	// on each side)
	std::vector < std::string > wrapped_lines;
	wrap_message(message, max_message_width, wrapped_lines);

	size_t total_lines = wrapped_lines.size();
	size_t y_offset = 1;		// Start printing from the second row (after
	// the title)
	size_t view_offset = 0;		// Keeps track of the current scroll position

	// Initial rendering of the wrapped message
	for (size_t i = 0; i < scr_max_y - 7 && i + view_offset < total_lines; ++i)
	{
		mvwprintw(warnWindow, i + y_offset, 1, "%s",
				  wrapped_lines[i + view_offset].c_str());
	}

	mvwprintw(warnWindow, scr_max_y - 6, (scr_max_x - 25) / 2,
			  "Press any key to close...");

	// Allow scrolling with up and down arrows
	wrefresh(warnWindow);
	while (true)
	{
		int ch = wgetch(warnWindow);

		// Scroll up or down with the arrow keys
		if (ch == ERR);
		else if (ch == KEY_UP && view_offset > 0)
		{
			--view_offset;		// Scroll up
		}
		else if (ch == KEY_DOWN && view_offset + (scr_max_y - 7) < total_lines)
		{
			++view_offset;		// Scroll down
		}
		else if (ch == KEY_UP || ch == KEY_LEFT || ch == KEY_DOWN
				 || ch == KEY_LEFT)
			continue;
		else
		{
			// Any non-arrow key closes the message box
			break;				// Exit on any non-arrow key
		}

		// Re-render the window with the updated view_offset
		werase(warnWindow);
		box(warnWindow, 0, 0);
		mvwprintw(warnWindow, 0, (scr_max_x - title.size() - 9) / 2,
				  "WARNING: %s", title.c_str());
		for (size_t i = 0; i < scr_max_y - 7 && i + view_offset < total_lines;
			 ++i)
		{
			mvwprintw(warnWindow, i + y_offset, 1, "%s",
					  wrapped_lines[i + view_offset].c_str());
		}

		mvwprintw(warnWindow, scr_max_y - 6, (scr_max_x - 25) / 2,
				  "Press any key to close...");
		wrefresh(warnWindow);	// Refresh the window with updated content
	}

	delwin(warnWindow);			// Clean up
	curs_set(prev);
	return true;
}

// Function to show a normal message
bool show_norm(const std::string & title, const std::string & message)
{
	WINDOW *normWindow = newwin(scr_max_y - 4, scr_max_x - 4, 2, 2);
	int prev = curs_set(0);

	if (!normWindow)
	{
		std::cerr << "ERROR (fallback) - " << title << "\n" << message << "\n";
		return false;
	}

	// Enable keypad input to capture arrow keys
	keypad(normWindow, TRUE);

	// Draw box around the message window
	box(normWindow, 0, 0);

	// Set background color for normal message
#if HAVE_COLOR
	if (console_color)
	{
		wbkgd(normWindow, COLOR_PAIR(COLOR_PAIR_NORMAL));
	}
#endif

	mvwprintw(normWindow, 0, (scr_max_x - title.size() - 1) / 2, "%s",
			  title.c_str());

	size_t max_message_width = scr_max_x - 4;	// Considering the border (2
	// on each side)
	std::vector < std::string > wrapped_lines;
	wrap_message(message, max_message_width, wrapped_lines);

	size_t total_lines = wrapped_lines.size();
	size_t y_offset = 1;		// Start printing from the second row (after
	// the title)
	size_t view_offset = 0;		// Keeps track of the current scroll position

	// Initial rendering of the wrapped message
	for (size_t i = 0; i < scr_max_y - 6 && i + view_offset < total_lines; ++i)
	{
		mvwprintw(normWindow, i + y_offset, 1, "%s",
				  wrapped_lines[i + view_offset].c_str());
	}

	mvwprintw(normWindow, scr_max_y - 6, (scr_max_x - 25) / 2,
			  "Press any key to close...");

	// Allow scrolling with up and down arrows
	wrefresh(normWindow);
	while (true)
	{
		int ch = wgetch(normWindow);

		// Scroll up or down with the arrow keys
		if (ch == ERR);
		else if (ch == KEY_UP && view_offset > 0)
		{
			--view_offset;		// Scroll up
		}
		else if (ch == KEY_DOWN && view_offset + (scr_max_y - 7) < total_lines)
		{
			++view_offset;		// Scroll down
		}
		else if (ch == KEY_UP || ch == KEY_LEFT || ch == KEY_DOWN
				 || ch == KEY_LEFT)
			continue;
		else
		{
			// Any non-arrow key closes the message box
			break;				// Exit on any non-arrow key
		}

		// Re-render the window with the updated view_offset
		werase(normWindow);
		box(normWindow, 0, 0);
		mvwprintw(normWindow, 0, (scr_max_x - title.size() - 1) / 2, "%s",
				  title.c_str());
		for (size_t i = 0; i < scr_max_y - 7 && i + view_offset < total_lines;
			 ++i)
		{
			mvwprintw(normWindow, i + y_offset, 1, "%s",
					  wrapped_lines[i + view_offset].c_str());
		}

		mvwprintw(normWindow, scr_max_y - 6, (scr_max_x - 25) / 2,
				  "Press any key to close...");
		wrefresh(normWindow);	// Refresh the window with updated content
	}

	delwin(normWindow);			// Clean up
	curs_set(prev);
	return true;
}
