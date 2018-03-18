#ifndef PULL_ASSERT_H_
#define PULL_ASSERT_H_

#if ENABLE_ASSERT == 1
#include <assert.h>
#define PULL_ASSERT(cond) assert(cond);

#else
#define PULL_ASSERT(cond) 

#endif /* ENABLE_ASSERT == 1 */

#endif /* PULL_ASSERT_H_ */
