#include <stdafx.h>

EXTERN_C
{
// leave a thread
void Task_exit();
// skip "timer" frames
void Task_sleep(int timer);
// set a thread
void Task_execute(int id, void(*func_ptr)());
// resume thread
void Task_signal(int id);
// suspend thread
void Task_suspend(int id);
// reroute current thread
void Task_chain(void(*func_ptr)());
}
