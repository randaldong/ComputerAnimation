#pragma once
#include "Channel.h"

class AnimationClip {
public:
	float tStart, tEnd;
	int numChannels;
	std::vector<Channel*> channels;

	AnimationClip();
	~AnimationClip();
	bool Load(const char* animfile = "assets/wasp2_walk.anim");
	// each channel performs precomputation, can be performed right after loading
	void Precompute(); 
	// each channel evaluate a float pose value on a specific time; passing poses vector by reference
	void Evaluate(float time, std::vector<float>& poses); 
};