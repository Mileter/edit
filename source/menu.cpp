/* 
   menu.cpp --- handles menu interaction

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

size_t highlight = 1;		// nothing is selected

std::vector < std::string > mainMenuItems = {
	"File",
	"Edit",
	"Search",
	"Options"
};

std::vector < std::string > fileSubmenuItems = {
	"(back)",
	"Open",
	"Save",
	"Save As",
	"Exit"
};

std::vector < std::string > editSubmenuItems = {
	"(back)",
	"Cut",
	"Copy",
	"Paste"
#if not HAVE_CLIPBOARD		// cannot sink to system clipboard
		, "More info... (limitations)"
#endif
};

std::vector < std::string > optionsSubmenuItems = {
	"(back)",
	"About Edit",
	"( ) Word Wrap",
#if USE_DOS_PATH
	"(x) DOS Line Endings (CRLF)",
#else
	"( ) DOS Line Endings (CRLF)",
#endif
	" x  Status Bar"
};

// Function to display editing menu
void display_menu(WINDOW * win, size_t highlight, int pair_selected, std::string extra_info = "")
{
	int x = getmaxx(win);	// max legnth

	size_t offset_x = 2;

	for (size_t i = 0; i < mainMenuItems.size(); i++)
	{
#if HAVE_COLOR
		if (i + 1 == highlight)
		{
			if (console_color)
				wattron(win, COLOR_PAIR(pair_selected) | A_BLINK);
			else
				wattron(win, A_BLINK);
		}
#else
		if (i + 1 == highlight)
			wattron(win, A_BLINK);
#endif

		mvwprintw(win, 0, offset_x, mainMenuItems[i].c_str());

		offset_x += mainMenuItems[i].size() + 2;
#if HAVE_COLOR
		if (console_color && i + 1 == highlight)
			wattroff(win, COLOR_PAIR(pair_selected) | A_BLINK);	// Remove
		// highlight
#else
		if (i == highlight)
			wattroff(win, A_BLINK);
#endif
	}

	mvwprintw(win, 0, x - 1 - extra_info.size(), "%s", extra_info.c_str());

	wrefresh(win);
}

void display_floating_menu(WINDOW * window, int highlight, int pair_selected, std::vector < std::string > &contextItems)
{
	int max_y, max_x;
	getmaxyx(window, max_y, max_x);

	werase(window);
	box(window, 0, 0);

	for (int i = 0; i < (int)contextItems.size() && i < max_y - 2; i++)
	{
#if HAVE_COLOR
		if (i + 1 == highlight)
		{
			if (console_color)
				wattron(window, COLOR_PAIR(pair_selected) | A_BLINK);
			else
				wattron(window, A_BLINK);
		}
#else
		if (i + 1 == highlight)
			wattron(window, A_BLINK);
#endif

		mvwprintw(window, i + 1, 1, "%s", contextItems[i].c_str());

#if HAVE_COLOR
		if (i + 1 == highlight)
		{
			if (console_color)
				wattroff(window, COLOR_PAIR(pair_selected) | A_BLINK);
			else
				wattroff(window, A_BLINK);
		}
#else
		if (i + 1 == highlight)
			wattroff(window, A_BLINK);
#endif

	}

	wrefresh(window);
}

bool menu_interact(WINDOW * host_menu, std::string extra_info, bool no_interact)
{
	display_menu(host_menu, 0, COLOR_PAIR_SELECTED, extra_info);
	// void display_menu(WINDOW * win, size_t highlight, int pair_normal,
	// int
	// pair_selected, std::string extra_info = "");

	size_t selection = 1;
	while (!no_interact)
	{
		int prev = curs_set(0);
		extrnal_refresh_ui();
		werase(host_menu);
		display_menu(host_menu, selection, COLOR_PAIR_SELECTED, extra_info);
		keypad(host_menu, true);
		display_status
			(" Press Enter to select a button. Press left and right to navigate. Press ESC to continue editing.");

		int ch = wgetch(host_menu);

		if (ch == ERR)
		{
			show_err("Recieved ERR as input",
				 "Something bad happened, and after closing this message, the program will quit.");
			return false;
		}
		else if (ch == 27)
		{
			no_interact = true;
		}
		else if (ch == KEY_LEFT)
		{
			if (selection > 1)
				selection--;
		}
		else if (ch == KEY_RIGHT)
		{
			if (selection < mainMenuItems.size())
				selection++;
		}
		else if (ch == '\r' || ch == '\n')
		{
			if (selection == 1)	// File
			{
				// to do
				WINDOW *floatingWin = newwin(7, 9, 1, 2);

#if HAVE_COLOR
				if (console_color)
					wbkgd(floatingWin, COLOR_PAIR(COLOR_PAIR_MENU_BAR));
#endif

				if (floatingWin == NULL)
				{
					show_err("Failed to open window.", "Will close the program, afterwards.");
					return false;
				}

				size_t fselection = 1;
				bool done = false;
				while (!done)
				{
					display_floating_menu(floatingWin,
							      fselection, COLOR_PAIR_SELECTED, fileSubmenuItems);

					keypad(floatingWin, true);
					int prev = curs_set(0);
					int ch = wgetch(floatingWin);
					curs_set(prev);

					if (ch == ERR)
					{
						show_err("Unexpected ERR Recieved",
							 "Something bad happened, and after closing this message, the program will quit.");
						fselection = 0;
						done = true;
					}
					else if (ch == KEY_LEFT)
					{
						fselection = 1;
						return true;
					}
					else if (ch == '\r' || ch == '\r')
					{
						done = true;
					}
					else if (ch == KEY_UP)
					{
						if (fselection > 1)	// down 
									// 
							// 
							// 
							// 
							// 
							// 
							// 
							// 
							// 
							// to
							// 1
							fselection--;
					}
					else if (ch == KEY_DOWN)
					{
						if (fselection < fileSubmenuItems.size())
							fselection++;
					}
				}

				delwin(floatingWin);

				if (fselection == 0)	// ERR
					return false;
				else if (fselection == 1)	// (go back)
					;	// do nothing
				else if (fselection == 2)	// Open
				{
					WINDOW *filediag = newwin(scr_max_y - 2,
								  scr_max_x, 1, 0);

					if (filediag == NULL)
					{
						show_err("Could not create window.",
							 "Something bad happened, and after closing this message, the program will quit.");
						return false;
					}

#if HAVE_COLOR
					if (console_color)
						wbkgd(filediag, COLOR_PAIR(COLOR_PAIR_SELECTED));
#endif

					display_status
						(" Press Enter to confirm. Press a letter to select drive letter.");

					std::string drive = FileDialog::getdrive(3, 3, false);

					display_status
						(" ENTER -> confirm; Q -> cancel; UP, DOWN -> navigation; ESC -> Parent directory");
					std::string tmp_filename = FileDialog::open(filediag, drive);

					if (tmp_filename == "")
						return true;	// Do nothing
					// if canceled

					try
					{
						readfile(tmp_filename, filebuf);
						filename = tmp_filename;
					}
					catch(const std::runtime_error & ex)
					{
						show_err("Error whilest reading file!", ex.what());
					}

					delwin(filediag);
					curs_set(prev);
					return true;
				}
				else if (fselection == 3)	// Save
				{
					if (filename == "")
					{
						show_norm("Could not save",
							  "Set a filename before saving using the Save As function.");
						curs_set(prev);
						return true;
					}

					try
					{
						writefile(filename, filebuf);
					}
					catch(const std::runtime_error & ex)
					{
						show_err("Error whilest reading file!", ex.what());
					}

					return true;
				}
				else if (fselection == 4)	// Save As
				{
					show_err("Not implemented yet",
						 "The module required for this function to work has not been implemented yet. \n\nThus this module is unuseable, making for a quite dumb editor.");
					curs_set(prev);
					return true;
				}
				else if (fselection == 5)	// Exit
				{
					// show_norm("Exit menu called", "The
					// exit button was
					// pressed.");
					curs_set(prev);
					return false;
				}
			}
			else if (selection == 2)	// Edit
			{
				show_warn("Not implemented yet.",
					  "Registers for the clipboard have not been implemented yet. Please wait 'till 1.0.0.0.");
			}
			else if (selection == 3)	// Search
			{
				show_warn("Not implemented yet.",
					  "Search has not been implemented yet. Such a feature would have to wait 'till 1.0.0.0.");
			}
			else if (selection == 4)	// Options
			{
				show_warn("Not implemented yet.",
					  "Word wrapping has not been implemented yet. Other options are disabled for use.");
			}
			else	// ERR
			{
				show_err("Got impossible selection/Unhandled event",
					 "The selection of mode #" + std::to_string(selection) +
					 " is not a valid selection. (1-4)\n\nThus it has been concluded that something went south, and thus, closure. After this selection will be reset to 1.");
				selection = 1;
			}
		}

		curs_set(prev);
	}
	return true;
}
