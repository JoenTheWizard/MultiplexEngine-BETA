#pragma once
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/detail/type_mat.hpp>
#include <glm/detail/type_vec.hpp>
#include <Shader.h>
class ParallaxMap
{
public:
   ParallaxMap();
   ~ParallaxMap();
   unsigned int CreateVAO();
   void Use();
   unsigned int quadVAO;
private:
   unsigned int quadVBO;
};

