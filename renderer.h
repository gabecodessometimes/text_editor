//renderer.h

#ifndef RENDERER_H
#define RENDERER_H

#include "file_handler.h"

#include <stdio.h>
#include <windows.h>

// sets the terminal color
void set_color(int color);

// places the cursor
void move_cursor(int x, int y);

// renders the editor
void render_editor(EditorState *editor);

// renders a specific lines
void render_line(EditorState *editor, int line_number);

// shifts the lines down
void shift_lines_down(EditorState *editor, int start_index);

// shifts the lines up
void shift_lines_up(EditorState *editor, int start_index);

// shifts the letters to the right
void shift_letters_right(EditorState *editor, int start_line_index, int start_letter_index);

// shifts the letters to the left
void shift_letters_left(EditorState *editor, int start_line_index, int start_letter_index);

#endif // RENDERER_H