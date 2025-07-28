#include "../includes/editor.h"
#include <fcntl.h>
#include <unistd.h>

// clear screen
void	clear_screen_startup(void)
{
	write(STDOUT_FILENO, "\x1b[2J", 4);   // Clear entire screen
	write(STDOUT_FILENO, "\x1b[H", 3);    // Move cursor to top-left
	write(STDOUT_FILENO, "\x1b[?25l", 6); // Hide cursor
}

// show cursor again
void	reset_screen(void)
{
	write(STDOUT_FILENO, "\x1b[?25h", 6); // show cursor
	write(STDOUT_FILENO, "\x1b[2J", 4);
	write(STDOUT_FILENO, "\x1b[H", 3);
}
// move the cursor
void	move_cursor(t_cursor *cursor, char c)
{
	if (c == 'w' && cursor->cy > 1)
		cursor->cy--;
	else if (c == 's')
		cursor->cy++;
	else if (c == 'a' && cursor->cx > 1)
		cursor->cx--;
	else if (c == 'd')
		cursor->cx++;
}

// draw the cursor
void	draw_cursor(t_cursor *cursor)
{
	int		visible_rows;
	int		visible_y;
	int		visible_x;
	char	cursor_str[30];
	int		len;

	visible_rows = g_window_rows - 1;
	visible_y = cursor->cy - cursor->scroll_y;
	visible_x = cursor->cx - cursor->scroll_x + 5; // +5 for line number width
	if (visible_y >= 1 && visible_y <= visible_rows && visible_x >= 6
		&& visible_x <= g_window_cols)
	{
		// Show cursor and position it
		write(STDOUT_FILENO, "\x1b[?25h", 6);
		len = sprintf(cursor_str, "\x1b[%d;%dH", visible_y, visible_x);
		write(STDOUT_FILENO, cursor_str, len);
	}
}

void	save_to_file(const char *filename)
{
	int	fd;
	int	last_char_pos;
	int	has_content_after;

	int y, x;
	fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		// Handle error
		return ;
	}
	for (y = 0; y < MAX_ROWS; y++)
	{
		// Find the last non-space character in this line
		last_char_pos = -1;
		for (x = 0; x < MAX_COLS; x++)
		{
			if (text_buffer[y][x] != ' ')
				last_char_pos = x;
		}
		// If line has content, write it
		if (last_char_pos >= 0)
		{
			write(fd, text_buffer[y], last_char_pos + 1);
			write(fd, "\n", 1); // Add newline
		}
		// Skip completely empty lines at the end
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
			// If there's content after, write empty line
			if (has_content_after)
				write(fd, "\n", 1);
		}
	}
	close(fd);
}

void	move_cursor_to(int row, int col)
{
	t_cursor	cursor;

	cursor.cx = row;
	cursor.cy = col;
	move_cursor(&cursor, 0); // assuming 0 or some dummy char
}

void	clear_command_prompt(void)
{
	move_cursor_to(MAX_ROWS - 1, 0);
	write(STDOUT_FILENO, "\033[K", 3); // Clear the line from cursor to end
}

void	update_command_line(const char *cmd)
{
	clear_command_prompt();
	move_cursor_to(MAX_ROWS - 1, 0);
	write(STDOUT_FILENO, ":", 1);
	write(STDOUT_FILENO, cmd, strlen(cmd));
}

void	set_cursor_bottom(void)
{
	move_cursor_to(MAX_ROWS - 1, 0);
}

void	print_command_prompt(void)
{
	char	prompt_str[20];
	int		len;

	len = sprintf(prompt_str, "\x1b[%d;1H:", g_window_rows);
	write(STDOUT_FILENO, prompt_str, len);
}

void	show_splash_screen(void)
{
	int		fd;
	char	buffer[1024];
	ssize_t	bytes_read;

	// Clear screen first
	write(STDOUT_FILENO, "\x1b[2J", 4); // Clear entire screen
	write(STDOUT_FILENO, "\x1b[H", 3);  // Move cursor to top-left
	// Try to open verbatron.txt
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
	// Wait 1 second instead of 3
	sleep(1);
	// Clear screen after splash
	write(STDOUT_FILENO, "\x1b[2J", 4);
	write(STDOUT_FILENO, "\x1b[H", 3);
}

void	load_file(const char *filename)
{
	int		fd;
	char	buffer[1024];
	ssize_t	bytes_read;
	int		row = 0, col;

	row = 0, col = 0;
	// Store the current filename
	strcpy(current_filename, filename);
	// Clear the text buffer first
	memset(text_buffer, ' ', sizeof(text_buffer));
	fd = open(filename, O_RDONLY);
	if (fd == -1)
	{
		// File doesn't exist - start with empty buffer but keep filename
		return ;
	}
	// Read file character by character and populate text_buffer
	while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0)
	{
		for (ssize_t i = 0; i < bytes_read && row < MAX_ROWS; i++)
		{
			if (buffer[i] == '\n')
			{
				// Fill rest of line with spaces and move to next line
				while (col < MAX_COLS)
					text_buffer[row][col++] = ' ';
				row++;
				col = 0;
			}
			else if (buffer[i] == '\t')
			{
				// Convert tabs to 4 spaces
				for (int j = 0; j < 4 && col < MAX_COLS; j++)
					text_buffer[row][col++] = ' ';
			}
			else if (col < MAX_COLS && buffer[i] >= 32 && buffer[i] <= 126)
			{
				// Only store printable characters
				text_buffer[row][col++] = buffer[i];
			}
		}
	}
	close(fd);
}
