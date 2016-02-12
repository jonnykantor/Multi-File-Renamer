/* Code written by: Jonathan Kantor, Jan 4, 2016
** The Dynamic_File_List class is my first attempt at a data structure for a larger project - the batch file re-namer.
** The purpose of this class is to provide a linked-list where each node is a struct containing a std::string vector
** for the full file name, a std::string for the extension, an index that indicates current row position, and an id
** that is set when the node is created, and corresponds to the vector holding the original file path/name.
**
** I don't expect this will ever be used commercially by myself (or anyone else). If anyone wants to use it for non-
** commercial means go right ahead; I don't require permission for use of this code in any endeavour but would like 
** some acknowledgement if it is used, thanks.
*/

#include <stdio.h>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstring>
#include "Dynamic_File_List.h"


#define DEBUG

Dynamic_File_List::Dynamic_File_List()
{
	root.next = NULL;
	root.index = 0;
	root.s.push_back("FNAME");
	root.ext.push_back("EXT");

	n_rows = 0;
	n_cols = 1;
	end = &root;
}

int Dynamic_File_List::getMaxColCount(){
	row_node* tmp = root.next;
	int max_col_count = 0;
	while(tmp!=NULL){
		size_t cur_row_size = tmp->s.size();
		if((int)cur_row_size > max_col_count) max_col_count = (int)cur_row_size; 
		tmp = tmp->next;
	}
	return max_col_count;
}

int Dynamic_File_List::getNumRows(){
	return n_rows;
}

int Dynamic_File_List::getNumCols(){
	return n_cols;
}

void Dynamic_File_List::setNumCols(int cols_to_set){
	n_cols = cols_to_set;
}

void Dynamic_File_List::setNumRows(int rows_to_set){
	n_rows = rows_to_set;
}

row_node* Dynamic_File_List::insertRow( row_node ** root_node_ptr, std::string input_str, int file_id){
	row_node * newNode = new row_node();

	newNode->s.push_back(input_str);
	newNode->next = NULL;
	newNode->id = file_id;

	if(*root_node_ptr == NULL) {
		newNode->index = 1;
		*root_node_ptr = newNode;
		n_rows++;
		end = newNode;
		return end;
	}
	else{
		row_node *tmp = *root_node_ptr;
		while(tmp->next!=NULL){
			tmp = tmp->next;
		}
		tmp->next = newNode;
		tmp->next->index = tmp->index+1;
		n_rows++;
		end = tmp->next;
		return end;
	}
	return NULL;
}

row_node* Dynamic_File_List::insertAtEnd(std::string input_str, int file_id){
	row_node ** end_node_ptr = &end;
	row_node * newNode = new row_node();

	newNode->s.push_back(input_str);
	newNode->next = NULL;
	newNode->id = file_id;

	row_node *tmp = *end_node_ptr;
	if(tmp->next!=NULL) { printf("PROBLEM\n"), exit(1); }

	tmp->next = newNode;
	tmp->next->index = tmp->index+1;
	n_rows++;
	end = tmp->next;
	return end;
}	

row_node* Dynamic_File_List::insertRowAtPos(std::string input_str, int pos, int file_id){
	if(pos > n_rows){
		return insertAtEnd(input_str, file_id);
	}
	else{	
		row_node ** root_node_ptr = &root.next;
		row_node * tmp = *root_node_ptr;
		row_node * newNode = new row_node();
		newNode->s.push_back(input_str);
		newNode->id = file_id;
		if(pos==1){
			
			newNode->next = root.next;
			root.next = newNode;
			tmp = newNode;
			newNode->index = 1;
			tmp->next->index++;
			n_rows++;
			for(int i = pos; i < n_rows; i++){
				tmp = tmp->next;
				if(tmp->next!=NULL) tmp->next->index++;
			}
			return newNode;			
		}
		while(tmp->next!=NULL){
			if(tmp->next->index==pos){
				newNode->next = tmp->next;
				tmp->next = newNode;
				tmp->next->index = tmp->index+1;
				n_rows++;
				for(int i = pos; i < n_rows; i++){
					tmp = tmp->next;
					if(tmp->next!=NULL) tmp->next->index++;
				}
				return newNode;
			}
			else tmp = tmp->next;
		}
	}
}

void Dynamic_File_List::insertRowAtPos(row_node* input_row, int pos){
	if(pos > n_rows) pos = n_rows+1;
	row_node * row_ptr = &root;
	row_node * new_node = new row_node();
	new_node->s.clear();
	new_node->ext.clear();
	new_node->s = input_row->s;
	new_node->id = input_row->id;
	new_node->ext = input_row->ext;
	while(row_ptr!=NULL){
		//if(row_ptr->next == NULL) break;
		if(row_ptr->index == pos-1){
			if(row_ptr->next == NULL){ //append to end of list
				new_node->next = NULL;
				new_node->index = row_ptr->index + 1;
				row_ptr->next = new_node;
				n_rows++;
			}
			else{ //insert in list, bumping row at position forward in list
				new_node->next = row_ptr->next;
				new_node->index = row_ptr->next->index;
				row_ptr->next = new_node;
				n_rows++;
				row_ptr = row_ptr->next;
				for(int i = pos; i < n_rows; i++){
					row_ptr = row_ptr->next;
					row_ptr->index = row_ptr->index+1;
				}
			}
		}
		row_ptr = row_ptr->next;
	}
}

void Dynamic_File_List::swapRows(int a, int b){
	if(a!=b){
		row_node * root_node_ptr = &root;
		row_node * tmp_a = NULL;
		row_node * tmp_b = NULL;
		row_node * tmp = root_node_ptr;

		while(tmp->next!=NULL){ //get corresponding nodes
			if(tmp->next->index == a){
				if(tmp_a==NULL) tmp_a = tmp; //TMP->NEXT IS SWAP TARGET A
			}
			if(tmp->next->index == b){
				if(tmp_b==NULL) tmp_b = tmp; //TMP->NEXT IS SWAP TARGET B
			}
			if(tmp_a!=NULL && tmp_b!=NULL) break;
			tmp = tmp->next;
		}

		if(a > b){ //make sure b-ptr is greater than a-ptr
			tmp = tmp_a;
			tmp_a = tmp_b;
			tmp_b = tmp;
		}

		if(end->index == tmp_b->next->index) { //move end-point (remember to swap indices later)
			end = tmp_a->next;
		}

		int tmp_ind = tmp_a->next->index;
		tmp_a->next->index = tmp_b->next->index;
		tmp_b->next->index = tmp_ind;

		tmp = tmp_a->next->next;
		tmp_a->next->next = tmp_b->next->next;
		tmp_b->next->next = tmp;
		tmp = tmp_a->next;
		tmp_a->next = tmp_b->next;
		tmp_b->next = tmp;
	}
}

void Dynamic_File_List::moveRowsToPos(int target_pos, int first_row_pos, int last_row_pos){
	if(target_pos < first_row_pos || target_pos > last_row_pos){
		row_node* range_begin_less_one = getRowAt(first_row_pos-1);
		row_node* range_begin = getRowAt(first_row_pos);
		row_node* range_end = getRowAt(last_row_pos);
		row_node* insert_pos_node = getRowAt(target_pos);
		row_node* insert_pos_less_one = getRowAt(target_pos-1);

		row_node* tmp_ptr_diff = NULL;
		row_node* tmp_ptr_range = getRowAt(first_row_pos);
		int diff, range;
		diff = 0; range = 0;

		if(target_pos < first_row_pos){
			diff = first_row_pos - target_pos;
			range = (last_row_pos - first_row_pos) + 1;
			tmp_ptr_diff = getRowAt(target_pos);
			for(int i = 0; i < diff; i++){
				tmp_ptr_diff->index = tmp_ptr_diff->index + range;
				tmp_ptr_diff = tmp_ptr_diff->next;
			}
			for(int i = 0; i < range; i++){
				tmp_ptr_range->index = tmp_ptr_range->index - diff;
				tmp_ptr_range = tmp_ptr_range->next;
			}
		}
		else{
			diff = target_pos - last_row_pos;
			range = (last_row_pos - first_row_pos) + 1;
			tmp_ptr_diff = getRowAt(last_row_pos + 1);
			for(int i = 0; i < diff; i++){
				tmp_ptr_diff->index = tmp_ptr_diff->index - range;
				tmp_ptr_diff = tmp_ptr_diff->next;
			}
			for(int i = 0; i < range; i++){
				tmp_ptr_range->index = tmp_ptr_range->index + diff;
				tmp_ptr_range = tmp_ptr_range->next;
			}
		}		

		if(target_pos > last_row_pos){
			range_begin_less_one->next = range_end->next;
			range_end->next = insert_pos_node->next;
			insert_pos_node->next = range_begin;
		}
		else{
			range_begin_less_one->next = range_end->next;
			range_end->next = insert_pos_node;
			insert_pos_less_one->next = range_begin;
		}
	}
}

row_node* Dynamic_File_List::getRowAt(int pos){
	if(pos>n_rows) return NULL;
	if(pos == 0) return &root;
	row_node ** root_node_ptr = &root.next;
	row_node * tmp = *root_node_ptr;
	if(tmp->index == pos) return tmp;
	while(tmp->next!=NULL){
		if(tmp->next->index == pos) return tmp->next;
		else tmp = tmp->next;
	}
	return NULL;
}

void Dynamic_File_List::deleteRow(int pos){
	if((pos>=1) && (pos<=n_rows)){
		row_node * root_node_ptr = &root;
		row_node * for_deletion;
		row_node * tmp = root_node_ptr;
		while(tmp!=NULL){
			if(tmp->next!=NULL){
				if(tmp->next->index == pos){
					for_deletion = tmp->next;
					tmp->next = tmp->next->next;
					for_deletion->next = NULL;
					for_deletion->index = 0;
					for_deletion->s.clear();
					delete for_deletion;
					n_rows--;
				}
				if(tmp->next!=NULL) if(tmp->next->index == tmp->index+2) tmp->next->index = tmp->next->index - 1;
			}
			tmp = tmp->next;
		}
	}
}

void Dynamic_File_List::appendToEndOfRow(row_node * row_ptr, std::string input_str){
	int row_size = row_ptr->s.size();
		if(!(row_size < n_cols)){
			n_cols = row_size+1;
		}
		row_ptr->s.push_back(input_str);
}

void Dynamic_File_List::appendToFrontOfRow(row_node * row_ptr, std::string input_str){
	int row_size = row_ptr->s.size();
		if(!(row_size < n_cols)){
			n_cols = row_size+1;
		}
		row_ptr->s.insert(row_ptr->s.begin(), input_str);
}

void Dynamic_File_List::insertItemAtRowPos(row_node * row_ptr, std::string input_str, int pos){

	int row_size = row_ptr->s.size();
	/*if(row_ptr->s[pos-1]=="") {
		row_ptr->s[pos-1].replace(row_ptr->s[pos-1].begin(), row_ptr->s[pos-1].end(), input_str);
	}
	else{*/
		if(!(row_size < n_cols)){
			n_cols = row_size+1;
		}
		row_ptr->s.insert(row_ptr->s.begin()+pos, input_str);
	//}
}

void Dynamic_File_List::replaceItemAtRowPos(row_node * row_ptr, std::string input_str, int pos){
	int row_size = row_ptr->s.size();
	if(row_size < pos){
		row_ptr->s.resize((size_t)pos, "");
		if(pos > n_cols) n_cols = pos;
		row_ptr->s[pos-1].replace(row_ptr->s[pos-1].begin(), row_ptr->s[pos-1].end(), input_str);
	}
	else(row_ptr->s[pos-1].replace(row_ptr->s[pos-1].begin(), row_ptr->s[pos-1].end(), input_str));
}

void Dynamic_File_List::removeItemAtRowPos(row_node * row_ptr, int pos){
	int cur_row_size = row_ptr->s.size();
	if(!(pos > cur_row_size)){ //remove element
		row_ptr->s.erase(row_ptr->s.begin()+pos-1);
	}		
}

void Dynamic_File_List::deleteAll(){
	row_node* root_node_ptr = &root;
	if(!(root_node_ptr->next==NULL)){
		row_node* tmp = root_node_ptr->next;
		row_node* del = root_node_ptr->next;
		while(tmp->next!=NULL){
			tmp = tmp->next;
			del->s.clear();
			del->ext.clear();
			del->index = 0;
			del->id = 0;
			del->next = NULL;
			delete del;
			del = tmp;
		}
		del->s.clear();
		del->ext.clear();
		del->index = 0;
		del->id = 0;
		del->next = NULL;
		delete del;		
	}	
	n_rows = 0;
	n_cols = 0;
	end = &root;
}

Dynamic_File_List::~Dynamic_File_List(){
	if(root.next!=NULL)deleteAll();
	n_rows = 0;
	n_cols = 0;
	end = NULL;
	root.s.clear();
	root.ext.clear();
	root.index = 0;
	root.id = 0;
	root.next = NULL;
}

/*
int main(){
	Dynamic_File_List DFL;

	DFL.insertAtEnd("Row 1", 1);
	DFL.insertAtEnd("Row 2", 2);
	DFL.insertAtEnd("Row 3", 3);

	row_node new_node;
	new_node.s = {"This", "Is", "A", "Test"};
	new_node.id = 55;
	new_node.ext = {"EXE"};

	DFL.insertRowAtPos(&new_node, 3);
	DFL.deleteRow(4);
	DFL.deleteRow(3);
	DFL.deleteRow(2);
	DFL.deleteRow(1);
	int val_size = 0;


	row_node * row_ptr = &DFL.root;
	while(row_ptr!=NULL){
		val_size = (int)row_ptr->s.size();
		printf("%d, %d\t", row_ptr->index, row_ptr->id);
		for(int i = 0; i < val_size; i++){
			printf("%s\t", row_ptr->s[i].c_str());
		}
		printf("\n");
		row_ptr = row_ptr->next;
	}
}*/
