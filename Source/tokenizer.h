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

typedef struct
{
	const char* source;
	const char* delims;
	const char* output;
} token_arr;

token_arr token_arr_init( const char* , const char* );

const char* next_token ( token_arr* );

void free_token_arr( token_arr* );
