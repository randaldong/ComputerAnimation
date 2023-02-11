#pragma once
#include "Keyframe.h"

class Channel {
public:
	char extpIn[256], extpOut[256];
	int numKeys;
	std::vector<Keyframe*> keyframes;

	Channel();
	~Channel();
	bool Load(Tokenizer* tknizer);
	void Precompute(); // compute tangents & cubic coefficients
	float Evaluate(float time);
	// compute # cycles between current time & 1st/last key's time in bounce extrapolation mode
	int ComputeCycleNum(float curTime, float keyTime, float duration);
};