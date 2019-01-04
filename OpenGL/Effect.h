#pragma once

#include "../Object.h"
#include "Constants.h"
#include <cmath>

using namespace std;

class RelativityStack;

class Effect
{
protected:
public:
	Effect(void) { }

	virtual void compute(Object* obj, Object* source, Constants constants) {}
};