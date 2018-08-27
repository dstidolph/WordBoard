/*
Written by David Stidolph on 8/24/2018
Copyright 2018 - all rights held by the author.  No reuse or publication without permission allowed

These classes are designed as an educational exercise to provide a 19x19 scrabble type board
and the emphesis is on clear coding, rather than performance or reducing memory usage.

State changes (adding words) are kept in WordBoardMove instances to provide "undo" and "redo"
capabilities.  Though not in the original problem statement, it was added for "extra credit"  :)

*/

#pragma once

#include "string"
#include "WordValidator.h"
#include <vector>
#include <list>

// Direction of word - horizontal (left->right) or vertical (top->down)
typedef enum {
	dirHorizontal, /// Horizontal Direction
	dirVertical /// Vertical Direction
} DirectionType;

/// <summary>
/// This class holds a 'Move' on the board.  The direction, the start location and holds the new text and the text that was replaced
/// </summary>
class WordBoardMove
{
public:
	int m_StartRow;
	int m_StartCol;
	DirectionType m_direction;
	std::string m_originalText;
	std::string m_newText;
};

/// <summary>
/// This holds the board contents, methods to manipulate the board and contents and the sequence of moves applied to that board
/// </summary>
class WordBoard
{
public:
	WordBoard();
	~WordBoard();

	// Initializes the board width/height and clears to empty (spaces ' ')
	bool Init(int width, int height);

	// Get the board information / contents
	int GetNumColumns() { return m_widthBoard; }
	int GetNumRows() { return m_heightBoard; }
	bool GetBoard(std::vector<std::string> &output); // return a vector of strings holding the board contents

	// Add words to board - returns true on success, false on cannot do it and sets errorText
	bool AddWordH(int row, int col, const std::string &word, std::string & errorText);
	bool AddWordV(int row, int col, const std::string &word, std::string & errorText);

	// Undo/Redo functions
	bool HasUndo() { return !m_Moves.empty(); }
	bool HasRedo() { return !m_redoMoves.empty(); } // Normally, redo is empty unless you have done Undo and NOT added any moves
	bool Undo(std::string &errorText); // Pull last move off m_undoMoves to undo and push onto m_redoMoves
	bool Redo(std::string &errorText); // Pull last move off m_redoMoves to redo and push onto m_undoMoves

	// Get specific squares from the board - returns true on success, false on failure
	bool GetBoardTextH(int row, int col, int width, std::string &output);
	bool GetBoardTextV(int row, int col, int height, std::string &output);

private:
	bool SetBoardTextH(int row, int col, std::string value);
	bool SetBoardTextV(int row, int col, std::string value);
	bool ApplyMove(const WordBoardMove &move);
	bool UndoMove(const WordBoardMove &move);
	bool GetBoardAt(int row, int col, char &value); // return the character at the specied position
	bool GetBoardRow(int row, std::string &output); // return the specific row as a string
	bool GetBoardCol(int col, std::string &output); // return the specific col as a string
	bool GetWordH(int row, int col, std::string &word); // return the word left<->right from this point with spaces breaking words or boundaries
	bool GetWordV(int row, int col, std::string &word); // return the word top<->bottom from this point with spaces breaking words or boundaries

	typedef std::vector<char> ColumnContainer;
	typedef std::vector<ColumnContainer> RowContainer;
	typedef std::list<WordBoardMove> MoveContainer; // stores 'Moves' for undo/redo function

	bool m_initialized;
	int m_widthBoard;
	int m_heightBoard;
	RowContainer m_board;
	MoveContainer m_Moves;
	MoveContainer m_redoMoves;
	WordValidator m_wordValidator;
};

