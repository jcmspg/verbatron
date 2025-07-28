#include "../includes/editor.h"

/*
 * VERBATRON Input Handling and Display
 * This file handles all keyboard input processing, screen rendering, and
 * command execution. It's the core of the user interface, managing both
 * input mode (typing) and command mode (vim-like commands).
 */

/*
 * Read a single keypress from stdin and decode special keys
 * Handles escape sequences for arrow keys and returns custom codes
 * 
 * @return: Integer representing the key pressed (ASCII or custom code)
 */
int	read_key(void)
{
    char	c;      // Current character read
    char	seq[2]; // Buffer for escape sequence characters

    // Wait for a character to be available (blocking read)
    while (read(STDIN_FILENO, &c, 1) != 1)
        ;

    // Handle escape sequences (arrow keys, function keys, etc.)
    if (c == '\x1b')  // ESC character starts an escape sequence
    {
        // Try to read the next character of the sequence
        if (read(STDIN_FILENO, &seq[0], 1) != 1)
            return ('\x1b');  // Just ESC if no more chars

        // Try to read the third character
        if (read(STDIN_FILENO, &seq[1], 1) != 1)
            return ('\x1b');  // Just ESC if incomplete sequence

        // Parse arrow key sequences: ESC [ A/B/C/D
        if (seq[0] == '[')
        {
            switch (seq[1])
            {
            case 'A':
                return (ARROW_UP);     // ESC[A = Up arrow
            case 'B':
                return (ARROW_DOWN);   // ESC[B = Down arrow
            case 'C':
                return (ARROW_RIGHT);  // ESC[C = Right arrow
            case 'D':
                return (ARROW_LEFT);   // ESC[D = Left arrow
            }
        }
        return ('\x1b');  // Unknown escape sequence, return ESC
    }
    return ((int)c);  // Regular character
}

/*
 * Render the text buffer with line numbers and scrolling
 * This is the main display function that draws all visible text
 * 
 * @param cursor: Current cursor position and scroll offsets
 */
void	draw_text_buffer(t_cursor *cursor)
{
    int		buffer_row;        // Which row in text_buffer we're drawing
    char	move_cursor_str[20]; // ANSI escape sequence buffer
    int		len;              // Length of escape sequence
    int		start_col;        // Starting column for text display

    int visible_rows = g_window_rows - 1; // Reserve bottom row for commands

    // Draw each visible row
    for (int y = 0; y < visible_rows; y++)
    {
        buffer_row = y + cursor->scroll_y;  // Account for vertical scrolling
        char line_num_str[32]; // Buffer for line number with color codes

        // Move cursor to beginning of current line
        len = sprintf(move_cursor_str, "\x1b[%d;1H", y + 1);
        write(STDOUT_FILENO, move_cursor_str, len);

        // Print line number with grey color
        if (buffer_row < MAX_ROWS)
        {
            // \x1b[90m = bright black (grey), \x1b[0m = reset color
            len = sprintf(line_num_str, "\x1b[90m%4d \x1b[0m", buffer_row + 1);
            write(STDOUT_FILENO, line_num_str, len);
        }
        else
        {
            // Beyond buffer - show grey empty space
            write(STDOUT_FILENO, "\x1b[90m     \x1b[0m", 11);
        }

        // Print text content (normal color)
        if (buffer_row < MAX_ROWS)
        {
            start_col = cursor->scroll_x;  // Account for horizontal scrolling
            int chars_to_write = g_window_cols - 5; // Account for line numbers (5 chars)

            // Don't write beyond buffer bounds
            if (start_col + chars_to_write > MAX_COLS)
                chars_to_write = MAX_COLS - start_col;

            // Write the visible portion of this line
            if (chars_to_write > 0 && start_col < MAX_COLS)
                write(STDOUT_FILENO, &text_buffer[buffer_row][start_col], chars_to_write);
        }

        // Clear rest of line to prevent artifacts
        write(STDOUT_FILENO, "\x1b[K", 3);
    }
    fflush(stdout);  // Ensure all output is displayed immediately
}

/*
 * Handle backspace key logic
 * Manages cursor movement and character deletion, including line wrapping
 * 
 * @param cursor: Cursor position to modify
 */
void	backspace_handle(t_cursor *cursor)
{
    if (cursor->cx > 1)
    {
        // Not at beginning of line - simply move left and clear character
        cursor->cx--;
        text_buffer[cursor->cy - 1][cursor->cx - 1] = ' ';
    }
    else if (cursor->cy > 1)
    {
        // At beginning of line - move to end of previous line
        cursor->cy--;

        // Find the end of the previous line (last non-space character + 1)
        cursor->cx = MAX_COLS;
        while (cursor->cx > 1 && text_buffer[cursor->cy - 1][cursor->cx - 1] == ' ')
            cursor->cx--;

        // Position cursor after the last character
        if (cursor->cx < MAX_COLS)
            cursor->cx++;
    }
    // If at position (1,1), do nothing - can't backspace further
}

/*
 * Process keypresses in INPUT mode
 * Handles typing, navigation, and mode switching
 * 
 * @param c: Key code from read_key()
 * @param cursor: Cursor position to modify
 */
void	process_keypress(int c, t_cursor *cursor)
{
    int visible_rows = g_window_rows - 1; // Reserve bottom row for commands

    if (c == 4) // Ctrl+D to exit (EOF character)
    {
        reset_screen();
        disable_raw_mode();
        exit(0);
    }
    else if (c == ARROW_UP && cursor->cy > 1)
    {
        cursor->cy--;
        // Scroll up if cursor goes above visible area
        if (cursor->cy <= cursor->scroll_y)
        {
            cursor->scroll_y = cursor->cy - 1;
            if (cursor->scroll_y < 0)
                cursor->scroll_y = 0;
        }
    }
    else if (c == ARROW_DOWN && cursor->cy < MAX_ROWS)
    {
        cursor->cy++;
        // Scroll down if cursor goes below visible area
        if (cursor->cy > cursor->scroll_y + visible_rows)
        {
            cursor->scroll_y = cursor->cy - visible_rows;
        }
    }
    else if (c == ARROW_LEFT && cursor->cx > 1)
    {
        cursor->cx--;
        // Scroll left if cursor goes left of visible area
        if (cursor->cx <= cursor->scroll_x)
        {
            cursor->scroll_x = cursor->cx - 1;
            if (cursor->scroll_x < 0)
                cursor->scroll_x = 0;
        }
    }
    else if (c == ARROW_RIGHT && cursor->cx < MAX_COLS)
    {
        cursor->cx++;
        // Scroll right if cursor goes right of visible area (accounting for line numbers)
        if (cursor->cx > cursor->scroll_x + g_window_cols - 5)
        {
            cursor->scroll_x = cursor->cx - (g_window_cols - 5);
        }
    }
    else if (c == 127) // Backspace key (DEL character)
        backspace_handle(cursor);
    else if (c == '\r' || c == '\n') // Enter key - new line
    {
        cursor->cx = 1;  // Move to beginning of line
        if (cursor->cy < MAX_ROWS)
            cursor->cy++;  // Move to next line
    }
    else if (c >= 32 && c <= 126) // Printable ASCII characters
    {
        // Insert character at cursor position
        text_buffer[cursor->cy - 1][cursor->cx - 1] = (char)c;
        cursor->cx++;

        // Handle line wrapping
        if (cursor->cx > MAX_COLS)
        {
            cursor->cx = 1;    // Wrap to beginning of next line
            cursor->cy++;
            if (cursor->cy > MAX_ROWS)
                cursor->cy = MAX_ROWS;  // Don't go beyond buffer
        }
    }
    else if (c == 27) // ESC key - enter command mode
    {
        current_mode = MODE_COMMAND;
        set_cursor_bottom();     // Move to bottom for command entry
        print_command_prompt();  // Show ":" prompt
    }
}

/*
 * Refresh the entire screen
 * Moves cursor to top-left and redraws all content
 * 
 * @param cursor: Current cursor state for rendering
 */
void	draw_screen(t_cursor *cursor)
{
    write(STDOUT_FILENO, "\x1b[H", 3); // ANSI: Move cursor to top-left (1,1)
    draw_text_buffer(cursor);
}

// Global variables for command mode
char	command_buffer[128] = {0}; // Buffer to store typed command
int		command_length = 0;       // Current length of command

/*
 * Process keypresses in COMMAND mode
 * Handles command entry, cursor movement, and command execution
 * 
 * @param c: Key code from read_key()
 * @param cursor: Cursor position (for navigation in command mode)
 */
void	process_command(int c, t_cursor *cursor)
{
    // Allow cursor movement in command mode (for visual feedback)
    if (c == ARROW_UP && cursor->cy > 1)
        cursor->cy--;
    else if (c == ARROW_DOWN && cursor->cy < MAX_ROWS)
        cursor->cy++;
    else if (c == ARROW_LEFT && cursor->cx > 1)
        cursor->cx--;
    else if (c == ARROW_RIGHT && cursor->cx < MAX_COLS)
        cursor->cx++;
    else if (c == 127 && command_length > 0) // Backspace in command
    {
        // Remove last character from command
        command_buffer[--command_length] = '\0';
        // Redraw command line (note: using printf for immediate output)
        printf("\x1b[%d;1H:%s\x1b[K", g_window_rows, command_buffer);
        fflush(stdout);
    }
    else if (c == '\n' || c == '\r') // Enter - execute command
    {
        handle_command(command_buffer, cursor);
        // Clear command buffer for next command
        command_length = 0;
        command_buffer[0] = '\0';
        // Clear command line but stay in command mode
        printf("\x1b[%d;1H\x1b[K", g_window_rows);
        fflush(stdout);
        // Note: handle_command decides whether to switch modes
    }
    else if (c == 27) // ESC - exit command mode without executing
    {
        command_length = 0;
        command_buffer[0] = '\0';
        printf("\x1b[%d;1H\x1b[K", g_window_rows);
        fflush(stdout);
        current_mode = MODE_INPUT;  // Return to input mode
    }
    else if (c >= 32 && c <= 126 && command_length < 127) // Printable characters
    {
        // Add character to command buffer
        command_buffer[command_length++] = (char)c;
        command_buffer[command_length] = '\0';
        // Show updated command
        printf("\x1b[%d;1H:%s", g_window_rows, command_buffer);
        fflush(stdout);
    }

    // Update cursor position in command mode (visual feedback)
    if (current_mode == MODE_COMMAND)
    {
        draw_cursor(cursor);
    }
}

/*
 * Execute typed commands (vim-like command system)
 * Handles file operations, mode switching, and editor control
 * 
 * @param cmd: Command string typed by user
 * @param cursor: Cursor position (may be reset for some commands)
 */
void	handle_command(const char *cmd, t_cursor *cursor)
{
    if (strcmp(cmd, "i") == 0 || strcmp(cmd, "input") == 0)
    {
        // Switch to input mode
        current_mode = MODE_INPUT;
    }
    else if (strncmp(cmd, "o ", 2) == 0) // "o filename" - open file
    {
        const char *filename = cmd + 2; // Skip "o " prefix
        if (strlen(filename) > 0)
        {
            load_file(filename);
            // Reset cursor to top-left after loading file
            cursor->cx = 1;
            cursor->cy = 1;
            cursor->scroll_x = 0;
            cursor->scroll_y = 0;
        }
    }
    else if (strncmp(cmd, "w ", 2) == 0) // "w filename" - save to specific file
    {
        const char *filename = cmd + 2; // Skip "w " prefix
        if (strlen(filename) > 0)
        {
            save_to_file(filename);
            strcpy(current_filename, filename); // Update current filename
        }
    }
    else if (strcmp(cmd, "w") == 0) // Just "w" - save to current file
    {
        if (strlen(current_filename) > 0)
        {
            save_to_file(current_filename); // Use current filename
        }
        else
        {
            save_to_file("output.txt"); // Fallback if no filename set
        }
    }
    else if (strcmp(cmd, "q") == 0) // Quit editor
    {
        reset_screen();
        disable_raw_mode();
        exit(0);
    }
    else if (strcmp(cmd, "wq") == 0) // Write and quit
    {
        // Save first
        if (strlen(current_filename) > 0)
            save_to_file(current_filename);
        else
            save_to_file("output.txt");
        
        // Then quit
        reset_screen();
        disable_raw_mode();
        exit(0);
    }
    // Unknown commands are silently ignored
}
