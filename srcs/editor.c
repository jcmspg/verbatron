#include "../includes/editor.h"
#include <fcntl.h>
#include <unistd.h>

/*
 * VERBATRON Core Editor Functions
 * This file contains the main editor functionality including screen management,
 * cursor control, file I/O, and the splash screen. These are the core operations
 * that make VERBATRON function as a text editor.
 */

/*
 * Clear screen and hide cursor for clean startup
 * Used when entering the main editor after splash screen
 */
void	clear_screen_startup(void)
{
    write(STDOUT_FILENO, "\x1b[2J", 4);   // ANSI: Clear entire screen
    write(STDOUT_FILENO, "\x1b[H", 3);    // ANSI: Move cursor to top-left (1,1)
    write(STDOUT_FILENO, "\x1b[?25l", 6); // ANSI: Hide cursor during drawing
}

/*
 * Restore screen and show cursor for clean exit
 * Called when quitting the editor to leave terminal in good state
 */
void	reset_screen(void)
{
    write(STDOUT_FILENO, "\x1b[?25h", 6); // ANSI: Show cursor
    write(STDOUT_FILENO, "\x1b[2J", 4);   // ANSI: Clear screen
    write(STDOUT_FILENO, "\x1b[H", 3);    // ANSI: Move to top-left
}

/*
 * Move cursor based on WASD keys (alternative navigation)
 * Currently used by move_cursor_to() function
 * 
 * @param cursor: Cursor structure to modify
 * @param c: Character pressed ('w'=up, 's'=down, 'a'=left, 'd'=right)
 */
void	move_cursor(t_cursor *cursor, char c)
{
    if (c == 'w' && cursor->cy > 1)      // Up (with bounds checking)
        cursor->cy--;
    else if (c == 's')                   // Down
        cursor->cy++;
    else if (c == 'a' && cursor->cx > 1) // Left (with bounds checking)
        cursor->cx--;
    else if (c == 'd')                   // Right
        cursor->cx++;
}

/*
 * Position and display the cursor on screen
 * Accounts for line numbers, scrolling, and window boundaries
 * 
 * @param cursor: Current cursor position and scroll state
 */
void	draw_cursor(t_cursor *cursor)
{
    int		visible_rows;   // Number of rows available for text
    int		visible_y;      // Cursor's screen position (row)
    int		visible_x;      // Cursor's screen position (column)
    char	cursor_str[30]; // Buffer for ANSI escape sequence
    int		len;           // Length of escape sequence

    visible_rows = g_window_rows - 1;               // Reserve bottom row for commands
    visible_y = cursor->cy - cursor->scroll_y;      // Cursor row relative to scroll
    visible_x = cursor->cx - cursor->scroll_x + 5;  // +5 to account for line numbers

    // Only show cursor if it's within the visible area
    if (visible_y >= 1 && visible_y <= visible_rows && 
        visible_x >= 6 && visible_x <= g_window_cols)
    {
        write(STDOUT_FILENO, "\x1b[?25h", 6);        // Show cursor
        len = sprintf(cursor_str, "\x1b[%d;%dH", visible_y, visible_x);
        write(STDOUT_FILENO, cursor_str, len);       // Position cursor
    }
}

/*
 * Save current text buffer to a file
 * Only saves lines that contain non-space characters
 * Handles proper line endings and removes trailing empty lines
 * 
 * @param filename: Path to file to save
 */
void	save_to_file(const char *filename)
{
    int	fd;               // File descriptor
    int	last_char_pos;    // Position of last non-space character in line
    int	has_content_after; // Flag: are there non-empty lines after this one?
    int y, x;             // Loop counters

    // Open file for writing (create if doesn't exist, truncate if does)
    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
    {
        // TODO: Better error handling (show message to user)
        return ;
    }

    // Process each line in the buffer
    for (y = 0; y < MAX_ROWS; y++)
    {
        // Find the rightmost non-space character in this line
        last_char_pos = -1;
        for (x = 0; x < MAX_COLS; x++)
        {
            if (text_buffer[y][x] != ' ')
                last_char_pos = x;
        }

        // If line has content, write it
        if (last_char_pos >= 0)
        {
            write(fd, text_buffer[y], last_char_pos + 1); // Write up to last char
            write(fd, "\n", 1);                           // Add newline
        }
        // Handle empty lines (all spaces)
        else
        {
            // Check if there are any non-empty lines after this one
            has_content_after = 0;
            for (int check_y = y + 1; check_y < MAX_ROWS; check_y++)
            {
                for (int check_x = 0; check_x < MAX_COLS; check_x++)
                {
                    if (text_buffer[check_y][check_x] != ' ')
                    {
                        has_content_after = 1;
                        break ;
                    }
                }
                if (has_content_after)
                    break ;
            }
            
            // Only write empty line if there's content after it
            // This prevents trailing empty lines in saved files
            if (has_content_after)
                write(fd, "\n", 1);
        }
    }
    close(fd);
}

/*
 * Move cursor to specific screen position (helper function)
 * 
 * @param row: Target row (screen coordinates)
 * @param col: Target column (screen coordinates)
 */
void	move_cursor_to(int row, int col)
{
    t_cursor	cursor;

    // Note: This function has a naming confusion - parameters are swapped
    cursor.cx = row;  // Should be col
    cursor.cy = col;  // Should be row
    move_cursor(&cursor, 0); // Call with dummy character
}

/*
 * Clear the command prompt line at bottom of screen
 * Used when entering/exiting command mode
 */
void	clear_command_prompt(void)
{
    move_cursor_to(MAX_ROWS - 1, 0);        // Move to bottom-left
    write(STDOUT_FILENO, "\033[K", 3);      // ANSI: Clear line from cursor to end
}

/*
 * Update the command line with current command text
 * Shows the ":" prompt followed by what user is typing
 * 
 * @param cmd: Command text to display
 */
void	update_command_line(const char *cmd)
{
    clear_command_prompt();
    move_cursor_to(MAX_ROWS - 1, 0);       // Move to bottom-left
    write(STDOUT_FILENO, ":", 1);          // Show command prompt
    write(STDOUT_FILENO, cmd, strlen(cmd)); // Show command text
}

/*
 * Move cursor to bottom of screen (command line area)
 */
void	set_cursor_bottom(void)
{
    move_cursor_to(MAX_ROWS - 1, 0);
}

/*
 * Display the command prompt (":") at bottom of screen
 * Uses dynamic window sizing to position correctly
 */
void	print_command_prompt(void)
{
    char	prompt_str[20]; // Buffer for ANSI escape sequence
    int		len;           // Length of escape sequence

    // Position cursor at bottom-left and show prompt
    len = sprintf(prompt_str, "\x1b[%d;1H:", g_window_rows);
    write(STDOUT_FILENO, prompt_str, len);
}

/*
 * Display startup splash screen from verbatron.txt
 * Shows for 1 second then clears screen
 * If verbatron.txt doesn't exist, shows simple fallback text
 */
void	show_splash_screen(void)
{
    int		fd;          // File descriptor for verbatron.txt
    char	buffer[1024]; // Read buffer
    ssize_t	bytes_read;  // Number of bytes read

    // Clear screen and position cursor for splash
    write(STDOUT_FILENO, "\x1b[2J", 4); // Clear entire screen
    write(STDOUT_FILENO, "\x1b[H", 3);  // Move cursor to top-left

    // Try to open and display verbatron.txt
    fd = open("verbatron.txt", O_RDONLY);
    if (fd != -1)
    {
        // Read and display file contents
        while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0)
        {
            write(STDOUT_FILENO, buffer, bytes_read);
        }
        close(fd);
    }
    else
    {
        // Fallback if file doesn't exist
        write(STDOUT_FILENO, "VERBATRON\n", 10);
        write(STDOUT_FILENO, "Text Editor\n", 12);
        write(STDOUT_FILENO, "Version 1.0\n", 12);
    }

    sleep(1);  // Display splash for 1 second

    // Clear screen after splash
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
}

/*
 * Load a file into the text buffer
 * Handles character conversion and proper line/column management
 * 
 * @param filename: Path to file to load
 */
void	load_file(const char *filename)
{
    int		fd;          // File descriptor
    char	buffer[1024]; // Read buffer
    ssize_t	bytes_read;  // Number of bytes read
    int		row, col;    // Current position in text buffer

    row = 0;
    col = 0;

    // Store filename for future save operations
    strcpy(current_filename, filename);

    // Clear the text buffer (fill with spaces)
    memset(text_buffer, ' ', sizeof(text_buffer));

    // Try to open the file
    fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        // File doesn't exist - start with empty buffer but keep filename
        // This allows saving new files with the specified name
        return ;
    }

    // Read file in chunks and process character by character
    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0)
    {
        for (ssize_t i = 0; i < bytes_read && row < MAX_ROWS; i++)
        {
            if (buffer[i] == '\n')
            {
                // End of line: fill rest with spaces and move to next line
                while (col < MAX_COLS)
                    text_buffer[row][col++] = ' ';
                row++;
                col = 0;
            }
            else if (buffer[i] == '\t')
            {
                // Convert tabs to 4 spaces for consistent display
                for (int j = 0; j < 4 && col < MAX_COLS; j++)
                    text_buffer[row][col++] = ' ';
            }
            else if (col < MAX_COLS && buffer[i] >= 32 && buffer[i] <= 126)
            {
                // Only store printable ASCII characters
                // This filters out control characters that could mess up display
                text_buffer[row][col++] = buffer[i];
            }
            // Non-printable characters are silently ignored
        }
    }
    close(fd);
}
