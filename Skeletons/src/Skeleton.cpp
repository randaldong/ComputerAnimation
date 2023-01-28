#include "Skeleton.h"

Skeleton::Skeleton()
{
	root = NULL;
}

Skeleton::~Skeleton()
{
}

bool Skeleton::Load(const char* filename)
{
	Tokenizer* tknizer = new Tokenizer();
	tknizer->Open(filename);
	tknizer->FindToken("balljoint");

	root = new Joint();
	root->Load(tknizer);

	tknizer->Close();
	return true;
}

void Skeleton::Update(glm::mat4 parentW)
{
	root->Update(parentW);
}

void Skeleton::Draw(const glm::mat4& viewProjMtx, GLuint shader)
{
	root->Draw(viewProjMtx, shader);
}