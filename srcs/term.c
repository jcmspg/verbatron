#include "../includes/editor.h"
#include <sys/ioctl.h>

/*
 * VERBATRON Terminal Management
 * This file handles all terminal-related operations including raw mode
 * setup, cleanup, signal handling, and window size detection. It's
 * responsible for the low-level terminal interface that enables immediate
 * key response and proper screen control.
 */

// Store original terminal settings for restoration on exit
static struct termios	og_termios;

/*
 * Error handler with cleanup
 * Prints error message, restores terminal state, and exits
 * Used when terminal operations fail critically
 * 
 * @param message: Error message to display
 */
void	die(const char *message)
{
    perror(message);  // Print system error message
    // Restore original terminal settings before dying
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &og_termios);
    exit(ERR_TCATTR_ERROR);
}

/*
 * Restore normal terminal mode
 * Called on exit to leave terminal in a clean state
 * Also clears screen and shows cursor
 */
void	disable_raw_mode(void)
{
    // Restore original terminal attributes
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &og_termios) == -1)
        die("tcsetattr");

    // Clean up screen state
    write(STDOUT_FILENO, "\x1b[2J", 4);   // ANSI: Clear entire screen
    write(STDOUT_FILENO, "\x1b[?25h", 6); // ANSI: Show cursor
    write(STDOUT_FILENO, "\x1b[H", 3);    // ANSI: Move cursor to top-left
}

/*
 * Enter raw mode for immediate key response
 * Disables line buffering, echo, and other terminal processing
 * Essential for a responsive text editor experience
 */
void	enable_raw_mode(void)
{
    struct termios	raw;

    // Get current terminal attributes and save them
    tcgetattr(STDIN_FILENO, &og_termios);
    
    // Register cleanup function to run on normal exit
    atexit(disable_raw_mode);

    // Start with current settings
    raw = og_termios;

    /*
     * Modify terminal flags for raw mode:
     * - ECHO: Don't echo typed characters (we'll handle display ourselves)
     * - ICANON: Disable canonical mode (line buffering)
     *   In canonical mode, input is only available after Enter is pressed
     *   In raw mode, each keypress is immediately available
     */
    raw.c_lflag &= ~(ECHO | ICANON);

    /*
     * Set read timeouts:
     * - VMIN: Minimum characters to read (0 = don't wait for specific count)
     * - VTIME: Timeout in deciseconds (1 = 0.1 seconds)
     *   This makes read() return immediately if characters are available,
     *   or after 0.1 seconds if no input (prevents blocking forever)
     */
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    // Apply the new terminal settings immediately
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

/*
 * Handle Ctrl+C (SIGINT) signal gracefully
 * Ensures proper cleanup even when user force-quits
 * 
 * @param sig: Signal number (unused but required by signal handler signature)
 */
void	sigint_handle(int sig)
{
    (void)sig;  // Suppress unused parameter warning
    
    disable_raw_mode();  // Restore terminal state
    write(STDOUT_FILENO, "Exiting...\n", 12);  // Friendly exit message
    exit(ERR_NO_ERROR);
}

/*
 * Get current terminal window dimensions
 * Uses ioctl system call to query terminal size
 * Falls back to default 24x80 if query fails
 * 
 * @param rows: Pointer to store number of rows
 * @param cols: Pointer to store number of columns
 */
void	get_window_size(int *rows, int *cols)
{
    struct winsize	ws;  // Window size structure

    /*
     * TIOCGWINSZ = Terminal I/O Control Get Window Size
     * This ioctl request fills the winsize structure with:
     * - ws_row: Number of rows (height)
     * - ws_col: Number of columns (width)
     * - ws_xpixel, ws_ypixel: Pixel dimensions (usually 0)
     */
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
    {
        // Fallback to standard VT100 terminal size if ioctl fails
        // This can happen in some terminal emulators or SSH sessions
        *rows = 24;
        *cols = 80;
    }
    else
    {
        // Use actual terminal dimensions
        *rows = ws.ws_row;
        *cols = ws.ws_col;
    }
}
