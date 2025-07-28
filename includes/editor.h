#ifndef EDITOR_H
# define EDITOR_H

/*
 * VERBATRON Function Declarations
 * This header declares all functions used throughout the editor.
 * Functions are organized by source file for better maintainability.
 */

// Include all system headers and type definitions
# include "includes.h"

/*
 * MAIN.C - Program entry point and global variable definitions
 */
int	main(int argc, char **argv);

/*
 * EDITOR.C - Core editor functionality
 */

// Screen management functions
void	show_splash_screen(void);      // Display startup screen from verbatron.txt
void	clear_screen_startup(void);    // Clear screen and hide cursor for startup
void	reset_screen(void);            // Restore screen and show cursor on exit

// Cursor management
void	draw_cursor(t_cursor *cursor); // Position and display cursor on screen
void	move_cursor(t_cursor *cursor, char c); // Move cursor based on WASD keys

// File operations
void	save_to_file(const char *filename);  // Write buffer contents to file
void	load_file(const char *filename);     // Read file contents into buffer

// Command line interface (bottom of screen)
void	move_cursor_to(int row, int col);           // Move cursor to specific position
void	clear_command_prompt(void);                 // Clear the command line
void	update_command_line(const char *buffer);    // Update command line display
void	set_cursor_bottom(void);                    // Move cursor to bottom row
void	print_command_prompt(void);                 // Display the ":" prompt

/*
 * INPUT.C - Input handling and display rendering
 */

// Input processing
int		read_key(void);                              // Read and decode keyboard input
void	process_keypress(int c, t_cursor *cursor);  // Handle keys in input mode
void	process_command(int c, t_cursor *cursor);   // Handle keys in command mode
void	handle_command(const char *cmd, t_cursor *cursor); // Execute typed commands

// Display rendering
void	draw_text_buffer(t_cursor *cursor);         // Render text with line numbers
void	draw_screen(t_cursor *cursor);              // Refresh entire screen

// Special key handlers
void	backspace_handle(t_cursor *cursor);         // Handle backspace key logic

/*
 * TERM.C - Terminal management and raw mode control
 */

// Terminal setup and cleanup
void	die(const char *message);                   // Error handler with cleanup
void	disable_raw_mode(void);                     // Restore normal terminal mode
void	enable_raw_mode(void);                      // Enter raw mode for key capture
void	sigint_handle(int sig);                     // Handle Ctrl+C interrupts
void	get_window_size(int *rows, int *cols);      // Get current terminal dimensions

#endif
