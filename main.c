//main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "renderer.h"
#include "file_handler.h"

// the main editor loop
void input_loop(EditorState *editor);

int main(int argc, char *argv[])
{
    EditorState *editor = malloc(sizeof(EditorState));

    editor->lines = malloc(MAX_FILE_SIZE * sizeof(char *));
    if (!editor->lines)
    {
        perror("Memory allocation failed.");
        exit(1);
    }

    for (int i = 0; i < MAX_FILE_SIZE; i++)
    {
        editor->lines[i] = malloc(MAX_LINE_SIZE * sizeof(char));
        if (!editor->lines[i])
        {
            perror("Memory allocation failed.");
            exit(1);
        }
    }

    // Initialize the editor
    editor->line_count = 0;
    editor->cursor_x = 0;
    editor->cursor_y = 0;
    editor->bottom = 10; // Assuming the initial bottom value is 10
    editor->filename = NULL;

    set_color(10);
    if (argc < 2)
    {
        printf("Please specify a file name.\n");
        return 1;
    }

    editor->filename = argv[1];
    load_file(editor);

    // Display editor for the first time
    system("cls");
    render_editor(editor);

    // reset the curspr
    editor->cursor_x = 4;
    editor->cursor_y = 2;
    move_cursor(editor->cursor_x, editor->cursor_y);

    input_loop(editor);
    clear_memory(editor);

    return 0;
}

void input_loop(EditorState *editor)
{
    // these are the location of the cursor relative to the document
    int absolute_y = 0;
    int absolute_x = 0;

    // this is the correct starting x whenever moving down a row
    int official_absolute_x = 0;

    int longest_length = MAX_LINE_SIZE; // the length of the line for restricting cursor movement in the x direction
    int size = MAX_FILE_SIZE;           // the size of the lines array

    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    INPUT_RECORD inputRecord;
    DWORD events;
    while (1)
    {
        ReadConsoleInput(hInput, &inputRecord, 1, &events);

        if (inputRecord.EventType == KEY_EVENT && inputRecord.Event.KeyEvent.bKeyDown)
        {
            int key = inputRecord.Event.KeyEvent.wVirtualKeyCode;
            int len = strlen(editor->lines[absolute_y]);
            
            // Handle arrow keys
            if (key == VK_UP)
            {
                if (absolute_y > 0)
                {
                    absolute_y--;
                    absolute_x = official_absolute_x;

                    if (absolute_y < editor->bottom - 10)
                    {
                        editor->bottom--;
                        system("cls");
                        render_editor(editor);
                    }
                }
            }
            else if (key == VK_DOWN)
            {
                if (absolute_y < editor->line_count - 1)
                {
                    absolute_y++;
                    absolute_x = official_absolute_x;

                    if (absolute_y >= editor->bottom)
                    {
                        editor->bottom++;
                        system("cls");
                        render_editor(editor);
                    }
                }
            }
            else if (key == VK_LEFT)
            {
                if (absolute_x > 0)
                {
                    absolute_x--;
                    official_absolute_x = absolute_x;
                }
            }
            else if (key == VK_RIGHT)
            {
                absolute_x++;
                official_absolute_x = absolute_x;
            }
            else if (key == VK_BACK)
            {
                if (absolute_x > 0)
                {
                    shift_letters_left(editor, absolute_y, absolute_x);
                    absolute_x--;
                    official_absolute_x = absolute_x;
                    render_line(editor, absolute_y);
                }
                else if (absolute_y > 0)
                {
                    int prev_len = strlen(editor->lines[absolute_y - 1]);
                    strcat(editor->lines[absolute_y - 1], editor->lines[absolute_y]);

                    shift_lines_up(editor, absolute_y);

                    absolute_y--;
                    absolute_x = prev_len - 1;

                    if (absolute_x <= 0)
                        absolute_x = 0;

                    official_absolute_x = absolute_x;
                    system("cls");
                    render_editor(editor);
                }
            }
            else if (key == VK_F5)
            {
                save_file(editor);
                printf("File saved successfully.\n");
            }
            else if (key == VK_DELETE)
            {
                if (absolute_x < len)
                {
                    for (int i = absolute_x; i < len; i++)
                    {
                        editor->lines[absolute_y][i] = editor->lines[absolute_y][i + 1]; // cycles through the line and moves it left
                    }

                    render_line(editor, absolute_y);
                }
                else if (absolute_y < editor->line_count - 1) // otherwise it moves the previous line upwards to the current line
                {
                    strcat(editor->lines[absolute_y], editor->lines[absolute_y + 1]);

                    shift_lines_up(editor, absolute_y);

                    system("cls");
                    render_editor(editor);
                }
            }
            else if (key == VK_RETURN)
            {
                if (editor->line_count < MAX_FILE_SIZE)
                {
                    shift_lines_down(editor, absolute_y);

                    // split the line if needed, otherwise just move the line down
                    strcpy(editor->lines[absolute_y + 1], &editor->lines[absolute_y][absolute_x]);
                    editor->lines[absolute_y][absolute_x] = '\0';

                    absolute_y++;
                    absolute_x = 0;
                    official_absolute_x = 0;
                    if (absolute_y >= editor->bottom)
                    {
                        editor->bottom++;
                    }
                    system("cls");
                    render_editor(editor);
                }
            }
            else if (key >= 0x20 && key <= 0x7E) // otherwise for alphabet characters
            {
                char ch = inputRecord.Event.KeyEvent.uChar.AsciiChar;

                shift_letters_right(editor, absolute_y, absolute_x);

                editor->lines[absolute_y][absolute_x] = ch;
                absolute_x++;
                official_absolute_x++;
                render_line(editor, absolute_y);
            }
            else if (key == VK_ESCAPE) // exit the program on the escape key
            {
                system("cls"); // Clear the terminal
                break;
            }

            longest_length = strlen(editor->lines[absolute_y]);
            absolute_x = (absolute_x > longest_length) ? longest_length : absolute_x;

            // set the cursor placement based on the offset of the menu
            editor->cursor_x = absolute_x + 4;
            editor->cursor_y = absolute_y + 2 - (editor->bottom - 10);
            move_cursor(editor->cursor_x, editor->cursor_y);
        }
    }
}