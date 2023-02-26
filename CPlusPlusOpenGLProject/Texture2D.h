#pragma once
#include <stb_image.h>
#include <glad/glad.h>
#include <iostream>

class Texture2D
{
public:
   int width, height, nrChannels;
   Texture2D(const char* fileName, bool mipmapNearest, bool isRGBA);
   ~Texture2D();
   void Use();
private:
   unsigned int texture2D;
};

