#! /usr/bin/make

INCLUDEDIR=/usr/include/
LIBDIR=/usr/lib

SOURCE= Dynamic_File_List.cpp Key_Press_Handler.cpp Multi_File_Renamer.cpp display_with_console.cpp tinyfiledialogs.c tokenizer.cpp
CC= g++
CFLAGS= -std=c++11
EXECUTABLE= MFR
LDFLAGS =
INCLUDEFLAG= -I. -I$(INCLUDEDIR) -Iinclude/
LIBFLAG= -L$(LIBDIR)

OBJECT= $(SOURCE:.cpp=.o)

all: $(OBJECT) depend
	$(CC) $(CFLAGS) $(INCLUDEFLAG) $(LIBFLAG) $(OBJECT) -o $(EXECUTABLE) $(LDFLAGS)

depend:
	$(CC) -M $(SOURCE) > depend

$(OBJECT):
	$(CC) $(CFLAGS) $(INCLUDEFLAG) -c -o $@ $(@:.o=.cpp)

clean_object:
	rm -f $(OBJECT)

clean:
	rm -f $(OBJECT) depend $(EXECUTABLE)

include depend