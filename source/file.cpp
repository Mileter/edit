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

using namespace std;

// Function to read a file and store its contents in a buffer
bool readfile(const std::string & file, std::string & buffer)
{
	std::ifstream infile(file);

	buffer = "";		// clear buffer

	// Throw an exception if the file cannot be opened
	if (!infile.is_open())
	{
		throw std::runtime_error("An unknown error occured when trying to open file \"" + file + "\".");
		return false;
	}

	buffer.assign(std::istreambuf_iterator < char >(infile), std::istreambuf_iterator < char >());

	// Check for I/O errors
	if (infile.bad())
	{
		throw std::runtime_error("I/O error occurred while reading the file \"" + file + "\".");
		return false;
	}

	return true;
}

// Function to write a buffer to a file
bool writefile(const std::string & file, const std::string & buffer)
{
	std::ofstream outfile(file, std::ios::trunc | std::ios::binary);

	// Throw an exception if the file cannot be opened
	if (!outfile.is_open())
	{
		throw std::runtime_error("Could not open file for writing: " + file);
		return false;
	}

	outfile.write(buffer.c_str(), buffer.size());

	// Check for I/O errors
	if (outfile.bad())
	{
		throw std::runtime_error("I/O error occurred while writing to the file: " + file);
		return false;
	}

	return true;
}

bool extractLinesFromBuf(vector < string > &result, std::string & buffer, size_t startLine, size_t numLines, char delim)
{
	if (startLine < 0)
	{
		throw std::runtime_error("Attempted to extract non-positive line number.");
		return false;
	}
	if (numLines <= 0)
	{
		throw std::runtime_error("Attempted to extract non-positive number of lines.");
		return false;
	}

	size_t start = 0;
	size_t currentLine = 1;

	while (currentLine < startLine)
	{
		start = buffer.find(delim, start);
		if (start == string::npos)
		{
			return true;	// out of bounds
		}

		start++;	// move forward until meeting the specified
		// offset
		currentLine++;	// this operation might be a little slow,
		// redoing
	}			// the above operations as many times, as many
	// as there is operations to go up and down a buffer.
	// this will be the initial one, after jumping to a
	// line, or opening a new file.

	for (size_t i = 0; i < numLines; i++)
	{
		size_t end = buffer.find(delim, start);
		if (start == string::npos)
			break;

		if (end == string::npos)
		{
			result.push_back(buffer.substr(start));	// push rest
			// of buf
			
			#ifdef DEBUG
			for( ; i < numLines; i++)
				result.push_back("~");
			#endif
			break;
		}

		result.push_back(buffer.substr(start, end - start));
		start = end + 1;
	}

	return true;
}


size_t getNthDelimWithOffset(std::string & buffer, size_t n, size_t offset, char delim)
{
	size_t it = 0;
	size_t count = 0;

	// the result can be the size of the buffer, if issues are there

	if (n == 0)		// Edge case: no delimeters at all
	{
		if (offset > buffer.size())	// unsigned cannot go
			// negetive.
		{
			throw runtime_error("Offset goes out of bounds.");
		}
		return offset;
	}

	for (; it < buffer.size(); it++)
	{
		if (buffer[it] == delim)
		{
			count++;
			if (count == n)
			{
				size_t result = it + offset;
				if (result > buffer.size())
				{	// cannot go below 0, as that is
					// unsigned storage does.
					throw std::runtime_error("Offset goes out of bounds.");
				}
				return result;
			}
		}
	}

	throw std::runtime_error("Delimiter not found enough times.");
}

bool extractSingleLineFromBuf(string & result, std::string & buffer, size_t startLine, char delim)
{
	vector < string > v;
	try
	{
		extractLinesFromBuf(v, buffer, startLine, 1, delim);
	}
	catch(std::runtime_error & r)
	{
		result = "? RUNTIME ERROR";
		throw r;
		return false;
	}

	if (v.size() < 1)
	{
		return false;	// no such line exists
	}

	result = v[0];
	return true;
}
