#ifndef TYPEDEFS_H
# define TYPEDEFS_H

// Define your types here
# define MAX_ROWS 1000 // Buffer size (much larger than screen)
# define MAX_COLS 120  // Buffer size

// for the command line buffer
# define CMD_BUF_SIZE 256

# define ARROW_UP 1000
# define ARROW_DOWN 1001
# define ARROW_RIGHT 1002
# define ARROW_LEFT 1003

extern char		text_buffer[MAX_ROWS][MAX_COLS];
extern char current_filename[256]; // Add this line

// Add global variables for window size
extern int		g_window_rows;
extern int		g_window_cols;

// Error Messages
enum
{
	ERR_NO_ERROR,
	ERR_TCATTR_ERROR,
	ERR_INVALID_ARG,
	ERR_FILE_NOT_FOUND,
	ERR_MEMORY_ALLOCATION,
	ERR_UNKNOWN,
	ERR_CLEAN_UP,
};

// MODES
typedef enum e_mode
{
	MODE_INPUT,
	MODE_COMMAND
}				t_mode;

extern t_mode	current_mode;

typedef struct s_cursor
{
	int cx;       // Cursor column (1-based)
	int cy;       // Cursor row (1-based)
	int scroll_x; // Horizontal scroll offset
	int scroll_y; // Vertical scroll offset
}				t_cursor;

#endif /* TYPEDEFS_H */
