#include "AnimationPlayer.h"

AnimationPlayer::AnimationPlayer(AnimationClip* Clip, AnimRig* Rig)
{
	clip = Clip;
	rig = Rig;
	tStart = clip->tStart;
	tEnd = clip->tEnd;
	curTime = tStart;
	
	// initialize poses to 0s so that index can be used later
	for (int i = 0; i < (3 * rig->skeleton->joints.size() + 3); i++)
		poses.push_back(0.0f);
}

AnimationPlayer::~AnimationPlayer()
{

}

void AnimationPlayer::Update()
{
	// evaluates current poses
	clip->Evaluate(curTime, poses);

	// set these poses to joints, first 3 poses are root translations
	rootTranslation = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		poses[0], poses[1], poses[2], 1.0f
	};

	for (int i = 0; i < rig->skeleton->joints.size(); i++) {
		rig->skeleton->joints[i]->JointDOF[0]->DOFvalue = poses[3 + i * 3];
		rig->skeleton->joints[i]->JointDOF[1]->DOFvalue = poses[3 + i * 3 + 1];
		rig->skeleton->joints[i]->JointDOF[2]->DOFvalue = poses[3 + i * 3 + 2];
	}

	// increments current time
	// set play mode (what to do after end of clip)
	if (strcmp(playMode, "To infinity!") == 0) 
		// always increment time
		curTime += deltaT * playSpeed;
	else if (strcmp(playMode, "Loop from start") == 0) {
		// increment time when not reach the end
		// back to start after reaching the end
		if (curTime <= tEnd)
			curTime += deltaT * playSpeed;
		else
			curTime = tStart;
	}
	else if (strcmp(playMode, "Stop at end") == 0) {
		// only increment time when not reach the end
		// do nothing after reaching the end
		if (curTime <= tEnd)
			curTime += deltaT * playSpeed;
	}
	else if (strcmp(playMode, "Walk back and forth") == 0) {
		bool isWalkBack = false;
		curTime += deltaT * playSpeed;
		if (curTime > tEnd) {
			deltaT *= -1;
		}
		else if (curTime < tStart) {
			deltaT *= -1;
		}
	}
}