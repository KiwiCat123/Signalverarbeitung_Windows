#include "Timer.h"
#include "Generator.h"
#include <Windows.h>

void timer_fnc() {
	BOOL ret;
	LARGE_INTEGER DueTime;
	LONG Period = PERIOD;

	DueTime.QuadPart = 0;
	Timer_Handle = CreateWaitableTimer(NULL, FALSE, NULL);
	if (!Timer_Handle) return;
	ret = SetWaitableTimer(Timer_Handle, &DueTime, Period, NULL, NULL, FALSE);

	/*while (!abortSig) {
		ret = WaitForSingleObject(Timer_Handle, INFINITE);
		//Sleep(100);
		_signal_generate = 0;
		//ReleaseSemaphore(SemHandle[0], 1, NULL); //Timer period over, release
	}*/
}