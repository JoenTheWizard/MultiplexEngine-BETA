#pragma once
#include <string>
#include <vector>
#include <cassert>
#include <iostream>
#include <glm/detail/type_vec.hpp>
#include <stb_image.h>

class TerrainTestDeleteLater
{
public:
	TerrainTestDeleteLater();
	~TerrainTestDeleteLater();

	void SetPossition(const glm::vec3& possition);
	void SetScale(const glm::vec3& scale);
	void SetRotation(const glm::vec3& rotation);
	void LoadFromHeightMap(const std::string& fileName);
	void Draw(glm::vec4& clip);

	glm::vec3 GetPossition();
	glm::vec3 GetScale();
	glm::vec3 GetRotation();
	float GetHeightOnPoint(float x, float z);

	float GetHeight(stbi_uc* image, size_t imageHeight, size_t imageWidth, size_t x, size_t y);
	glm::vec4 GetPixelRGBA(stbi_uc* image, size_t imageWidth, size_t x, size_t y);
	float BaryyCentric(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 poss);
};

