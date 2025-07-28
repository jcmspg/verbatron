#ifndef TYPEDEFS_H
# define TYPEDEFS_H

/*
 * VERBATRON Type Definitions
 * This file contains all the type definitions, constants, and external variable
 * declarations used throughout the editor. Having all types in one place makes
 * the code more maintainable and prevents circular dependencies.
 */

// Buffer dimensions - these define the maximum size of text the editor can handle
# define MAX_ROWS 1000 // Maximum number of lines (buffer size, not screen size)
# define MAX_COLS 120  // Maximum characters per line

// Command buffer size for storing user commands in command mode
# define CMD_BUF_SIZE 256

// Custom key codes for arrow keys (since they send escape sequences)
// We use values > 255 to avoid conflicts with regular ASCII characters
# define ARROW_UP 1000
# define ARROW_DOWN 1001
# define ARROW_RIGHT 1002
# define ARROW_LEFT 1003

// Global text buffer - the main storage for all text content
// External declaration means it's defined in main.c but used everywhere
extern char		text_buffer[MAX_ROWS][MAX_COLS];

// Current filename being edited (empty string if new file)
extern char		current_filename[256];

// Dynamic window size variables - updated when terminal is resized
extern int		g_window_rows;  // Current terminal height
extern int		g_window_cols;  // Current terminal width

/*
 * Error codes for consistent error handling throughout the application
 * Using an enum ensures type safety and makes debugging easier
 */
enum
{
    ERR_NO_ERROR,        // Success
    ERR_TCATTR_ERROR,    // Terminal attribute setting failed
    ERR_INVALID_ARG,     // Invalid command line argument
    ERR_FILE_NOT_FOUND,  // File couldn't be opened
    ERR_MEMORY_ALLOCATION, // malloc/calloc failed
    ERR_UNKNOWN,         // Unexpected error
    ERR_CLEAN_UP,        // Error during cleanup
};

/*
 * Editor modes - VERBATRON operates in two modes like vim:
 * - INPUT: Normal typing/editing mode
 * - COMMAND: Command entry mode (accessed with ESC)
 */
typedef enum e_mode
{
    MODE_INPUT,    // User can type and edit text
    MODE_COMMAND   // User enters commands (like :w, :q, etc.)
}				t_mode;

// Current editor mode - controls how key presses are interpreted
extern t_mode	current_mode;

/*
 * Cursor structure - tracks both the logical position in the buffer
 * and the scroll offsets for displaying large files in small terminals
 */
typedef struct s_cursor
{
    int cx;       // Cursor column (1-based, position in current line)
    int cy;       // Cursor row (1-based, line number)
    int scroll_x; // Horizontal scroll offset (for long lines)
    int scroll_y; // Vertical scroll offset (for many lines)
}				t_cursor;

#endif /* TYPEDEFS_H */
