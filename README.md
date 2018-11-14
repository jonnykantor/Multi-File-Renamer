# Multi-File Renamer

Author: Jonathan Kantor

Project: Multi-File Renamer (until I decide on a better name at least)

Version: 1.0.0 - first upload

## Outline:  
This application allows the user to specify multiple files via a file dialogue box, then perform any combination of available operations upon the text from the selected files' names. The user can then save these changes, which will update the names of the selected files on disk.

The application uses a terminal as its GUI. The GUI updates on valid key-presses. Each update displays the edited state of the selected files' names and a menu based on the application state. Changes to file names are not written until they are saved.

The selected files' names are displayed in a grid format. Each file occupies one row in the grid. The file name occupies one column and the extension occupies a second if available. User operations may add or remove columns. When changes are saved, the text in the columns will be combined to create the new name. 

Almost all operations available to the user work over ranges of rows, columns or both as specified by the user. If the user wishes to work on individual rows or columns they can specify that the range affect only the areas they wish.


## Current operations include:
### Filename modifying operations:
#### Insertion:
Insert either text or an iterator into the files' names at a user-specified column position, over a user-specified range of rows. Iterators will always begin at '1' and increase by one per row in the range. If the user wishes to insert text they must first specify what they wish to insert. This operation will result in a new column being inserted at the user-specified position.
#### Replacement:
Similar to insertion, however the user may now specify a range of columns in addition to a range of rows. For each selected row, all columns in the column range will be replaced by one column. The new column contains either the user-specified text or an iterator.

#### Erasure:
When activated, the values in the user-specified row-range and column-range will be erased. This will not delete the rows entirely, even if all column values in the row are erased (if all column values in a row are erased, they will temporarily be replaced by a single column containing nothing).

#### Splitting:
The user may specify a given column, and a character-position in that column, along with a row-range for the operation to affect. When activated, the operation will split the given column at the user-specified character-position into two columns, with the name up to the character position in the first column, and the name following the character position in the second. For example: If a column contained the phrase "FooBar", the user could use the split operation at the 3rd character to split the column into column A containing: "Foo" and column B containing: "Bar".

### File list modifying operations:
#### Row-deletion:
When activated, every row in the user specified row-range will be removed entirely from the file list; note that this will NOT delete the files themselves from their containing folder, but merely remove them from the list of possible files to make name changes to.

#### Row-movement:
When activated, every row in the user specified row-range will be moved in-list to the new user-specified target row, displacing rows ahead of or behind the target depending on whether the move involved moving the rows up in the list or down. For example: If a list contains 10 file names, and the user wishes to move rows 2 to 5 up to row 7, the new order would be: 1-6-7-2-3-4-5-8-9-10; whereas if the user wishes to move rows 4-9 down to row 1, the new order would be 4-5-6-7-8-9-1-2-3-10. Note that, again, this will not affect the position of the files referred to by their counterparts in the list.

#### Loading new files:
When activated the current list of file-names will be discarded, and the user may specify a new group of files to load in for alterations.

### General operations:
#### Saving changes:
When activated, the user will be prompted to confirm whether they wish to save, if the user confirms this operation, the file names as they stand on the screen will replace the old file names of the files loaded by the user.

#### Undo/Redo:
The application contains both undo and redo stacks. Note that currently saving will empty the stacks entirely.

## OS specific Issues:
### Unix:
None on platforms I've compiled on, though there are plenty I have not.

### Windows:
tinyfiledialogs.c never makes a call to CoInitializeEx, which might be needed by a system if it uses the COM library (which explorer does); this caused crashes on my 64 bit Windows 8.1 system but none whatsoever on my 32 bit Windows 7 system. I've put in a call specific to Windows systems in the Multi_File_Renamer.cpp main() function, and this should take care of it.

However there are two more... less than satisfactory features of the tinyfiledialogs implementation for Windows that I haven't had the chance to get around yet: The first is that there is a maximum upper limit on the number of files that can be loaded at once via the GetOpenFileName() call in tinyfiledialogs: 32 files. The second is that there is a maximum character length on the return from the same GetOpenFileName() call of 1024 characters - this only counts the path once, but is still a rather low limit given that files can have an upper limit of around 260 characters (including path) depending on the OS. As the author of tinyfiledialogs has stated they do not want the source code changed, I will have to return to this and write my own Open File dialog box in the future. For now though, the limits will have to do.

## Compiling and running this application:
Currently only the source code is provided for this application; while in the future I hope to provide an installer or executable that currently isn't something I have the time (or if I'm being honest the expertise or equipment) to do. If you wish to use the application you will need to compile it yourself.

### For Linux: 
You will need g++ 4.9 or another compiler that can accommodate the c++11 std. Typically you can compile from the command line via: 
	
`g++ -std=c++11 -o "<executable name>" <every c and cpp file here>`

and run the executable from the command line: './<executable name>'. Alternately you can use the makefile I've provided.

### For Windows: 
Due to library requirements from tinyfiledialogs you will need to link against several libraries. If you don't have these libraries you can use the statically linked .exe I've provided here. If you do want to compile yourself, I used the Visual Studio development console directly to compile this application, and if you have that you should be able to do the same, using the command: 

`lc /Fe<YourExecutableName> /EHsc <every c and cpp file here> -user32.lib -comdlg32.lib -shell32.lib -ole32.lib`

I'm working on a version that doesn't use tinyfiledialogs as using it for this project turned out to be overkill

### For Mac:
I haven't tested on an OS-X device as I don't have access to one, however I believe it should compile using the same process as with Linux above.

## Outside Credit:

tokenizer.cpp is almost entirely (aside from some small adaptation) code I've taken from www.cplusplus.com forums member Duoas here: http://www.cplusplus.com/forum/articles/4860/# msg27665; as this isn't a commercial project nor is the code seemingly protected in any way I haven't sought his permission to use it here, however I felt some recognition was necessary.

Similarly I am (as of version 1.0.0 at least) utilizing tinyfiledialogs, a cross-platform basic GUI library from Vareille that can be found here: sourceforge.net/projects/tinyfiledialogs/ . The author states (s)he does not require notification and so I have not provided it, although they do request some indication that their work was used, and so here it is.
