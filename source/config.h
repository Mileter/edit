/* 
   config.h --- definations for system specific headers and library locations

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

// curses header location
#include <curses.h>
// some other common curses library headers include
// #include <ncurses.h>
// #include <ncursesw.h>
// #include <pdcurses.h>

// these parameter is required
#define USE_DOS_PATH 1

// comment these out if autocheck is wanted (compiler abilities)
#define HAVE_WIDE 1				// wide support
#define HAVE_COLOR 1			// color support

// #define HAVE_CLIPBOARD 1 // clipboard support

// only use these if you REALLY have to, and has_colors is not working
// #define FORCE_COLOR_ON
