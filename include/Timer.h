#pragma once
#include <Windows.h>
#include <stdbool.h>

#define PERIOD 100 //timer period in ms

void timer_fnc(); //start and control timer

HANDLE Timer_Handle;
volatile bool _signal_generate; //timer flag, ready for new sample, false = timer ended