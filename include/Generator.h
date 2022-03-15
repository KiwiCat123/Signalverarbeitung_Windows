#pragma once
#include "Samples.h"
#include <stdbool.h>
#include <Windows.h>

enum eSIGNAL {SINUS, RECTANGLE};

unsigned long generate(SIGNAL_OUT** SignalReturn, enum eSIGNAL eSignal, SignalPoint sAmplitude, SignalTime dPeriod, SignalTime dLength, const unsigned long long ullSampleRate);

SignalPoint _Rectangle(SignalTime dCurTime, const SignalPoint amplitude, SignalTime dSetPeriodTime);

int generate_RT(enum eSIGNAL eSignal, SignalPoint sAmplitude, const unsigned long ulPeriod, const unsigned long ulSamplePeriod);
volatile bool _signal_generate; //timer flag, ready for new sample, false = timer ended
volatile bool _generator_ready; //flag generator finished new sample, false = ready (new sample ready)
volatile bool abortSig; //Signal to end in RT-mode, true = end
volatile SignalPoint generateOutBuf; //output buffer for single sample from generator