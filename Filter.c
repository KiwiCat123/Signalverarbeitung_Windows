#include "Samples.h"
#include <stdlib.h>
#include <limits.h>
#include "Filter_1.h"
#include "Generator.h"
#include <stdbool.h>
#include <math.h>

const double dCoeff[9] = {
   0.005069883484836,  0.02935816274752,   0.1107437912658,   0.2193406809055,
	 0.2709749631927,   0.2193406809055,   0.1107437912658,  0.02935816274752,
   0.005069883484836
};

SIGNAL_OUT* filter(SIGNAL_OUT SignalInput[], unsigned long amount) {
	SIGNAL_OUT* FilterOutput = NULL;
	SignalPoint sampleBuffer[FILTER_LENGTH] = {0,0,0,0,0,0,0,0,0};
	double dResult = 0.0;
	unsigned long count = 0;
	int i;
	int bufferPos = 0;

	FilterOutput = malloc(amount * sizeof(SIGNAL_OUT));
	if (!FilterOutput) return NULL;

	for (count = 0; count < amount; count++) {
		sampleBuffer[bufferPos] = SignalInput[count].point; //new sample to beginning of buffer

		for (i = 0; i < FILTER_LENGTH; i++) { //Filter MAC (next sample point for filtered signal)
			dResult += dCoeff[i] * (double)sampleBuffer[bufferPos];
			bufferPos = (bufferPos + 1) % FILTER_LENGTH; //pointer on next position in buffer
		}
		if (--bufferPos == -1) //right shift, pointer on new beginnig
			bufferPos = FILTER_LENGTH - 1;
		
		FilterOutput[count].time = SignalInput[count].time; //copy time

		if (dResult >= 0.0) dResult += 0.5; //round result for integer conversion
		else dResult -= 0.5;
		if (dResult > MAX_SIG_VALUE) dResult = MAX_SIG_VALUE; //prevent overflow
		else if (dResult < MIN_SIG_VALUE) dResult = MIN_SIG_VALUE;
		FilterOutput[count].point = (short)dResult; //write filtered sample to output
		dResult = 0.0;
	}

	return FilterOutput;
}

int filter_RT() {
	SignalPoint sampleBuffer[FILTER_LENGTH] = { 0,0,0,0,0,0,0,0,0 };
	SignalPoint* sampleBufPtr = &(sampleBuffer[0]);
	int bufferPos = 0;
	int i;
	double dResult = 0;

	while (!abortSig) {
		for (i = 0; i < FILTER_LENGTH; i++) {
			dResult += dCoeff[i] * (double)sampleBuffer[bufferPos];
			bufferPos = (bufferPos + 1) % FILTER_LENGTH; //pointer on next position in buffer
		}
		if (--bufferPos == -1) //right shift, pointer on new beginnig
			bufferPos = FILTER_LENGTH - 1;
		
		while (_generator_ready) { //wait for generator flag to get set
			if (abortSig) return -1;
		}
		genSample = sampleBuffer[bufferPos] = generateOutBuf; //read new sample
		_generator_ready = true; //reset generator flag, sample read from buffer

		if (dResult >= 0.0) dResult += 0.5; //round result for integer conversion
		else dResult -= 0.5;
		if (dResult > MAX_SIG_VALUE) dResult = MAX_SIG_VALUE; //prevent overflow
		else if (dResult < MIN_SIG_VALUE) dResult = MIN_SIG_VALUE;
		while (!_signal_out) { //wait for output to read last sample
			if (abortSig) return -1;
		}
		filterOutBuf = (SignalPoint)dResult; //write filtered sample in output buffer
		_signal_out = false; //new sample ready (filtered)
		dResult = 0.0;
	}

	return 0;
}