//DebugViewer.h
#ifndef DEBUGVIEWER_H
#define DEBUGVIEWER_H
#include "stdafx.h"
#include "Utsimo.h"

class Utsimo;

class DebugViewer{
public:
	DebugViewer(Utsimo* uts);
	void init();
	void step();
	float lastStepTime;
	float stepLength;

	Utsimo* utsimo;
};

#endif //DEBUGVIEWER_H