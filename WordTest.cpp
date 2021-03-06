// WordTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "WordBoard.h"
#include <iostream>

using namespace std;

/// <summary>
/// Prints out the board contents.  Reads board size so it is flexible...
/// </summary>
/// <param name="board">The board.</param>
void DumpBoard(WordBoard &board)
{
	vector<string> boardData;
	if (board.GetBoard(boardData))
	{
		// Print the header
		int width = board.GetNumColumns();
		cout << "   "; // this gives space for the left header area
		for (int col = 0; col < width; col++)
		{
			if (col >= 9)
				cout << col / 10;
			else
				cout << " ";
		}
		cout << endl;
		cout << "   "; // this gives space for the left header area
		for (int col = 0; col < width; col++)
			cout << col % 10;
		cout << endl;

		// Print the lines
		int lineNum = 0;
		for (auto i = boardData.begin(); i != boardData.end(); i++, lineNum++)
		{
			if (lineNum < 10)
			{
				cout << " " << lineNum << ":" << i->c_str() << endl;
			}
			else
			{
				cout << lineNum << ":" << i->c_str() << endl;
			}
		}
		cout << endl;
	}
	else
		cerr << "Failed to get the board data!  Cannot render" << endl;
}

int main()
{
	cout << "WordTest - testing WordBoard class" << endl << endl; // title followed by blank line
	string errorText;
	WordBoard board;
	if (board.Init(19, 19))
	{
		cout << "Blank board:" << endl;
		DumpBoard(board);
		cout << "Add ANON horizontal at 5,5" << endl;
		if (!board.AddWordH(5, 5, "ANON", errorText))
			cerr << "Failure from AddWordH for 'ANON': " << errorText.c_str() << endl;
		DumpBoard(board);
		cout << "Add BAKER at 7,5 (expected failure because this is NOT connected to existing word" << endl;
		if (board.AddWordH(7, 5, "BAKER", errorText)) // expected failure - not connected
			cerr << "Failure - BAKER was added disconnected so should fail but success was reported" << endl;
		else
			cout << "Correct failed to add - returned text: " << errorText.c_str() << endl;
		cout << "Add NEXT vertical at 5,6 so it overlaps with the first N in ANON" << endl;
		if (!board.AddWordV(5, 6, "NEXT", errorText))
			cerr << "Failure adding 'NEXT' " << errorText.c_str() << endl;
		DumpBoard(board);
		cout << "Extending ANON into ANONYMAS by AddWordH at 5,9" << endl;
		if (!board.AddWordH(5, 9, "YMAS", errorText))
			cerr << "Failure from AddWord for 'YMAS' to end of ANON: " << errorText.c_str() << endl;
		DumpBoard(board);
		cout << "Testing UNDO - UNDO all moves, make sure empty, then test REDO" << endl;
		std::vector<string> boardData, boardEmpty, boardRestored;
		board.GetBoard(boardData);
		if (!board.Undo(errorText))
			cerr << "Undo failure 1: " << errorText.c_str() << endl;
		cout << "After Undo 1" << endl;
		DumpBoard(board);
		if (!board.Undo(errorText))
			cerr << "Undo failure 2: " << errorText.c_str() << endl;
		cout << "After Undo 2" << endl;
		DumpBoard(board);
		if (!board.Undo(errorText))
			cerr << "Undo failure 3: " << errorText.c_str() << endl;
		cout << "After Undo 3 - should be empty" << endl;
		DumpBoard(board);
		if (board.Undo(errorText))
			cerr << "We should have failure for Undo 4, but it returned success!" << endl;
		board.GetBoard(boardEmpty);
		// Check is all clear
		bool empty = true;
		for (auto row : boardEmpty)
		{
			for (size_t col = 0; empty && (col < row.length()); col++)
			{
				if (' ' != row[col])
					empty = false;
			}
		}
		if (empty)
			cout << "Board tests empty so Undo works" << endl;
		else
			cerr << "Board does NOT test empty, so Undo failed!" << endl;

		cout << "Test Redo to restore board" << endl;
		if (!board.Redo(errorText))
			cerr << "Redo failure 1: " << errorText.c_str() << endl;
		cout << "After Redo 1 - should have ANON" << endl;
		DumpBoard(board);
		if (!board.Redo(errorText))
			cerr << "Redo failure 2: " << errorText.c_str() << endl;
		cout << "After Redo 2 - should have ANON and NEXT" << endl;
		DumpBoard(board);
		if (!board.Redo(errorText))
			cerr << "Redo failure 3: " << errorText.c_str() << endl;
		cout << "After Redo 3 - should have ANONYMOUS and NEXT" << endl;
		DumpBoard(board);
		if (board.Redo(errorText))
			cerr << "We should have failure for Redo 4, but it returned success!" << endl;
		board.GetBoard(boardRestored);
		// Verify match with boardData
		bool match = boardData.size() == boardRestored.size();
		if (match)
		{
			for (size_t i = 0; match && (i < boardData.size()); i++)
				match = 0 == boardData[i].compare(boardRestored[i]);
		}
		if (match)
			cout << "Restored board matches state before Undo/Redo" << endl;
		else
			cerr << "Failure from Undo/Redo - board does not match" << endl;
	}
	else
		cerr << "Failure from WordBoard.Init -- cannot perform tests" << endl;
	cout << "Done with tests." << endl;
	return 0;
}

