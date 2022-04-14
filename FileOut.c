#include "FileOut.h"
#include "Samples.h"
#include <stdio.h>
#include <stdbool.h>
#include "Generator.h"
#include "Timer.h"
#include "Filter_1.h"


void statistic(unsigned long long time_diff);
volatile bool abortSig; //Signal to end in RT-mode, true = end

int writeCSV(SIGNAL_OUT SignalGenerator[], SIGNAL_OUT SignalFiltered[], unsigned long count, char path[]) {
	FILE* outFile;
	unsigned int i = 0;
	errno_t err;

	err = fopen_s(&outFile, path, "w");
	if (err) return err;

	fprintf_s(outFile, "Time;Value(generator);Value(filterd)\n");
	for (i = 0; i < count; i++) {
		fprintf_s(outFile, "%f;%i;%i\n",SignalGenerator[i].time,SignalGenerator[i].point,SignalFiltered[i].point);
		//printf_s("%f;%i;%i\n", SignalGenerator[i].time, SignalGenerator[i].point, SignalFiltered[i].point);
	}

	fclose(outFile);
	return 0;
}

void consoleOut() {
	SignalPoint sampleOut = 0;
	SignalPoint generatorSample = 0;
	unsigned long long _time_diff = 0; //time difference between samples
	LARGE_INTEGER TimeInLongLong;
	double TimeInDouble; //time difference in double in ms
	long long oldTime = 0;
	TimeInLongLong.QuadPart = 0;

	WaitForSingleObject(Timer_Handle, INFINITE); //wait for timer
	_signal_out = true; //reset filter flag

	while (!abortSig) {
		while (WaitForSingleObject(Timer_Handle, INFINITE) == WAIT_TIMEOUT) { //wait for timer
			if (abortSig) return;
		}

		//calculate time between new samples
		oldTime = TimeInLongLong.QuadPart;
		QueryPerformanceCounter(&TimeInLongLong);
		_time_diff = TimeInLongLong.QuadPart - oldTime;
		
		//convert time to ms (double)
		TimeInDouble = (double)_time_diff / 10000.0;
		statistic(_time_diff);

		printf_s("%i, %i, %.4f ms\n", generatorSample, sampleOut, TimeInDouble);

		while (_signal_out) { //wait for generator flag to get set
			if (abortSig) return;
		}
		sampleOut = filterOutBuf; //read new sample from filter
		generatorSample = genSample; //read corresponding generator sample
		_signal_out = true; //reset filter flag, sample read from buffer
	}
}