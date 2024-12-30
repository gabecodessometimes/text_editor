//file_handler.h

#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILE_SIZE 256 // Maximum number of lines in the file
#define MAX_LINE_SIZE 256 // Maximum number of characters in a line

typedef struct
{
    char **lines; // Stores the file lines to be worked with
    int line_count;   // To track the number of lines
    int cursor_x;
    int cursor_y;
    int bottom;
    char *filename;
} EditorState;

// file functions
void load_file(EditorState *editor);
void save_file(EditorState *editor);

// to free up memory
void clear_memory(EditorState *editor);

#endif // FILE_HANDLER_H