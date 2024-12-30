//file_handler.c

#include "file_handler.h"

void load_file(EditorState *editor)
{
    // Open the file
    FILE *fptr = fopen(editor->filename, "r");
    if (fptr == NULL)
    {
        printf("Unable to open the file or the file does not exist.\n");
        exit(1);
    }

    printf("File opened successfully.\n");

    // Read in the file
    while (fgets(editor->lines[editor->line_count], MAX_FILE_SIZE, fptr) && editor->line_count < MAX_FILE_SIZE)
    {
        editor->lines[editor->line_count][strcspn(editor->lines[editor->line_count], "\n")] = '\0';
        editor->line_count++;
    }

    fclose(fptr); // Close the file after reading
}

void save_file(EditorState *editor)
{
    FILE *fptr = fopen(editor->filename, "w");
    if (fptr == NULL)
    {
        printf("Unable to open the file or the file does not exist.\n");
        exit(1);
    }

    for (int i = 0; i < editor->line_count; i++)
    {
        fprintf(fptr, "%s\n", editor->lines[i]);
    }

    fclose(fptr);
}

void clear_memory(EditorState *editor)
{
    for (int i = 0; i < editor->line_count; i++)
    {
        free(editor->lines[i]);
    }

    free(editor->lines);
}
