#ifndef LIBPULL_COMMON_ASSERT_H_
#define LIBPULL_COMMON_ASSERT_H_

#if ENABLE_ASSERT == 1
#include <assert.h>
#define PULL_ASSERT(cond) assert(cond);

#else
#define PULL_ASSERT(cond) 

#endif /* ENABLE_ASSERT == 1 */

#endif /* LIBPULL_COMMON_ASSERT_H_ */
