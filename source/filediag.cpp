/* 
   filediag.cpp --- handles extra file dialogs

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

namespace FileDialog
{
	namespace fs = std::filesystem;
	struct FileData
	{
		std::string path;
		bool selected = false;
		bool is_directory = false;
	};

	  std::string current_dir = ".";
	  std::vector < FileData > files;

	int current_selection = 0;
	int start_row = 0;			// Start of the file list

	// Function to navigate to a directory
	void navigate_to_dir(const std::string & dir)
	{
		current_dir = fs::path(dir).lexically_normal().string();
		files.clear();
		if (fs::exists(current_dir) && fs::is_directory(current_dir))
		{
		  for (const auto & entry:fs::directory_iterator
				 (current_dir))
			{
				FileData file_data;
				file_data.path = entry.path().string();
				file_data.is_directory = fs::is_directory(entry);
				files.push_back(file_data);
			}
		}
	}

	// Function to display the file dialog and handle navigation
	std::string file_dialog(WINDOW * win, const std::string & title,
							const std::string & message, bool isSave)
	{
		int rows, cols;
		getmaxyx(win, rows, cols);

		// Ensure that we can show at least one file
		int max_visible_files = rows - 3;	// 2 for the title and message, 1
		// for padding
		if (max_visible_files <= 0)
		{
			return "";			// Invalid state, cannot show files
		}

		start_row = 0;

		while (true)
		{
			werase(win);
			keypad(win, true);
			box(win, 0, 0);

			mvwprintw(win, 0, (cols - title.size()) / 2, title.c_str());
			mvwprintw(win, 1, 1, message.c_str());

			if (files.size() == 0)
			{
				WINDOW *errwin = newwin(4, 21, 1, 1);
				if (errwin == NULL)
					return "";

#if HAVE_COLOR

				if (console_color)
					wbkgd(errwin, COLOR_PAIR(COLOR_PAIR_ERR));

#endif

				werase(errwin);
				box(errwin, 0, 0);

				mvwprintw(errwin, 0, 5, "Fatal Error");
				mvwprintw(errwin, 1, 1, "No files to display");
				mvwprintw(errwin, 2, 1, "dir: %14s", current_dir.c_str());

				wrefresh(errwin);

				int prev = curs_set(0);
				wgetch(errwin);
				curs_set(prev);

				delwin(errwin);
				return "";
			}

			// Display the files in the window with scrolling
			for (int i = start_row;
				 i < std::min(start_row + max_visible_files,
							  static_cast < int >(files.size())); ++i)
			{
				bool is_selected = files[i].selected;
				bool is_directory = files[i].is_directory;
				std::string display =
					(is_directory ? "DIR " : (is_selected ? "[x] " : "[ ] ")) +
					files[i].path;

				if (i == current_selection)
				{
					wattron(win, A_REVERSE);
					mvwprintw(win, i - start_row + 2, 1, display.c_str());
					wattroff(win, A_REVERSE);
				}
				else
				{
					mvwprintw(win, i - start_row + 2, 1, display.c_str());
				}
			}

			wrefresh(win);
			int prev = curs_set(0);
			int ch = wgetch(win);
			curs_set(prev);
			switch (ch)
			{
			case KEY_UP:
				if (current_selection > 0)
				{
					--current_selection;
					if (current_selection < start_row)
					{
						--start_row;
					}
				}
				break;
			case KEY_DOWN:
				if (current_selection < (int)files.size() - 1)
				{
					++current_selection;
					if (current_selection >= start_row + max_visible_files)
					{
						++start_row;
					}
				}
				break;
			case '\r':
			case '\n':
				{
					FileData & selected = files[current_selection];

					if (selected.is_directory)
					{
						// Navigate into the directory
						navigate_to_dir(selected.path);
						start_row = 0;
						current_selection = 0;
					}
					else
					{
						// If it's a file, return the selected path
						return selected.path;
					}
					break;
				}
			case KEY_BACKSPACE:
			case 8:			// ascii backspace
			case 27:			// Escape key (to navigate out of directory)
				if (fs::exists(fs::path(current_dir).parent_path()))
				{
					navigate_to_dir(fs::path(current_dir).parent_path().
									string());
					start_row = 0;
					current_selection = 0;
				}
				break;
			case 'q':			// Exit on 'q'
				return "";		// Return empty string to indicate
				// cancellation
			case ' ':			// Toggle selection on spacebar
				files[current_selection].selected =
					!files[current_selection].selected;
				break;
			default:
				break;
			}
		}
	}

	std::string open(WINDOW * win, std::string dir)
	{
		navigate_to_dir(dir);
		return file_dialog(win, "Open File", "Select a file to open.", false);
	}

	std::string open_mult(WINDOW * win, std::string dir)
	{
		navigate_to_dir(dir);
		return file_dialog(win, "Open Multiple Files", "Select files to open.",
						   false);
	}

	std::string save(WINDOW * win, std::string dir)
	{
		navigate_to_dir(dir);
		return file_dialog(win, "Save File",
						   "Select a location to save the file.", true);
	}

	std::string saveas(WINDOW * win, std::string dir)
	{
		navigate_to_dir(dir);
		return file_dialog(win, "Save As",
						   "Select a location to save the file.", true);
	}

	std::string filepath(WINDOW * win, std::string dir, std::string title,
						 std::string message, bool isSave)
	{
		navigate_to_dir(dir);
		return file_dialog(win, title, message, isSave);
	}


	std::string getdrive_diag(WINDOW * win, bool instruction)
	{
#if USE_DOS_PATH
		bool selected = false;
		int max_x = getmaxx(win);
		char drive_letter = '?';
		while (!selected)
		{
			werase(win);
			box(win, 0, 0);
			keypad(win, true);

			if (instruction)
			{
				mvwprintw(win, 0, (max_x - 12) / 2, "Choose drive");
				mvwprintw(win, 1, 1,
						  "Press a letter corresponding to a drive to select it.");
				mvwprintw(win, 2, 1, "Press Enter to confirm. %c:",
						  drive_letter);
			}
			else
			{
				mvwprintw(win, 0, 1, "?:");
				mvwprintw(win, 1, 1, "%c:", drive_letter);
			}
			wrefresh(win);

			int prev = curs_set(0);
			char ch = wgetch(win);
			curs_set(prev);

			if (std::tolower(ch) >= 'a' && std::tolower(ch) <= 'z')
				drive_letter = std::toupper(ch);
			else if (ch == '\r' || ch == '\n' || ch == 27)
			{
				if (drive_letter != '?')
					selected = true;
			}
		}
		std::string newpath = std::string(1, drive_letter) + ":/";
		return newpath;
#else
		return "/";
#endif
	}

	std::string getdrive(int y, int x, bool instruction)
	{
		WINDOW *win =
			newwin((instruction ? 4 : 3), (instruction ? 55 : 4), y, x);
		if (win == NULL)
			return "/";
		std::string drv = getdrive_diag(win, instruction);
		delwin(win);
		return drv;
	}
}								// namespace FileDialog
