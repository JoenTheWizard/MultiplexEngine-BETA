#pragma once
#pragma warning (disable : 4996)
#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <glm/detail/type_vec.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
class TerrainGenerate
{
public:
   TerrainGenerate();
   ~TerrainGenerate();
   unsigned char* loadBMP(const char* filename);
   unsigned int createTerrainVAO(float size, float h);
   std::vector<std::vector<float>> heightMap;
   int hmSize = 0;
};

