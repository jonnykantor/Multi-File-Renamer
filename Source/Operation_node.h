/*
** The Operation struct defined below is used in conjunction with two std::stack objects to
** implement undo/redo functionality without needing any deep copy of the data at any point.
**
** This code was written by Jonathan Kantor on January 30th, 2016. If you'd like to use it
** for any reason go right ahead; if you do, please acknowledge the usage somewhere.
*/

#include <stack>
#include <cstdio>
#include <vector>
#include <string>
#include <stdexcept>

struct Operation{
	std::vector<std::string> data;
	int op_code, col_x, col_y, row_x, row_y;
	bool check_next;
};

void setOperationAttribs
(Operation * inp_op, int code, int first_col, int last_col, int first_row, int last_row, bool more){
	inp_op->op_code = code;
	inp_op->col_x = first_col;
	inp_op->col_y = last_col;
	inp_op->row_x = first_row;
	inp_op->row_y = last_row;
	inp_op->check_next = more;
}

void debugPrintAllParams(Operation * inp_op){
	size_t data_length = inp_op->data.size();
	for(int i = 0; i < data_length; i++) printf("\t%s", inp_op->data[i].c_str());
	printf("\nOp code: %d, Colx: %d, Coly: %d, Rowx: %d, Rowy: %d\n", inp_op->op_code, inp_op->col_x, inp_op->col_y, inp_op->row_x, inp_op->row_y);
	(inp_op->check_next == true) ? printf("More: true\n") : printf("More: false\n");
}
