/*
Written by David Stidolph on 8 / 24 / 2018
Copyright 2018 - all rights held by the author.No reuse or publication without permission allowed
*/

#include "stdafx.h"
#include "WordValidator.h"
#include "resource.h"
#include <cstdio>
#include <algorithm>
#include <fstream>
#include <streambuf>

WordValidator::WordValidator()
{
}


WordValidator::~WordValidator()
{
}

/// <summary>
/// Processes the word list, which is loaded into m_StringsBuffer, so we can create another vector
/// with pointers to the strings within the buffer.  This lets us load very fast and just manipulate
/// the data in memory without a lot of allocations.
/// </summary>
/// <returns>true on success, false on failure</returns>
bool WordValidator::ProcessWordList()
{
	/* Step 1
	Walk through the word list counting lines so that we know how many lines we are talking about.
	This allows us to resize one time the final list so we can initialize it once (faster) instead
	of just keep pushing back items on the container (triggering copy on resize).

	The text file I found (https://drive.google.com/file/d/0B9-WNydZzCHrdDVEc09CamJOZHc/view) has
	each line end with \r\n.  Coding to allow for \n in case file edited on Linux.
	*/
	m_Strings.clear(); // set to empty
	DWORD size = int(m_StringsBuffer.size());
	DWORD count = 0; // count of words
	const char *pCount = &m_StringsBuffer[0];
	bool processessingSeperator = true; // trace we have hit \r or \n
	for (DWORD i = 0; i < size; i++, pCount++)
	{
		if (('\r' == *pCount) || ('\n' == *pCount))
		{
			if (!processessingSeperator)
			{
				count++;
				processessingSeperator = true;
			}
		}
		else if (processessingSeperator)
		{
			count++;
			processessingSeperator = false;
		}
		// if not seperator or just comming off one just continue on through the word until the next seperator
	}
	const char *last = &m_StringsBuffer[size - 1];
	if (('\r' != *last) && ('\n' != *last))
		count++; // get the final line "straggler"

	/* Step 2
	Now we know how many lines (words) we have so we can set up the pointers to the data and
	NULL terminate the words by replacing the \r or \n (or both) to \0.
	*/
	m_Strings.reserve(count);
	char *pData = &m_StringsBuffer[0];
	processessingSeperator = true; // act as if we just had a CR so the first word will be added
	for (DWORD i = 0; i < size; i++, pData++)
	{
		switch (*pData)
		{
		case '\r':
			processessingSeperator = true;
			*pData = '\0';
			break;
		case '\n':
			processessingSeperator = true;
			*pData = '\0';
			break;
		default:
			if (processessingSeperator)
			{
				// This is the first line after CR, so we have something to add (our pointer into the buffer)
				m_Strings.push_back(pData); // we have already done reserve so no new allocation is needed - avoid copy!
				processessingSeperator = false;
			}
			break;
		}
	}
	return !m_Strings.empty();
}

/// <summary>
/// Initializes word list passing in the specified filename.
/// </summary>
/// <param name="filename">Path to the text file to load</param>
/// <returns>true on success, false on failure</returns>
bool WordValidator::Initialize(LPCSTR filename)
{
	std::ifstream file(filename);
	std::string str;

	file.seekg(0, std::ios::end);
	m_StringsBuffer.reserve(DWORD(file.tellg())); // assume we will not have a file over 4GB in size
	file.seekg(0, std::ios::beg);

	m_StringsBuffer.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	return ProcessWordList(); // return success/failure from parsing data
}

#if defined(_WIN32)
/// <summary>
/// Initializes the word list passing in the specified resource identifier.
/// </summary>
/// <param name="resourceID">The resource identifier holding the text file.</param>
/// <returns></returns>
bool WordValidator::Initialize(int resourceID)
{
	bool success = false;
	HMODULE handle = ::GetModuleHandle(NULL);
	if (NULL != handle)
	{
		HRSRC rc = ::FindResource(handle, MAKEINTRESOURCE(resourceID), MAKEINTRESOURCE(TEXTFILE));
		if (NULL != rc)
		{
			HGLOBAL rcData = ::LoadResource(handle, rc);
			if (NULL != rcData)
			{
				DWORD size = ::SizeofResource(handle, rc);
				if (size > 0)
				{
					DWORD count = 0;
					char *pResourceData = static_cast<char*>(::LockResource(rcData));
					if (NULL != pResourceData)
					{
						success = true;
						m_StringsBuffer.resize(size);
						memcpy(&m_StringsBuffer[0], pResourceData, size);

						success = ProcessWordList();
					}
				}
			}
		}
	}
	return success;
}
#endif

/// <summary>
/// compares two const char * data for the binary_search
/// </summary>
/// <param name="aux1">1st string</param>
/// <param name="aux2">2nd string</param>
/// <returns><c>true</c> if the 1st string is "less" than the 2nd, otherwise, <c>false</c>.</returns>
bool compareFunction(const char *aux1, const char *aux2)
{
	if (strcmp(aux1, aux2) < 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/// <summary>
/// Determines whether the specified word is valid (is in the list)
/// </summary>
/// <param name="word">The word.</param>
/// <returns>
///   <c>true</c> if the specified word is valid (found in the list); otherwise, <c>false</c>.
/// </returns>
bool WordValidator::isValid(const std::string &word) const
{
	std::string upperWord = word;
	transform(upperWord.begin(), upperWord.end(), upperWord.begin(), ::toupper);
	bool found = std::binary_search(m_Strings.begin(), m_Strings.end(), upperWord.c_str(), compareFunction);
	return found;
}
