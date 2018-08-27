This is a simple C++ project to demonstrate my skills.  I received it as a programming exercise for a job application.

I had to put it off for a couple of days while attending to critical errands and started work on it saturday morning August 25th.
I remembered an article I read in BYTE magazine 20 years ago about adding UNDO into a project.  You turn it into a state machine where
each state holds the state change (before and after) and you keep them in a list that can be traversed.

I applied this to the scrabble game.  Each "Move" holds the row/column location, the orientation (up/down or left/right), the original
contents and the new.  I store the moves and if a move is undone it is placed in another list in case of REDO.  If a new move is generated
it goes on the move list and clears the REDO list.

The problem description did list UBUNTU as the desired platform, but all I have access to at the moment is Windows and the free version
of Visual Studio.  With that limitation I have achieve PART of the desired result, but not all.

There is the question of placement of words beyond the first.  The problem specified that new words must share at least one space.
This can be interpreted where a new word must OVERLAP an existing word - rather than also support adjacent to.  I could easily make this
change, but the interpretation must be decided on...

