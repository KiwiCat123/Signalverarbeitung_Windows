#pragma once
#include "Samples.h"
#include <stdbool.h>

#define FILTER_LENGTH 9
SIGNAL_OUT* filter(SIGNAL_OUT SignalInput[], unsigned long amount);
int filter_RT();
volatile bool _signal_out; //sample ready for output flag, false = ready (new sample ready)
volatile SignalPoint filterOutBuf; //output buffer for single sample from filter