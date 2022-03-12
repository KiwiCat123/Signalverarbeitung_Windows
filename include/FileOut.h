#pragma once
#include "Signal.h"
#include <stdbool.h>
#include <Windows.h>
#include <time.h>

int writeCSV(SIGNAL_OUT SignalGenerator[], SIGNAL_OUT SignalFiltered[], unsigned long count, char path[]);

void consoleOut();