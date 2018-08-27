#include "stdafx.h"
#include "WordBoard.h"
#include "resource.h"

WordBoard::WordBoard()
{
}


WordBoard::~WordBoard()
{
}

/// <summary>
/// Initializes the board.
/// </summary>
/// <param name="width">The width of the board.</param>
/// <param name="height">The height of the board.</param>
/// <returns>true on success</returns>
bool WordBoard::Init(int width, int height)
{
	m_heightBoard = height;
	m_widthBoard = width;
	m_board.resize(height);
	for (int nRow = 0; nRow < height; nRow++)
	{
		m_board[nRow].resize(width, ' '); // resize to proper width and set to empty (' ' space character)
	}
	m_redoMoves.clear();
	m_Moves.clear();
#if defined(_WIN32)
	// Built on windows, so uses resource bound into executable
	m_initialized = m_wordValidator.Initialize(IDR_TEXTFILE1);
#else
	// Built on other than windows, so loads external text file from file system
	m_initialized = m_wordValidator.Initialize("./WordList.txt");
#endif
	return m_initialized;
}

bool WordBoard::GetBoardAt(int row, int col, char &value)
{
	bool success = false;
	if (m_initialized)
	{
		if ((row >= 0) && (row < m_heightBoard))
		{
			if ((col >= 0) && (col < m_widthBoard))
				value = m_board.at(row).at(col);
		}
	}
	return success;
}

/// <summary>
/// Gets a specific board row.  The string will be the board width and characters
/// hold the letters (space ' ' is empty and default value if unassigned)
/// </summary>
/// <param name="row">The row index (0 to height-1).</param>
/// <param name="output">The output.</param>
/// <returns></returns>
bool WordBoard::GetBoardRow(int row, std::string &output)
{
	bool success = false;
	if (m_initialized)
	{
		if ((row >= 0) && (row < m_heightBoard))
		{
			success = true;
			output.resize(m_widthBoard);
			for(int nCol = 0; nCol < m_widthBoard; nCol++)
				output[nCol] = m_board[row][nCol];
		}
	}
	return success;
}

bool WordBoard::GetBoardCol(int col, std::string &output) // return the specific col as a string
{
	bool success = false;
	if (m_initialized)
	{
		if ((col >= 0) && (col < m_heightBoard))
		{
			output.resize(m_heightBoard);
			for (int nRow = 0; nRow < m_heightBoard; nRow++)
				output[nRow] = m_board[nRow][col];
		}
	}
	return success;
}

/// <summary>
/// Gets the content of the board at this time.
/// </summary>
/// <param name="output">vector of rows holding the output.</param>
/// <returns>true on success</returns>
bool WordBoard::GetBoard(std::vector<std::string> &output)
{
	bool success = false;
	if (m_initialized)
	{
		success = true; // assume success, fail on any failure
		output.resize(m_heightBoard);
		for (int nRow = 0; success && (nRow < m_heightBoard); nRow++)
		{
			success = GetBoardRow(nRow, output[nRow]);
		}
	}
	return success;
}

bool WordBoard::GetWordH(int row, int col, std::string &word) // return the word left<->right from this point with spaces breaking words or boundaries
{
	bool success = false;
	if ((row >= 0) && (row < m_heightBoard) && (col >= 0) && (col < m_widthBoard))
	{
		std::string line;
		GetBoardRow(row, line);
		int left = col;
		while ((left > 0) && (' ' != m_board[row][left-1]))
			left--; // move to the leftmost non space or index 0 (left boundary)
		int right = col;
		while ((right < (m_widthBoard-1)) && (' ' != m_board[row][right+1]))
			right++;
		word = line.substr(left, right - left + 1);
	}
	return success;
}

bool WordBoard::GetWordV(int row, int col, std::string &word) // return the word top<->bottom from this point with spaces breaking words or boundaries
{
	bool success = false;
	if ((row >= 0) && (row < m_heightBoard) && (col >= 0) && (col < m_widthBoard))
	{
		std::string line;
		GetBoardCol(col, line);
		int top = row;
		while ((top > 0) && (' ' != m_board[top-1][col]))
			top--; // move to the topmost non space or index 0 (top boundary)
		int bottom = row;
		while ((bottom < (m_heightBoard - 1)) && (' ' != m_board[bottom+1][col]))
			bottom++;
		word = line.substr(top, bottom - top + 1);
	}
	return success;
}

/// <summary>
/// Adds the word to the board.
///   Will: match against any existing words
///         make sure it attaches to existing text
///
///   if error, return false and text expaining the problem
/// </summary>
/// <param name="row">The row.</param>
/// <param name="col">The col.</param>
/// <param name="word">The word.</param>
/// <param name="errorText">The error text.</param>
/// <returns></returns>
bool WordBoard::AddWordH(int row, int col, const std::string &word, std::string & errorText)
{
	bool success = false;
	if ((row >= 0) && (row < m_heightBoard) && (col >= 0) && (col < m_widthBoard))
	{
		int endPos = col + int(word.length()) - 1;
		if (endPos < m_widthBoard)
		{
			// Create a move, apply it and then check if valid and undo if needed
			WordBoardMove move;
			move.m_direction = dirHorizontal;
			move.m_StartRow = row;
			move.m_StartCol = col;
			move.m_newText = word;
			GetBoardTextH(row, col, int(word.length()), move.m_originalText);
			SetBoardTextH(row, col, move.m_newText);

			// Ok, now we have a potentially valid placement - need to check if it works with others
			if (m_Moves.empty())
			{
				// it is valid because it is the first on the board
				if (m_wordValidator.isValid(word))
				{
					m_Moves.push_back(move);
					success = true;
				}
			}
			else
			{
				bool extraMatch = false;
				success = true; // Assume it is true and find out if it is NOT (then undo)
				bool isValid = true;
				std::string above(m_widthBoard, ' ');
				std::string below(m_widthBoard, ' ');
				if (row > 0) // check the row above (if there is one)
					GetBoardTextH(row - 1, col, int(word.length()), above);
				if (row < (m_heightBoard - 1))
					GetBoardTextH(row + 1, col, int(word.length()), below);
				// Check the vertical 'words'
				for (int nCol = 0; success && (nCol < int(word.length())); nCol++)
				{
					if ((' ' != above[nCol]) || (' ' != below[nCol]))
					{
						extraMatch = true;
						std::string vText;
						if (GetWordV(row, nCol, vText))
						{
							success = m_wordValidator.isValid(vText);
							if (!success)
								errorText = "Invalid Vertical match of word: " + vText;
						}
					}
				}
				if (success)
				{
					// Passed vertical words, so check left and right extensions
					std::string hText;
					GetWordH(row, col, hText);
					if (hText.length() != word.length())
						extraMatch = true;
					success = m_wordValidator.isValid(hText);
					if (!success)
						errorText = "Invalid Horizontal match of word: " + hText;
				}
				if (success)
				{
					if (extraMatch)
						m_Moves.push_back(move);
					else
					{
						success = false;
						errorText = "Moves beyond the first must 'attach' to existing text";
						UndoMove(move);
					}
				}
				else
				{
					// We have a failure, so do the undo!
					UndoMove(move);
				}
			}
		}
		else
			errorText = "Word would go beyond right edge of board";
	}
	else
		errorText = "Specied row/col position is outside the bounds of the board";
	return success;
}

bool WordBoard::AddWordV(int row, int col, const std::string &word, std::string & errorText)
{
	bool success = false;
	if ((row >= 0) && (row < m_heightBoard) && (col >= 0) && (col < m_widthBoard))
	{
		int endPos = row + int(word.length()) - 1;
		if (endPos < m_heightBoard)
		{
			// Create a move, apply it and then check if valid and undo if needed
			WordBoardMove move;
			move.m_direction = dirVertical;
			move.m_StartRow = row;
			move.m_StartCol = col;
			move.m_newText = word;
			GetBoardTextV(row, col, int(word.length()), move.m_originalText);
			SetBoardTextV(row, col, move.m_newText);

			// Ok, now we have a potentially valid placement - need to check if it works with others
			if (m_Moves.empty())
			{
				// it is valid because it is the first on the board
				m_Moves.push_back(move);
				success = true;
			}
			else
			{
				bool extraMatch = false;
				success = true; // Assume it is true and find out if it is NOT (then undo)
				bool isValid = true;
				std::string left(m_heightBoard, ' ');
				std::string right(m_heightBoard, ' ');
				if (col > 0) // check the row above (if there is one)
					GetBoardTextV(row, col-1, int(word.length()), left);
				if (col < (m_widthBoard - 1))
					GetBoardTextH(row + 1, col, int(word.length()), right);
				// Check the vertical 'words'
				for (int nCol = 0; success && (nCol < int(word.length())); nCol++)
				{
					if ((' ' != left[nCol]) || (' ' != right[nCol]))
					{
						extraMatch = true;
						std::string hText;
						if (GetWordH(row, nCol, hText))
						{
							success = m_wordValidator.isValid(hText);
							if (!success)
								errorText = "Invalid Vertical match of word: " + hText;
						}
					}
				}
				if (success)
				{
					// Passed horizontal words, so check above / below possible extensions
					std::string vText;
					GetWordV(row, col, vText);
					if (vText.length() != word.length())
						extraMatch = true;
					success = m_wordValidator.isValid(vText);
					if (!success)
						errorText = "Invalid Vertical match of word: " + vText;
				}
				if (success)
				{
					if (extraMatch)
						m_Moves.push_back(move);
					else
					{
						success = false;
						errorText = "Moves beyond the first must 'attach' to existing text";
						UndoMove(move);
					}
				}
				else
				{
					// We have a failure, so do the undo!
					UndoMove(move);
				}
			}
		}
		else
			errorText = "Word would go beyond bottom edge of board";
	}
	else
		errorText = "Specied row/col position is outside the bounds of the board";
	return success;
}

/// <summary>
/// Gets the horizontal board text starting at row,col to the right for the length of 'width'.
/// </summary>
/// <param name="row">The row.</param>
/// <param name="col">The col.</param>
/// <param name="width">The width.</param>
/// <param name="output">The output.</param>
/// <returns></returns>
bool WordBoard::GetBoardTextH(int row, int col, int width, std::string &output)
{
	bool success;
	if ((row >= 0) && (col >= 0) && (row < m_heightBoard) && ((col+width) <= m_widthBoard))
	{
		output.resize(width);
		for (int index = 0; index < width; index++)
			output[index] = m_board[row][index+col];
		success = true;
	}
	return success;
}

/// <summary>
/// Sets the horizontal board contents at row,col for the length of the string to the given string.
/// </summary>
/// <param name="row">The row (0..board height).</param>
/// <param name="col">The col (0..board width).</param>
/// <param name="value">The value to set the board to.</param>
/// <returns></returns>
bool WordBoard::SetBoardTextH(int row, int col, std::string value)
{
	bool success;
	if ((row >= 0) && (col >= 0) && (row < m_heightBoard) && ((col + int(value.length())) <= m_widthBoard))
	{
		for (int nCol = col; nCol < (col + int(value.length())); nCol++)
			m_board[row][nCol] = value[nCol - col];
		success = true;
	}
	return success;
}

/// <summary>
/// Gets the vertical board contents at row,col down the height amount into the output string.
/// </summary>
/// <param name="row">The row (0..board height).</param>
/// <param name="col">The col (0..board width).</param>
/// <param name="height">The number of characters from the board to get (length but going from top down).</param>
/// <param name="output">Set to the contents of the board from the position downward.</param>
/// <returns></returns>
bool WordBoard::GetBoardTextV(int row, int col, int height, std::string &output)
{
	bool success;
	if ((row >= 0) && (col >= 0) && (row < m_heightBoard) && ((row + height) <= m_heightBoard))
	{
		output.resize(height);
		for (int nRow = row; nRow < (row + height); nRow++)
			output[nRow - row] = m_board[nRow][col];
		success = true;
	}
	return success;
}

/// <summary>
/// Sets the board contents at row,col to the text in value in the downward direction.
/// </summary>
/// <param name="row">The row (0..board height).</param>
/// <param name="col">The col (0..board width).</param>
/// <param name="value">The contents to set the board to.</param>
/// <returns></returns>
bool WordBoard::SetBoardTextV(int row, int col, std::string value)
{
	bool success;
	if ((row >= 0) && (col >= 0) && (row < m_heightBoard) && ((row + int(value.length())) <= m_heightBoard))
	{
		for (int nRow = row; nRow < (row + int(value.length())); nRow++)
			m_board[nRow][col] = value[nRow - row];
		success = true;
	}
	return success;
}

/// <summary>
/// Applies the move - it overwrites with the new text at the specified location
/// </summary>
/// <param name="move">The move.</param>
/// <returns></returns>
bool WordBoard::ApplyMove(const WordBoardMove &move)
{
	bool success = false;
	if (dirHorizontal == move.m_direction)
	{
		success = SetBoardTextH(move.m_StartRow, move.m_StartCol, move.m_newText);
	}
	else if (dirVertical == move.m_direction)
	{
		success = SetBoardTextV(move.m_StartRow, move.m_StartCol, move.m_newText);
	}
	return success;
}

/// <summary>
/// Undoes the move - it overwrites with the original text at the specified location.
/// </summary>
/// <param name="move">The move.</param>
/// <returns></returns>
bool WordBoard::UndoMove(const WordBoardMove &move)
{
	bool success = false;
	if (dirHorizontal == move.m_direction)
	{
		success = SetBoardTextH(move.m_StartRow, move.m_StartCol, move.m_originalText);
	}
	else if (dirVertical == move.m_direction)
	{
		success = SetBoardTextV(move.m_StartRow, move.m_StartCol, move.m_originalText);
	}
	return success;
}

bool WordBoard::Undo(std::string &errorText) // Pull last move off m_undoMoves to undo and push onto m_redoMoves
{
	bool success = false;
	if (!m_Moves.empty())
	{

		if (UndoMove(m_Moves.back()))
		{
			m_redoMoves.push_back(m_Moves.back());
			m_Moves.pop_back();
			success = true;
		}
		else
			errorText = "Error undoing the move";
	}
	else
		errorText = "Undo list is empty, nothing to undo";
	return success;
}

bool WordBoard::Redo(std::string &errorText) // Pull last move off m_redoMoves to redo and push onto m_undoMoves
{
	bool success = false;
	if (!m_redoMoves.empty())
	{

		if (ApplyMove(m_redoMoves.back()))
		{
			m_Moves.push_back(m_redoMoves.back());
			m_redoMoves.pop_back();
			success = true;
		}
		else
			errorText = "Error undoing the move";
	}
	else
		errorText = "Undo list is empty, nothing to undo";
	return success;
}