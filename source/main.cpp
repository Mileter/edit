/* 
   main.cpp --- handles bootstrap and command line arguments

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

std::vector < std::string > filebuf;
std::string filename;		// filename path

#if USE_DOS_PATH
bool useCRLF = true;
#else
bool useCRLF = false;
#endif

int main(int argc, char **argv)
{
	init_curs();

	if (argc == 2)
	{
		try
		{
			filename = argv[1];
			readfile(filename, filebuf);	// read into buffer at 
							// 
			// 
			// 
			// 
			// 
			// 
			// 
			// first
		}
		catch(const std::runtime_error & ex)
		{
			show_err("Error whilest reading file!", ex.what());
		}
	}
	else
	{
		show_norm("License and Information",
			  (std::string)
			  "Miles R. Chang has not finished this program yet. Thus, please wait for 1.0 for a most stable release. Close this dialog, and know that this software is still is development.\n(MSG_NIGHTLY)\n\nBy continuing, you agree to the following license: \n\nMIT License\n\n"
			  + (std::string) MIT_LICENSE);
		filebuf = { " " };
	}

	while (mainloop() != false)
	{
		continue;
	}

	uninit_curs();
	return 0;
}
