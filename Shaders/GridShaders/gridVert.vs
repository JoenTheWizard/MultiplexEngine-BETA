#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormals;
layout (location = 2) in vec2 aTexture;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

uniform float posX;
uniform float posY;
uniform bool isCollider;

out vec3 positionCurr;
out vec2 TextCoords;
out vec3 FragPos;
out vec3 Normals;

out vec4 FragPosLightSpace;
void main()
{
	//vec3 exerciseQuestion = (isCollider) ? aPos :
	//	vec3(aPos.x + posX, aPos.y + posY, aPos.z + 3);
	positionCurr = gl_Position.xyz;
	TextCoords = aTexture;
	Normals = aNormals;
	FragPos = vec3(model*vec4(aPos,1.0));
	FragPosLightSpace = lightSpaceMatrix * vec4(FragPos,1.0);
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
}