/* This code was adapted from code written by www.cplusplus.com forums member 
** Duoas here: http://www.cplusplus.com/forum/articles/4860/#msg27665
** It is being used without his permission, however this is unlikely to be
** a commercial product.
**
** Code adapted by Jonathan Kantor, Wed, December 30 2015
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <string>
#include "tokenizer.h"

token_arr token_arr_init( const char* input_str, const char* delimiters){
	token_arr input;
	input.source = input_str;
	input.delims = delimiters;
	input.output = new char[strlen(input_str) + 1]; //strlen does not include null-terminating char

	return input;
}

const char* next_token (token_arr* inp){
	size_t n;

	if(!inp || !inp->source || !inp->delims || !inp->output) return NULL;

	inp->source += strspn(inp->source, inp->delims); //advance past initial delimiters if any
	
 	n = strcspn(inp->source, inp->delims); //length until first delimeter from last advance point
	strncpy((char *)inp->output, inp->source, n);
	*((char*)inp->output + n) = '\0'; //add null terminating character to indicate end of token
	inp->source += n; //advance index for next call

	return inp->output;
}

void free_token_arr(token_arr* inp){
	if(inp){
		if(inp->output){
			delete[] inp->output;
		}
		inp->source = NULL;
		inp->delims = NULL;
		inp->output = NULL;
	}
}