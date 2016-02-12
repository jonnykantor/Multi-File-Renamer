#include <stdio.h>
#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include "display_with_console.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	static const char* illegal_chars = "\\/:*?\"|&<>"; 
	static const char* clear_line = "\r";
#else
	static const char* illegal_chars = "/|";	
	static const char* clear_line = "\33[2K\r";
#endif


void clear_screen(){
	#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
		std::system("cls");		
	#else
		std::system("clear");
	#endif
}

std::vector<int> console_paddingVectorMake(Dynamic_File_List* p_DFL){
 	int num_rows = p_DFL->getNumRows();
 	int num_cols = p_DFL->getNumCols();
 	int item_size = 0;
 	row_node* tmp = p_DFL->root.next;
 	std::vector<int> ret_vec;

 	while(tmp!=NULL){
 		size_t tmp_sz = tmp->s.size();
 		for(int j = 0; j < (int)tmp_sz; j++){
 			item_size = strlen(tmp->s[j].c_str());

 			if(ret_vec.size() < tmp_sz) ret_vec.resize(tmp_sz, 0);
 			if(ret_vec[j] < item_size) ret_vec[j] = item_size;
 		}
 		tmp = tmp->next;
 	}
 	return ret_vec;
}

void console_printRangeSelectionState(list_attribs* p_LA){
	std::string tmp;
	tmp.clear();
	std::vector<std::string> range_vec;
	range_vec.clear();
	int state = p_LA->cur_state;
	if(state == INSERT || state == REPLACE || state == SPLIT_COLUMNS || state == ERASE || state == MOVE_ROWS || state == DELETE_ROWS){
		tmp.replace(tmp.begin(), tmp.end(), "From Row X ["
							+ std::to_string(p_LA->r_first)
							+ "] to Row Y ["
							+ std::to_string(p_LA->r_last)
							+ "]");
		range_vec.push_back(tmp);
		if(state != DELETE_ROWS){
			tmp.clear();
			range_vec.push_back(" | ");
			switch(state){
				case INSERT:
					tmp.replace(tmp.begin(), tmp.end(), "At Column W ["
										+ std::to_string(p_LA->c_first)
										+ "]");
					range_vec.push_back(tmp);
					break;
				case REPLACE:
				case ERASE:
					tmp.replace(tmp.begin(), tmp.end(), "From column W ["
										+ std::to_string(p_LA->c_first)
										+ "] to column Z ["
										+ std::to_string(p_LA->c_last) 
										+ "]");
					range_vec.push_back(tmp);
					break;
				/*
					tmp.replace(tmp.begin(), tmp.end(), "From column W ["
										+ std::to_string(p_LA->c_first)
										+ "] to column Z ["
										+ std::to_string(p_LA->c_last) 
										+ "]");
					range_vec.push_back(tmp);
					break;*/
				case SPLIT_COLUMNS:
					tmp.replace(tmp.begin(), tmp.end(), "At column W ["
										+ std::to_string(p_LA->c_first)
										+ "] | At Position ["
										+ std::to_string(p_LA->c_last)
										+ "] (Column Z)");
					range_vec.push_back(tmp);
					break;
				case MOVE_ROWS:
					tmp.replace(tmp.begin(), tmp.end(), "To position ["
										+ std::to_string(p_LA->move_rows_target)
										+ "] (Column W)");
					range_vec.push_back(tmp);
					break;
			}
		}
	}
	printf(clear_line); //clear current line
	for(int i = 0; i < (int)range_vec.size(); i++){
		printf("%s", range_vec[i].c_str());
	}
}

bool isNotIllegalString(std::string input_str, const char* cstr){
	return ((int)input_str.find_first_of(cstr) < 0) ? true : false;	
}

void console_printMenuState(list_attribs* p_LA, Dynamic_File_List* p_DFL){

	p_LA->menu_vec.clear();

	switch(p_LA->cur_state){ //State Output
		case MAIN_MENU:
			p_LA->menu_vec = {"[Main Menu] ******************************* (u) Undo (r) Redo",
							"KEY - OPTION", 							 
							"(i) - Insert phrase or iterator |#| (r) - Replace column values", 							
							"(x) - Split a column            |#| (m) - Move rows around",
							"(e) - Erase column entries      |#| (d) - Delete rows",							
							"(s) - Save changes              |#| (l) - Load new files",
							"(ESC) - Exit (unsaved changes will be lost)"};
			break;
		case SET_PHRASE:
			printf(clear_line); //clear current line
			printf("Please enter phrase for insertion [Enter to confirm]: ");
			std::getline(std::cin, p_LA->phrase);
			while(!isNotIllegalString(p_LA->phrase, illegal_chars)){
				printf(clear_line); //clear current line
				printf("Note: characters '%s' are disallowed: ", illegal_chars);
				std::getline(std::cin, p_LA->phrase);	
			}
			p_LA->isPhraseSet = true;
			break;
		case INSERT:
			p_LA->menu_vec = {"[Insertion Options]*************************************(u) Undo, (n) Redo",
							"KEY - OPTION",
							"(s) - Set phrase for insertion |#| (i) - Insert an iterator",
							"(b) - Back to main menu        |#| (ESC) - Exit (unsaved changes will be lost)"};

			if(p_LA->isPhraseSet==true){
				p_LA->menu_vec.push_back("@@@ SET PHRASE IS: \"" + p_LA->phrase + "\" (c) - CONFIRM REPLACEMENT? @@@");
			}

			p_LA->menu_vec.push_back("");
			p_LA->menu_vec.push_back("*****************************DIRECTIONAL CONTROLS*****************************");
			p_LA->menu_vec.push_back("Row X -  Up-key/Down-key  |#|  Column W - Left-key/Right-key");
			p_LA->menu_vec.push_back("Row Y - Alt+Up/Alt+Down   |#|");
			break;
		case REPLACE:
			p_LA->menu_vec = {"[Replacement Options]***********************************(u) Undo, (n) Redo",
							"KEY - OPTION !!!NOTE: THIS WILL REPLACE COLUMN VALUES!!!",
							"(s) - Set phrase for insertion |#| (i) - Insert an iterator",
							"(b) - Back to main menu        |#| (ESC) - Exit (unsaved changes will be lost)"};

			if(p_LA->isPhraseSet==true){
				p_LA->menu_vec.push_back("@@@ SET PHRASE IS: \"" + p_LA->phrase + "\" (c) - CONFIRM REPLACEMENT? @@@");
			}

			p_LA->menu_vec.push_back("");
			p_LA->menu_vec.push_back("*****************************DIRECTIONAL CONTROLS*****************************");
			p_LA->menu_vec.push_back("Row X -  Up-key/Down-key  |#|  Column W - Left-key/Right-key");
			p_LA->menu_vec.push_back("Row Y - Alt+Up/Alt+Down   |#|  Column Z - Alt+Left/Alt+Right");
			break;
		case ERASE:
			p_LA->menu_vec = {"[Erase Options]*************************************(u) Undo, (n) Redo",
							"KEY - OPTION",
							"(c) - Confirm erasure of selected range |#| (b) - Back to main menu",
							"(ESC) - Exit (unsaved changes will be lost)",
							"",
							"*****************************DIRECTIONAL CONTROLS*****************************",
							"Row X -  Up-key/Down-key  |#|  Column W - Left-key/Right-key",
							"Row Y - Alt+Up/Alt+Down   |#|  Column Z - Alt+Left/Alt+Right"			
							};

							
			break;
		case DELETE_ROWS:
			p_LA->menu_vec = {"[Delete rows]*************************************(u) Undo, (n) Redo",
							"KEY - OPTION",
							"(c) - Confirm deletion of selected rows |#| (b) - Back to main menu",
							"(ESC) - Exit (unsaved changes will be lost)",
							"",
							"*****************************DIRECTIONAL CONTROLS*****************************",
							"Row X -  Up-key/Down-key",
							"Row Y - Alt+Up/Alt+Down"};

			break;
		case MOVE_ROWS:
			p_LA->menu_vec = {"[Move rows]*************************************(u) Undo, (n) Redo",
							"KEY - OPTION",
							"(c) - Confirm movement of selected rows |#| (b) - Back to main menu",
							"(ESC) - Exit (unsaved changes will be lost)",
							"",
							"*****************************DIRECTIONAL CONTROLS*****************************",
							"Row X -  Up-key/Down-key",
							"Row Y - Alt+Up/Alt+Down"};

			break;
		case SAVE_CHANGES:
			p_LA->menu_vec = {"[Save changes]************************************(u) Undo, (n) Redo",
							"KEY - OPTION",
							"(c) - Confirm save? [THIS MUST BE MANUALLY UNDONE]",
							"(b) - Back to main menu",
							"(ESC) - Exit (unsaved changes will be lost)"};
			
			break;
		case SPLIT_COLUMNS:
			p_LA->menu_vec = {"[Split column at position]************************(u) Undo, (n) Redo",
							"KEY - OPTION",
							"(c) - Confirm split at position for selected rows",
							"(b) - Back to main menu",
							"(ESC) - Exit (unsaved changes will be lost)",
							"",
							"*****************************DIRECTIONAL CONTROLS*****************************",
							"Row X -  Up-key/Down-key  |#|  Column W - Left-key/Right-key",
							"Row Y - Alt+Up/Alt+Down   |#|  Column Z - Alt+Left/Alt+Right"};

			
		case ITERATOR:
			break;
	}

	if(p_LA->cur_state != SET_PHRASE){
		printf(clear_line); //clear current line
		for(int i = 0; i < p_LA->menu_vec.size(); i++){
			printf("\n%s", p_LA->menu_vec[i].c_str());
		}
		printf("\n");
	}
}

void console_printDFL(Dynamic_File_List* p_DFL){
	std::vector<int> padding = console_paddingVectorMake(p_DFL);

	int num_cols = p_DFL->getNumCols();
	int num_rows = p_DFL->getNumRows();
	int sz = 0;
	int ex_padding = 6;

	row_node* prnt_ptr = &p_DFL->root;
	//root node and column headers
	
	if((int)padding.size() > 0){
	printf("%s\t", prnt_ptr->s[0].c_str());
		for(int i = 0; i < num_cols; i++){
			printf("%s%d", "col ", i+1);
			for(int pad = 0; pad < (padding[i] - 5 + ex_padding); pad++) printf("%-1s", " ");			//(padding[i] + ex_padding)
		}
	printf("%s\n", prnt_ptr->ext[0].c_str());	
	}
	else printf("\nThere are no rows to print! You'll have to load more (press l)\n");
	prnt_ptr = prnt_ptr->next;

	//Row Numbers, File Names, and extensions
	int cur_row_ind = 0;
	for(int i = 1; i <= num_rows; i++){
		sz = prnt_ptr->s.size();
		cur_row_ind = prnt_ptr->index;

		
		if(cur_row_ind!=0) printf("%d.\t", cur_row_ind);
		for(int j = 0; j < sz; j++){
			std::string to_prnt = prnt_ptr->s[j];
			if(!to_prnt.compare(" ")){ 
				//to_prnt = "<sp>";
				printf("%7s", to_prnt.c_str());
			}
			else printf("%s", to_prnt.c_str());
			for(int pad = (0 + strlen(prnt_ptr->s[j].c_str())); pad < (padding[j] + ex_padding); pad++) printf("%-1s", " ");
		}
		if(prnt_ptr->ext.size()>0){
			for(int k = 0; k < (num_cols - prnt_ptr->s.size()); k++){ 
				for(int pad = 0; pad < (padding[num_cols-1-k] + ex_padding); pad++) printf("%1s", " ");
			}
			printf("%s", prnt_ptr->ext[0].c_str());
		}
		printf("\n");
		prnt_ptr = prnt_ptr->next;
	}
}

void console_clearAndPrint(list_attribs* p_LA, Dynamic_File_List* p_DFL){
	clear_screen();
	console_printDFL(p_DFL);
	console_printMenuState(p_LA, p_DFL);
}