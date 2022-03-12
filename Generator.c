#include <stdio.h>
#include <stdlib.h>
#include "Signal.h"
#include "Generator.h"
#include <Windows.h>
#include "Timer.h"


int generate_RT(enum eSIGNAL eSignal, SignalPoint sAmplitude, SignalTime dPeriod, const unsigned long long ullSampleRate) {
	SignalTime dTimePerPoint; //time between 2 points of output signal
	SignalTime dCurTime = 0; //current time for next point

	//_signal_generate = true;
	dTimePerPoint = 1.0 / ullSampleRate;

	//first point
	switch (eSignal) {
	case SINUS: {
		generateOutBuf = 0;
	} break;
	case RECTANGLE: {
		generateOutBuf = _Rectangle(0.0, 0, dPeriod);
	} break;
	default: {
		return -1;
	}
	};

	dCurTime += dTimePerPoint; //increase time

	//while (_signal_generate); //wait until set to 0 again from other thread
	//_signal_generate = true; //reset timer flag
	//ReleaseSemaphore(SemHandle[1], 1, NULL); //release output thread
	WaitForSingleObject(Timer_Handle, INFINITE); //wait for timer
	_signal_out = false; //set output flag, sample ready for output

	while (!abortSig) {
		switch (eSignal) {
		case SINUS: {
			generateOutBuf = 0;
		} break;
		case RECTANGLE: {
			generateOutBuf = _Rectangle(dCurTime, sAmplitude, 0.0);
		} break;
		};

		//increase time, check if period is over
		dCurTime += dTimePerPoint;
		if (dCurTime > dPeriod) {
			dCurTime -= dPeriod;
			dCurTime -= dTimePerPoint;
		}

		//while (_signal_generate); //wait until set to 0 again from other thread
		//_signal_generate = true; //reset timer flag
		//ReleaseSemaphore(SemHandle[1], 1, NULL); //release output thread
		WaitForSingleObject(Timer_Handle, INFINITE); //wait for timer
		_signal_out = false; //set output flag, sample ready for output
		
	}

	return 0;
}

unsigned long generate(SIGNAL_OUT** SignalReturn, enum eSIGNAL eSignal, SignalPoint sAmplitude, SignalTime dPeriod, SignalTime dLength, const unsigned long long ullSampleRate) {
	SIGNAL_OUT* SignalOut = NULL; //array of points for output signal with value and time
	SIGNAL_OUT* pHelpOut = NULL; //pointer on current signal point structure
	unsigned long ulMaxPoints = 0; //number of overall points of output signal
	SignalTime dCurTime = 0; //current time for next point
	SignalTime dTimePerPoint = 0; //time between 2 points of output signal
	SignalPoint sNewPointValue; //value of next out signal point

	if (!SignalReturn) return 0; //output buffer available?

	dTimePerPoint = 1.0 / ullSampleRate;
	ulMaxPoints = (unsigned long)(dLength / dTimePerPoint) + 1;
	SignalOut = malloc(ulMaxPoints * sizeof(SIGNAL_OUT));
	if (!SignalOut) return 0;
	pHelpOut = SignalOut;

	//first point
	switch (eSignal) {
	case SINUS: {
		sNewPointValue = 0;
	} break;
	case RECTANGLE: {
		sNewPointValue = _Rectangle(0.0, 0, dPeriod);
	} break;
	default: {
		free(SignalOut);
		return 0;
	}
	};

	pHelpOut->point = sNewPointValue;
	pHelpOut->time = (float)dCurTime;
	pHelpOut++;
	dCurTime += dTimePerPoint;

	do {
		switch (eSignal) {
		case SINUS: {
			sNewPointValue = 0;
		} break;
		case RECTANGLE: {
			sNewPointValue = _Rectangle(dCurTime, sAmplitude, 0.0);
		} break;
		};

		pHelpOut->point = sNewPointValue;
		pHelpOut->time = (float)dCurTime;
		pHelpOut++;
		dCurTime += dTimePerPoint;
	} while (dCurTime < dLength);

	*SignalReturn = SignalOut;
	return ulMaxPoints;
}

SignalPoint _Rectangle(SignalTime dCurTime, const SignalPoint amplitude, const SignalTime dSetPeriodTime) {
	static SignalTime dPeriod = 0.0;
	
	if (dSetPeriodTime != 0.0) { //set new Time for one period
		dPeriod = dSetPeriodTime;
		return 0;
	}

	if (dPeriod == 0.0) return 0; //generate without period set?

	//generate
	if (dCurTime > dPeriod) {
		do {
			dCurTime -= dPeriod;
		} while (dCurTime > dPeriod);
	}
	if (dCurTime < (dPeriod / 2.0)) return amplitude;
	else return (-1 * amplitude);
}