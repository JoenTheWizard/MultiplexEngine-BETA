#include "TerrainGenerate.h"
TerrainGenerate::TerrainGenerate()
{ }

TerrainGenerate::~TerrainGenerate()
{ }

unsigned char* TerrainGenerate::loadBMP(const char* filename)
{
   int i;
   FILE* f = fopen(filename, "rb");
   if (f == NULL)
      throw "Argument Exception";

   unsigned char info[54];
   fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

   // extract image height and width from header
   int width = *(int*)&info[18];
   int height = *(int*)&info[22];

   std::cout << std::endl;
   std::cout << "Heightmap Name: " << filename << std::endl;
   std::cout << " Width: " << width << std::endl;
   std::cout << "Height: " << height << std::endl;

   int row_padded = (width * 3 + 3) & (~3);
   unsigned char* data = new unsigned char[row_padded];
   unsigned char tmp;

   std::vector<float> tmpVec;
   for (int i = 0; i < height; i++)
   {
      tmpVec.clear();
      fread(data, sizeof(unsigned char), row_padded, f);
      for (int j = 0; j < width * 3; j += 3)
      {
         // Convert (B, G, R) to (R, G, B)
         tmp = data[j];
         data[j] = data[j + 2];
         data[j + 2] = tmp;

         //cout << "R: " << (int)data[j] << " G: " << (int)data[j + 1] << " B: " << (int)data[j + 2] << endl;
         tmpVec.push_back((float)data[j] / 255.0);
      }
      heightMap.push_back(tmpVec);
   }
   fclose(f);
   return data;
}

unsigned int TerrainGenerate::createTerrainVAO(float size, float h)
{
   std::vector<glm::vec3> renderHM;
   for (int x = 0; x < heightMap.size() - 1; x++) {
      for (int y = 0; y < heightMap[0].size() - 1; y++)
      {
         glm::vec3 p0 = glm::vec3(y * size, heightMap[x][y] * h, x * size);
         glm::vec3 p1 = glm::vec3((y + 1) * size, heightMap[x][y + 1] * h, x * size);
         glm::vec3 p2 = glm::vec3((y + 1) * size, heightMap[x + 1][y + 1] * h, (x + 1) * size);

         glm::vec3 p0_1 = glm::vec3(y * size, heightMap[x][y] * h, x * size);
         glm::vec3 p1_1 = glm::vec3((y + 1) * size, heightMap[x + 1][y + 1] * h, (x + 1) * size);
         glm::vec3 p2_1 = glm::vec3(y * size, heightMap[x + 1][y] * h, (x + 1) * size);

         renderHM.push_back(glm::vec3(y * size, heightMap[x][y] * h, x * size)); renderHM.push_back(glm::normalize(glm::cross((p1 - p0), (p2 - p0))));
         renderHM.push_back(glm::vec3((y + 1) * size, heightMap[x][y + 1] * h, x * size)); renderHM.push_back(glm::normalize(glm::cross((p1 - p0), (p2 - p0))));
         renderHM.push_back(glm::vec3((y + 1) * size, heightMap[x + 1][y + 1] * h, (x + 1) * size)); renderHM.push_back(glm::normalize(glm::cross((p1 - p0), (p2 - p0))));

         renderHM.push_back(glm::vec3(y * size, heightMap[x][y] * h, x * size)); renderHM.push_back(glm::normalize(glm::cross((p1_1 - p0_1), (p2_1 - p0_1))));
         renderHM.push_back(glm::vec3((y + 1) * size, heightMap[x + 1][y + 1] * h, (x + 1) * size)); renderHM.push_back(glm::normalize(glm::cross((p1_1 - p0_1), (p2_1 - p0_1))));
         renderHM.push_back(glm::vec3(y * size, heightMap[x + 1][y] * h, (x + 1) * size)); renderHM.push_back(glm::normalize(glm::cross((p1_1 - p0_1), (p2_1 - p0_1))));
      }
   }
   unsigned int hmVBO, hmVAO;
   glGenVertexArrays(1, &hmVAO);
   glGenBuffers(1, &hmVBO);
   glBindVertexArray(hmVAO);
   glBindBuffer(GL_ARRAY_BUFFER, hmVBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * renderHM.size(), &renderHM[0].x, GL_STATIC_DRAW);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
   hmSize = renderHM.size();
   return hmVAO;
}