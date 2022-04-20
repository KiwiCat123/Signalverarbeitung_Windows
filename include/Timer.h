#pragma once
#include <Windows.h>
#include <stdbool.h>

#define PERIOD 10 //timer period in ms

void timer_fnc(); //start and control timer

HANDLE Timer_Handle; //Handle to timer
HANDLE Timer_Semaphore; //Semaphore to activate Filter, Generator
volatile bool _signal_generate; //timer flag, ready for new sample, false = timer ended