#ifndef AGENTS_COROUTINE_H_
#define AGENTS_COROUTINE_H_

#define PULL_BEGIN() static int state=0; switch(state) { case 0:
#define PULL_CONTINUE(curr, val) do { state=curr; return val; case val:;} while(0)
#define PULL_FINISH() }

#endif
