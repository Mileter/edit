/* 
   edit.h --- all global functions

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

#pragma once
#ifndef _EDIT_H_
#define _EDIT_H_

#define VERSION 0.1
#define COPYRIGHT "Copyright 2024 Miles R. Chang"
#define MIT_LICENSE "Copyright 2024 Miles R. Chang\n\nPermission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the \"Software\"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:\n\nThe above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.\n\nTHE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.\n\nExcept as contained in this notice, the name of Miles R. Chang shall not be used in advertising or otherwise to promote the sale, use or other dealings in this Software without prior written authorization from Miles R. Chang."

#include "config.h"

#include <string>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iostream>

// file.cpp
// honestly, pretty simple file operations
#include <fstream>

// expect to handle std::runtime_error if there is a problem whilest reading
bool readfile(const std::string & file,	// IN
			  std::vector < std::string > &buffer);	// OUT
bool writefile(const std::string & file,	// IN
			   const std::vector < std::string > &buffer);	// IN

// main.cpp
extern std::vector < std::string > filebuf;
extern std::string filename;	// filename path

// strext.cpp
std::string trim(const std::string & str);
bool wrap_message(std::string message, size_t max_width,
				  std::vector < std::string > &wrapped_lines);

// ui.cpp
// auto assume HAVE_WIDE and HAVE_COLOR
#ifndef HAVE_WIDE
#ifdef WACS_S1
#define HAVE_WIDE 1
#else
#define HAVE_WIDE 0
#endif
#endif

#ifndef HAVE_COLOR
#ifdef A_COLOR
#define HAVE_COLOR 1
#else
#define HAVE_COLOR 0
#endif
#endif

#if HAVE_COLOR
#define COLOR_PAIR_ERR        0x10	// start at an offset of 16
#define COLOR_PAIR_WARN       0x11

#define COLOR_PAIR_NORMAL     0x12
#define COLOR_PAIR_SELECTED   0x13

#define COLOR_PAIR_MENU_BAR   0x14
#define COLOR_PAIR_MENU_PRESS 0x15
#endif

extern size_t scr_max_y;		// max width and height of screen
extern size_t scr_max_x;
extern bool console_color;		// does the CONSOLE have color support

extern bool useCRLF;            // use DOS style CRLF line endings?

void init_curs();
void uninit_curs();

void extrnal_refresh_ui();		// refresh from external control
void display_status(std::string message);

bool mainloop();				// mainloop; displays editor window

// menu.cpp
// menu elements

// Function to display menus
bool menu_interact(WINDOW * host_menu, std::string extra_info,
				   bool no_interact = false);

// diag.cpp
// dialogs of that not of editing directly persay
#include <filesystem>
bool show_fatal(const std::string & title, const std::string & message);
bool show_err(const std::string & title, const std::string & message);
bool show_warn(const std::string & title, const std::string & message);
bool show_norm(const std::string & title, const std::string & message);
namespace FileDialog
{
	// Function to navigate to a directory
	void navigate_to_dir(const std::string & dir);
	// Function to display the file dialog and handle navigation
	  std::string open(WINDOW * win, std::string dir = ".");
	  std::string open_mult(WINDOW * win, std::string dir = ".");
	  std::string save(WINDOW * win, std::string dir = ".");
	  std::string saveas(WINDOW * win, std::string dir = ".");
	  std::string filepath(WINDOW * win, std::string dir =
						   ".", std::string title =
						   "Open file", std::string message = "", bool isSave =
						   false);
	  std::string getdrive_diag(WINDOW * win, bool instruction = true);
	  std::string getdrive(int y = 1, int x = 1, bool instruction = true);
}								// namespace FileDialog

#endif
