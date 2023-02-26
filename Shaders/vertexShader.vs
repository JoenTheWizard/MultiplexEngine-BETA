#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTextureCoord;
out vec3 ourColor;

uniform float offSet;
uniform float offSetY;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 currentPosition;
out vec2 TexCoord;
void main()
{
   vec3 exerciseQuestion = vec3(aPos.x + offSet, aPos.y + offSetY, aPos.z);
   gl_Position = projection * view * model * vec4(exerciseQuestion, 1.0);
   ourColor = aColor;
   currentPosition = exerciseQuestion;
   TexCoord = aTextureCoord;
}