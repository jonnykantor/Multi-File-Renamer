#! /usr/bin/make

INCLUDEDIR=/usr/include/
LIBDIR=/usr/lib

SOURCE= Dynamic_File_List.cpp Key_Press_Handler.cpp Multi_File_Renamer.cpp display_with_console.cpp tokenizer.cpp
CSOURCE= tinyfiledialogs.c
CC= g++
CFLAGS= -std=c++11
EXECUTABLE= MFR
INCLUDEFLAG= -I. -I$(INCLUDEDIR) -Iinclude/
LIBFLAG= -L$(LIBDIR)

COBJ= $(CSOURCE:.c=.o)
OBJECT= $(SOURCE:.cpp=.o)

all: $(COBJ) $(OBJECT) depend
	$(CC) $(CFLAGS) $(INCLUDEFLAG) $(LIBFLAG) $(OBJECT) $(COBJ) -o $(EXECUTABLE)
	make clean_object

depend:
	$(CC) -M $(SOURCE) -M $(CSOURCE) > depend

$(COBJ):
	$(CC) $(INCLUDEFLAG) -c -o $@ $(@:.o=.c)

$(OBJECT):
	$(CC) $(CFLAGS) $(INCLUDEFLAG) -c -o $@ $(@:.o=.cpp)

clean_object:
	rm -f $(COBJ)
	rm -f $(OBJECT)

clean:
	rm -f $(COBJ) depend $(EXECUTABLE)
	rm -f $(OBJECT) depend $(EXECUTABLE)

include depend