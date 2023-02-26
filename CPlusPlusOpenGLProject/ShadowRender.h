#pragma once
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/detail/type_mat.hpp>
#include <glm/detail/type_vec.hpp>
class ShadowRender
{
public:
   ShadowRender(const unsigned int width, const unsigned int height);
   ~ShadowRender();
   unsigned int depthMapFBO;
   unsigned int depthMap;
   unsigned int WIDTH;
   unsigned int HEIGHT;
   glm::mat4 setLightSpaceMatrix(float near_plane, float far_plane, glm::vec3 lightPosition, glm::vec3 lightDirection);
   void UseBuffer();
};

