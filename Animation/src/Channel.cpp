#include "Channel.h"

Channel::Channel()
{

}

Channel::~Channel()
{

}

bool Channel::Load(Tokenizer* tknizer)
{
	tknizer->FindToken("extrapolate");
	tknizer->GetToken(extpIn);
	tknizer->GetToken(extpOut);

	tknizer->FindToken("keys");
	numKeys = tknizer->GetFloat();
	tknizer->FindToken("{");
	// parse each key in this channel
	for (int i = 0; i < numKeys; i++) {
		Keyframe* kf = new Keyframe();
		kf->Load(tknizer);
		keyframes.push_back(kf);
	}
	return true;
}

void Channel::Precompute()
{
	////////////////////////////////////////////////////
	// precompute tangentIn & tangentOut for each key //
	////////////////////////////////////////////////////
	// flat: tan = 0;
	// linear: tanIn points to previous key; tanOut points to next key
	// smooth: the current key is the point that satisfies Lagrange Mean Value Theorem
	//         with its previous and next keys being the boundaries. 
	//         But for the first and last key, just use the linear rule
	if (numKeys == 1) {
		keyframes[0]->tanIn = 0;
		keyframes[0]->tanOut = 0;
		return;
	}

	for (int i = 0; i < numKeys; i++) {
		// compute tanIn from ruleIn, could be "flat", "linear", "smooth"
		if (strcmp(keyframes[i]->ruleIn, "flat") == 0) {
			keyframes[i]->tanIn = 0;
		}
		else if (strcmp(keyframes[i]->ruleIn, "linear") == 0) {
			// the first key has no previous key, use its the second key's tanIn
			if (i == 0) 
				keyframes[i]->tanIn = (keyframes[i + 1]->value - keyframes[i]->value) / (keyframes[i + 1]->time - keyframes[i]->time);
			else 
				keyframes[i]->tanIn = (keyframes[i]->value - keyframes[i - 1]->value) / (keyframes[i]->time - keyframes[i - 1]->time);
		}
		else if (strcmp(keyframes[i]->ruleIn, "smooth") == 0) {
			if (i == 0)
				keyframes[i]->tanIn = (keyframes[i + 1]->value - keyframes[i]->value) / (keyframes[i + 1]->time - keyframes[i]->time);
			else if (i == numKeys - 1)
				keyframes[i]->tanIn = (keyframes[i]->value - keyframes[i - 1]->value) / (keyframes[i]->time - keyframes[i - 1]->time);
			else
				keyframes[i]->tanIn = (keyframes[i + 1]->value - keyframes[i - 1]->value) / (keyframes[i + 1]->time - keyframes[i - 1]->time);
		}

		// compute tanOut from ruleOut, could be "flat", "linear", "smooth"
		if (strcmp(keyframes[i]->ruleOut, "flat") == 0) {
			keyframes[i]->tanOut = 0;
		}
		else if (strcmp(keyframes[i]->ruleOut, "linear") == 0) {
			// the last key has no next key, use previous key's tanOut
			if (i == numKeys - 1)
				keyframes[i]->tanOut = (keyframes[i]->value - keyframes[i - 1]->value) / (keyframes[i]->time - keyframes[i - 1]->time);

			else
				keyframes[i]->tanOut = (keyframes[i + 1]->value - keyframes[i]->value) / (keyframes[i + 1]->time - keyframes[i]->time);
		}
		else if (strcmp(keyframes[i]->ruleOut, "smooth") == 0) {
			if (i == 0)
				keyframes[i]->tanOut = (keyframes[i + 1]->value - keyframes[i]->value) / (keyframes[i + 1]->time - keyframes[i]->time);
			else if (i == numKeys - 1)
				keyframes[i]->tanOut = (keyframes[i]->value - keyframes[i - 1]->value) / (keyframes[i]->time - keyframes[i - 1]->time);
			else
				keyframes[i]->tanOut = (keyframes[i + 1]->value - keyframes[i - 1]->value) / (keyframes[i + 1]->time - keyframes[i - 1]->time);
		}
	}

	///////////////////////////////////
	// precompute cubic coefficients //
	///////////////////////////////////
	// 1. scale tangent
	// 2. the current key stores coefficients of the curve between itself and the next key
	//    so the last key does nor have this type of data
	for (int i = 0; i < numKeys - 1; i++) {
		glm::vec4 g;
		g.x = keyframes[i]->value;
		g.y = keyframes[i + 1]->value;
		g.z = (keyframes[i + 1]->time - keyframes[i]->time) * keyframes[i]->tanOut;
		g.w = (keyframes[i + 1]->time - keyframes[i]->time) * keyframes[i+1]->tanIn;

		keyframes[i]->a = glm::dot(glm::vec4(2.0f, -2.0f, 1.0f, 1.0f), g);
		keyframes[i]->b = glm::dot(glm::vec4(-3.0f, 3.0f, -2.0f, -1.0f), g);
		keyframes[i]->c = g.z;
		keyframes[i]->d = g.x;
	}
}

float Channel::Evaluate(float time)
{
	float evalValue = 0;
	float duration = keyframes[numKeys - 1]->time - keyframes[0]->time; // time duration
	float deltaVal = keyframes[numKeys - 1]->value - keyframes[0]->value; // delta value in a duration
	// 1. find the proper span; 2. evaluate cubic equation for the span
	//    Four cases for time: 
	//    1.1 on some key: use this key's value
	//	  1.2 before the first key: use extpIn ("constant", "linear", "cycle", "cycle_offset", "bounce")
	//    1.3 after the last key: use extpOut ("constant", "linear", "cycle", "cycle_offset", "bounce")
	//    1.4 within 1st and last keys and between 2 internal keys: narrow the span and evaluate the cubic
	
	// 1.1 on some key: use this key's value
	for (int i = 0; i < numKeys; i++) {
		if (time == keyframes[i]->time) {
			evalValue = keyframes[i]->value;
			return evalValue;
		}
	}

	// 1.2 before the first key: use extpIn
	if (time < keyframes[0]->time) {
		if (strcmp(extpIn, "constant") == 0)
			evalValue = keyframes[0]->value;
		else if (strcmp(extpIn, "linear") == 0)
			evalValue = keyframes[0]->value - keyframes[0]->tanIn * (keyframes[0]->time - time);
		else if (strcmp(extpIn, "cycle") == 0)
			// evaluate (time + duration); translate current time by one duration at a time
			evalValue = Evaluate(time + duration);
		else if (strcmp(extpIn, "cycle_offset") == 0)
			// the whole curve move down by deltaVal
			evalValue = Evaluate(time + duration) - deltaVal;
		else if (strcmp(extpIn, "bounce") == 0) {
			// have to check whether the curve is flipped, use # cycles between current time and 1st key's time
			// odd: not flipped; even: flipped
			int numCycles = ComputeCycleNum(time, keyframes[0]->time, duration);
			if (numCycles % 2 == 1)
				// lag odd number of cycles, curve is not flipped
				evalValue = Evaluate(time + duration * (numCycles + 1));
			else
				// lag even number of cycles, curve is flipped
				evalValue = Evaluate(2 * keyframes[0]->time - (time + duration * numCycles));
		}
	}

	// 1.3 after the last key: use extpOut
	else if (time > keyframes[numKeys - 1]->time) {
		if (strcmp(extpOut, "constant") == 0)
			evalValue = keyframes[numKeys - 1]->value;
		else if (strcmp(extpOut, "linear") == 0)
			evalValue = keyframes[numKeys - 1]->value + keyframes[numKeys - 1]->tanOut * (time - keyframes[numKeys - 1]->time);
		else if (strcmp(extpOut, "cycle") == 0)
			// evaluate (time - duration); translate current time by one duration at a time
			evalValue = Evaluate(time - duration);
		else if (strcmp(extpOut, "cycle_offset") == 0)
			// the whole curve move up by deltaVal
			evalValue = Evaluate(time - duration) + deltaVal;
		else if (strcmp(extpOut, "bounce") == 0) {
			// have to check whether the curve is flipped, use # cycles between current time and last key's time
			// odd: not flipped; even: flipped
			int numCycles = ComputeCycleNum(time, keyframes[numKeys - 1]->time, duration);
			if (numCycles % 2 == 1)
				// ahead odd number of cycles, curve is not flipped
				evalValue = Evaluate(time - duration * (numCycles + 1));
			else
				// ahead even number of cycles, curve is flipped
				evalValue = Evaluate(2 * keyframes[numKeys - 1]->time - (time - duration * numCycles));
		}
	}

	// 1.4 between 2 keys: narrow span & evaluate cubic
	else {
		// binary search to narrow the span so that time falls in [left, right] and right - left = 1
		int left = 0, right = numKeys - 1;
		while (right - left > 1) {
			int mid = left + (right - left) / 2;
			if (time < keyframes[mid]->time)
				right = mid;
			else if (time > keyframes[mid]->time)
				left = mid;
		}
		float u = (time - keyframes[left]->time) / (keyframes[right]->time - keyframes[left]->time);
		// coefficients of the curve between key left and key right is stored in key left
		evalValue = keyframes[left]->d + u * (keyframes[left]->c + u * (keyframes[left]->b + u * (keyframes[left]->a)));
	}

	return evalValue;
}

int Channel::ComputeCycleNum(float curTime, float keyTime, float duration)
{
	int count = 0;
	if (curTime < keyTime) {
		while (curTime < keyTime){
			curTime += duration;
			count++;
		}
		count--; // the last addition has made curTime > keyTime, need to undo it
	}
	else {
		while (curTime > keyTime) {
			curTime -= duration;
			count++;
		}
		count--; // the last subtraction has made curTime < keyTime, need to undo it
	}
	return count;
}
