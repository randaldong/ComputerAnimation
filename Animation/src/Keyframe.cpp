#include "Keyframe.h"

Keyframe::Keyframe()
{

}

Keyframe::~Keyframe()
{

}

bool Keyframe::Load(Tokenizer* tknizer)
{
	time = tknizer->GetFloat();
	value = tknizer->GetFloat();

	// tangent types could either be char[] or float
	tknizer->SkipWhitespace();
	char tanType = tknizer->CheckChar();
	if (97 <= tanType && tanType <= 142) {
		tknizer->GetToken(ruleIn);
		tknizer->GetToken(ruleOut);
	}
	else {
		tanIn = tknizer->GetFloat();
		tanOut = tknizer->GetFloat();
	}
	return true;
}