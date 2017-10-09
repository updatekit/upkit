#ifndef EXTERNAL_H_
#define EXTERNAL_H_
#include <stdint.h>

/* This files contains a set of external symbols that must be
 * defined by the application using it */



/* This array of integer identify the number of momory objects
 * storing in memory having a valid manifest. You should define
 * for each element of the array a value indicating a positive value
 * the memory object you are reffering to and finish the array with
 * a negative value of type OBJ_END;
 * Example of definition in you application:
 *
 *     const int8_t memory_objects[] = {OBJ_1, OBJ2, OBJ_END};
 *
 */
#define OBJ_END -1
extern const int8_t memory_objects[];

#endif
