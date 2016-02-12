#include <stdio.h>
#include <errno.h>
#include "Key_Press_Handler.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <conio.h>

int keypress_handler(){
	int input_val = _getch();
	if(input_val == 0){
		input_val = _getch();
		switch(input_val){
			case 152: return SHIFT_UP;
			case 160: return SHIFT_DOWN;
			case 157: return SHIFT_RIGHT;
			case 155: return SHIFT_LEFT;
		}
	}
	else if(input_val == 224){
		input_val = _getch();
		switch(input_val){
			case 72: return UP;
			case 80: return DOWN;
			case 77: return RIGHT;
			case 75: return LEFT;
		}
	}
	else{
		switch(input_val){
			case 27: return ESC;
	    case 66: case 98: return B;
      case 67: case 99: return C;
      case 68: case 100: return D;
      case 69: case 101: return E;
      case 73: case 105: return I;
      case 76: case 108: return L;
      case 77: case 109: return M;
      case 78: case 110: return N;
      case 82: case 114: return R;
      case 83: case 115: return S;
      case 85: case 117: return U;
      case 87: case 119: return W;
      case 88: case 120: return X;       		
       	}
	}
	return KEYS_TOTAL;
}

int getKeyVal(){
	int retval = KEYS_TOTAL;
	int wait_for_input = 1;
	while(wait_for_input == 1)
    {
        retval = keypress_handler();
        if(retval != KEYS_TOTAL) wait_for_input = 0;
    }
	return retval;
}

#else //UNIX
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

int keypress_handler(int term_fd, unsigned char * ch){
	switch((int)read(term_fd, ch, sizeof(unsigned char)))
	{
        case 0:
        default:
        	return KEYS_TOTAL;
        case -1:
            fprintf(stdout, "Read error %s", strerror(errno));
            exit(1);
        case 1:       
     		if(*ch==27){
       			char c = getchar();
       			if(c==91){
       		 		switch(getchar()){
       		 			case 49:
       		 				c = getchar();
       		 				if(c==59) c = getchar();
     		 					if(c==51){
     		 						switch(getchar()){
     		 							case 65: return SHIFT_UP;
     		 							case 66: return SHIFT_DOWN;
     		 							case 67: return SHIFT_RIGHT;
     		 							case 68: return SHIFT_LEFT;
     		 						}
     		 					}
       		 			case 65: return UP;
       		 			case 66: return DOWN;
       		 			case 67: return RIGHT;
       		 			case 68: return LEFT;
       		 		}
       		 	}
       		 	else if(c==(-1)) return ESC;       
       		}
       		else{
       			switch(*ch){
       				case 66: case 98: return B;
       				case 67: case 99:	return C;
       				case 68: case 100: return D;
       				case 69: case 101: return E;
       				case 73: case 105: return I;
       				case 76: case 108: return L;
       				case 77: case 109: return M;
       				case 78: case 110: return N;
       				case 82: case 114: return R;
       				case 83: case 115: return S;
              case 85: case 117: return U;
       				case 87: case 119: return W;
       				case 88: case 120: return X;
       			}
       		}
            //fprintf(stdout, "\n");
            break;                 
    }	
}

bool isSetTerminalToHideInput(int fd_terminal, termios * term_ptr)
{
   struct termios tmp_term_vals;
   errno=0;
   tcgetattr(fd_terminal, term_ptr);  

   tmp_term_vals = *term_ptr; 
   tmp_term_vals.c_lflag = tmp_term_vals.c_lflag & ~ECHO;
   tmp_term_vals.c_lflag = tmp_term_vals.c_lflag & ~ICANON;  
   tmp_term_vals.c_cc[VMIN] = 0;  //change?
   tmp_term_vals.c_cc[VTIME] = 0; //change?

  if( (tcgetattr(fd_terminal, term_ptr) == 0) 
    && (tcsetattr(fd_terminal, TCSAFLUSH, &tmp_term_vals) == 0) ){

    struct termios tmp_term_vals_check;

    if(((tcgetattr(fd_terminal, &tmp_term_vals_check) == 0) 
      && (tmp_term_vals.c_lflag == tmp_term_vals_check.c_lflag) 
      && (tmp_term_vals.c_cc[VMIN] == tmp_term_vals_check.c_cc[VMIN]) 
      && (tmp_term_vals.c_cc[VTIME] == tmp_term_vals_check.c_cc[VTIME])))

      return true;
    else return false; 
  }
  else return false;
}

int getKeyVal()
{
    struct timespec tsp={0,500};
    struct termios  term_vals;
    struct termios * term_ptr = &term_vals;
    int wait_for_input = 1;
    
    int fd_terminal = fileno( stdin );
    int retval = KEYS_TOTAL;
    unsigned char keypress[0];

    fflush(stdout); //write everything prior to clearing terminal

    if(!isSetTerminalToHideInput(fd_terminal, term_ptr)) exit(1); //set terminal values to new settings
    while(wait_for_input == 1) {
        nanosleep(&tsp, NULL); //check later
        retval = keypress_handler(fd_terminal, keypress);
        if(retval != KEYS_TOTAL) wait_for_input = 0;
    }
    if( tcsetattr(fd_terminal, TCSADRAIN, term_ptr) == -1 ) exit(1); //reset terminal values to the original settings
    return retval;
}

#endif

int main_loop(){
	int retval = getKeyVal();
	return retval;
}