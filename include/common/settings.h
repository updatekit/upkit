#ifndef _SETTINGS_H
#define _SETTINGS_H

/* Deault configurations. Redefine with symbols in your
 * build system to override them
 */

// Logger
#ifndef LOGGER_VERBOSITY
#define LOGGER_VERBOSITY 3
#endif

// Memory Object buffer size: is a buffer used to copy two objects
#ifndef MEMORY_OBJ_BUFFER_SIZE
#define MEMORY_OBJ_BUFFER_SIZE 0x1000
#endif

#endif
