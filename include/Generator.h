#pragma once
#include "Signal.h"
#include <stdbool.h>
#include <Windows.h>

enum eSIGNAL {SINUS, RECTANGLE};

unsigned long generate(SIGNAL_OUT** SignalReturn, enum eSIGNAL eSignal, SignalPoint sAmplitude, SignalTime dPeriod, SignalTime dLength, const unsigned long long ullSampleRate);

SignalPoint _Rectangle(SignalTime dCurTime, const SignalPoint amplitude, SignalTime dSetPeriodTime);

int generate_RT(enum eSIGNAL eSignal, SignalPoint sAmplitude, SignalTime dPeriod, const unsigned long long ullSampleRate);
volatile bool _signal_generate; //timer flag, ready for new sample, false = timer ended
volatile bool _signal_out; //sample ready for output flag, false = ready (new sample ready)
volatile bool abortSig; //Signal to end in RT-mode, true = end
volatile SignalPoint generateOutBuf; //output buffer for sample