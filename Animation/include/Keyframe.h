#pragma once
#include "core.h"
#include "Tokenizer.h"

class Keyframe {
public:
	float time;
	float value;
	float tanIn, tanOut; // tan values for fixed tangent mode
	// Tangent rules to compute tan values that are not given, stored in char[]
	char ruleIn[256], ruleOut[256]; 
	float a, b, c, d; // Cubic coefficients

	Keyframe();
	~Keyframe();
	bool Load(Tokenizer* tknizer);
};
