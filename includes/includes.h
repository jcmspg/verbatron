#ifndef INCLUDES_H
# define INCLUDES_H

/*
 * VERBATRON System Includes
 * This file centralizes all system includes needed by the editor.
 * Having all includes in one place makes it easier to manage dependencies
 * and ensures consistent availability across all source files.
 */

// Character classification and conversion
# include <ctype.h>     // isprint(), isspace(), etc.

// Error number definitions
# include <errno.h>     // errno, ENOENT, etc.

// File control operations
# include <fcntl.h>     // open(), O_RDONLY, O_WRONLY, etc.

// Implementation limits
# include <limits.h>    // INT_MAX, CHAR_MAX, etc.

// Locale-specific information
# include <locale.h>    // setlocale() (currently unused but available)

// Mathematical functions
# include <math.h>      // Currently unused but available for future features

// POSIX threads
# include <pthread.h>   // Currently unused but available for threading

// POSIX semaphores
# include <semaphore.h> // Currently unused but available for synchronization

// Signal handling
# include <signal.h>    // signal(), SIGINT, etc.

// Variable argument lists
# include <stdarg.h>    // va_list, va_start(), etc. (for printf-like functions)

// Boolean type
# include <stdbool.h>   // bool, true, false

// Standard definitions
# include <stddef.h>    // size_t, NULL, etc.

// Fixed-width integer types
# include <stdint.h>    // int32_t, uint64_t, etc.

// Standard I/O
# include <stdio.h>     // printf(), sprintf(), FILE, etc.

// Standard library
# include <stdlib.h>    // malloc(), exit(), atoi(), etc.

// String manipulation
# include <string.h>    // strlen(), strcpy(), strcmp(), memset(), etc.

// Terminal I/O control
# include <sys/ioctl.h> // ioctl(), TIOCGWINSZ (for getting window size)

// File status
# include <sys/stat.h>  // stat(), chmod(), file permissions

// System types
# include <sys/types.h> // ssize_t, pid_t, etc.

// Process control
# include <sys/wait.h>  // wait(), waitpid() (currently unused)

// Terminal I/O
# include <termios.h>   // tcgetattr(), tcsetattr(), raw mode control

// Time functions
# include <time.h>      // time(), sleep() (used for splash screen)

// POSIX API
# include <unistd.h>    // read(), write(), close(), STDIN_FILENO, etc.

// Wide character support
# include <wchar.h>     // Currently unused but available for Unicode support

// Include our project-specific type definitions
# include "typedefs.h"

#endif
