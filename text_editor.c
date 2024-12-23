// a very basic text editor written in C
// Author: Gabriel Sullivan

#include <stdio.h>
#include <string.h>
#include <windows.h>

char lines[256][256] = {0}; // Stores the file lines to be worked with
int line_count = 0;   // To track the number of lines
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

void render_line(int line_number);

// the main editor loop
void input_loop();

int main(int argc, char *argv[])
{
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
    while (fgets(lines[line_count], 256, fptr) && line_count < 256)
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
        if (i < 10)
        {
            printf("%d   %s\n", i, lines[i]);
        }
        else if (i < 100)
        {
            printf("%d  %s\n", i, lines[i]);
        }
        else
        {
            printf("%d %s\n", i, lines[i]);
        }
    }
}

void render_line(int line_number)
{
    move_cursor(0, line_number + 2); // Adjust for header offset
    if (line_number < 10)
        printf("%d   %s\033[K", line_number, lines[line_number]); // Clear to end of line
    else if (line_number < 100)
        printf("%d  %s\033[K", line_number, lines[line_number]);
    else
        printf("%d %s\033[K", line_number, lines[line_number]);
}

void input_loop()
{
    // these are the location of the cursor relative to the document
    int absolute_y = 0;
    int absolute_x = 0;

    int longest_length = 257;

    // this is the correct starting x whenever moving down a row
    int official_absolute_x = 0;

    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    INPUT_RECORD inputRecord;
    DWORD events;
    while (1)
    {
        ReadConsoleInput(hInput, &inputRecord, 1, &events);

        if (inputRecord.EventType == KEY_EVENT && inputRecord.Event.KeyEvent.bKeyDown)
        {
            int key = inputRecord.Event.KeyEvent.wVirtualKeyCode;

            // Handle arrow keys
            if (key == VK_UP)
            {
                if (absolute_y > 0)
                {
                    absolute_y--;
                    absolute_x = official_absolute_x;

                    if(absolute_y < bottom - 10) {
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

                    if(absolute_y >= bottom) {
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
                    int len = strlen(lines[absolute_y]);

                    for (int i = absolute_x - 1; i < len; i++)
                    {
                        lines[absolute_y][i] = lines[absolute_y][i + 1];
                    }

                    absolute_x--;
                    official_absolute_x = absolute_x;
                    render_line(absolute_y);
                }
                else if (absolute_y > 0)
                {
                    int prev_len = strlen(lines[absolute_y - 1]);
                    strcat(lines[absolute_y - 1], lines[absolute_y]);

                    // Shift the lines up
                    for (int i = absolute_y; i < line_count - 1; i++)
                    {
                        strcpy(lines[i], lines[i + 1]);
                    }

                    line_count--;
                    absolute_y--;
                    absolute_x = prev_len - 1;
                    official_absolute_x = absolute_x;
                    render_editor();
                }
            }
            else if (key == VK_RETURN)
            {
                int len = strlen(lines[absolute_y]);

                if (line_count < 256)
                {
                    // Shift the lines down
                    for (int i = line_count; i > absolute_y; i--)
                    {
                        strcpy(lines[i], lines[i - 1]);
                    }

                    strcpy(lines[absolute_y + 1], &lines[absolute_y][absolute_x]);
                    lines[absolute_y][absolute_x] = '\0';

                    line_count++;
                    absolute_y++;
                    absolute_x = 0;
                    official_absolute_x = 0;
                    if(absolute_y >= bottom) {
                        bottom++;
                    }
                    system("cls");
                    render_editor();
                }
            }
            else if (key == VK_DELETE)
            {
            }
            else if (key >= 0x20 && key <= 0x7E)
            {
                char ch = inputRecord.Event.KeyEvent.uChar.AsciiChar;
                int len = strlen(lines[absolute_y]);

                for (int i = len; i >= absolute_x; i--)
                {
                    lines[absolute_y][i + 1] = lines[absolute_y][i];
                }

                lines[absolute_y][absolute_x] = ch;
                absolute_x++;
                official_absolute_x++;
                render_line(absolute_y);
            }

            longest_length = strlen(lines[absolute_y]);
            absolute_x = (absolute_x > longest_length) ? longest_length : absolute_x;

            cursor_x = absolute_x + 4;
            cursor_y = absolute_y + 2 - (bottom - 10);
            move_cursor(cursor_x, cursor_y);

            // Exit on ESC key
            if (key == VK_ESCAPE)
            {
                system("cls"); // Clear the terminal
                break;
            }
        }
    }
}
