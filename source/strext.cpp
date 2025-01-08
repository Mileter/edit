/* 
   strext.cpp --- extra string operations to help with ui

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

// Utility function to trim spaces from the start and end of a string
std::string trim(const std::string & str)
{
	size_t start = str.find_first_not_of(" \t\n");
	size_t end = str.find_last_not_of(" \t\n");
	if (start == std::string::npos || end == std::string::npos)
		return "";
	return str.substr(start, end - start + 1);
}

// Function to wrap the message into lines based on max_width, splitting by
// words and preserving newlines
bool
wrap_message(std::string message, size_t max_width,
	     std::vector < std::string > &wrapped_lines)
{
	size_t start = 0;
	size_t end = 0;

	// Split the message by newline characters, processing one segment at
	// a
	// time
	while ((end = message.find('\n', start)) != std::string::npos)
	{
		std::string line_segment = message.substr(start, end - start);	// Extract 
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
		// 
		// 

		// each 
		// segment 
		// between 
		// newlines
		std::istringstream line_stream(line_segment);
		std::string word;
		std::string current_line;

		// Process the words in the current segment
		while (line_stream >> word)
		{
			word = trim(word);	// Trim any leading/trailing
			// spaces in words

			// Check if the word fits in the current line
			if (current_line.empty())
			{
				current_line = word;	// First word in the
				// line
			}
			else if (current_line.size() + 1 + word.size() <=
				 max_width)
			{
				current_line += " " + word;	// Add word
				// with space
				// to the line
			}
			else
			{
				wrapped_lines.push_back(current_line);	// Add 
									// 
				// 
				// 
				// 
				// 
				// 
				// 
				// the 
				// current
				// line to wrapped
				// lines
				current_line = word;	// Start a new line
				// with the current
				// word
			}
		}

		// Add any remaining text in the current line to wrapped lines
		if (!current_line.empty())
		{
			wrapped_lines.push_back(current_line);
		}

		// Add an empty line to represent the newline in the input
		// message
		wrapped_lines.push_back("");	// This ensures a blank line
		// where the 
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
		// 
		// 
		// 
		// 
		// '\n' was.

		start = end + 1;	// Move to the next part of the
		// message after
		// the newline
	}

	// Process any remaining text after the last newline
	if (start < message.size())
	{
		std::string line_segment = message.substr(start);
		std::istringstream line_stream(line_segment);
		std::string word;
		std::string current_line;

		while (line_stream >> word)
		{
			word = trim(word);	// Trim any leading/trailing
			// spaces in words

			// Check if the word fits in the current line
			if (current_line.empty())
			{
				current_line = word;	// First word in the
				// line
			}
			else if (current_line.size() + 1 + word.size() <=
				 max_width)
			{
				current_line += " " + word;	// Add word
				// with space
				// to the line
			}
			else
			{
				wrapped_lines.push_back(current_line);	// Add 
									// 
				// 
				// 
				// 
				// 
				// 
				// 
				// the 
				// current
				// line to wrapped
				// lines
				current_line = word;	// Start a new line
				// with the current
				// word
			}
		}

		// Add any remaining text in the current line to wrapped lines
		if (!current_line.empty())
		{
			wrapped_lines.push_back(current_line);
		}
	}

	return true;
}
