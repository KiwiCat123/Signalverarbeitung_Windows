#include <Windows.h>
#include <stdio.h>
#include "Generator.h"
#include "Samples.h"
#include "FileOut.h"
#include "Filter_1.h"
#include <signal.h>
#include <stdbool.h>
#include "Timer.h"
#include <limits.h>

#define TEST_SAMPLES 50

volatile bool abortSig = FALSE; //Signal to end in RT-mode, true = end
unsigned long long* collectedTimes = NULL; //collected time differences by consoleOut()
int amount = 0; //amount of collectedTimes
void result_statistics(); //evaluate collected statistic values

void SigHandler(int a) {
	printf_s("ctrl-c!\n");
	abortSig = TRUE; //signals threads to abort in RT-mode
}

int main(int argc, char* argv[]) {
	SIGNAL_OUT* aSignalOutput = NULL; //collected samples from generator
	SIGNAL_OUT* aFilteredOutput = NULL; //collected samples from filter
	unsigned long ulCountPoints = 0; //number of overall points of output signal
	unsigned long long ullSampleFrq = SAMPLE_FRQ;
	char path[] = "out.csv"; //generated signal file
	int ret = -5;
	HANDLE ThreadHandle[2];

	signal(SIGINT, SigHandler); //catch CTRL+C

	if (*(argv[1]) == 'a') {
		ulCountPoints = generate(&aSignalOutput, RECTANGLE, MAX_SIG_VALUE, 1e-3, 1e-2, ullSampleFrq);
		aFilteredOutput = filter(aSignalOutput, ulCountPoints);
		ret = writeCSV(aSignalOutput, aFilteredOutput, ulCountPoints, path);
	}
	else if (*(argv[1]) == 'b') {
		timer_fnc(); //start Timer
		ThreadHandle[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)consoleOut, NULL, 0, NULL); //output Thread
		if (ThreadHandle[0] == NULL) return -2;
		ThreadHandle[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)filter_RT, NULL, 0, NULL); //filter thread
		if (ThreadHandle[1] == NULL) return -2;
		ret = generate_RT(RECTANGLE, MAX_SIG_VALUE, PERIOD*8, PERIOD); //start generator
		WaitForMultipleObjects(2, ThreadHandle, TRUE, INFINITE);

		result_statistics();

		free(collectedTimes);
	}

	return ret;
}

void statistic(unsigned long long time_diff) {
	static bool init = false;

	if (!init) {
		collectedTimes = malloc(sizeof(unsigned long long));
		if (collectedTimes = NULL) {
			while (1);
		}
		init = true;
		return;
	}

	amount++;
	collectedTimes = realloc(collectedTimes, sizeof(unsigned long long)*amount);
	if (!collectedTimes) return;
	collectedTimes[amount - 1] = time_diff;

	if (amount == TEST_SAMPLES)
		abortSig = true;
}

void result_statistics() {
	int i;
	unsigned long long average = 0;
	unsigned long long max = 0;
	unsigned long long min = ULLONG_MAX;
	double temp1;
	double temp2_convert;

	for (i = 0; i < amount; i++) {
		average += collectedTimes[i];
		if (average > (ULLONG_MAX - collectedTimes[i]))
			return;
	}
	average = average / amount;

	for (i = 0; i < amount; i++) {
		if (max < collectedTimes[i])
			max = collectedTimes[i];
	}

	for (i = 0; i < amount; i++) {
		if (min > collectedTimes[i])
			min = collectedTimes[i];
	}

	printf_s("\n\n\n");
	temp2_convert = (double)average / 10000.0;
	temp1 = temp2_convert - PERIOD;
	printf_s("average: %.4f ms  %+.4f ms\n", temp2_convert, temp1);

	temp2_convert = (double)min / 10000.0;
	temp1 = temp2_convert - PERIOD;
	printf_s("min: %.4f ms  %+.4f ms\n", temp2_convert, temp1);

	temp2_convert = (double)max / 10000.0;
	temp1 = temp2_convert - PERIOD;
	printf_s("max: %.4f ms  %+.4f ms\n", temp2_convert, temp1);

	temp1 = (double)(max - min) / 10000.0;
	printf_s("diff: %+.4f ms\n", temp1);
}