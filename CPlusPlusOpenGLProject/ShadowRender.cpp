#include "ShadowRender.h"
#include <cstddef>
ShadowRender::ShadowRender(const unsigned int width, const unsigned int height)
{
   WIDTH = width;
   HEIGHT = height;
   //Bind the depth buffer as FBO
   glGenFramebuffers(1, &depthMapFBO);
   //Making the depth texture
   glGenTextures(1, &depthMap);
   glBindTexture(GL_TEXTURE_2D, depthMap);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
   float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
   glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
   glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
   glDrawBuffer(GL_NONE);
   glReadBuffer(GL_NONE);
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
ShadowRender::~ShadowRender(){}

glm::mat4 ShadowRender::setLightSpaceMatrix(float near_plane, float far_plane, glm::vec3 lightPosition, glm::vec3 lightDirection)
{
   glm::mat4 lightProjection, lightView;
   lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
   lightView = glm::lookAt(lightPosition, glm::vec3(0.0f), lightDirection);
   return lightProjection * lightView;
}

void ShadowRender::UseBuffer()
{
   glViewport(0, 0, WIDTH, HEIGHT);
   glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
   glClear(GL_DEPTH_BUFFER_BIT);
}