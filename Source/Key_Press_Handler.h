#include <stdio.h>

enum keys{
  SHIFT_UP = 0,
  SHIFT_DOWN,
  SHIFT_RIGHT,
  SHIFT_LEFT,
	UP,
	DOWN, 
	LEFT, 
	RIGHT, 
	ESC,
  B, //Back to Main Menu
  C, //Confirm
  D, //Delete Rows
  E, //Erase Items
  I, //Insert Items (Main menu)/Iterator Insertion (Insert/Replace)
  L, //Load new files
  M, //Mode (to be removed in favor of ranges)
  N, //Number (to be removed in favor of iterators)
  R, //Replace items (Main menu)
  S, //Save list (Main menu)/Set phrase (Insert/Replace)
  U, //Undo
  W, //Row range (to be removed in favor of A key)
  X, //Split column at position
	KEYS_TOTAL
};

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <conio.h>

int keypress_handler();

int getKeyVal();

#else //UNIX

#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>

bool isSetTerminalToHideInput(int, termios *);

int keypress_handler(int, unsigned char *);

int getKeyVal();
#endif

int main_loop();