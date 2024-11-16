/* 
   file.cpp --- handles file I/O

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

// Function to read a file and store its contents in a buffer
bool readfile(const std::string & file,	// IN
			  std::vector < std::string > &buffer)	// OUT
{
	std::ifstream infile(file);

	// Throw an exception if the file cannot be opened
	if (!infile.is_open())
	{
		throw std::runtime_error("Could not open file for reading: " + file);
		return false;
	}

	std::string line;
	while (std::getline(infile, line))
		buffer.push_back(line + " ");

	// Throw an exception if there was an I/O error while reading
	if (infile.bad())
	{
		throw std::runtime_error("I/O error occurred while reading the file: "
								 + file);
		return false;
	}

	infile.close();
	return true;
}

// Function to write a buffer to a file
bool writefile(const std::string & file,	// IN
			   const std::vector < std::string > &buffer)	// IN
{
	std::ofstream outfile(file, std::ios::trunc);	// Open file and truncate
	// it if it exists

	// Throw an exception if the file cannot be opened
	if (!outfile.is_open())
	{
		throw std::runtime_error("Could not open file for writing: " + file);
		return false;
	}

  for (const auto & line:buffer)
	{
		std::string str = line.substr(0, line.size() - 2);
		outfile << line << std::endl;
	}

	// Throw an exception if there was an I/O error while writing
	if (outfile.bad())
	{
		throw
			std::runtime_error("I/O error occurred while writing to the file: "
							   + file);
		return false;
	}

	outfile.close();
	return true;
}
