#include "../includes/editor.h"
#include <sys/ioctl.h>

static struct termios	og_termios;

void	die(const char *message)
{
	perror(message);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &og_termios);
	exit(ERR_TCATTR_ERROR);
}

void	disable_raw_mode(void)
{
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &og_termios) == -1)
		die("tcsetattr");
	write(STDOUT_FILENO, "\x1b[2J", 4);   // clear screen
	write(STDOUT_FILENO, "\x1b[?25h", 6); // Ensure cursor visible again
	write(STDOUT_FILENO, "\x1b[H", 3);    // move cursor to top-left
}

void	enable_raw_mode(void)
{
	struct termios	raw;

	tcgetattr(STDIN_FILENO, &og_termios);
	atexit(disable_raw_mode);
	raw = og_termios;
	raw.c_lflag &= ~(ECHO | ICANON);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void	sigint_handle(int sig)
{
	(void)sig;
	disable_raw_mode();
	write(STDOUT_FILENO, "Exiting...\n", 12);
	exit(ERR_NO_ERROR);
}

void	get_window_size(int *rows, int *cols)
{
	struct winsize	ws;

	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
	{
		// Fallback to default if ioctl fails
		*rows = 24;
		*cols = 80;
	}
	else
	{
		*rows = ws.ws_row;
		*cols = ws.ws_col;
	}
}
