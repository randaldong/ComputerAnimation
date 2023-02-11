#include "AnimationClip.h"

AnimationClip::AnimationClip()
{

}

AnimationClip::~AnimationClip()
{

}

bool AnimationClip::Load(const char* animfile)
{
	Tokenizer* tknizer = new Tokenizer();
	tknizer->Open(animfile);
	tknizer->FindToken("range");
	tStart = tknizer->GetFloat();
	tEnd = tknizer->GetFloat();
	tknizer->FindToken("numchannels");
	numChannels = tknizer->GetFloat();
	// parse eaach channel in this animation clip
	for (int i = 0; i < numChannels; i++) {
		Channel* chn = new Channel();
		chn->Load(tknizer);
		channels.push_back(chn);
	}
	tknizer->Close();
	Precompute();
	return true;
}

void AnimationClip::Precompute()
{
	for (int i = 0; i < numChannels; i++)
		channels[i]->Precompute();
}

void AnimationClip::Evaluate(float time, std::vector<float>& poses)
{
	for (int i = 0; i < numChannels; i++)
		poses[i] = channels[i]->Evaluate(time);
}
