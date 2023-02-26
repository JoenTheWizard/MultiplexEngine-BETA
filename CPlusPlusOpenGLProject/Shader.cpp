#include "Shader.h"
#include <glm/detail/type_mat.hpp>


Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
   std::string vertexCode;
   std::string fragmentCode;
   std::string geometryCode;
   std::ifstream vShaderFile;
   std::ifstream fShaderFile;
   std::ifstream gShaderFile;

   vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
   fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
   gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

   try {
      //Open files
      vShaderFile.open(vertexPath);
      fShaderFile.open(fragmentPath);
      std::stringstream vShaderStream, fShaderStream;

      //Read file's buffer contents into streams
      vShaderStream << vShaderFile.rdbuf();
      fShaderStream << fShaderFile.rdbuf();

      //Close file handlers
      vShaderFile.close();
      fShaderFile.close();

      //Convert stream into string
      vertexCode = vShaderStream.str();
      fragmentCode = fShaderStream.str();
      //if the geometry shader is given then load it
      if (geometryPath != nullptr)
      {
         gShaderFile.open(geometryPath);
         std::stringstream gShaderStream;
         gShaderStream << gShaderFile.rdbuf();
         gShaderFile.close();
         geometryCode = gShaderStream.str();
      }
   }
   catch (std::ifstream::failure e) {
      std::cout << "There was an error with compiling the shader program..." << std::endl;
   }
   const char* vShaderCode = vertexCode.c_str();
   const char* fShaderCode = fragmentCode.c_str();

   unsigned int vertex, fragment;
   int success;
   char infoLog[512];
   
   //Compiling vertex shader
   vertex = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(vertex, 1, &vShaderCode, NULL);
   glCompileShader(vertex);

   //Compile the fragment shader
   fragment = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fragment, 1, &fShaderCode, NULL);
   glCompileShader(fragment);

   unsigned int geometry;
   if (geometryPath != nullptr)
   {
      const char* gShaderCode = geometryCode.c_str();
      geometry = glCreateShader(GL_GEOMETRY_SHADER);
      glShaderSource(geometry, 1, &gShaderCode, NULL);
      glCompileShader(geometry);
   }

   //Assign the shader program with vertex and fragment code
   ID = glCreateProgram();
   glAttachShader(ID, vertex);
   glAttachShader(ID, fragment);
   if (geometryPath != nullptr)
      glAttachShader(ID, geometry);
   glLinkProgram(ID);

   //Print any linking errors from the Shader Program
   glGetProgramiv(ID, GL_LINK_STATUS, &success);
   if (!success)
   {
      glGetProgramInfoLog(ID, sizeof(infoLog), NULL, infoLog);
      std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
   }
   glDeleteShader(vertex);
   glDeleteShader(fragment);
   if (geometryPath != nullptr)
      glDeleteShader(geometry);
}

void Shader::use()
{
   glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) const
{
   glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
   glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
   glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat3(const std::string& name, float value1, float value2, float value3) const
{
   glUniform3f(glGetUniformLocation(ID,name.c_str()), value1, value2, value3);
}
void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
   glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
   glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}