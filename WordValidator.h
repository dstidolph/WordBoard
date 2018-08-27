/*
This class was defined in the programming problem (partially defined) and I have provided an
implimentation to load a valid word list and be able to check against it.

I found a scrabble word list online at https://drive.google.com/file/d/0B9-WNydZzCHrdDVEc09CamJOZHc/view
This file holds 276,653 words.
*/

#pragma once

#include "string"
#include <vector>

class WordValidator
{
public:
	WordValidator();
	virtual ~WordValidator();

#if defined(_WIN32)
	bool Initialize(int resourceID); // Initialize using built in resource file instead of external textfile
#endif
	bool Initialize(LPCSTR filename); // Initialize using external textfile

	virtual bool isValid(const std::string &word) const;
private:
	bool ProcessWordList(); // Process the loaded word list, which will be stored in m_StringsBuffer

	std::vector<LPCSTR> m_Strings; // Holds sorted list of pointers to strings in m_StringsBuffer
	std::vector<char> m_StringsBuffer; // This holds a copy of the words
};

