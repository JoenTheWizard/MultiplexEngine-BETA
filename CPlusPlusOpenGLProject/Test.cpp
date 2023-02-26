#include <iostream>
#include <glad/glad.h>
#include <windows.h>
#include <thread>
#include <fstream>

#include "assimp/config.h"

#include <Shader.h>
#include "Camera.h"
#include <PhysicsWorld.h>
#include <Model.h>
#include <Texture2D.h>

#include <imgui.h>
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <stb_image.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <ShadowRender.h>
#include <ParallaxMap.h>

//#include "btBulletDynamicsCommon.h"
//#include "btBulletCollisionCommon.h"
#pragma warning (disable : 4996)


float globFloat = 0;
float globFloatY = 0;
int clickedAmount = 0;
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
   glViewport(0, 0, width, height);
}
//deltatime/ timing
float deltaTime = 0.0f;
float lastFraming = 0.0f;

//Camera class initialization
Camera cameraGame(glm::vec3(0.0f, 0.0f, 5.0f));

//Global bool to check if this is the first time to move mouse
bool isFirstTimeMove = true;
//Camera Euler Angles
float lastX = 400;
float lastY = 300;

bool button_pressed = false;
float secondsA = 0;

bool flashLight = false;

bool isClipping = false;
bool isSprinting = false;

//Bullet Physics world
//btDynamicsWorld* world;
//btDispatcher* dispatcher;
//btCollisionConfiguration* collisionConfig;
//btBroadphaseInterface* broadphase;
//btConstraintSolver* solver;
//vector<btRigidBody*> rBodies;
VOID CALLBACK TimerProc(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime)
{
   //std::cout << "CALLBACK " << dwTime << '\n';
   std::cout.flush();
   if (button_pressed)
      secondsA += deltaTime;
}

void isPress()
{
   button_pressed = true;
}

void resetIsPres()
{
   button_pressed = false;
   secondsA = 0.0f;
}
//Loading cube map
unsigned int loadCubeMap(std::vector<string> faces)
{
   unsigned int textureID;
   glGenTextures(1, &textureID);
   glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

   int width, height, nrChannels;
   for (unsigned int i = 0; i < faces.size(); i++)
   {
      unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
      if (data)
      {
         glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
         stbi_image_free(data);
      }
      else {
         cout << "Cube map texture failed to load at path: " << faces[i] << endl;
         stbi_image_free(data);
      }
   }
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

   return textureID;
}
//Spotlight shader setup
bool flashlightOn = false;
void SetupSpotlight(Shader shader)
{  
   shader.setBool("spotLight.isFlashlightOn", flashlightOn);
   shader.setFloat3("spotLight.direction", cameraGame.Front.x, cameraGame.Front.y, cameraGame.Front.z);
   //Spot light lighting settings
   shader.setFloat3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
   shader.setFloat3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
   shader.setFloat3("spotLight.specular", 1.0f, 1.0f, 1.0f);
   //Spot light attenuation
   shader.setFloat("spotLight.constant", 1.0f);
   shader.setFloat("spotLight.linear", 0.09);
   shader.setFloat("spotLight.quadratic", 0.032);
   //Spot light soft edging
   shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
   shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
}
unsigned int skyBoxVAO()
{
   float skyboxVertices[] = {
      // positions          
      -1.0f,  1.0f, -1.0f,
      -1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
       1.0f,  1.0f, -1.0f,
      -1.0f,  1.0f, -1.0f,

      -1.0f, -1.0f,  1.0f,
      -1.0f, -1.0f, -1.0f,
      -1.0f,  1.0f, -1.0f,
      -1.0f,  1.0f, -1.0f,
      -1.0f,  1.0f,  1.0f,
      -1.0f, -1.0f,  1.0f,

       1.0f, -1.0f, -1.0f,
       1.0f, -1.0f,  1.0f,
       1.0f,  1.0f,  1.0f,
       1.0f,  1.0f,  1.0f,
       1.0f,  1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,

      -1.0f, -1.0f,  1.0f,
      -1.0f,  1.0f,  1.0f,
       1.0f,  1.0f,  1.0f,
       1.0f,  1.0f,  1.0f,
       1.0f, -1.0f,  1.0f,
      -1.0f, -1.0f,  1.0f,

      -1.0f,  1.0f, -1.0f,
       1.0f,  1.0f, -1.0f,
       1.0f,  1.0f,  1.0f,
       1.0f,  1.0f,  1.0f,
      -1.0f,  1.0f,  1.0f,
      -1.0f,  1.0f, -1.0f,

      -1.0f, -1.0f, -1.0f,
      -1.0f, -1.0f,  1.0f,
       1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
      -1.0f, -1.0f,  1.0f,
       1.0f, -1.0f,  1.0f
   };
   unsigned int skyboxVAO, skyboxVBO;
   glGenVertexArrays(1, &skyboxVAO);
   glGenBuffers(1, &skyboxVBO);
   glBindVertexArray(skyboxVAO);
   glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
   return skyboxVAO;
}
//Text buffer for GUI (and other GUI settings)
char guiBuf[512] = {};
bool isMouseVisible = false;
bool showShadowBuffer = false;
bool showGBuffer = false;
int indexGUI = 1;
float waterVelocity = 0.03;
//Global vals for weapon
float rWep = 0.0;
float gWep = 0.3;
float bWep = 0.6;
//Gamma
float gamma = 1.0f;
vector<string> splitStr(string s, string delimiter) {
   size_t pos_start = 0, pos_end, delim_len = delimiter.length();
   string token;
   vector<string> res;

   while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
      token = s.substr(pos_start, pos_end - pos_start);
      pos_start = pos_end + delim_len;
      res.push_back(token);
   }

   res.push_back(s.substr(pos_start));
   return res;
}

void processInput(GLFWwindow* window)
{
   //sprint
   float speed;
   if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
      isSprinting = true;
      speed = 4.0f * deltaTime;
   }
   else {
      isSprinting = false;
      speed = deltaTime;
   }
   if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);

   //Movement for camera
   if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
      cameraGame.ProcessKeyboard(FORWARD, speed);
   if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
      cameraGame.ProcessKeyboard(BACKWARD, speed);
   if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
      cameraGame.ProcessKeyboard(LEFT, speed);
   if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
      cameraGame.ProcessKeyboard(RIGHT, speed);
   if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
      cameraGame.ProcessKeyboard(UP, speed);
   if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
      cameraGame.ProcessKeyboard(DOWN, speed);
   if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
      glfwSetTime(0);
}

const char* vertexShaderSource = "#version 330 core\n" \
                                 "layout (location = 0) in vec3 aPos;\n" \
                                 "out vec3 colorVec;\n" \
                                 "void main()" \
                                 "{\n" \
                                 "colorVec = -1.5*sin(aPos);\n" \
                                 "gl_Position = vec4(aPos.x,aPos.y,aPos.z,1.0f);\n" \
                                 "}\0";

const char* fragmentShaderSource = "#version 330 core\n" \
                                    "out vec4 FragColor;\n" \
                                    "uniform vec4 myCol;\n" \
                                    "void main()" \
                                    "{\n" \
                                    "vec3 myColor = vec3(.0f, 0.2f, 0.67f);" \
                                    "FragColor = myCol;\n" \
                                    "}\0";

const char* fragmentShaderSource1 = "#version 330 core\n" \
                                    "out vec4 FragColor;\n" \
                                    "in vec3 colorVec;\n" \
                                    "void main()" \
                                    "{\n" \
                                    "vec3 myColor = vec3(1.0f, 1.0f, 0.0f);" \
                                    "FragColor = vec4(colorVec, 1.0f);\n" \
                                    "}\0";
//Read BMP File (HEIGHTMAP TESTING)
vector<vector<float>> heightMap;
unsigned char* readBMP(const char* filename)
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

   cout << endl;
   cout << "  Name: " << filename << endl;
   cout << " Width: " << width << endl;
   cout << "Height: " << height << endl;

   int row_padded = (width * 3 + 3) & (~3);
   unsigned char* data = new unsigned char[row_padded];
   unsigned char tmp;

   vector<float> tmpVec;
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
         tmpVec.push_back((float)data[j]/255.0);
      }
      heightMap.push_back(tmpVec);
   }
   fclose(f);
   return data;
}
int hmSize = 0;
void getTangentAndBitangent(glm::vec3 *tangent, glm::vec3 *bitangent, glm::vec3 *tangent2, glm::vec3 *bitangent2,glm::vec3 p0,
   glm::vec3 p1, glm::vec3 p2, glm::vec3 p2_1) {
   //First triangle
   glm::vec3 edge1 = p1 - p0;
   glm::vec3 edge2 = p2 - p0;
   glm::vec3 dUV1 = glm::vec3(1, 1, 0) - glm::vec3(0, 1, 0);
   glm::vec3 dUV2 = glm::vec3(1, 0, 0) - glm::vec3(0, 1, 0);
   float f = 1.0f / (dUV1.x * dUV2.y - dUV2.x * dUV1.y);
   tangent->x = f * (dUV2.y * edge1.x - dUV1.y * edge2.x);
   tangent->y = f * (dUV2.y * edge1.y - dUV1.y * edge2.y);
   tangent->z = f * (dUV2.y * edge1.z - dUV1.y * edge2.z);

   bitangent->x = f * (-dUV2.x * edge1.x + dUV1.x * edge2.x);
   bitangent->y = f * (-dUV2.x * edge1.y + dUV1.x * edge2.y);
   bitangent->z = f * (-dUV2.x * edge1.z + dUV1.x * edge2.z);

   //Second triangle
   edge1 = p2 - p0;
   edge2 = p2_1 - p0;
   dUV1 = glm::vec3(1, 0, 0) - glm::vec3(0, 1, 0);
   dUV2 = glm::vec3(0, 0, 0) - glm::vec3(0, 1, 0);
   f = 1.0f / (dUV1.x - dUV2.y - dUV2.x * dUV1.y);
   tangent2->x = f * (dUV2.y * edge1.x - dUV1.y * edge2.x);
   tangent2->y = f * (dUV2.y * edge1.y - dUV1.y * edge2.y);
   tangent2->z = f * (dUV2.y * edge1.z - dUV1.y * edge2.z);

   bitangent2->x = f * (-dUV2.x*edge1.x+dUV1.x*edge2.x);
   bitangent2->y = f * (-dUV2.x*edge1.y+dUV1.x*edge2.y);
   bitangent2->z = f * (-dUV2.x*edge1.z+dUV1.x*edge2.z);
}
//For terrain collision
unsigned int setupHeightMapVAO(float size, float h)
{
   vector<glm::vec3> renderHM;
   for (int x = 0; x < heightMap.size() - 1; x++) {
      for (int y = 0; y < heightMap[0].size()-1; y++)
      {
         glm::vec3 p0 = glm::vec3(y * size, heightMap[x][y] * h, x * size); //pos1
         glm::vec3 p1 = glm::vec3((y + 1) * size, heightMap[x][y + 1] * h, x * size); //pos2
         glm::vec3 p2 = glm::vec3((y + 1) * size, heightMap[x + 1][y + 1] * h, (x + 1) * size); //pos3

         glm::vec3 p0_1 = glm::vec3(y * size, heightMap[x][y] * h, x * size);
         glm::vec3 p1_1 = glm::vec3((y + 1) * size, heightMap[x + 1][y + 1] * h, (x + 1) * size);
         glm::vec3 p2_1 = glm::vec3(y * size, heightMap[x + 1][y] * h, (x + 1) * size); //pos4
         
         //Tangent and Bitangent calculations
         glm::vec3 tangent1, bitangent1;
         glm::vec3 tangent2, bitangent2;
         getTangentAndBitangent(&tangent1, &bitangent1,&tangent2,&bitangent2,p0, p1, p2,p2_1);
         glm::vec3 nm = glm::vec3(0, 0, 1);
         //First triangle
         renderHM.push_back(p0); //pos
         renderHM.push_back(nm); // normals
         renderHM.push_back(glm::vec3(0,1,0)); // textures
         renderHM.push_back(tangent1); //tangents
         renderHM.push_back(bitangent1); //bitangents

         renderHM.push_back(p1);
         renderHM.push_back(nm);
         renderHM.push_back(glm::vec3(1, 1, 0));
         renderHM.push_back(tangent1);
         renderHM.push_back(bitangent1);

         renderHM.push_back(p2);
         renderHM.push_back(nm);
         renderHM.push_back(glm::vec3(1, 0, 0));
         renderHM.push_back(tangent1);
         renderHM.push_back(bitangent1);

         //Second triangle
         renderHM.push_back(p0_1);
         renderHM.push_back(nm);
         renderHM.push_back(glm::vec3(0, 1, 0));
         renderHM.push_back(tangent2);
         renderHM.push_back(bitangent2);

         renderHM.push_back(p1_1);
         renderHM.push_back(nm);
         renderHM.push_back(glm::vec3(1, 0, 0));
         renderHM.push_back(tangent2);
         renderHM.push_back(bitangent2);

         renderHM.push_back(p2_1);
         renderHM.push_back(nm);
         renderHM.push_back(glm::vec3(0, 0, 0));
         renderHM.push_back(tangent2);
         renderHM.push_back(bitangent2);
      }
   }
   unsigned int hmVBO, hmVAO;
   glGenVertexArrays(1, &hmVAO);
   glGenBuffers(1, &hmVBO);
   glBindVertexArray(hmVAO);
   glBindBuffer(GL_ARRAY_BUFFER, hmVBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * renderHM.size(), &renderHM[0].x, GL_STATIC_DRAW);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 15 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,15*sizeof(float), (void*)(3*sizeof(float)));
   glEnableVertexAttribArray(2);
   glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 15 * sizeof(float), (void*)(6 * sizeof(float)));
   glEnableVertexAttribArray(3);
   glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 15 * sizeof(float), (void*)(9 * sizeof(float)));
   glEnableVertexAttribArray(4);
   glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 15 * sizeof(float), (void*)(12 * sizeof(float)));
   hmSize = renderHM.size();
   return hmVAO; 
}
//End of heightmap

//Plane VAO for Shadows
unsigned int SetplaneVAO()
{
   unsigned int planeVAO;
   float planeVertices[] = {
      // positions            // normals         // texcoords
       25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
      -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
      -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

       25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
      -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
       25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
   };
   // plane VAO
   unsigned int planeVBO;
   glGenVertexArrays(1, &planeVAO);
   glGenBuffers(1, &planeVBO);
   glBindVertexArray(planeVAO);
   glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
   glEnableVertexAttribArray(2);
   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
   glBindVertexArray(0);
   return planeVAO;
}

unsigned int gridVAO()
{
   vector<glm::vec3> verts;
   for (int x = 0; x < 32; x++) {
      for (int y = 0; y < 32; y++)
      {
         glm::vec3 p0 = glm::vec3(x, sin(y), y);
         glm::vec3 p1 = glm::vec3(x + 1, sin(y), y);
         glm::vec3 p2 = glm::vec3(x + 1, sin(y + 1), y + 1);

         glm::vec3 p0_1 = glm::vec3(x, sin(y), y);
         glm::vec3 p1_1 = glm::vec3(x + 1, sin(y + 1), y + 1);
         glm::vec3 p2_1 = glm::vec3(x, sin(y + 1), y + 1);

         verts.push_back(glm::vec3(x, sin(y), y)); verts.push_back(glm::normalize(glm::cross((p1 - p0), (p2 - p0))));
         verts.push_back(glm::vec3(x + 1, sin(y), y)); verts.push_back(glm::normalize(glm::cross((p1 - p0), (p2 - p0))));
         verts.push_back(glm::vec3(x + 1, sin(y+1), y + 1)); verts.push_back(glm::normalize(glm::cross((p1 - p0), (p2 - p0))));

         verts.push_back(glm::vec3(x, sin(y), y)); verts.push_back(glm::normalize(glm::cross((p1_1 - p0_1), (p2_1 - p0_1))));
         verts.push_back(glm::vec3(x + 1, sin(y+1), y + 1)); verts.push_back(glm::normalize(glm::cross((p1_1 - p0_1), (p2_1 - p0_1))));
         verts.push_back(glm::vec3(x, sin(y+1), y + 1)); verts.push_back(glm::normalize(glm::cross((p1_1 - p0_1), (p2_1 - p0_1))));
      }
   }
   unsigned int hmVBO, hmVAO;
   glGenVertexArrays(1, &hmVAO);
   glGenBuffers(1, &hmVBO);
   glBindVertexArray(hmVAO);
   glBindBuffer(GL_ARRAY_BUFFER, hmVBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * verts.size(), &verts[0].x, GL_STATIC_DRAW);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
   cout << verts.size() << endl;
   return hmVAO;
}

//Mouse Movement
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
   if (isFirstTimeMove)
   {
      lastX = xpos;
      lastY = ypos;
      isFirstTimeMove = false;
   }
   float xOffSet = xpos - lastX;
   float yOffSet = lastY - ypos;

   lastX = xpos;
   lastY = ypos;

   cameraGame.ProcessMouseMovement(xOffSet, yOffSet);
}

void scroll_callback(GLFWwindow* window, double xpos, double ypos)
{
   cameraGame.ProcessMouseScroll(ypos);
}

float get_resolution(GLFWwindow* window) {
   int window_width;
   int window_height;
   glfwGetWindowSize(window, &window_width, &window_height);

   return (float)window_width / (float)window_height;
}
void GetDesktopResolution(int& horizontal, int& vertical)
{
   RECT desktop;
   // Get a handle to the desktop window
   const HWND hDesktop = GetDesktopWindow();
   // Get the size of screen to the variable desktop
   GetWindowRect(hDesktop, &desktop);
   // The top left corner will have coordinates (0,0)
   // and the bottom right corner will have coordinates
   // (horizontal, vertical)
   horizontal = desktop.right;
   vertical = desktop.bottom;
}
int main(int argc, char* argv[])
{  
   //Start of GLFW Configuration
   glfwInit();
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   //glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); //Set to borderless
   int horzScreen = 0;
   int vertScreen = 0;
   GetDesktopResolution(horzScreen,vertScreen);
   //borderless fullscreen
   GLFWwindow* window = glfwCreateWindow(horzScreen,vertScreen, "Multiplex Engine", NULL, NULL);
   if (window == NULL)
   {
      std::cout << "There was an error with loading the window!" << std::endl;
      glfwTerminate();
      return -1;
   }
   glfwMakeContextCurrent(window);
   glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
   //Mouse call back event handler
   glfwSetCursorPosCallback(window, mouse_callback);
   glfwSetScrollCallback(window, scroll_callback);
   //Disables mouse
   glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //GLFW_CURSOR_DISABLED

   if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
   {
     std::cout << "Failed to load GLAD!" << std::endl;
     return -1;
   }
   //End of GLFW Configuration

   // ==== SECTION 1 - DRAWING TRIANGLE ====

   //Assigning the vertex Shader
   unsigned int vertexShader;
   vertexShader = glCreateShader(GL_VERTEX_SHADER); //Uses GL_VERTEX_SHADER for the vertex shader

   //Compiling the vertex Shader
   glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
   glCompileShader(vertexShader);

   //Assigning the fragment shader
   unsigned int fragmentShader;
   fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); //Uses GL_FRAGMENT_SHADER for fragment shader

   //Compiling the fragment Shader
   glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
   glCompileShader(fragmentShader);

   //Shader Program to combine both the fragment Shader and vertex Shader
   unsigned int shaderProgram;
   shaderProgram = glCreateProgram();

   //Attach and link the shaders to the program
   glAttachShader(shaderProgram, vertexShader);
   glAttachShader(shaderProgram, fragmentShader);
   glLinkProgram(shaderProgram);

   //Deletes the shaders after link for buffer recall
   glDeleteShader(vertexShader);
   glDeleteShader(fragmentShader);

   // Excercise for second shader
   unsigned int fragmentShaderSecond;
   fragmentShaderSecond = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fragmentShaderSecond, 1, &fragmentShaderSource1, NULL);
   glCompileShader(fragmentShaderSecond);

   unsigned int shaderProgram2;
   shaderProgram2 = glCreateProgram();
   glAttachShader(shaderProgram2, vertexShader);
   glAttachShader(shaderProgram2, fragmentShaderSecond);
   glLinkProgram(shaderProgram2);

   //======= Set up vertex data (and buffer(s)) and configure vertex attributes =========

   float vertices[] = {
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
       -0.5f, -0.5f, 0.0f, // bottom left

       -0.5f,  0.5f, 0.0f   // top left 
       -0.5f,  0.5f, 0.0f   // top left 
       -0.5f,  0.5f, 0.0f   // top left 
   };
   unsigned int indices[] = {  // note that we start from 0!
       0, 1, 3,  // first Triangle
       1, 2, 3   // second Triangle
   };

   float secondTriangle[] = {
       -0.15f, 0.5f, 0.0f,  // left
       -0.35f, 0.0f, 0.0f,  // right
       0.0f, 0.0f, 0.0f,   // top 
   };

   //Element Buffer Object assign
   
   //End of EBO assignment
   //Assigning the Vertex Buffer Object and Vertex Array Object
   unsigned int VBO, VAO, EBO;
   glGenVertexArrays(1, &VAO);
   glGenBuffers(1, &VBO);
   glGenBuffers(1, &EBO);
   // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
   glBindVertexArray(VAO);

   glBindBuffer(GL_ARRAY_BUFFER, VBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

   //Element Buffer Object, similar to VBOs but is used for division in objects
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);

   //Note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound 
   //vertex buffer object so afterwards we can safely unbind
   glBindBuffer(GL_ARRAY_BUFFER, 0);

   //Remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
   //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

   // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
   // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
   glBindVertexArray(0);

   unsigned int VBO1, VAO1;
   glGenVertexArrays(1, &VAO1);
   glGenBuffers(1, &VBO1);
   // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
   glBindVertexArray(VAO1);

   glBindBuffer(GL_ARRAY_BUFFER, VBO1);
   glBufferData(GL_ARRAY_BUFFER, sizeof(secondTriangle), secondTriangle, GL_STATIC_DRAW);

   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);

   //Note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound 
   //vertex buffer object so afterwards we can safely unbind
   glBindBuffer(GL_ARRAY_BUFFER, 0);

   //Remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
   //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

   // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
   // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.

   // ================================ INTERPOLATION ==================================

   Shader ourShader("D:/CPlusPlusOpenGL/Shaders/vertexShader.vs", "D:/CPlusPlusOpenGL/Shaders/fragmentShader.fs");
   Shader gridShader("D:/CPlusPlusOpenGL/Shaders/GridShaders/gridVert.vs", "D:/CPlusPlusOpenGL/Shaders/GridShaders/gridFrag.fs");

   float verticesI[] = {
      // positions          // colors           // texture coords
       0.5f,  0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f, // top right
       0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f, // bottom left
      -0.5f,  0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
   };
   unsigned int indicesInterp[]{
      0,1,3,
      1,2,3
   };

   unsigned int VBOinterp, VAOinterp, EBOInterp;
   glGenVertexArrays(1, &VAOinterp);
   glGenBuffers(1, &VBOinterp);
   glGenBuffers(1, &EBOInterp);

   glBindVertexArray(VAOinterp);

   glBindBuffer(GL_ARRAY_BUFFER, VBOinterp);
   glBufferData(GL_ARRAY_BUFFER, sizeof(verticesI), verticesI, GL_STATIC_DRAW);

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOInterp);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesInterp), indicesInterp, GL_STATIC_DRAW);

   // position attribute
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);
   // color attribute
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
   glEnableVertexAttribArray(1);
   // texture attribute
   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
   glEnableVertexAttribArray(2);

   //====--- INSTANCING TEST --====
   glm::vec2 translations[100];
   int index = 0;
   float offSet = -10.5f;
   for (int i = -10; i < 10; i+=2)
   {
      for (int j = -10; j < 10; j += 2)
      {
         glm::vec2 translation;
         translation.x = (float)j + offSet;
         translation.y = (float)i + offSet;
         translations[index++] = translation;
      }
   }

   unsigned int instanceVBO;
   glGenBuffers(1, &instanceVBO);
   glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100, &translations[0], GL_STATIC_DRAW);
   glBindBuffer(GL_ARRAY_BUFFER, 0);

   float cubeArr[] = {
      // positions          // normals           // texture coords
     -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
      0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
      0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
      0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
     -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
     -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

     -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
      0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
     -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
     -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

     -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
     -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
     -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
     -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
     -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
     -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
      0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
      0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
      0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
      0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

     -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
      0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
      0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
      0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
     -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
     -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

     -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
      0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
     -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
     -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
   };
   unsigned int VAOCube, VBOCube;
   glGenVertexArrays(1, &VAOCube);
   glGenBuffers(1, &VBOCube);

   glBindVertexArray(VAOCube);

   glBindBuffer(GL_ARRAY_BUFFER, VBOCube);
   glBufferData(GL_ARRAY_BUFFER, sizeof(cubeArr), cubeArr, GL_STATIC_DRAW);

   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);
   // texture coord attribute
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
   glEnableVertexAttribArray(1);

   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
   glEnableVertexAttribArray(2);

   glEnableVertexAttribArray(3); // ---=== Instancing Vertex Attribute ===---
   glBindBuffer(GL_ARRAY_BUFFER, instanceVBO); // this attribute comes from a different vertex buffer
   glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glVertexAttribDivisor(3, 1); // tell OpenGL this is an instanced vertex attribute.

   float LIGHTINGVertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
   };

   unsigned int ColorLightVBO, cubeVAO;
   glGenVertexArrays(1, &cubeVAO);
   glGenBuffers(1, &ColorLightVBO);

   glBindBuffer(GL_ARRAY_BUFFER, ColorLightVBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(LIGHTINGVertices), LIGHTINGVertices, GL_STATIC_DRAW);

   glBindVertexArray(cubeVAO);

   // position attribute
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);
   // normal attribute
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
   glEnableVertexAttribArray(1);

   // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
   unsigned int lightCubeVAO;
   glGenVertexArrays(1, &lightCubeVAO);
   glBindVertexArray(lightCubeVAO);

   glBindBuffer(GL_ARRAY_BUFFER, ColorLightVBO);
   // note that we update the lamp's position attribute's stride to reflect the updated buffer data
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);

   //QUAD VBO AND VAO
   float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
   };
   unsigned int quadVAO, quadVBO;
   glGenVertexArrays(1, &quadVAO);
   glGenBuffers(1, &quadVBO);
   glBindVertexArray(quadVAO);
   glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

   //3D MODELS
   stbi_set_flip_vertically_on_load(true);
   Model backpackModel("D:/CPlusPlusOpenGL/3D Models/Backpack/backpack.obj");
   Model lampModel("C:/Users/Baher/Desktop/fg/dfdfdfdfdf/stl/lampmodel.STL");

   Shader cubeShader("D:/CPlusPlusOpenGL/Shaders/vertCube.vs", "D:/CPlusPlusOpenGL/Shaders/fragCube.fs");
   Shader lightShader("D:/CPlusPlusOpenGL/Shaders/Lighting/lightVert.vs", "D:/CPlusPlusOpenGL/Shaders/lighting/lightFrag.fs");
   Shader lightShaderA("D:/CPlusPlusOpenGL/Shaders/Lighting/lightColor.vs", "D:/CPlusPlusOpenGL/Shaders/lighting/lightColor.fs");

   //UBO Shader
   Shader ubo("D:/CPlusPlusOpenGL/Shaders/FrameBuffer/UBO/UBOVert.vs", "D:/CPlusPlusOpenGL/Shaders/FrameBuffer/UBO/frag1.fs");
   Shader ubo1("D:/CPlusPlusOpenGL/Shaders/FrameBuffer/UBO/UBOVert.vs", "D:/CPlusPlusOpenGL/Shaders/FrameBuffer/UBO/frag2.fs");
   //Heightmap
   Shader heightMapShader("D:/CPlusPlusOpenGL/Shaders/Heightmap/hmVS.vs", "D:/CPlusPlusOpenGL/Shaders/Heightmap/hmFS.fs");

   //Instancing Shaders
   Shader instanceShader("D:/CPlusPlusOpenGL/Shaders/Instancing/instancingVert.vs","D:/CPlusPlusOpenGL/Shaders/Instancing/instancingFrag.fs");

   unsigned int uniformBlockRed = glGetUniformBlockIndex(ubo.ID, "Matricies");
   unsigned int uniformBlockBlue = glGetUniformBlockIndex(ubo1.ID, "Matricies");
   glUniformBlockBinding(ubo.ID, uniformBlockRed, 0);
   glUniformBlockBinding(ubo1.ID, uniformBlockBlue, 0);
   unsigned int uboMatricies;
   glGenBuffers(1, &uboMatricies);
   glBindBuffer(GL_UNIFORM_BUFFER, uboMatricies);
   glBufferData(GL_UNIFORM_BUFFER, 2*sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
   glBindBuffer(GL_UNIFORM_BUFFER, 0);
   glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatricies, 0, 2 * sizeof(glm::mat4)); //define range of buffer

   //3D MODEL SHADERS
   // Geometry shader example (uncomment to test)
   //Shader backpackShader("D:/CPlusPlusOpenGL/Shaders/3DModelShaders/backpackVS.vs", "D:/CPlusPlusOpenGL/Shaders/3DModelShaders/backpackFS.fs","D:/CPlusPlusOpenGL/Shaders/GeometryShaders/exploding.gs");
   //Shader backpackNormals("D:/CPlusPlusOpenGL/Shaders/GeometryShaders/Normals/vertexnormal.vs","D:/CPlusPlusOpenGL/Shaders/GeometryShaders/Normals/fragmentnormal.fs","D:/CPlusPlusOpenGL/Shaders/GeometryShaders/Normals/normals.gs");

   Shader backpackShader("D:/CPlusPlusOpenGL/Shaders/3DModelShaders/backpackVS.vs", "D:/CPlusPlusOpenGL/Shaders/3DModelShaders/backpackFS.fs");

   //Stencil testing shader
   Shader stencilCube("D:/CPlusPlusOpenGL/Shaders/Lighting/lightVert.vs", "D:/CPlusPlusOpenGL/Shaders/Lighting/stencilFragment.fs");

   // ===== SET UP TEXTURE =======

   unsigned int texture1, texture2;
   // texture 1
   // ---------
   glGenTextures(1, &texture1);
   glBindTexture(GL_TEXTURE_2D, texture1);
   // set the texture wrapping parameters
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   // set texture filtering parameters
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   // load image, create texture and generate mipmaps
   int width, height, nrChannels;
   stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
   // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
   unsigned char* data = stbi_load("D:/randomTextures/woodenbox.png", &width, &height, &nrChannels, 0);
   if (data)
   {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
   }
   else
   {
      std::cout << "Failed to load texture" << std::endl;
   }
   stbi_image_free(data);
   // texture 2
   // ---------
   glGenTextures(1, &texture2);
   glBindTexture(GL_TEXTURE_2D, texture2);
   // set the texture wrapping parameters
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   // set texture filtering parameters
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   // load image, create texture and generate mipmaps
   data = stbi_load("D:/randomTextures/coolframe.png", &width, &height, &nrChannels, 0);
   if (data)
   {
      // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
   }
   else
   {
      std::cout << "Failed to load texture" << std::endl;
   }
   stbi_image_free(data);

   Texture2D grassTexture("D:/randomTextures/grass.png", true, false);
   Texture2D waterTexture("D:/randomTextures/water.png", true, false);
   Texture2D stoneTexture("D:/randomTextures/stone.png", true, false);
   Texture2D stoneTextureNormal("D:/randomTextures/stonenormal.png", true, false);

   Texture2D grassBlend("D:/randomTextures/grassblend.png", true, true);
   Texture2D transparentWindow("D:/randomTextures/transparentwindow.png", true, true);

   Shader grassTransparent("D:/CPlusPlusOpenGL/Shaders/Blend/blendvert.vs","D:/CPlusPlusOpenGL/Shaders/Blend/blendfrag.fs");
   //DuDV Texture
   Texture2D dudvMap("D:/randomTextures/dudvmap.jpg", true, false);

   Shader fboBuffer("D:/CPlusPlusOpenGL/Shaders/FrameBuffer/fboVert.vs","D:/CPlusPlusOpenGL/Shaders/FrameBuffer/fboFrag.fs");

   glBindVertexArray(0);
   glEnable(GL_DEPTH_TEST);

   //STENCILS TESTING
   glEnable(GL_STENCIL_TEST);
   glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
   glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

   //Face culling -- FACE CULLING INFO -- Face culling depends on clockwise and anticlockwise set of vertices (triangles).
   // AntiCW is front and CW is back. Culling
   //is the action of discarding triangles

   /*glEnable(GL_CULL_FACE);
   glCullFace(GL_BACK);
   glFrontFace(GL_CCW);*/

   //=====---- FRAME BUFFERS ----======
   // Set the texture for the FBO
   fboBuffer.use();
   fboBuffer.setInt("screenTexture", 0);
   //Creating Frame Buffer Object
   unsigned int fbo;
   glGenFramebuffers(1, &fbo);
   glBindFramebuffer(GL_FRAMEBUFFER, fbo);
   //Texture for the frame buffer
   unsigned int textureColorbuffer;
   glGenTextures(1, &textureColorbuffer);
   glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL); //'SRGB' is used for Gamma Correction
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

   //Render buffer object for depth and stencil attachment buffers
   unsigned int rbo;
   glGenRenderbuffers(1, &rbo);
   glBindRenderbuffer(GL_RENDERBUFFER, rbo);
   glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600); // use a single renderbuffer object for both a depth AND stencil buffer.
   glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it

   if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
   //Go back to standard framebuffer
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   //=====END OF FBO=======

   //Cubemap generation
   Shader cubemapShader("D:/CPlusPlusOpenGL/Shaders/CubeMapShader/cubemapVert.vs",
      "D:/CPlusPlusOpenGL/Shaders/CubeMapShader/cubemapFrag.fs");
   vector<std::string> faces
   {
      "D:/randomTextures/skybox/right.jpg",
      "D:/randomTextures/skybox/left.jpg",
      "D:/randomTextures/skybox/bottom.jpg",
      "D:/randomTextures/skybox/top.jpg",
      "D:/randomTextures/skybox/front.jpg",
      "D:/randomTextures/skybox/back.jpg"
   };
   unsigned int cubemapTexture = loadCubeMap(faces);
   unsigned int cubemapVAO = skyBoxVAO();
   //End of cubemap generation
   
   //Heightmap initialization
   readBMP("D:/randomTextures/heightMap.bmp");
   unsigned int heightMapVAO = setupHeightMapVAO(3, 60);

   //Grid initialization
   Shader gVAOShader("D:/CPlusPlusOpenGL/Shaders/GridShaders/gVerts.vs","D:/CPlusPlusOpenGL/Shaders/GridShaders/gFrag.fs");
   unsigned int gVAO = gridVAO();

   glm::vec3 cubePositions[] = {
       glm::vec3(0.0f,  3.0f,  -4.0f),
       glm::vec3(2.0f,  5.0f, -15.0f),
       glm::vec3(-1.5f, -2.2f, -2.5f),
       glm::vec3(-3.8f, -2.0f, -12.3f),
       glm::vec3(2.4f, -0.4f, -3.5f),
       glm::vec3(-1.7f,  3.0f, -7.5f),
       glm::vec3(1.3f, -2.0f, -2.5f),
       glm::vec3(1.5f,  2.0f, -2.5f),
       glm::vec3(1.5f,  0.2f, -1.5f),
       glm::vec3(-1.3f,  1.0f, -1.5f)
   };
   glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f,  7.2f,  -2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3(0.0f,  0.0f, -3.0f)
   };

   //Dear ImGUI
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGuiIO& io = ImGui::GetIO(); (void)io;
   io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
   //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

   // Setup Dear ImGui style
   ImGui::StyleColorsDark();
   //ImGui::StyleColorsClassic();

   // Setup Platform/Renderer backends
   ImGui_ImplGlfw_InitForOpenGL(window, true);
   ImGui_ImplOpenGL3_Init("#version 130");

   strcpy(guiBuf, "Press END to turn on/off flashlight!\n> ");
   bool showWindowGUI = true;

   //SHADOW MAPPING
   ShadowRender shadow(1024, 1024);
   Shader shadowShaderDepth("D:/CPlusPlusOpenGL/Shaders/Shadows/shadowvs.vs", "D:/CPlusPlusOpenGL/Shaders/Shadows/shadowfs.fs");
   Shader shadowQuad("D:/CPlusPlusOpenGL/Shaders/Shadows/shadowquad.vs", "D:/CPlusPlusOpenGL/Shaders/Shadows/shadowquad.fs");
   unsigned int mainPlaneVAO = SetplaneVAO();

   //Parallax Mapping
   ParallaxMap parallax;
   unsigned int pmQuad = parallax.CreateVAO();
   Shader parallaxShader("D:/CPlusPlusOpenGL/Shaders/Shadows/ParallaxMapping/parallaxVS.vs", "D:/CPlusPlusOpenGL/Shaders/Shadows/ParallaxMapping/parallaxFS.fs");
   Texture2D parallaxMap("D:/randomTextures/parallax.png", true, false);
   Texture2D brickNormal("D:/randomTextures/brickNormal.png", true, false);
   Texture2D brickTexture("D:/randomTextures/bricks.jpg", true, false);
   //unsigned int pmVAO = pm;

#pragma region GBUFFER_TEST
   Shader shaderGeometryPass("D:/CPlusPlusOpenGL/Shaders/FrameBuffer/GBuffer/gbuffer.vs", 
       "D:/CPlusPlusOpenGL/Shaders/FrameBuffer/GBuffer/gbuffer.fs");
   Shader shaderLightingPass("D:/CPlusPlusOpenGL/Shaders/FrameBuffer/GBuffer/deferred.vs",
       "D:/CPlusPlusOpenGL/Shaders/FrameBuffer/GBuffer/deferred.fs");
   Shader shaderLightBox("D:/CPlusPlusOpenGL/Shaders/FrameBuffer/GBuffer/deferredlightbox.vs",
       "D:/CPlusPlusOpenGL/Shaders/FrameBuffer/GBuffer/deferredlightbox.fs");

   unsigned int gBuffer;
   glGenFramebuffers(1, &gBuffer);
   glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
   unsigned int gPosition, gNormal, gAlbedoSpec;
   // position color buffer
   glGenTextures(1, &gPosition);
   glBindTexture(GL_TEXTURE_2D, gPosition);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 800, 600, 0, GL_RGBA, GL_FLOAT, NULL);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
   // normal color buffer
   glGenTextures(1, &gNormal);
   glBindTexture(GL_TEXTURE_2D, gNormal);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 800, 600, 0, GL_RGBA, GL_FLOAT, NULL);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
   // color + specular color buffer
   glGenTextures(1, &gAlbedoSpec);
   glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
   // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
   unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
   glDrawBuffers(3, attachments);
   // create and attach depth buffer (renderbuffer)
   unsigned int rboDepth;
   glGenRenderbuffers(1, &rboDepth);
   glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
   glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 800, 600);
   glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
   // finally check if framebuffer is complete
   if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
       std::cout << "Framebuffer not complete!" << std::endl;
   glBindFramebuffer(GL_FRAMEBUFFER, 0);

   std::vector<glm::vec3> objectPositions;
   objectPositions.push_back(glm::vec3(-3.0, -0.5, -3.0));
   objectPositions.push_back(glm::vec3(0.0, -0.5, -3.0));
   objectPositions.push_back(glm::vec3(3.0, -0.5, -3.0));
   objectPositions.push_back(glm::vec3(-3.0, -0.5, 0.0));
   objectPositions.push_back(glm::vec3(0.0, -0.5, 0.0));
   objectPositions.push_back(glm::vec3(3.0, -0.5, 0.0));
   objectPositions.push_back(glm::vec3(-3.0, -0.5, 3.0));
   objectPositions.push_back(glm::vec3(0.0, -0.5, 3.0));
   objectPositions.push_back(glm::vec3(3.0, -0.5, 3.0));

   // lighting info
    // -------------
   const unsigned int NR_LIGHTS = 32;
   std::vector<glm::vec3> lightPositions;
   std::vector<glm::vec3> lightColors;
   srand(13);
   for (unsigned int i = 0; i < NR_LIGHTS; i++)
   {
       // calculate slightly random offsets
       float xPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
       float yPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 4.0);
       float zPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
       lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
       // also calculate random color
       float rColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
       float gColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
       float bColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
       lightColors.push_back(glm::vec3(rColor, gColor, bColor));
   }

   // shader configuration
   // --------------------
   shaderLightingPass.use();
   shaderLightingPass.setInt("gPosition", 0);
   shaderLightingPass.setInt("gNormal", 1);
   shaderLightingPass.setInt("gAlbedoSpec", 2);
#pragma endregion

   //This is used for wireframing
   //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   UINT TimerId = SetTimer(NULL, 0, 1, (TIMERPROC)&TimerProc); //1 millisecond
   //Render loop! (Very important for rendering pixels)
   while (!glfwWindowShouldClose(window))
   {
      //Delta time (important for better input steps)
      float currentTime = glfwGetTime();
      deltaTime = currentTime - lastFraming;
      lastFraming = currentTime;
      
      //Bullet Physics!
      //world->stepSimulation(1/60.f);

      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      //Process input reads the key and checks if inputs have been pressed through out the render loop
      processInput(window);

      //Return WIDTH and HEIGHT of screen
      int window_width;
      int window_height;
      glfwGetWindowSize(window, &window_width, &window_height);

      glm::mat4 model = glm::mat4(1.0f);
      model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 1.0f));

      glm::mat4 view = cameraGame.GetViewMatrix();

      glm::mat4 projection = glm::mat4(1.0f);
      projection = glm::perspective(glm::radians(cameraGame.Zoom), get_resolution(window), 0.1f, 200.0f);

      glm::mat4 lightSpaceMat = shadow.setLightSpaceMatrix(3.0f, 10.5f, glm::vec3(2 * sin(glfwGetTime()), 4, -2), glm::vec3(0, 1, 0)); //For Shadow rendering

      glm::mat4 model2 = glm::mat4(1.0f);

      // render
        // ------
      glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#pragma region GBuffer_Render
      glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      model = glm::mat4(1.0f);
      shaderGeometryPass.use();
      shaderGeometryPass.setMat4("projection", projection);
      shaderGeometryPass.setMat4("view", view);
      for (unsigned int i = 0; i < objectPositions.size(); i++)
      {
          model = glm::mat4(1.0f);
          model = glm::translate(model, objectPositions[i]);
          model = glm::scale(model, glm::vec3(0.25f));
          shaderGeometryPass.setMat4("model", model);
          backpackModel.Draw(shaderGeometryPass);
      }
      //glBindFramebuffer(GL_FRAMEBUFFER, fbo);
#pragma endregion

      //FRAME BUFFER OBJECT INITIALIZATION
      glBindFramebuffer(GL_FRAMEBUFFER, fbo);
      glEnable(GL_DEPTH_TEST);
      //Clears background color buffer and replaces it each frame buffer
      //glClearColor(0,0,0, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

      //Stencil Mask. Settings Everything from here to 0x00 in the stencil buffer
      glStencilMask(0x00);

#pragma region GBUFFER SHADER USAGE
      if (showGBuffer) {
          shaderLightingPass.use();
          glActiveTexture(GL_TEXTURE0);
          glBindTexture(GL_TEXTURE_2D, gPosition);
          glActiveTexture(GL_TEXTURE1);
          glBindTexture(GL_TEXTURE_2D, gNormal);
          glActiveTexture(GL_TEXTURE2);
          glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
          for (unsigned int i = 0; i < lightPositions.size(); i++)
          {
              shaderLightingPass.setFloat3("lights[" + std::to_string(i) + "].Position", lightPositions[i].x, lightPositions[i].y, lightPositions[i].z);
              shaderLightingPass.setFloat3("lights[" + std::to_string(i) + "].Color", lightColors[i].x, lightColors[i].y, lightColors[i].z);
              // update attenuation parameters and calculate radius
              const float constant = 1.0f; // note that we don't send this to the shader, we assume it is always 1.0 (in our case)
              const float linear = 0.7f;
              const float quadratic = 1.8f;
              shaderLightingPass.setFloat("lights[" + std::to_string(i) + "].Linear", linear);
              shaderLightingPass.setFloat("lights[" + std::to_string(i) + "].Quadratic", quadratic);
              // then calculate radius of light volume/sphere
              const float maxBrightness = std::fmaxf(std::fmaxf(lightColors[i].r, lightColors[i].g), lightColors[i].b);
              float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
              shaderLightingPass.setFloat("lights[" + std::to_string(i) + "].Radius", radius);
          }
          shaderLightingPass.setFloat3("viewPos", cameraGame.Position.x, cameraGame.Position.y, cameraGame.Position.z);

          shaderLightingPass.setMat4("model", model);
          glBindVertexArray(quadVAO);
          glDrawArrays(GL_TRIANGLES, 0, 6);

          // 2.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
           // ----------------------------------------------------------------------------------
          glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
          glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
          // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
          // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
          // depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
          glBlitFramebuffer(0, 0, 800, 600, 0, 0, 800, 600, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
          glBindFramebuffer(GL_FRAMEBUFFER, fbo);
      }
#pragma endregion

      //glDepthFunc(GL_LEQUAL);
      glDepthMask(GL_FALSE);
      cubemapShader.use();
      cubemapShader.setMat4("projection", projection);
      view = glm::mat4(glm::mat3(cameraGame.GetViewMatrix()));
      cubemapShader.setMat4("view", view);
      glBindVertexArray(cubemapVAO);
      glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
      glDrawArrays(GL_TRIANGLES, 0, 36);
      //glDepthFunc(GL_LESS);
      glDepthMask(GL_TRUE);
      view = cameraGame.GetViewMatrix();
      
      //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      gridShader.use();

      gridShader.setInt("grasstexture", 0);
      gridShader.setInt("waterTexture", 1);
      gridShader.setInt("depthMap", 2);

      glActiveTexture(GL_TEXTURE0);
      grassTexture.Use();
      glActiveTexture(GL_TEXTURE1);
      waterTexture.Use();
      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_2D, shadow.depthMap);

      gridShader.setMat4("lightSpaceMatrix", lightSpaceMat);
      gridShader.setFloat3("viewPos", cameraGame.Position.x, cameraGame.Position.y, cameraGame.Position.z);

      gridShader.setMat4("view", view);
      gridShader.setMat4("projection", projection);

      gridShader.setFloat("timer", glfwGetTime());
      gridShader.setBool("isWater", false);
      gridShader.setBool("isCollider", false);
      model2 = glm::translate(model2, glm::vec3(0, -2.5, 0));
      gridShader.setMat4("model", model2);
      gridShader.setFloat3("lightPos", 2 * sin(glfwGetTime()), 4.0f, 2.0f);

      glBindVertexArray(mainPlaneVAO);
      glDrawArrays(GL_TRIANGLES, 0, 6);
      //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

      gridShader.setBool("isWater", true);
      for (float i = 1; i < 5; i++)
      {
         for (float j = 1; j < 5; j++)
         {
            gridShader.setFloat("posX", i - 10);
            gridShader.setFloat("posY", j - 20);
            glBindVertexArray(VAOinterp);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
         }
      }
      //----Collision testing----
      gridShader.setBool("isCollider", true);
      glm::vec3 objPos = glm::vec3(5, -1.5, 0);
      glm::vec3 objSize = glm::vec3(3, 2.5, 1);
      model2 = glm::mat4(1.0f);
      model2 = glm::translate(model2, objPos);
      model2 = glm::scale(model2, objSize);
      gridShader.setMat4("model", model2);
      glBindVertexArray(VAOinterp);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

      //Check collision on X,Y,Z
      bool collisionX = objPos.x + objSize.x >= cameraGame.Position.x && cameraGame.Position.x + 1.0f >= objPos.x;
      bool collisionY = objPos.y + objSize.y >= cameraGame.Position.y && cameraGame.Position.y + 1.0f >= objPos.y;
      bool collisionZ = objPos.z + objSize.z >= cameraGame.Position.z && cameraGame.Position.z + 1.0f >= objPos.z;
      if (collisionX && collisionY && collisionZ) {
         cout << "Teleported!" << endl;
         cameraGame.Position = glm::vec3(5,5,5);
         //cameraGame.ProcessKeyboard(BACKWARD, deltaTime); //actually colliding
      }
      //End of collision testing

      //Parallax mapping test
      glActiveTexture(GL_TEXTURE0);
      brickTexture.Use();
      glActiveTexture(GL_TEXTURE1);
      brickNormal.Use();
      glActiveTexture(GL_TEXTURE2);
      parallaxMap.Use();
      parallaxShader.use();
      parallaxShader.setMat4("projection", projection);
      parallaxShader.setMat4("view", view);
      parallaxShader.setFloat("heightScale", 0.1);
      model = glm::mat4(1.0);
      model = glm::translate(model, glm::vec3(3, 0, -6));
      model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0, 1, 0));
      parallaxShader.setInt("diffuseMap", 0);
      parallaxShader.setInt("normalMap", 1);
      parallaxShader.setInt("heightMap", 2);
      parallaxShader.setFloat3("viewPos", cameraGame.Position.x, cameraGame.Position.y, cameraGame.Position.z);
      parallaxShader.setFloat3("lightPos", cameraGame.Position.x, cameraGame.Position.y, cameraGame.Position.z);
      parallaxShader.setMat4("model", model);
      parallax.Use();

      //Heightmap Rendering
      //glPolygonMode(GL_FRONT_AND_BACK, (isSprinting) ? GL_FILL : GL_LINE);
      glActiveTexture(GL_TEXTURE0);
      dudvMap.Use();
      glActiveTexture(GL_TEXTURE1);
      waterTexture.Use();
      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
      glActiveTexture(GL_TEXTURE3);
      grassTexture.Use();
      glActiveTexture(GL_TEXTURE4);
      stoneTexture.Use();
      glActiveTexture(GL_TEXTURE5);
      stoneTextureNormal.Use();

      heightMapShader.use();
      heightMapShader.setMat4("projection", projection);
      heightMapShader.setMat4("view", view);
      heightMapShader.setFloat3("viewPosition", cameraGame.Position.x, cameraGame.Position.y, cameraGame.Position.z);
      heightMapShader.setFloat("waterVelocity", waterVelocity * glfwGetTime());
      heightMapShader.setInt("dudvmap",0);
      heightMapShader.setInt("waterText",1);
      heightMapShader.setInt("cubemap",2);
      heightMapShader.setInt("grass",3);
      //Stone texture and it's normals
      heightMapShader.setInt("stone",4);
      heightMapShader.setInt("stoneNormal",5);
      SetupSpotlight(heightMapShader); //for the spotlight
      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(0,4.3f,0));
      heightMapShader.setMat4("model", model);
      glBindVertexArray(heightMapVAO);
      //glDrawArrays(GL_TRIANGLE_STRIPS, 0, hmSize);
      glDrawArrays(GL_TRIANGLES, 0, hmSize);

      //Sine wave
      gVAOShader.use();
      glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
      gVAOShader.setMat4("projection", projection);
      gVAOShader.setMat4("view", view);
      gVAOShader.setFloat3("viewPos", cameraGame.Position.x, cameraGame.Position.y, cameraGame.Position.z);
      gVAOShader.setInt("cubemap",0);
      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(4,-2,8));
      gVAOShader.setMat4("model", model);
      glBindVertexArray(gVAO);
      glDrawArrays(GL_TRIANGLES, 0, 6000);

      //glm Transformation rotation with matricies
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      ourShader.use();
      ourShader.setInt("ourTexture", 0);
      ourShader.setInt("ourTexture2", 1);

      ourShader.setMat4("model", model);
      ourShader.setMat4("view", view);
      ourShader.setMat4("projection", projection);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture1);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, texture2);
      ourShader.setFloat("offSet", globFloat);
      ourShader.setFloat("offSetY", globFloatY);
      glBindVertexArray(VAOinterp);
      //glDrawArrays(GL_TRIANGLES, 0, 3);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

      cubeShader.use();
      //Material Textures
      cubeShader.setInt("material.diffuse", 0);
      cubeShader.setInt("material.specular", 1);

      cubeShader.setMat4("model", model);
      cubeShader.setMat4("view", view);
      cubeShader.setMat4("projection", projection);

      glm::vec3 lightColor1 = glm::vec3(.45f, 0, 0);

      glm::vec3 diffuseColor1 = lightColor1 * glm::vec3(0.9f);
      glm::vec3 ambientColor1 = diffuseColor1 * glm::vec3(0.3f);

      cubeShader.setFloat3("viewPosition", cameraGame.Position.x, cameraGame.Position.y, cameraGame.Position.z);

      //Directional Light setup
      cubeShader.setFloat3("dirLight.direction", -0.2f, -1.0f, -0.3f);
      cubeShader.setFloat3("dirLight.ambient", 0.02f, 0.02f, 0.02f);
      cubeShader.setFloat3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
      cubeShader.setFloat3("dirLight.specular", 0.5f, 0.5f, 0.5f);

      //Point Lights setup
      for (int i = 0; i < 4; i++)
      {
         cubeShader.setFloat3("pointLights["+std::to_string(i)+"].position", pointLightPositions[i].x ,pointLightPositions[i].y, pointLightPositions[i].z);
         cubeShader.setFloat3("pointLights["+std::to_string(i)+"].ambient", 0.05f, 0.05f, 0.05f);
         cubeShader.setFloat3("pointLights["+std::to_string(i)+"].diffuse", 0.8f, 0.8f, 0.8f);
         cubeShader.setFloat3("pointLights["+std::to_string(i)+"].specular", 1,1,1);
         //Attenuation
         cubeShader.setFloat("pointLights["+std::to_string(i)+"].constant", 1.0f);
         cubeShader.setFloat("pointLights["+std::to_string(i)+"].linear", 0.9f);
         cubeShader.setFloat("pointLights["+std::to_string(i)+"].quadratic", 0.032f);
      }
      cubeShader.setFloat3("pointLights[4].position", 2 * sin(glfwGetTime()), 4.0f, -4.0f);
      cubeShader.setFloat3("pointLights[4].ambient", 0.05f, 0.05f, 0.05f);
      cubeShader.setFloat3("pointLights[4].diffuse", 0.8f, 0.8f, 0.8f);
      cubeShader.setFloat3("pointLights[4].specular", 1, 1, 1);
      cubeShader.setFloat("pointLights[4].constant", 1.0f);
      cubeShader.setFloat("pointLights[4].linear", 0.9f);
      cubeShader.setFloat("pointLights[4].quadratic", 0.032f);

      //Spot Light setup
      cubeShader.setFloat3("spotLight.position", cameraGame.Position.x, cameraGame.Position.y, cameraGame.Position.z);
      cubeShader.setFloat3("spotLight.direction", cameraGame.Front.x, cameraGame.Front.y, cameraGame.Front.z);
      //Spot light lighting settings
      cubeShader.setFloat3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
      cubeShader.setFloat3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
      cubeShader.setFloat3("spotLight.specular", 1.0f, 1.0f, 1.0f);
      //Spot light attenuation
      cubeShader.setFloat("spotLight.constant", 1.0f);
      cubeShader.setFloat("spotLight.linear", 0.09);
      cubeShader.setFloat("spotLight.quadratic", 0.032);
      //Spot light soft edging
      cubeShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
      cubeShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

      cubeShader.setFloat("material.shininess", 64.0f);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture1);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, texture2);
      glBindVertexArray(VAOCube);
      glDrawArrays(GL_TRIANGLES, 0, 36);

      for (unsigned int i = 0; i < 10; i++)
      {
         glm::mat4 model = glm::mat4(1.0f);
         model = glm::translate(model, cubePositions[i]);
         float angle = 20.0f * i;
         model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

         cubeShader.setMat4("model", model);

         glDrawArrays(GL_TRIANGLES, 0, 36);
      }

      //Light cube
      lightShader.use();
      model = glm::mat4(1.0f);
      //model = glm::translate(model, glm::vec3(1.2f, 4.0f, 2.0f));
      model = glm::translate(model, glm::vec3(2*sin(glfwGetTime()), 4.0f, -4.0f));
      model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
      lightShader.setMat4("projection", projection);
      lightShader.setMat4("view", view);
      lightShader.setMat4("model", model);
      glBindVertexArray(lightCubeVAO);
      glDrawArrays(GL_TRIANGLES, 0, 36);

      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(2 * sin(glfwGetTime()), 4.0f, 2.0f));
      model = glm::scale(model, glm::vec3(0.2f));
      lightShader.setMat4("model", model);
      glDrawArrays(GL_TRIANGLES, 0, 36);

      for (unsigned int i = 0; i < 4; i++)
      {
         model = glm::mat4(1.0f);
         model = glm::translate(model, glm::vec3(pointLightPositions[i].x, pointLightPositions[i].y, pointLightPositions[i].z));
         model = glm::scale(model, glm::vec3(0.2f));
         lightShader.setMat4("model", model);
         glDrawArrays(GL_TRIANGLES, 0, 36);
      }

      lightShaderA.use();
      model = glm::mat4(1.0f);

      // CAMERA PHYSICS
      /*Object cameraPhys;
      cameraPhys.Mass = 20.0f;
      cameraPhys.Velocity = glm::vec3(0,9.0f,0);

      phys.AddObject(&cameraPhys);

      if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
         isPress();
      else if (glfwGetKey(window, GLFW_KEY_U) == GLFW_RELEASE)
      {
         if (cameraGame.Position.y <= -2 && isClipping)
            resetIsPres();
         else if (!isClipping)
            resetIsPres();
      }
      phys.Step(secondsA);
      if (isClipping) {
         if (cameraPhys.Position.y < .5)
            cameraPhys.Position.y = .5;
         cameraGame.Position.y = cameraPhys.Position.y - 2.5;
      }*/

      //PHYSICS
      PhysicsWorld phys;
      Object objPhys;
      objPhys.Mass = 1.0f;
      objPhys.Force = glm::vec3(0, 0, 0);
      objPhys.Velocity = glm::vec3(0, 40, 0);

      phys.AddObject(&objPhys);

      if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
         isPress();
      else if (glfwGetKey(window, GLFW_KEY_U) == GLFW_RELEASE)
         resetIsPres();
      phys.Step(secondsA);

      //PHONG SHADING MODEL
      model = glm::translate(model, glm::vec3(objPhys.Position.x, objPhys.Position.y, objPhys.Position.z+2));
      model = glm::scale(model, glm::vec3(2, 2, 2));
      glm::vec3 lightColor = glm::vec3(.45f,0,0);
      
      glm::vec3 diffuseColor = lightColor * glm::vec3(0.9f);
      glm::vec3 ambientColor = diffuseColor * glm::vec3(0.3f);

      lightShaderA.setFloat3("objectColor", 0.3f, 1.0f, 0.2f);
      lightShaderA.setFloat3("lightColor", 1.0f, 1.0f, 1.0f);
      lightShaderA.setFloat3("lightPosition", 2*sin(glfwGetTime()), 4.0f, 0.0f);

      //LIGHT STRUCT
      lightShaderA.setFloat3("light.position", 2*sin(glfwGetTime()), 4.0f, 2.0f);

      lightShaderA.setFloat3("light.ambient", ambientColor.x, ambientColor.y, ambientColor.z);
      lightShaderA.setFloat3("light.diffuse", diffuseColor.x, diffuseColor.y, diffuseColor.z);
      lightShaderA.setFloat3("light.specular", 1.0f,1.0f,1.0f);

      //MATERIAL STRUCT
      lightShaderA.setFloat3("material.ambient", 1.0f,.5f,.31f);
      lightShaderA.setFloat3("material.diffuse", 1.0f,.5f,.31f);
      lightShaderA.setFloat3("material.specular", .5f,.5f,.5f);
      lightShaderA.setFloat("material.shininess", 32.0f);

      lightShaderA.setMat4("projection", projection);
      lightShaderA.setMat4("view", view);
      lightShaderA.setMat4("model", model);
      lightShaderA.setFloat3("viewPosition", cameraGame.Position.x, cameraGame.Position.y, cameraGame.Position.z);
      //Normal matrix for fixating the normalization vector! (inverse property in GLSL is inefficient)
      lightShaderA.setMat3("inverseModel", glm::inverse(model));
      glBindVertexArray(cubeVAO);
      glDrawArrays(GL_TRIANGLES, 0, 36);

      //3D MODEL
      backpackShader.use();
      backpackShader.setFloat("time", glfwGetTime());
      backpackShader.setBool("isFPS", false);

      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(0.0f, -2.0f, 5.0f));
      model = glm::scale(model, glm::vec3(.25f,.25f,.25f));
      model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0, 1, 0));
      backpackShader.setBool("isLighted", true);
      backpackShader.setMat4("model", model);
      backpackShader.setMat4("view", view);
      backpackShader.setMat4("projection", projection);

      //Spot Lights struct
      backpackShader.setFloat3("viewPosition", cameraGame.Position.x, cameraGame.Position.y, cameraGame.Position.z);

      backpackShader.setFloat3("sl.position", cameraGame.Position.x, cameraGame.Position.y, cameraGame.Position.y);
      backpackShader.setFloat3("sl.direction", cameraGame.Front.x, cameraGame.Front.y, cameraGame.Front.z);

      backpackShader.setFloat("sl.constant", 1.0f);
      backpackShader.setFloat("sl.linear", 0.09f);
      backpackShader.setFloat("sl.quadratic", 0.032f);

      backpackShader.setFloat("sl.cutOff", glm::cos(glm::radians(12.5f)));
      backpackShader.setFloat("sl.outerCutOff", glm::cos(glm::radians(15.0f)));

      backpackShader.setFloat3("sl.ambient", 0.0f, 0.0f, 0.0f);
      backpackShader.setFloat3("sl.diffuse", 1.0f, 1.0f, 1.0f);
      backpackShader.setFloat3("sl.specular", 1.0f, 1.0f, 1.0f);

      backpackModel.Draw(backpackShader);
      
      //Lamp model made from MV3DViewer
      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(1,-2,5));
      model = glm::scale(model, glm::vec3(.01f, .01f, .01f));
      backpackShader.setMat4("model", model);
      backpackShader.setBool("isLighted", false);
      lampModel.Draw(backpackShader);

      //Write to stencil buffer
      glStencilFunc(GL_ALWAYS, 1, 0xFF);
      glStencilMask(0xFF);

      lightShader.use();
      model = glm::mat4(1.0f);
      //model = glm::translate(model, glm::vec3(1.2f, 4.0f, 2.0f));
      model = glm::translate(model, glm::vec3(4, -1.5, 5));
      lightShader.setMat4("projection", projection);
      lightShader.setMat4("view", view);
      lightShader.setMat4("model", model);
      glBindVertexArray(lightCubeVAO);
      glDrawArrays(GL_TRIANGLES, 0, 36);

      //slightly scaled versions of the light cube. Stencil buffer is now already filled with 1s making it time to disable the stencil writing...
      glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
      glStencilMask(0x00);
      //glDisable(GL_DEPTH_TEST);

      stencilCube.use();
      float scale = .1f;
      glBindVertexArray(lightCubeVAO);
      model = glm::mat4(1.0);
      model = glm::translate(model, glm::vec3(4,-1.5,5));
      model = glm::scale(model, glm::vec3(1 + scale, 1+scale, 1+scale));
      stencilCube.setMat4("model", model);
      stencilCube.setMat4("projection", projection);
      stencilCube.setMat4("view", view);
      glDrawArrays(GL_TRIANGLES, 0, 36);
      glBindVertexArray(0);
      glStencilMask(0xFF);
      glStencilFunc(GL_ALWAYS, 0, 0xFF);
      //glEnable(GL_DEPTH_TEST);

      grassTransparent.use();
      glActiveTexture(GL_TEXTURE0);
      grassBlend.Use();
      grassTransparent.setInt("grassBlend", 0);

      glActiveTexture(GL_TEXTURE1);
      transparentWindow.Use();
      grassTransparent.setInt("transwindow", 1);

      grassTransparent.setBool("isWindow", false);
      grassTransparent.setMat4("projection", projection);
      grassTransparent.setMat4("view", view);

      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(5,-2.5, 4.5));
      grassTransparent.setMat4("model", model);

      glBindVertexArray(VAOinterp);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

      //Blending
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      grassTransparent.setBool("isWindow", true);

      for (unsigned int i = 0; i < 3; i++) {
         model = glm::mat4(1.0f);
         model = glm::translate(model, glm::vec3(7, -2.5, 4.5 + i));
         grassTransparent.setMat4("model", model);
         glBindVertexArray(VAOinterp);
         glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
      }
      //Reflection and refraction
      backpackShader.use();
      glActiveTexture(GL_TEXTURE3);
      backpackShader.setInt("skybox",3);
      glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
      backpackShader.setBool("isReflect", true);
      backpackShader.setBool("isLighted", false);
      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(-4, -2, 4));
      model = glm::scale(model, glm::vec3(.01f, .01f, .01f));
      model = glm::rotate_slow(model, (float)glfwGetTime(), glm::vec3(0, 1, 0));
      backpackShader.setMat4("model", model);
      lampModel.Draw(backpackShader);
      backpackShader.setBool("isReflect", false);

      //Testing UBO with shaders
      glBindBuffer(GL_UNIFORM_BUFFER, uboMatricies);
      glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
      glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
      glBindBuffer(GL_UNIFORM_BUFFER, 0);
      ubo.use(); //use the ubo shader program
      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(-7,-2,4));
      ubo.setMat4("model", model);
      glBindVertexArray(VAOCube);
      glDrawArrays(GL_TRIANGLES, 0, 36);
      ubo1.use();
      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(-9,-2,4));
      ubo1.setMat4("model", model);
      glBindVertexArray(VAOCube);
      glDrawArrays(GL_TRIANGLES, 0, 36);

      //Draw Instanced
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      instanceShader.use();
      instanceShader.setMat4("projection", projection);
      instanceShader.setMat4("view", view);
      instanceShader.setMat4("model", model);
      glBindVertexArray(VAOCube);
      glDrawArraysInstanced(GL_TRIANGLES, 0, 36, 100);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

      //Shadow Mapping Initialization
      shadowShaderDepth.use();
      shadowShaderDepth.setMat4("lightSpaceMatrix", lightSpaceMat);
      shadow.UseBuffer();
      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(0.0f, -2.0f, 5.0f));
      model = glm::scale(model, glm::vec3(.25f, .25f, .25f));
      model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0, 1, 0));
      shadowShaderDepth.setMat4("model", model);
      backpackModel.Draw(shadowShaderDepth);
      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(0, 4.0f, 2.0f));
      model = glm::scale(model, glm::vec3(0.2f));
      shadowShaderDepth.setMat4("model", model);
      glBindVertexArray(lightCubeVAO);
      glDrawArrays(GL_TRIANGLES, 0, 36);
      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(objPhys.Position.x, objPhys.Position.y, objPhys.Position.z + 2));
      model = glm::scale(model, glm::vec3(2, 2, 2));
      shadowShaderDepth.setMat4("model", model);
      glBindVertexArray(cubeVAO);
      glDrawArrays(GL_TRIANGLES, 0, 36);

      //Resize framebuffer on window resize
      glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
      glBindRenderbuffer(GL_RENDERBUFFER, rbo);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window_width, window_height);

      //Main frame buffer
      glViewport(0, 0, window_width, window_height);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glDisable(GL_DEPTH_TEST);
      glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      //Bind FBO texture
      glActiveTexture(GL_TEXTURE0);
      fboBuffer.use();
      fboBuffer.setFloat("time", glfwGetTime());
      fboBuffer.setFloat("gamma", gamma);
      glBindVertexArray(quadVAO);
      glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
      glDrawArrays(GL_TRIANGLES, 0, 6);

      //FPS view model
      glEnable(GL_DEPTH_TEST);
      backpackShader.use();
      backpackShader.setBool("isFPS", true);
      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(.5, -0.5, -1.9));
      model = glm::scale(model, glm::vec3(.027f, .027f, .027f));
      model = glm::rotate(model, glm::radians(10.f), glm::vec3(0, 1, 0));
      model = glm::rotate(model, (isSprinting) ? 0.05f*(float)cos(3.5*glfwGetTime()) : .035f*(float)cos(glfwGetTime()), glm::vec3(1, 0, 0));
      backpackShader.setMat4("model", model);
      backpackShader.setBool("isLighted", false);
      backpackShader.setFloat3("weaponColor", rWep, gWep, bWep);
      lampModel.Draw(backpackShader);

      //Render Shadow Depth Frame buffer on quad
      if (showShadowBuffer) {
         shadowQuad.use();
         glActiveTexture(GL_TEXTURE0);
         glBindTexture(GL_TEXTURE_2D, shadow.depthMap);
         shadowQuad.setInt("depthMap", 0);
         shadowQuad.setFloat("near_plane", 1.0f);
         shadowQuad.setFloat("far_plane", 7.5f);
         glBindVertexArray(quadVAO);
         glDrawArrays(GL_TRIANGLES, 0, 6);
      }

      //ImGUI
      glfwSetInputMode(window, GLFW_CURSOR, (isMouseVisible) ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
      ImGui::Begin("Input Console", &isSprinting, ImGuiWindowFlags_MenuBar);
      ImGui::Text("Multiplex Engine - created by JoenTheWizard");
      ImGui::Separator();
      ImGui::Text("Welcome to the Multiplex Engine (BETA) console! Type help for a list of commands!");
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0,200,0,1));
      ImGui::InputTextMultiline("", guiBuf, IM_ARRAYSIZE(guiBuf), ImVec2(-1, ImGui::GetWindowContentRegionMax().y - 100), ImGuiInputTextFlags_EnterReturnsTrue);
      ImGui::Checkbox("Show shadow depth buffer", &showShadowBuffer);
      ImGui::Checkbox("Show G-depth buffer", &showGBuffer);
      if (ImGui::IsKeyPressed(io.KeyMap[ImGuiKey_End]))
         flashlightOn = !flashlightOn;
      bool pressed_enter = ImGui::IsKeyPressed(io.KeyMap[ImGuiKey_Enter]);
      if (pressed_enter) {
         string guiParams(guiBuf);
         vector<string> line = splitStr(guiParams, ">");
         string arguments = line.at(line.size() - 1);
         vector<string> pos = splitStr(arguments, " ");
         if (pos.at(1) == "help")
            strcat(guiBuf, "\n---List of commands---\n\t-help -- shows list of commands\n\t-tp [x][y][z] -- teleports player\n\t-wat_vel [velocity] -- changes water velocity\n\t-gamma [value] -- changes gamma correction value (default is 1.0)\n> ");
         else if (pos.at(1) == "tp") {
            try {
               if (pos.size() > 4) {
                  cameraGame.Position.x = stof(pos.at(2));
                  cameraGame.Position.y = stof(pos.at(3));
                  cameraGame.Position.z = stof(pos.at(4));
                  strcat(guiBuf, "\nSuccessfully teleported!\n> ");
               }
               else
                  strcat(guiBuf, "\nError with parsing that command, not enough arguments\n> ");
            }
            catch (std::exception e) {
               cout << e.what() << endl;
               strcat(guiBuf, "\nThere was an error with parsing that command\n> ");
            }
         }
         else if (pos.at(1) == "weapon") {
            try {
               rWep = stof(pos.at(2));
               gWep = stof(pos.at(3));
               bWep = stof(pos.at(4));
               strcat(guiBuf, "\nChanged color weapon\n> ");
            }
            catch (exception e) {
               cout << e.what() << endl;
            }
         }
         else if (pos.at(1) == "wat_vel") {
            waterVelocity = stof(pos.at(2));
            strcat(guiBuf, "\nChanged water velocity\n> ");
         } 
         else if (pos.at(1) == "gamma") {
            gamma = stof(pos.at(2));
            strcat(guiBuf, "\nChanged gamma value\n>");
         }
      }
      if (ImGui::IsKeyPressed(io.KeyMap[ImGuiKey_Home]))
         isMouseVisible = !isMouseVisible;
      ImGui::PopStyleColor();
      ImGui::End();

      glBindVertexArray(0);

      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      //Display on window
      glfwSwapBuffers(window);

      //input handling
      glfwPollEvents();
   }
   //DELETE VAOs
   glDeleteVertexArrays(1, &quadVAO);
   glDeleteVertexArrays(1, &VAO);
   glDeleteVertexArrays(1, &VAOCube);
   glDeleteVertexArrays(1, &VAOinterp);
   glDeleteVertexArrays(1, &heightMapVAO);
   glDeleteVertexArrays(1, &quadVAO);
   glDeleteVertexArrays(1, &lightCubeVAO);
   glDeleteVertexArrays(1, &gVAO);
   glDeleteVertexArrays(1, &cubemapVAO);
   //DELETE VBOs
   glDeleteBuffers(1, &VBO);
   glDeleteBuffers(1, &quadVBO);
   glDeleteBuffers(1, &VBO1);
   glDeleteBuffers(1, &ColorLightVBO);
   glDeleteBuffers(1, &instanceVBO);
   glDeleteBuffers(1, &VBOinterp);
   glDeleteBuffers(1, &VBOCube);
   glDeleteProgram(shaderProgram);
   glfwTerminate();
   return 0;
}