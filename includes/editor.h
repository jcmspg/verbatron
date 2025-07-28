#ifndef EDITOR_H
# define EDITOR_H

// Put your editor-specific includes and definitions here
# include "includes.h"

// main.c
int	main(int argc, char **argv); // Update this line

// Splash screen
void	show_splash_screen(void);

// processing commands
void	process_command(int c, t_cursor *cursor);
void	handle_command(const char *cmd, t_cursor *cursor);
void	save_to_file(const char *filename);
void	load_file(const char *filename); // Add this line

void	move_cursor_to(int row, int col);
void	clear_command_prompt(void);
void	update_command_line(const char *buffer);
void	set_cursor_bottom(void);
void	print_command_prompt(void);

// term.c
void	die(const char *message);
void	disable_raw_mode(void);
void	enable_raw_mode(void);
void	sigint_handle(int sig);
void	get_window_size(int *rows, int *cols); // Add this line

void	clear_screen_startup(void);
void	reset_screen(void);

// input.c
int		read_key(void);
void	draw_text_buffer(t_cursor *cursor);
void	process_keypress(int c, t_cursor *cursor);
void	draw_screen(t_cursor *cursor);

// special key handles
void	backspace_handle(t_cursor *cursor);

// editor.c
void	draw_cursor(t_cursor *cursor);
void	move_cursor(t_cursor *cursor, char c);

#endif
