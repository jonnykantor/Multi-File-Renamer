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

typedef struct row_node
{
		std::vector<std::string> s; 	//file name
		std::vector<std::string> ext; 	//extension
		int index; 						//index
		int id;							//set on creation, not to be altered
		row_node *next; 				//ptr
} row_node;

class Dynamic_File_List
{

private:
	int n_rows;
	int n_cols;
	row_node *end; //points to last row

public:	
	row_node root;
	Dynamic_File_List();
	~Dynamic_File_List();
	int getMaxColCount();
	int getNumRows();
	int getNumCols();
	void setNumCols(int);
	void setNumRows(int);
	row_node* insertRow(row_node**, std::string, int);
	row_node* insertAtEnd(std::string, int);
	row_node* insertRowAtPos(std::string, int, int);
	void insertRowAtPos(row_node *, int);
	void appendToEndOfRow(row_node *, std::string);
	void appendToFrontOfRow(row_node *, std::string);
	void insertItemAtRowPos(row_node *, std::string, int);
	void replaceItemAtRowPos(row_node *, std::string, int);
	void removeItemAtRowPos(row_node *, int);
	void swapRows(int, int);
	void moveRowsToPos(int , int, int);
	row_node* getRowAt(int);
	void deleteRow(int);
	void deleteAll();
	void printAll();
};