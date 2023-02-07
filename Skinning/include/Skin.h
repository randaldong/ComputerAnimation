#pragma once
#include "Vertex.h"
#include "Tokenizer.h"
#include <vector>
#include "Skeleton.h"

class Skin
{
public:
	int vertexNum;
	std::vector<Vertex*> vertices;
	// The skeleton associated with this skin; used for linking joints
	Skeleton* skeleton; 

	// shader-related
	GLuint VAO, VBO_positions, VBO_normals, EBO;
	std::vector<glm::vec3> bindingPositions;
	std::vector<glm::vec3> bindingNormals;
	std::vector<glm::vec3> shaderPositions;
	std::vector<glm::vec3> shaderNormals;
	std::vector<unsigned int> shaderIndices;



	Skin(Skeleton* skel);
	~Skin();

	void BindBuffer();
	bool Load(const char* filename = "assets/wasp.skin");
	void Update();
	void Draw(bool isDrawOriginalSkin, const glm::mat4& viewProjMtx, GLuint shader);

};