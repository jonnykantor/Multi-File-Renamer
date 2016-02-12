#include <stdio.h>
#include <vector>
#include <string> 
#include <cstdlib>

#include "tinyfiledialogs.h"
#include "tokenizer.h"
#include "Key_Press_Handler.h"
#include "display_with_console.h"
#include "Operation_node.h"

//define slash delimeter based on host OS
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <Objbase.h>
static const std::string slash = "\\";
#else
static const std::string slash = "/";
#endif

typedef std::vector <std::string> row_ind;
typedef std::vector <row_ind> mat_col_ind;

void tearDown(Dynamic_File_List*);

void init();

#pragma warning(disable:4996) /* allows usage of strncpy, strcpy, strcat, sprintf, fopen */

void resetListAttribs(list_attribs* p_LA, Dynamic_File_List* p_DFL){
	p_LA->phrase.clear();
	p_LA->isPhraseSet = false;
	p_LA->menu_vec.clear();
	p_LA->num_rows = p_DFL->getNumRows();
	p_LA->num_cols = p_DFL->getNumCols();
	(p_LA->num_rows > 0) ? p_LA->r_first = 1 : p_LA->r_first = 0;
	p_LA->r_last = p_DFL->getNumRows();
	(p_LA->num_cols > 0) ? p_LA->c_first = 1 : p_LA->c_first = 0; 
	p_LA->c_last = p_DFL->getNumCols();
}

void resize_matrix(mat_col_ind& mat, int num_rows, int num_cols){
	int lower_size_bound;
	if(mat.size() < num_rows) lower_size_bound = mat.size();
	else lower_size_bound = num_rows;

	for(int i = 0; i < lower_size_bound; i++){
		mat[i].resize(num_cols);
	}

	mat.resize(num_rows, row_ind(num_cols)); 
}

void insertPhrase(Dynamic_File_List* p_DFL, list_attribs* p_LA, std::stack<Operation> * op_stack){
	int column_index = p_LA->c_first - 1;
	row_node* index;
	if(p_LA->r_first == 1) index = p_DFL->root.next;
	else index = p_DFL->getRowAt(p_LA->r_first);
	//UNDO/REDO
	Operation input_op;
	setOperationAttribs(&input_op, -1, -1, -1, -1, -1, false); //end undo-chain for this operation
	op_stack->push(input_op);
	//END UNDO/REDO
	for(int i = p_LA->r_first; i <= p_LA->r_last; i++){ //for each row i from r_first to r_last
		if(index->s.size() < p_LA->c_first) {
			int diff = p_LA->c_first - index->s.size();			
			for(int j = 0; j < diff-1; j++){
				p_DFL->appendToEndOfRow(index, "");
			}

			//UNDO/REDO
			if(diff > 0){
				Operation input_op;
				setOperationAttribs(&input_op, INSERT, (p_LA->c_first - diff)+1, column_index, i, i, true);
				input_op.data.push_back("");
				op_stack->push(input_op);
			}
			//END UNDO/REDO
			p_DFL->appendToEndOfRow(index, p_LA->phrase);
			//p_DFL->insertItemAtRowPos(index, p_LA->phrase, column_index);

			//UNDO/REDO
			Operation input_op;
			setOperationAttribs(&input_op, INSERT, column_index+1, column_index+1, i, i, true);
			input_op.data.push_back(p_LA->phrase);
			op_stack->push(input_op);
			//END UNDO/REDO
		}
		else if(index->s.size() > p_LA->c_first){
			p_DFL->insertItemAtRowPos(index, p_LA->phrase, column_index);
			//UNDO/REDO		
			Operation input_op;
			setOperationAttribs(&input_op, INSERT, column_index+1, column_index+1, i, i, true);
			input_op.data.push_back(p_LA->phrase);
			op_stack->push(input_op);
			//END UNDO/REDO
		}
		else { 
			if(index->s[column_index]=="") index->s[column_index].replace(index->s[column_index].begin(), index->s[column_index].end(), p_LA->phrase);
			else p_DFL->insertItemAtRowPos(index, p_LA->phrase, column_index);

			//UNDO/REDO		
			Operation input_op;
			setOperationAttribs(&input_op, INSERT, column_index+1, column_index+1, i, i, true);
			input_op.data.push_back(p_LA->phrase);
			op_stack->push(input_op);
			//END UNDO/REDO
		}
		index = index->next;
	}
	p_LA->num_cols = p_DFL->getNumCols();
}

void splitColAtPos(Dynamic_File_List* p_DFL, list_attribs* p_LA, int pos, std::stack<Operation> * op_stack){
	row_node* ind_ptr = p_DFL->getRowAt(p_LA->r_first);
	int col_pos = p_LA->c_first;
	//UNDO/REDO
	Operation input_op;
	setOperationAttribs(&input_op, -1, -1, -1, -1, -1, false);
	op_stack->push(input_op);
	//END UNDO/REDO
	while(ind_ptr!=NULL && ind_ptr->index <= p_LA->r_last){
		//if(ind_ptr->index > p_LA->r_last) break;
		int len = strlen(ind_ptr->s[col_pos-1].c_str());
		if(len < pos ) pos = len;		
		std::string tmp_str_latter = ind_ptr->s[col_pos-1].substr(pos, len-pos);
		std::string tmp_str_former = ind_ptr->s[col_pos-1].substr(0, pos);		
		p_DFL->insertItemAtRowPos(ind_ptr, tmp_str_former, col_pos-1);
		ind_ptr->s[col_pos].replace(ind_ptr->s[col_pos].begin(), ind_ptr->s[col_pos].end(), tmp_str_latter);
		//UNDO/REDO
		Operation input_op;
		setOperationAttribs(&input_op, SPLIT_COLUMNS, col_pos, col_pos+1, ind_ptr->index, ind_ptr->index, true);
		input_op.data.push_back(tmp_str_former); input_op.data.push_back(tmp_str_latter);
		op_stack->push(input_op);
		//END UNDO/REDO
		ind_ptr = ind_ptr->next;
	}
}

//combines column at position c_first with column at position x+1 (using c_last)
void combineColumns(Dynamic_File_List* p_DFL, list_attribs* p_LA, std::stack<Operation> * op_stack){
	row_node* ind_ptr = p_DFL->getRowAt(p_LA->r_first);
	int col_pos = p_LA->c_first;
	int col_pos_plus_one = col_pos+1;
	//UNDO/REDO
	Operation input_op;
	setOperationAttribs(&input_op, -1, -1, -1, -1, -1, false);
	op_stack->push(input_op);
	//END UNDO/REDO
	while(ind_ptr!=NULL && ind_ptr->index <= p_LA->r_last){
		//UNDO/REDO
		Operation input_op;
		setOperationAttribs(&input_op, COMBINE_COLUMNS, col_pos, (int)strlen(ind_ptr->s[col_pos-1].c_str()), ind_ptr->index, ind_ptr->index, true);
		input_op.data.push_back(ind_ptr->s[col_pos-1]);
		input_op.data.push_back(ind_ptr->s[col_pos]);
		op_stack->push(input_op);
		//END UNDO/REDO
		std::string tmp = ind_ptr->s[col_pos-1] + ind_ptr->s[col_pos_plus_one-1];
		ind_ptr->s[col_pos-1].replace(ind_ptr->s[col_pos-1].begin(), ind_ptr->s[col_pos-1].end(), tmp);
		p_DFL->removeItemAtRowPos(ind_ptr, col_pos_plus_one);
		ind_ptr = ind_ptr->next;
	}
} 

void insertIterator(Dynamic_File_List* p_DFL, list_attribs* p_LA, std::stack<Operation> * op_stack){
	int column_index = p_LA->c_first - 1;
	row_node* index;
	if(p_LA->r_first == 1) index = p_DFL->root.next;
	else index = p_DFL->getRowAt(p_LA->r_first);
	//UNDO/REDO
	Operation input_op;
	setOperationAttribs(&input_op, -1, -1, -1, -1, -1, false);
	op_stack->push(input_op);
	//UNDO/REDO END 
	int iter = 0;
	for(int i = p_LA->r_first; i <= p_LA->r_last; i++){
		iter++;
		if(index->s.size() < p_LA->c_first) {
			int diff = p_LA->c_first - index->s.size();			
			for(int j = 0; j < diff-1; j++){
				p_DFL->appendToEndOfRow(index, "");
			}
			//UNDO/REDO
			if(diff > 0){
				Operation input_op;
				setOperationAttribs(&input_op, ITERATOR, (p_LA->c_first - diff), column_index, i, i, true);
				input_op.data.push_back("");				
				op_stack->push(input_op);
			}			
			//END UNDO/REDO
			std::string tmp_str = std::to_string(iter);
			p_DFL->insertItemAtRowPos(index, tmp_str, column_index);
			//UNDO/REDO
			Operation input_op;
			setOperationAttribs(&input_op, ITERATOR, column_index+1, column_index+1, i, i, true);
			input_op.data.push_back(tmp_str);
			op_stack->push(input_op);
			//END UNDO/REDO
		}
		else if(index->s.size() > p_LA->c_first){
			p_DFL->insertItemAtRowPos(index, std::to_string(iter), column_index);
			//UNDO/REDO		
			Operation input_op;
			setOperationAttribs(&input_op, ITERATOR, column_index+1, column_index+1, i, i, true);
			input_op.data.push_back(std::to_string(iter));
			op_stack->push(input_op);
			//END UNDO/REDO
		}
		else {
			if(index->s[column_index]=="") index->s[column_index].replace(index->s[column_index].begin(), index->s[column_index].end(), std::to_string(iter));
			else p_DFL->insertItemAtRowPos(index, std::to_string(iter), column_index);
			//UNDO/REDO
			Operation input_op;
			setOperationAttribs(&input_op, ITERATOR, column_index+1, column_index+1, i, i, true);
			input_op.data.push_back(std::to_string(iter));
			op_stack->push(input_op);
			//END UNDO/REDO
		}		
		index = index->next;
	}	
	p_LA->num_cols = p_DFL->getNumCols();
}

void deleteRowItems(Dynamic_File_List* p_DFL, list_attribs* p_LA, std::stack<Operation> * op_stack){
	
	int row_first, row_last, col_first, col_last, cur_col_count, max_col_count;
	row_first = p_LA->r_first; row_last = p_LA->r_last;	col_first = p_LA->c_first; col_last = p_LA->c_last;
	cur_col_count = p_DFL->getNumCols();

	row_node* index;
	index = p_DFL->getRowAt(row_first);

	//UNDO/REDO
	Operation input_op;
	setOperationAttribs(&input_op, -1, -1, -1, -1, -1, false);
	op_stack->push(input_op);
	//END UNDO/REDO

	while(index!=NULL && index->index<=row_last){
		//UNDO/REDO
		Operation input_op;
		setOperationAttribs(&input_op, ERASE, col_first, col_last, index->index, index->index, true);
		//END UNDO/REDO
		for(int i = col_last; i >= col_first; i--){
			//UNDO/REDO
			input_op.data.push_back(index->s[i-1]);
			//END UNDO/REDO
			if(index->s.size() >= i){
				if(i == 1 && index->s.size() == 1){
					index->s[i-1].replace(index->s[i-1].begin(), index->s[i-1].end(), "");
				}
				else {
					p_DFL->removeItemAtRowPos(index, i);
				}
			}
		}
		//UNDO/REDO
		op_stack->push(input_op);
		//END UNDO/REDO		
		index = index->next;
	}	

	//re-calculate column count
	max_col_count = p_DFL->getMaxColCount();
	cur_col_count = p_DFL->getNumCols();
	if(max_col_count != cur_col_count) p_DFL->setNumCols(max_col_count);
}

void removeRowFromDFL(Dynamic_File_List* p_DFL, list_attribs* p_LA, std::stack<Operation> * op_stack){
	int row_first = p_LA->r_first;
	int dif = p_LA->r_last - row_first;
	
	Operation input_op;
	setOperationAttribs(&input_op, -1, -1, -1, -1, -1, false);
	op_stack->push(input_op);

	row_node* row_ptr;
	
	for(int i = 0; i <= dif; i++){ 
		row_ptr = p_DFL->getRowAt(row_first);
		setOperationAttribs(&input_op, DELETE_ROWS, 0, row_ptr->id, row_first, row_first + i, true); //row_first + i gives relative final position
		input_op.data.clear();
		input_op.data = row_ptr->s;
		if((int)row_ptr->ext.size() > 0) input_op.data.push_back(row_ptr->ext[0]);
		else input_op.data.push_back("");
		op_stack->push(input_op);
		p_DFL->deleteRow(row_first);
	}
}

void insertRowsFromStack(Dynamic_File_List* p_DFL, list_attribs* p_LA, std::stack<Operation> * op_stack, std::vector<std::string> * row_values){
	row_node new_row;
	new_row.s.clear();
	std::vector<std::string> tmp_vec;
	tmp_vec.clear();
	tmp_vec = *row_values; 	//ROW ENTRIES
	std::string tmp_str; 	//EXT
	tmp_str.clear();
	tmp_str = tmp_vec[tmp_vec.size()-1];
	tmp_vec.resize(tmp_vec.size() - 1);

	new_row.s = tmp_vec;
	new_row.ext.push_back(tmp_str);
	new_row.id = p_LA->c_last;

	Operation input_op;
	setOperationAttribs(&input_op, NEW_ROW, 0, new_row.id, p_LA->r_first, p_LA->r_first, false);
	op_stack->push(input_op);

	p_DFL->insertRowAtPos(&new_row, p_LA->r_first);
}

void saveAllChanges(Dynamic_File_List* p_DFL, list_attribs* p_LA){
	row_node* ind_ptr = p_DFL->root.next;
	while(ind_ptr!=NULL){
		std::string old_name;
		std::string new_name;
		old_name = p_LA->file_paths[ind_ptr->id];
		new_name = p_LA->full_path_prefix;
		for(int i = 0; i < (int)ind_ptr->s.size(); i++){
			new_name = new_name + ind_ptr->s[i];
		}
		if(ind_ptr->ext[0] != "") new_name = new_name + "." + ind_ptr->ext[0];
		int result;
		result = rename(old_name.c_str(), new_name.c_str());
		ind_ptr = ind_ptr->next;
	}
}

void clearStack(std::stack<Operation> * op_stack){
	while(!op_stack->empty()) op_stack->pop();
}

void undo_StackHandler(std::stack<Operation> * undo_stack, std::stack<Operation> * redo_stack, Dynamic_File_List * p_DFL, list_attribs * p_LA){
	//going to have to store and restore p_LA values needed for each operation
	int old_r_first = p_LA->r_first;
	int old_r_last 	= p_LA->r_last;
	int old_c_first = p_LA->c_first;
	int old_c_last 	= p_LA->c_last;
	int old_targ_row = p_LA->move_rows_target;
	std::string old_phrase = p_LA->phrase;

	//all backup data stored
	Operation curr_op;
	int row_first, row_last, col_first, col_last, op_code;
	std::vector<std::string> column_values;

	bool isReplaceOp = false; //indicates a replace operation is in progress when true
	bool keepGoing = false;
	while(!undo_stack->empty()){
		curr_op = undo_stack->top();
		

		//get temp values for undo operation
		column_values.clear();
		column_values = curr_op.data;
		row_first = curr_op.row_x;
		row_last = curr_op.row_y;
		col_first = curr_op.col_x;
		col_last = curr_op.col_y;
		op_code = curr_op.op_code;
		//end get temp vals
		p_LA->r_first = row_first;
		p_LA->r_last = row_last;
		p_LA->c_first = col_first;
		p_LA->c_last = col_last;
		p_LA->move_rows_target = col_first;

		if(op_code!=(-1)){
			switch(op_code){
				case UNDO:
				case REDO:
					(curr_op.check_next == true) ? keepGoing = true : keepGoing = false;
					break;
				case INSERT:
					p_LA->phrase = column_values[0];
					deleteRowItems(p_DFL, p_LA, redo_stack);
					break;
				case DELETE_ROWS: //reverse deletion of rows
					insertRowsFromStack(p_DFL, p_LA, redo_stack, &column_values);
					break;
				case MOVE_ROWS: //need to explicitly push to redo stack for move
					{
					int move_row_first;
					int move_row_last;
					int move_target;
					if(col_first < row_first){
						move_row_first = col_first;
						move_row_last = col_first + (row_last - row_first);
						move_target = row_last;
					}
					else if (col_first > row_last){
						move_row_last = col_first;
						move_row_first = col_first - (row_last - row_first);
						move_target = row_first;
					}
					p_DFL->moveRowsToPos(move_target, move_row_first, move_row_last);
					//UNDO/REDO
					Operation input_op;
					setOperationAttribs(&input_op, MOVE_ROWS, move_target, move_target, move_row_first, move_row_last, false);
					redo_stack->push(input_op);
					//END UNDO/REDO
					break;
					}
				case REPLACE:
					if(curr_op.check_next == true){ 
						isReplaceOp = true;
						Operation input_op;
						setOperationAttribs(&input_op, REPLACE, -1, -1, -1, -1, false);
						redo_stack->push(input_op);
					}
					else{
						isReplaceOp = false;
						Operation input_op;
						setOperationAttribs(&input_op, REPLACE, 0, 0, 0, 0, true);
						redo_stack->push(input_op);	
					}
					break;
				case SPLIT_COLUMNS:
					combineColumns(p_DFL, p_LA, redo_stack);
					break;
				case ERASE:
					for(int i = 0; i < (int)column_values.size(); i++){
						p_LA->phrase = column_values[i];
						insertPhrase(p_DFL, p_LA, redo_stack);
					}				
					break;
				case ITERATOR:
					p_LA->phrase = column_values[0];
					deleteRowItems(p_DFL, p_LA, redo_stack);
					break;
				case COMBINE_COLUMNS:
					splitColAtPos(p_DFL, p_LA, col_last, redo_stack);
					break;
				case NEW_ROW:
					removeRowFromDFL(p_DFL, p_LA, redo_stack);
					break;
			}	
		}

		if(curr_op.check_next == false && (isReplaceOp == false && keepGoing == false) ){ 
			undo_stack->pop();
			break;
		}
		undo_stack->pop();
	}
	
	p_LA->r_first = old_r_first;
	p_LA->r_last = old_r_last;
	p_LA->c_first = old_c_first;
	p_LA->c_last = old_c_last;
	p_LA->move_rows_target = old_targ_row;
	p_LA->phrase = old_phrase;

	if(p_DFL->getMaxColCount() != p_DFL->getNumCols()) p_DFL->setNumCols(p_DFL->getMaxColCount());

	p_LA->num_rows = p_DFL->getNumRows();
	p_LA->num_cols = p_DFL->getNumCols();
	(p_LA->num_rows > 0) ? p_LA->r_first = 1 : p_LA->r_first = 0;
	p_LA->r_last = p_DFL->getNumRows();
	(p_LA->num_cols > 0) ? p_LA->c_first = 1 : p_LA->c_first = 0; 
	p_LA->c_last = p_DFL->getNumCols();
}

bool confirmOperationSwitch(list_attribs* p_LA, Dynamic_File_List* p_DFL, std::stack<Operation> * undo_stack){
	//DUMP REDO STACK HERE
	bool isValid = false;

	switch(p_LA->cur_state){
		case INSERT:{
			if(p_LA->isPhraseSet == false) break;
			p_LA->isPhraseSet = false;
			insertPhrase(p_DFL, p_LA, undo_stack);
			resetListAttribs(p_LA, p_DFL);			
			isValid = true;
			break;
		}

		case REPLACE:{
			if(p_LA->isPhraseSet == false) break;
			p_LA->isPhraseSet = false;
			//UNDO/REDO
			Operation input_op;
			setOperationAttribs(&input_op, REPLACE, -1, -1, -1, -1, false);
			undo_stack->push(input_op);
			//END UNDO/REDO
			deleteRowItems(p_DFL, p_LA, undo_stack);
			insertPhrase(p_DFL, p_LA, undo_stack);
			//UNDO/REDO
			setOperationAttribs(&input_op, REPLACE, 0, 0, 0, 0, true);
			undo_stack->push(input_op);
			//END UNDO/REDO
			resetListAttribs(p_LA, p_DFL);
			isValid = true;
			break;
		}

		case ERASE:{			
			deleteRowItems(p_DFL, p_LA, undo_stack);
			resetListAttribs(p_LA, p_DFL);
			isValid = true;
			break;
		}

		case DELETE_ROWS:{
			removeRowFromDFL(p_DFL, p_LA, undo_stack);
			resetListAttribs(p_LA, p_DFL);
			isValid = true;
			break;
		}

		case MOVE_ROWS:{
			p_DFL->moveRowsToPos(p_LA->move_rows_target, p_LA->r_first, p_LA->r_last);
			//UNDO/REDO
			Operation input_op;
			setOperationAttribs(&input_op, MOVE_ROWS, p_LA->move_rows_target, p_LA->move_rows_target, p_LA->r_first, p_LA->r_last, false);
			undo_stack->push(input_op);
			//END UNDO/REDO
			resetListAttribs(p_LA, p_DFL);
			isValid = true;
			break;
		}

		case SAVE_CHANGES:{ 
			saveAllChanges(p_DFL, p_LA);
			isValid = true;
			clearStack(undo_stack);
			/*
			**I will need to clear the undo stack here
			**and delete all rows
			**and reset all list attributes
			**as saved changes cannot be undone
			**and the files referenced are now the new saved files
			**so they must be reloaded if more changes are to be made
			*/
			break;
		}

		case SPLIT_COLUMNS:{
			splitColAtPos(p_DFL, p_LA, p_LA->c_last, undo_stack);
			resetListAttribs(p_LA, p_DFL);
			isValid = true;
			break;
		}
	}
	return isValid;
}

void keyvalInterpreter(int key_val, list_attribs* p_LA, Dynamic_File_List* p_DFL, std::stack<Operation> * undo_stack, std::stack<Operation> * redo_stack){
	switch(key_val){
		case ESC:
			clearStack(undo_stack);
			clearStack(redo_stack);
			clear_screen(); //CONSOLE
			printf("\n"); //CONSOLE
			break;
		case UP: 
			if(p_LA->cur_state != MAIN_MENU && p_LA->cur_state != INIT){
				if(p_LA->r_first > 1) p_LA->r_first--;
			}
			break;
		case DOWN:
			if(p_LA->cur_state != MAIN_MENU && p_LA->cur_state != INIT){
				if(p_LA->r_first < p_LA->num_rows){
					if(p_LA->r_first < p_LA->r_last) p_LA->r_first++;
					else {p_LA->r_first++; p_LA->r_last++;}
				}
			}
			break;
		case LEFT:
			if(p_LA->cur_state == INSERT || p_LA->cur_state == REPLACE || p_LA->cur_state == SPLIT_COLUMNS || p_LA->cur_state == ERASE){
				if(p_LA->c_first > 1) p_LA->c_first--;
			}
			else if(p_LA->cur_state == MOVE_ROWS){
				if(p_LA->move_rows_target > 1) p_LA->move_rows_target--;
			}
			break;
		case RIGHT:
			if(p_LA->cur_state == INSERT || p_LA->cur_state == REPLACE || p_LA->cur_state == ERASE){
				if(p_LA->c_first < p_LA->num_cols){
					if(p_LA->c_first < p_LA->c_last) p_LA->c_first++;
					else {p_LA->c_first++; p_LA->c_last++;}
				}
			}
			else if(p_LA->cur_state == MOVE_ROWS){
				if(p_LA->move_rows_target < p_LA->num_rows) p_LA->move_rows_target++;	
			}
			else if(p_LA->cur_state == SPLIT_COLUMNS){
				if(p_LA->c_first < p_LA->num_cols) p_LA->c_first++;
			}
			break;
		case SHIFT_UP:
			if(p_LA->cur_state != MAIN_MENU && p_LA->cur_state != INIT){
				if(p_LA->r_last > 1){
					if(p_LA->r_last > p_LA->r_first) p_LA->r_last--;
					else {p_LA->r_first--; p_LA->r_last--;}
				}
			}
			break;
		case SHIFT_DOWN:
			if(p_LA->cur_state != MAIN_MENU && p_LA->cur_state != INIT){
				if(p_LA->r_last < p_LA->num_rows) p_LA->r_last++;
			}
			break;
		case SHIFT_LEFT:
			if( (p_LA->cur_state != MAIN_MENU && p_LA->cur_state != INIT) && p_LA->cur_state != SPLIT_COLUMNS){
				if(p_LA->c_last > 1){
					if(p_LA->c_last > p_LA->c_first) p_LA->c_last--;
					else {p_LA->c_last--; p_LA->c_first--;}
				} 
			}
			else if(p_LA->cur_state == SPLIT_COLUMNS){
				if(p_LA->c_last > 1) p_LA->c_last--;
			}
			break;
		case SHIFT_RIGHT:
			if( (p_LA->cur_state != MAIN_MENU && p_LA->cur_state != INIT) && p_LA->cur_state != SPLIT_COLUMNS){
				if(p_LA->c_last < p_LA->num_cols) p_LA->c_last++;
			}
			else if(p_LA->cur_state == SPLIT_COLUMNS){
				if(p_LA->c_last < 260) p_LA->c_last++; //soft limit, will revert to size of entry if larger than
			}
			break;
		case B: //Back
			if(p_LA->cur_state != MAIN_MENU && p_LA->cur_state != INIT){
				p_LA->last_state = p_LA->cur_state;
				p_LA->cur_state = MAIN_MENU;
				console_clearAndPrint(p_LA, p_DFL);
			}
			break;
		case C: //Confirm for various states
			if(confirmOperationSwitch(p_LA, p_DFL, undo_stack)){
				//clear redo stack here
				clearStack(redo_stack);
				p_LA->last_state = p_LA->cur_state;
				p_LA->cur_state = MAIN_MENU;
				console_clearAndPrint(p_LA, p_DFL);
			}			
			break;
		case D: //set state to delete rows
			if(p_LA->cur_state == MAIN_MENU){
				p_LA->last_state = MAIN_MENU;
				p_LA->cur_state = DELETE_ROWS;
				console_clearAndPrint(p_LA, p_DFL);
			}
			break;
		case E:
			if(p_LA->cur_state == MAIN_MENU){
				p_LA->last_state = MAIN_MENU;
				p_LA->cur_state = ERASE;
				console_clearAndPrint(p_LA, p_DFL);
			}
			break;
		case I: //Insert/Iterator //SOME OF THIS SHOULD BE IN confirm FUNCTION
			if(p_LA->cur_state == MAIN_MENU) { //move to insert
				p_LA->last_state = MAIN_MENU;
				p_LA->cur_state = INSERT;
				console_clearAndPrint(p_LA, p_DFL);
			}
			else if(p_LA->cur_state == INSERT) { //move to iterator
				p_LA->last_state = INSERT;
				p_LA->cur_state = MAIN_MENU;
				insertIterator(p_DFL, p_LA, undo_stack);
				clearStack(redo_stack);
				console_clearAndPrint(p_LA, p_DFL);
			}
			else if(p_LA->cur_state == REPLACE){
				p_LA->last_state = REPLACE;
				p_LA->cur_state = MAIN_MENU;
				//UNDO/REDO
				Operation input_op;
				setOperationAttribs(&input_op, REPLACE, -1, -1, -1, -1, false);
				undo_stack->push(input_op);
				//END UNDO/REDO
				deleteRowItems(p_DFL, p_LA, undo_stack);
				insertIterator(p_DFL, p_LA, undo_stack);
				//UNDO/REDO
				setOperationAttribs(&input_op, REPLACE, 0, 0, 0, 0, true);
				undo_stack->push(input_op);
				//END UNDO/REDO
				clearStack(redo_stack);
				resetListAttribs(p_LA, p_DFL);
				console_clearAndPrint(p_LA, p_DFL);
			}
			break;
		case L: //For loading new file data
			if(p_LA->cur_state == MAIN_MENU) {
				p_LA->last_state = MAIN_MENU;
				p_LA->cur_state = EXIT_RELOAD;
				clearStack(undo_stack);
				clearStack(redo_stack);
				tearDown(p_DFL);				
				init();
			}
			break;
		case M: //mode
			if(p_LA->cur_state == MAIN_MENU){
				p_LA->last_state = p_LA->cur_state;
				p_LA->cur_state = MOVE_ROWS;
				console_clearAndPrint(p_LA, p_DFL);
			}
			break;
		case N: //deprecated, get rid of
			if(redo_stack->size() > 0){ 
				Operation input_op;
				setOperationAttribs(&input_op, REDO, -1, -1, -1, -1, false);
				undo_stack->push(input_op);				
				undo_StackHandler(redo_stack, undo_stack, p_DFL, p_LA);
				setOperationAttribs(&input_op, REDO, 0, 0, 0, 0, true);
				undo_stack->push(input_op);
				console_clearAndPrint(p_LA, p_DFL);
			}
			break;
		case R:
			if(p_LA->cur_state == MAIN_MENU){
				p_LA->last_state = MAIN_MENU;
				p_LA->cur_state = REPLACE;
				console_clearAndPrint(p_LA, p_DFL);
			}
			break;
		case S: //save/set
			if(p_LA->cur_state == MAIN_MENU){
				p_LA->last_state = p_LA->cur_state;
				p_LA->cur_state = SAVE_CHANGES;
				console_clearAndPrint(p_LA, p_DFL);
			}
			else if(p_LA->cur_state == INSERT || p_LA->cur_state == REPLACE){
				p_LA->last_state = p_LA->cur_state;
				p_LA->cur_state = SET_PHRASE;
				console_clearAndPrint(p_LA, p_DFL);
				p_LA->cur_state = p_LA->last_state;
				p_LA->last_state = SET_PHRASE;
				console_clearAndPrint(p_LA, p_DFL);
			}
			break;
		case U: //Pop undo if able
			if(undo_stack->size() > 0){ 
				Operation input_op;
				setOperationAttribs(&input_op, UNDO, -1, -1, -1, -1, false);
				redo_stack->push(input_op);
				undo_StackHandler(undo_stack, redo_stack, p_DFL, p_LA);
				setOperationAttribs(&input_op, UNDO, 0, 0, 0, 0, true);
				redo_stack->push(input_op);
				console_clearAndPrint(p_LA, p_DFL);
			}
			break;
		case W: //DEBUG KEY TO FLUSH STACKS TO SCREEN
			{
			#ifdef DEBUG
			printf("###UNDO STACK CONTENTS###\n");	
			int stack_size = undo_stack->size();
			for(int i = 0; i < stack_size; i++){
				Operation top_op = undo_stack->top();
				debugPrintAllParams(&top_op);
				undo_stack->pop();
			}
			#endif //DEBUG
			#ifdef DEBUG
			printf("###REDO STACK CONTENTS###\n");	
			stack_size = redo_stack->size();
			for(int i = 0; i < stack_size; i++){
				Operation top_op = redo_stack->top();
				debugPrintAllParams(&top_op);
				redo_stack->pop();
			}
			#endif //DEBUG
			}
			break;
		case X: //Split columns state
			if(p_LA->cur_state == MAIN_MENU){
				p_LA->last_state = p_LA->cur_state;
				p_LA->cur_state = SPLIT_COLUMNS;
				console_clearAndPrint(p_LA, p_DFL);
			}
			break;

	}
}

void handler_loop(Dynamic_File_List* p_DFL, list_attribs* p_LA, std::stack<Operation> * undo_stack, std::stack<Operation> * redo_stack){
	clear_screen();
	console_printDFL(p_DFL);
	int ret_val = B; //set to avoid warnings
	printf("\n");
	console_printMenuState(p_LA, p_DFL); //CONSOLE
	while(ret_val!=ESC && p_LA->cur_state != EXIT_RELOAD){
		ret_val = main_loop(); //in keypress_handler.cpp
		keyvalInterpreter(ret_val, p_LA, p_DFL, undo_stack, redo_stack);
		console_printRangeSelectionState(p_LA);
	}
	printf("\n");
}

void setInitValues(list_attribs* p_LA, Dynamic_File_List* p_DFL){
	p_LA->num_rows = p_DFL->getNumRows();
	p_LA->num_cols = p_DFL->getNumCols();
	p_LA->isPhraseSet = false;
	p_LA->isPhrSetReq = false;
	p_LA->cur_state = MAIN_MENU;
	p_LA->r_first = 1;
	p_LA->r_last = p_LA->num_rows;
	p_LA->c_first = 1;
	p_LA->c_last = p_LA->num_cols;
	p_LA->move_rows_target = 1;	
}	

void list_setup(Dynamic_File_List* p_DFL, list_attribs* p_LA, std::stack<Operation> * undo_stack, std::stack<Operation> * redo_stack){
	
	size_t num_elems = p_LA->file_paths.size(); //number of files for this call
	size_t sub_string_index;

	std::vector<std::string> file_listing;
	file_listing.clear();

	for(int i = 0; i < num_elems; ++i){
		sub_string_index = p_LA->file_paths[i].rfind(slash);
		if(sub_string_index!=std::string::npos){			
			std::string tmp = p_LA->file_paths[i].substr(sub_string_index+1, ((int)p_LA->file_paths[i].size() - sub_string_index));
			file_listing.push_back(tmp);
		} 
	}

	p_LA->full_path_prefix.clear();
	p_LA->full_path_prefix = p_LA->file_paths[num_elems-1].substr(0, sub_string_index+1);


	//instantiate matrix
	mat_col_ind mat_i;
	mat_i.clear();

	resize_matrix(mat_i, file_listing.size(), 0);

	char inp_delims[] = ".";
	std::string tmp;
   	std::string tmp_ext;
 
	for(int i = 0; i < file_listing.size(); i++){
		token_arr token_file_fixs = token_arr_init(file_listing[i].c_str(), inp_delims);		
		while(*next_token( &token_file_fixs )){
			mat_i[i].push_back(token_file_fixs.output);		
   		}

   		if((int)mat_i[i].size() > 2){
   			for(int j = 0; j < ((int)mat_i[i].size() - 1); j++){
				if(tmp=="") tmp = mat_i[i][j];
				else tmp = tmp + "." + mat_i[i][j];
			}
			tmp_ext = mat_i[i][(int)mat_i[i].size()-1];
			mat_i[i].clear();
			mat_i[i].push_back(tmp);
			mat_i[i].push_back(tmp_ext);
   		}
   		else if((int)mat_i[i].size() == 1){
   			mat_i[i].push_back("");
   		} 
   		free_token_arr( &token_file_fixs );
	}

	//Populate DFL from file_listing matrix
	//NOTE: This currently cannot handle there being 0 files chosen
	row_node* p_newrow;
	for(int i = 0; i < file_listing.size(); i++){
		p_newrow = p_DFL->insertAtEnd(mat_i[i][0], i);
		p_newrow->ext.push_back(mat_i[i][1]);
	}

	setInitValues(p_LA, p_DFL);

	handler_loop(p_DFL, p_LA, undo_stack, redo_stack);
}

void tearDown(Dynamic_File_List *p_DFL){ //no memory leaks, but delete still freeing more than necessary
	clear_screen();
	//p_DFL->deleteAll();
	//delete p_DFL;
}

void init(){
	char inp_delims[] = "|"; //for delimiting multiple inputs
	char const * lTheOpenFileName;
		
	lTheOpenFileName = tinyfd_openFileDialog ( "Select Files to Alter",	NULL,	0, NULL, NULL, 1);
	if(lTheOpenFileName == NULL) exit(0);
	
	list_attribs LA;
	list_attribs* p_LA = &LA;
	
    std::vector<std::string> multi_file_vec;
    multi_file_vec.clear();
    p_LA->file_paths.clear();
	
    token_arr aggr_file_names_token = token_arr_init(lTheOpenFileName, inp_delims);
	
    while(*next_token( &aggr_file_names_token )){
    	p_LA->file_paths.push_back(aggr_file_names_token.output);
    }
	
	free_token_arr( &aggr_file_names_token );
	
	std::stack<Operation> undo_stack;
	std::stack<Operation> redo_stack;
	
	Dynamic_File_List DFL;
	Dynamic_File_List* p_DFL = &DFL;
	
	list_setup(p_DFL, p_LA, &undo_stack, &redo_stack);	
}

int main(int argc, char** argv){
	#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	if(!CoInitializeEx(NULL, COINIT_APARTMENTTHREADED) == S_OK) exit(0);
	#endif
	init();
}

#pragma warning(default:4996)
