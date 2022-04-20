#include "Timer.h"
#include "Generator.h"
#include <Windows.h>

void timer_fnc() {
	BOOL ret;
	LARGE_INTEGER DueTime;
	LONG Period = PERIOD;

	Timer_Semaphore = CreateSemaphore(NULL, 0, 2, NULL);
	if (!Timer_Semaphore) return;
	
	DueTime.QuadPart = 0;
	Timer_Handle = CreateWaitableTimer(NULL, FALSE, NULL);
	if (!Timer_Handle) return;
	ret = SetWaitableTimer(Timer_Handle, &DueTime, Period, NULL, NULL, FALSE);
}