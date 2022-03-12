#include "FileOut.h"
#include "Samples.h"
#include <stdio.h>
#include <stdbool.h>
#include "Generator.h"


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
	SignalPoint sampleOut;
	unsigned long long _time_diff = 0; //time difference between samples
	LARGE_INTEGER TimeInLongLong;
	double TimeInDouble; //time difference in double in ms
	long long oldTime = 0;

	TimeInLongLong.QuadPart = 0;

	_generator_ready = true; //reset timer flag

	while (!abortSig) {
		while (_generator_ready); //wait for generator flag to get set
		_generator_ready = true; //reset timer flag
		/*while (WaitForSingleObject(SemHandle[1], 500) == WAIT_TIMEOUT) {
			if (abortSig) return;
		}*/

		//read new sample
		sampleOut = generateOutBuf;
		//_signal_generate = false;

		//calculate time between new samples
		oldTime = TimeInLongLong.QuadPart;
		QueryPerformanceCounter(&TimeInLongLong);
		_time_diff = TimeInLongLong.QuadPart - oldTime;
		
		//convert time to ms (double)
		TimeInDouble = (double)_time_diff / 10000.0;
		statistic(_time_diff);

		printf_s("%i, %.4f ms\n", sampleOut, TimeInDouble);
	}
}