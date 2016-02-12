#include <stdio.h>
#include <string>
#include <cstring>
#include <vector>
#include "Dynamic_File_List.h"

enum states{
	INIT = 0,
	MAIN_MENU,
	INSERT,
	REPLACE,
	ERASE,
	DELETE_ROWS,
	ITERATOR,
	SET_PHRASE,
	EXIT_RELOAD,
	MOVE_ROWS,
	SAVE_CHANGES,
	SPLIT_COLUMNS,
	COMBINE_COLUMNS,
	UNDO,
	REDO,
	NEW_ROW,
	STATES_TOTAL
};

typedef struct list_attribs{
	std::vector<std::string> menu_vec;
	std::vector<std::string> file_paths;
	std::string full_path_prefix;
	std::string phrase;
	int num_rows;
	int num_cols;
	int move_rows_target;
	int last_state;
	int cur_state;
	int r_first; 		//range first row
	int r_last; 		//range last row
	int c_first;		//range first col
	int c_last;			//range last col
	bool isPhrSetReq;
	bool isPhraseSet;
} list_attribs;

void clear_screen();

std::vector<int> console_paddingVectorMake(Dynamic_File_List* p_DFL);

void console_printRangeSelectionState(list_attribs*);

bool isNotIllegalString(std::string *, const char*);

void console_printMenuState(list_attribs*, Dynamic_File_List*);

void console_printDFL(Dynamic_File_List* p_DFL);

void console_clearAndPrint(list_attribs*, Dynamic_File_List*);