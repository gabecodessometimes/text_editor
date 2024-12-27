// a very basic text editor written in C
// Author: Gabriel Sullivan

// to be added: proper memory allocation for the lines
// freeing memory after lines are deleted and allocating memory as lines are created
// freeing memory when items are deleted or backspaced
// maybe add functions to further encapsculate the code for instance a function to delete a line or a function to add a line

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

const int MAX_FILE_SIZE = 256; // Maximum number of lines in the file
const int MAX_LINE_SIZE = 256; // Maximum number of characters in a line

char **lines = NULL; // Stores the file lines to be worked with
int line_count = 0;  // To track the number of lines
int cursor_x = 0;
int cursor_y = 0;
int bottom = 10;
char *filename;

// sets the terminal color
void set_color(int color);

// places the cursor
void move_cursor(int x, int y);

// loads in the file
void load_file();

// renders the editor
void render_editor();

// renders a specific lines
void render_line(int line_number);

// shifts the lines down
void shift_lines_down(int start_index);

//shifts the lines up
void shift_lines_up(int start_index);

// shifts the letters to the right
void shift_letters_right(int start_line_index, int start_letter_index);

// shifts the letters to the left
void shift_letters_left(int start_line_index, int start_letter_index);

// the main editor loop
void input_loop();

// to free up memory
void clear_memory();

int main(int argc, char *argv[])
{
    lines = malloc(MAX_FILE_SIZE * sizeof(char *));
    if (!lines)
    {
        perror("Memory allocation failed.");
        exit(1);
    }

    for (int i = 0; i < MAX_FILE_SIZE; i++)
    {
        lines[i] = malloc(MAX_LINE_SIZE * sizeof(char));
        if (!lines[i])
        {
            perror("Memory allocation failed.");
            exit(1);
        }
    }

    set_color(10);
    if (argc < 2)
    {
        printf("Please specify a file name.\n");
        return 1;
    }

    filename = argv[1];
    load_file();

    // Display editor for the first time
    system("cls");
    render_editor();

    // reset the curspr
    cursor_x = 4;
    cursor_y = 2;
    move_cursor(cursor_x, cursor_y);

    input_loop();
    clear_memory();

    return 0;
}

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

void load_file()
{
    // Open the file
    FILE *fptr = fopen(filename, "r");
    if (fptr == NULL)
    {
        printf("Unable to open the file or the file does not exist.\n");
        exit(1);
    }

    printf("File opened successfully.\n");

    // Read in the file
    while (fgets(lines[line_count], MAX_FILE_SIZE, fptr) && line_count < MAX_FILE_SIZE)
    {
        lines[line_count][strcspn(lines[line_count], "\n")] = '\0';
        line_count++;
    }

    fclose(fptr); // Close the file after reading
}

void render_editor()
{
    move_cursor(0, 0);

    printf("%s\n\n", filename);

    // Print the file
    for (int i = bottom - 10; i < bottom; i++)
    {
        if(lines[i] == NULL || i > line_count - 1)
            break;

        // i is increased by 1 for the label
        if (i < 11) // line label numbers 1 - 10
            printf("%d   %s\n", i + 1, lines[i]);
        else if (i < 101) // line label numbers 11 - 100
            printf("%d  %s\n", i + 1, lines[i]);
        else // all other line label numbers
            printf("%d %s\n", i + 1, lines[i]);
    }
}

void render_line(int line_index)
{
    move_cursor(0, (line_index - (bottom - 10)) + 2); // Adjust for header offset
    if (line_index <= bottom)
    { // line_index is increased by 1 for the label
        if (line_index < 11) // line label numbers 1 - 10
            printf("%d   %s\033[K", line_index+1, lines[line_index]); // Clear to end of line
        else if (line_index < 101) // line label numbers 11 - 100
            printf("%d  %s\033[K", line_index+1, lines[line_index]);
        else // all other line label numbers
            printf("%d %s\033[K", line_index+1, lines[line_index]);
    }
}

void shift_lines_down(int start_index)
{
    if (line_count >= MAX_FILE_SIZE) return;
    
    // Shift the lines down
    for (int i = line_count - 1; i > start_index; i--)
    {
        strcpy(lines[i], lines[i - 1]);
    }

    line_count++;
}

void shift_lines_up(int start_index) {
    for (int i = start_index; i < line_count - 1; i++)
    {
        strcpy(lines[i], lines[i + 1]);
    }

    line_count--;
}

void shift_letters_right(int start_line_index, int start_letter_index) {
    int len = strlen(lines[start_line_index]);
    
    if (len+1 >= MAX_LINE_SIZE) return;

    for (int i = len; i >= start_letter_index; i--)
    {
        lines[start_line_index][i + 1] = lines[start_line_index][i];
    }
}

void shift_letters_left(int start_line_index, int start_letter_index) {
    if (start_letter_index <= 0 && start_line_index <= 0) return;
    if (start_letter_index <= 0) return;
    
    int len = strlen(lines[start_line_index]);
    
    for (int i = start_letter_index - 1; i < len; i++)
    {
        lines[start_line_index][i] = lines[start_line_index][i + 1];
    }
}

void input_loop()
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
            int len = strlen(lines[absolute_y]);
            // Handle arrow keys
            if (key == VK_UP)
            {
                if (absolute_y > 0)
                {
                    absolute_y--;
                    absolute_x = official_absolute_x;

                    if (absolute_y < bottom - 10)
                    {
                        bottom--;
                        system("cls");
                        render_editor();
                    }
                }
            }
            else if (key == VK_DOWN)
            {
                if (absolute_y < line_count - 1)
                {
                    absolute_y++;
                    absolute_x = official_absolute_x;

                    if (absolute_y >= bottom)
                    {
                        bottom++;
                        system("cls");
                        render_editor();
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
                    shift_letters_left(absolute_y, absolute_x);
                    absolute_x--;
                    official_absolute_x = absolute_x;
                    render_line(absolute_y);
                }
                else if (absolute_y > 0)
                {
                    int prev_len = strlen(lines[absolute_y - 1]);
                    strcat(lines[absolute_y - 1], lines[absolute_y]);

                    shift_lines_up(absolute_y);

                    absolute_y--;
                    absolute_x = prev_len - 1;

                    if (absolute_x <= 0) absolute_x = 0;

                    official_absolute_x = absolute_x;
                    system("cls");
                    render_editor();
                }
            }
            else if (key == VK_DELETE)
            {
                if (absolute_x < len)
                {
                    for (int i = absolute_x; i < len; i++)
                    {
                        lines[absolute_y][i] = lines[absolute_y][i + 1]; // cycles through the line and moves it left
                    }

                    render_line(absolute_y);
                }
                else if (absolute_y < line_count - 1) // otherwise it moves the previous line upwards to the current line
                {
                    strcat(lines[absolute_y], lines[absolute_y + 1]);

                    shift_lines_up(absolute_y);

                    system("cls");
                    render_editor();
                }
            }
            else if (key == VK_RETURN)
            {
                if (line_count < MAX_FILE_SIZE)
                {
                    shift_lines_down(absolute_y);

                    // split the line if needed, otherwise just move the line down
                    strcpy(lines[absolute_y + 1], &lines[absolute_y][absolute_x]);
                    lines[absolute_y][absolute_x] = '\0';

                    absolute_y++;
                    absolute_x = 0;
                    official_absolute_x = 0;
                    if (absolute_y >= bottom)
                    {
                        bottom++;
                    }
                    system("cls");
                    render_editor();
                }
            }
            else if (key >= 0x20 && key <= 0x7E) // otherwise for alphabet characters
            {
                char ch = inputRecord.Event.KeyEvent.uChar.AsciiChar;

                shift_letters_right(absolute_y, absolute_x);

                lines[absolute_y][absolute_x] = ch;
                absolute_x++;
                official_absolute_x++;
                render_line(absolute_y);
            }
            else if (key == VK_ESCAPE) // exit the program on the escape key
            {
                system("cls"); // Clear the terminal
                break;
            }

            longest_length = strlen(lines[absolute_y]);
            absolute_x = (absolute_x > longest_length) ? longest_length : absolute_x;

            // set the cursor placement based on the offset of the menu
            cursor_x = absolute_x + 4;
            cursor_y = absolute_y + 2 - (bottom - 10);
            move_cursor(cursor_x, cursor_y);
        }
    }
}

void clear_memory()
{
    for (int i = 0; i < line_count; i++)
    {
        free(lines[i]);
    }

    free(lines);
}
