#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 Normals;
out vec3 FragPos;
//out vec2 TextCoords;
void main()
{
	gl_Position = projection * view * model * vec4(aPos,1.0);
	Normals = aNormal;
	FragPos = vec3(model * vec4(aPos,1.0));
	//TextCoords = vec2(aPos.x/2.0+0.5, aPos.y/2.0+0.5);
	//ClipSpace = gl_Position;
}