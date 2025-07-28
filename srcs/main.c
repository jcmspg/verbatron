#include "../includes/editor.h"

t_mode	current_mode = MODE_INPUT;
char	text_buffer[MAX_ROWS][MAX_COLS];
char	current_filename[256] = {0}; // Add this line
int		g_window_rows = 24;
int		g_window_cols = 80;

int	main(int argc, char **argv)
{
	t_cursor	cursor;
	int			c;

	// Show splash screen before enabling raw mode
	show_splash_screen();
	// Get actual window size
	get_window_size(&g_window_rows, &g_window_cols);
	// Load file if provided as argument
	if (argc > 1)
	{
		load_file(argv[1]);
		strcpy(current_filename, argv[1]); // Store the filename
	}
	else
	{
		memset(text_buffer, ' ', sizeof(text_buffer));
		current_filename[0] = '\0'; // No filename
	}
	cursor.cx = 1;
	cursor.cy = 1;
	cursor.scroll_x = 0;
	cursor.scroll_y = 0;
	enable_raw_mode();
	clear_screen_startup();
	// Draw the initial screen with file contents (if any)
	draw_screen(&cursor);
	draw_cursor(&cursor);
	while (1)
	{
		c = read_key();
		if (current_mode == MODE_INPUT)
		{
			process_keypress(c, &cursor);
			draw_screen(&cursor);
			draw_cursor(&cursor);
		}
		else if (current_mode == MODE_COMMAND)
		{
			process_command(c, &cursor);
			if (c == ARROW_UP || c == ARROW_DOWN || c == ARROW_LEFT
				|| c == ARROW_RIGHT)
			{
				draw_screen(&cursor);
				draw_cursor(&cursor);
			}
		}
	}
	return (ERR_NO_ERROR);
}
