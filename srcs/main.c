#include "../includes/editor.h"

/*
 * VERBATRON Main Entry Point
 * This file contains the main function and global variable definitions.
 * It handles program initialization, command line arguments, and the main
 * event loop that drives the entire editor.
 */

// Global variable definitions (declared as extern in typedefs.h)
t_mode	current_mode = MODE_INPUT;      // Start in input mode
char	text_buffer[MAX_ROWS][MAX_COLS]; // Main text storage
char	current_filename[256] = {0};     // Currently opened file
int		g_window_rows = 24;             // Terminal height (default)
int		g_window_cols = 80;             // Terminal width (default)

/*
 * Main function - program entry point
 * Handles initialization, file loading, and main event loop
 * 
 * @param argc: Number of command line arguments
 * @param argv: Array of command line argument strings
 * @return: Error code (0 for success)
 */
int	main(int argc, char **argv)
{
    t_cursor	cursor;     // Cursor position and scroll state
    int			c;          // Current key press

    // Show splash screen before entering raw mode (normal terminal behavior)
    show_splash_screen();
    
    // Get actual terminal dimensions (adapts to any terminal size)
    get_window_size(&g_window_rows, &g_window_cols);
    
    // Handle file loading from command line argument
    if (argc > 1)
    {
        load_file(argv[1]);                    // Load specified file
        strcpy(current_filename, argv[1]);     // Remember filename for saving
    }
    else
    {
        // No file specified - start with empty buffer
        memset(text_buffer, ' ', sizeof(text_buffer));
        current_filename[0] = '\0';            // Mark as new file
    }
    
    // Initialize cursor to top-left corner with no scrolling
    cursor.cx = 1;        // Column 1 (1-based indexing)
    cursor.cy = 1;        // Row 1 (1-based indexing)
    cursor.scroll_x = 0;  // No horizontal scroll
    cursor.scroll_y = 0;  // No vertical scroll
    
    // Enter raw mode for immediate key response (no buffering/echo)
    enable_raw_mode();
    clear_screen_startup();
    
    // Draw initial screen with file contents (if any)
    draw_screen(&cursor);
    draw_cursor(&cursor);
    
    /*
     * Main event loop - runs until user quits
     * Different key handling based on current mode:
     * - INPUT mode: typing, navigation, editing
     * - COMMAND mode: command entry and execution
     */
    while (1)
    {
        c = read_key();  // Get next key press (blocking)
        
        if (current_mode == MODE_INPUT)
        {
            // Input mode: handle typing and navigation
            process_keypress(c, &cursor);
            draw_screen(&cursor);    // Refresh display
            draw_cursor(&cursor);    // Update cursor position
        }
        else if (current_mode == MODE_COMMAND)
        {
            // Command mode: handle command entry
            process_command(c, &cursor);
            
            // Only redraw screen for navigation keys in command mode
            if (c == ARROW_UP || c == ARROW_DOWN || c == ARROW_LEFT || c == ARROW_RIGHT)
            {
                draw_screen(&cursor);
                draw_cursor(&cursor);
            }
        }
    }
    return (ERR_NO_ERROR);
}
