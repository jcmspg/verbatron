#include "../includes/editor.h"

// char	text_buffer[MAX_ROWS][MAX_COLS];
// key press processors

int	read_key(void)
{
	char	c;
	char	seq[2];

	while (read(STDIN_FILENO, &c, 1) != 1)
		;
	if (c == '\x1b')
	{
		if (read(STDIN_FILENO, &seq[0], 1) != 1)
			return ('\x1b');
		if (read(STDIN_FILENO, &seq[1], 1) != 1)
			return ('\x1b');
		if (seq[0] == '[')
		{
			switch (seq[1])
			{
			case 'A':
				return (ARROW_UP);
			case 'B':
				return (ARROW_DOWN);
			case 'C':
				return (ARROW_RIGHT);
			case 'D':
				return (ARROW_LEFT);
			}
		}
		return ('\x1b');
	}
	return ((int)c);
}

void	draw_text_buffer(t_cursor *cursor)
{
	int		buffer_row;
	char	move_cursor_str[20];
	int		len;
	int		start_col;

	int visible_rows = g_window_rows - 1; // Reserve bottom row for commands
	for (int y = 0; y < visible_rows; y++)
	{
		buffer_row = y + cursor->scroll_y;
		char line_num_str[32]; // Increased to accommodate color codes
		// Move cursor to beginning of line
		len = sprintf(move_cursor_str, "\x1b[%d;1H", y + 1);
		write(STDIN_FILENO, move_cursor_str, len);
		// Print line number with grey color
		if (buffer_row < MAX_ROWS)
		{
			// \x1b[90m = bright black (grey), \x1b[0m = reset color
			len = sprintf(line_num_str, "\x1b[90m%4d \x1b[0m", buffer_row + 1);
			write(STDIN_FILENO, line_num_str, len);
		}
		else
		{
			write(STDIN_FILENO, "\x1b[90m     \x1b[0m", 11);
			// Grey empty space
		}
		// Print text content (normal color)
		if (buffer_row < MAX_ROWS)
		{
			start_col = cursor->scroll_x;
			int chars_to_write = g_window_cols - 5; // Account for line numbers
			if (start_col + chars_to_write > MAX_COLS)
				chars_to_write = MAX_COLS - start_col;
			if (chars_to_write > 0 && start_col < MAX_COLS)
				write(STDIN_FILENO, &text_buffer[buffer_row][start_col],
					chars_to_write);
		}
		// Clear rest of line
		write(STDIN_FILENO, "\x1b[K", 3);
	}
	fflush(stdout);
}

void	backspace_handle(t_cursor *cursor)
{
	if (cursor->cx > 1)
	{
		// Simply move cursor left and clear the character
		cursor->cx--;
		text_buffer[cursor->cy - 1][cursor->cx - 1] = ' ';
	}
	else if (cursor->cy > 1)
	{
		// At beginning of line - move to end of previous line
		cursor->cy--;
		// Find the end of the previous line (last non-space character + 1)
		cursor->cx = MAX_COLS;
		while (cursor->cx > 1 && text_buffer[cursor->cy - 1][cursor->cx
			- 1] == ' ')
			cursor->cx--;
		// Position cursor after the last character
		if (cursor->cx < MAX_COLS)
			cursor->cx++;
	}
	// If at position (1,1), do nothing
}

void	process_keypress(int c, t_cursor *cursor)
{
	int visible_rows = g_window_rows - 1; // Reserve bottom row for commands
	if (c == 4)                           // Ctrl+D to exit
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
	else if (c == 127) // backspace
		backspace_handle(cursor);
	else if (c == '\r' || c == '\n')
	{
		cursor->cx = 1;
		if (cursor->cy < MAX_ROWS)
			cursor->cy++;
	}
	else if (c >= 32 && c <= 126)
	{
		text_buffer[cursor->cy - 1][cursor->cx - 1] = (char)c;
		cursor->cx++;
		if (cursor->cx > MAX_COLS)
		{
			cursor->cx = 1;
			cursor->cy++;
			if (cursor->cy > MAX_ROWS)
				cursor->cy = MAX_ROWS;
		}
	}
	else if (c == 27) // ESC key
	{
		current_mode = MODE_COMMAND;
		set_cursor_bottom();    // Optional: move to bottom for command entry
		print_command_prompt(); // e.g., show ":" or similar
	}
}

void	draw_screen(t_cursor *cursor)
{
	write(STDOUT_FILENO, "\x1b[H", 3); // Move to top-left
	draw_text_buffer(cursor);
}

char	command_buffer[128] = {0}; // Initialize to zero
int		command_length = 0;

void	process_command(int c, t_cursor *cursor)
{
	if (c == ARROW_UP && cursor->cy > 1)
		cursor->cy--;
	else if (c == ARROW_DOWN && cursor->cy < MAX_ROWS)
		cursor->cy++;
	else if (c == ARROW_LEFT && cursor->cx > 1)
		cursor->cx--;
	else if (c == ARROW_RIGHT && cursor->cx < MAX_COLS)
		cursor->cx++;
	else if (c == 127 && command_length > 0) // backspace
	{
		command_buffer[--command_length] = '\0';
		printf("\x1b[%d;1H:%s\x1b[K", MAX_ROWS, command_buffer);
		fflush(stdout);
	}
	else if (c == '\n' || c == '\r') // Enter - execute command
	{
		handle_command(command_buffer, cursor);
		command_length = 0;
		command_buffer[0] = '\0';
		// Clear command line but stay in command mode
		printf("\x1b[%d;1H\x1b[K", MAX_ROWS);
		fflush(stdout);
		// Don't change mode here - let handle_command decide
	}
	else if (c == 27) // ESC - exit command mode without executing
	{
		command_length = 0;
		command_buffer[0] = '\0';
		printf("\x1b[%d;1H\x1b[K", MAX_ROWS);
		fflush(stdout);
		current_mode = MODE_INPUT;
	}
	else if (c >= 32 && c <= 126 && command_length < 127)
	// Printable characters
	{
		command_buffer[command_length++] = (char)c;
		command_buffer[command_length] = '\0';
		printf("\x1b[%d;1H:%s", MAX_ROWS, command_buffer);
		fflush(stdout);
	}
	// Update cursor position in command mode
	if (current_mode == MODE_COMMAND)
	{
		draw_cursor(cursor);
	}
}

void	handle_command(const char *cmd, t_cursor *cursor)
{
	if (strcmp(cmd, "i") == 0 || strcmp(cmd, "input") == 0)
	{
		current_mode = MODE_INPUT;
	}
	else if (strncmp(cmd, "o ", 2) == 0) // "o filename" - open file
	{
		const char *filename = cmd + 2; // Skip "o "
		if (strlen(filename) > 0)
		{
			load_file(filename);
			cursor->cx = 1;
			cursor->cy = 1;
			cursor->scroll_x = 0;
			cursor->scroll_y = 0;
		}
	}
	else if (strncmp(cmd, "w ", 2) == 0) // "w filename" - save to specific file
	{
		const char *filename = cmd + 2; // Skip "w "
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
	else if (strcmp(cmd, "q") == 0) // quit
	{
		reset_screen();
		disable_raw_mode();
		exit(0);
	}
	else if (strcmp(cmd, "wq") == 0) // write and quit
	{
		if (strlen(current_filename) > 0)
			save_to_file(current_filename);
		else
			save_to_file("output.txt");
		reset_screen();
		disable_raw_mode();
		exit(0);
	}
}
