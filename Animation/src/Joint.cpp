#include "Joint.h"
#include "Cube.h"
#include "cmath"
#include <iostream>

Joint::Joint()
{
	offset = { 0.0f, 0.0f, 0.0f };
	boxmin = { -0.1f, -0.1f, -0.1f };
	boxmax = { 0.1f, 0.1f, 0.1f };
	L = glm::mat4(1.0f);
	W = glm::mat4(1.0f);
	DOF* DOFx = new DOF();
	DOF* DOFy = new DOF();
	DOF* DOFz = new DOF();
	JointDOF.push_back(DOFx);
	JointDOF.push_back(DOFy);
	JointDOF.push_back(DOFz);
	cube = new Cube();
	strcpy_s(JointName, "");
}

Joint::~Joint()
{
}

bool Joint::Load(Tokenizer* tknizer)
{
	float DOFmin, DOFmax;
	char name[256];
	tknizer->GetToken(name);
	strcpy_s(JointName, name);
	tknizer->FindToken("{");

	while (1)
	{
		char temp[256];
		tknizer->GetToken(temp);

		if (strcmp(temp, "offset") == 0)
		{
			offset.x = tknizer->GetFloat();
			offset.y = tknizer->GetFloat();
			offset.z = tknizer->GetFloat();
		}
		else if (strcmp(temp, "boxmin") == 0)
		{
			boxmin.x = tknizer->GetFloat();
			boxmin.y = tknizer->GetFloat();
			boxmin.z = tknizer->GetFloat();
		}
		else if (strcmp(temp, "boxmax") == 0)
		{
			boxmax.x = tknizer->GetFloat();
			boxmax.y = tknizer->GetFloat();
			boxmax.z = tknizer->GetFloat();
		}
		else if (strcmp(temp, "rotxlimit") == 0)
		{
			DOFmin = tknizer->GetFloat();
			DOFmax = tknizer->GetFloat();
			JointDOF[0]->SetMinMax(DOFmin, DOFmax);
		}
		else if (strcmp(temp, "rotylimit") == 0)
		{
			DOFmin = tknizer->GetFloat();
			DOFmax = tknizer->GetFloat();
			JointDOF[1]->SetMinMax(DOFmin, DOFmax);
		}
		else if (strcmp(temp, "rotzlimit") == 0)
		{
			DOFmin = tknizer->GetFloat();
			DOFmax = tknizer->GetFloat();
			JointDOF[2]->SetMinMax(DOFmin, DOFmax);
		}
		else if (strcmp(temp, "pose") == 0)
		{
			pose.x = tknizer->GetFloat();
			pose.y = tknizer->GetFloat();
			pose.z = tknizer->GetFloat();
			JointDOF[0]->SetValue(pose.x);
			JointDOF[1]->SetValue(pose.y);
			JointDOF[2]->SetValue(pose.z);
		}
		else if (strcmp(temp, "balljoint") == 0)
		{
			Joint* jnt = new Joint();
			jnt->Load(tknizer);
			AddChild(jnt);
		}
		else if (strcmp(temp, "}") == 0)
		{
			cube->buildCube(boxmin, boxmax);
			return true;
		}
		else
		{
			tknizer->SkipLine(); // Unrecognized token
		}
	}
	return false;
}

void Joint::ResetAll()
{
	JointDOF[0]->SetValue(pose.x);
	JointDOF[1]->SetValue(pose.y);
	JointDOF[2]->SetValue(pose.z);
	for (auto child : children) {
		child->ResetAll();
	}
}

void Joint::Update(glm::mat4 parentW)
{
	float thetaX = JointDOF[0]->GetValue();
	float thetaY = JointDOF[1]->GetValue();
	float thetaZ = JointDOF[2]->GetValue();

	glm::mat4 Rx = glm::mat4(
		glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
		glm::vec4(0.0f, cos(thetaX), sin(thetaX), 0.0f),
		glm::vec4(0.0f, -sin(thetaX), cos(thetaX), 0.0f),
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
	);
	glm::mat4 Ry = glm::mat4(
		glm::vec4(cos(thetaY), 0.0f, -sin(thetaY), 0.0f),
		glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
		glm::vec4(sin(thetaY), 0.0f, cos(thetaY), 0.0f),
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
	);
	glm::mat4 Rz = glm::mat4(
		glm::vec4(cos(thetaZ), sin(thetaZ), 0.0f, 0.0f),
		glm::vec4(-sin(thetaZ), cos(thetaZ), 0.0f, 0.0f),
		glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
	);
	glm::mat4 T = glm::mat4(
		glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
		glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
		glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
		glm::vec4(offset, 1.0f)
	);

	L = T * Rz * Ry * Rx;
	W = parentW * L;

	for (int i = 0; i < children.size(); i++) {
		children[i]->Update(W);
	}
}

void Joint::AddChild(Joint* newChild)
{
	children.push_back(newChild);
}

void Joint::Draw(const glm::mat4& viewProjMtx, GLuint shader)
{
	cube->drawCube(W, viewProjMtx, shader);
	for (int i = 0; i < children.size(); i++) {
		children[i]->Draw(viewProjMtx, shader);
	}
}

void Joint::BuildJointVector(std::vector<Joint*>* joints_ref)
{
	joints_ref->push_back(this);
	for (int i = 0; i < children.size(); i++) {
		children[i]->BuildJointVector(joints_ref);
	}
}
