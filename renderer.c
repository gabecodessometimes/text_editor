//renderer.c

#include "renderer.h"

#include <stdlib.h>
#include <string.h>

void set_color(int color)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void move_cursor(int x, int y)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD position = {x, y}; // x: column, y: row
    SetConsoleCursorPosition(hConsole, position);
}

void render_editor(EditorState *editor)
{
    move_cursor(0, 0);

    printf("%s\n\n", editor->filename);

    // Print the file
    for (int i = editor->bottom - 10; i < editor->bottom; i++)
    {
        if (editor->lines[i] == NULL || i > editor->line_count - 1)
            break;

        // i is increased by 1 for the label
        if (i < 11) // line label numbers 1 - 10
            printf("%d   %s\n", i + 1, editor->lines[i]);
        else if (i < 101) // line label numbers 11 - 100
            printf("%d  %s\n", i + 1, editor->lines[i]);
        else // all other line label numbers
            printf("%d %s\n", i + 1, editor->lines[i]);
    }
}

void render_line(EditorState *editor, int line_index)
{
    move_cursor(0, (line_index - (editor->bottom - 10)) + 2); // Adjust for header offset
    if (line_index <= editor->bottom)
    {                                                                   // line_index is increased by 1 for the label
        if (line_index < 11)                                            // line label numbers 1 - 10
            printf("%d   %s\033[K", line_index + 1, editor->lines[line_index]); // Clear to end of line
        else if (line_index < 101)                                      // line label numbers 11 - 100
            printf("%d  %s\033[K", line_index + 1, editor->lines[line_index]);
        else // all other line label numbers
            printf("%d %s\033[K", line_index + 1, editor->lines[line_index]);
    }
}

void shift_lines_down(EditorState *editor, int start_index)
{
    if (editor->line_count >= MAX_FILE_SIZE)
        return;

    // Shift the lines down
    for (int i = editor->line_count - 1; i > start_index; i--)
    {
        strcpy(editor->lines[i], editor->lines[i - 1]);
    }

    editor->line_count++;
}

void shift_lines_up(EditorState *editor, int start_index)
{
    for (int i = start_index; i < editor->line_count - 1; i++)
    {
        strcpy(editor->lines[i], editor->lines[i + 1]);
    }

    editor->line_count--;
}

void shift_letters_right(EditorState *editor, int start_line_index, int start_letter_index)
{
    int len = strlen(editor->lines[start_line_index]);

    if (len + 1 >= MAX_LINE_SIZE)
        return;

    for (int i = len; i >= start_letter_index; i--)
    {
        editor->lines[start_line_index][i + 1] = editor->lines[start_line_index][i];
    }
}

void shift_letters_left(EditorState *editor, int start_line_index, int start_letter_index)
{
    if (start_letter_index <= 0 && start_line_index <= 0)
        return;
    if (start_letter_index <= 0)
        return;

    int len = strlen(editor->lines[start_line_index]);

    for (int i = start_letter_index - 1; i < len; i++)
    {
        editor->lines[start_line_index][i] = editor->lines[start_line_index][i + 1];
    }
}
