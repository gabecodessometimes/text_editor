#include <stdio.h>
#include <string.h>
#include <windows.h>

char lines[256][256]; // Stores the file lines to be worked with
int line_count = 0;   // To track the number of lines
int cursor_x = 0;
int cursor_y = 0;
char *filename;

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

void render_editor()
{
    system("cls"); // Clear the terminal

    printf("%s\n\n", filename);

    // Print the file
    for (int i = 0; i < line_count; i++)
    {
        if(i < 10) {
            printf("%d   %s\n", i, lines[i]);
        } else if(i < 100) {
            printf("%d  %s\n", i, lines[i]);
        } else {
            printf("%d %s\n", i, lines[i]);
        }
    }
}

int main(int argc, char *argv[])
{
    set_color(10);
    if (argc < 2)
    {
        printf("Please specify a file name.\n");
        return 1;
    }

    filename = argv[1];

    // Open the file
    FILE *fptr = fopen(filename, "r");
    if (fptr == NULL)
    {
        printf("Unable to open the file or the file does not exist.\n");
        return 1;
    }

    printf("File opened successfully.\n");

    // Read in the file
    while (fgets(lines[line_count], 256, fptr) && line_count < 256)
    {
        lines[line_count][strcspn(lines[line_count], "\n")] = '\0';
        line_count++;
    }

    fclose(fptr); // Close the file after reading

    render_editor(); // Display editor

    int longest_length = 257;

    // reset the curspr
    cursor_x = 4;
    cursor_y = 2;
    move_cursor(cursor_x, cursor_y);

    // these are the location of the cursor relative to the document
    char absolute_y = 0;
    char absolute_x = 0;

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
                }
            }
            else if (key == VK_DOWN)
            {
                if (absolute_y < line_count - 1)
                {
                    absolute_y++;
                    absolute_x = official_absolute_x;
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
                render_editor();
            }

            longest_length = strlen(lines[absolute_y]);
            absolute_x = (absolute_x > longest_length) ? longest_length : absolute_x;

            cursor_x = absolute_x + 4;
            cursor_y = absolute_y + 2;
            move_cursor(cursor_x, cursor_y);

            // Exit on ESC key
            if (key == VK_ESCAPE)
            {
                system("cls"); // Clear the terminal
                break;
            }
        }
    }

    return 0;
}
