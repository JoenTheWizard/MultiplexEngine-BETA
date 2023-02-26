#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPosition;

out VS_OUT {
    vec2 texCoords;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
//bool for determining the fps view model
uniform bool isFPS;
void main()
{
    gl_Position = (isFPS) ? projection * model * vec4(aPos, 1.0)
		: projection * view * model * vec4(aPos, 1.0);
	
	//Texture Coordinates
	TexCoords = aTexCoords;    
	//Normals
	Normal = mat3(transpose(inverse(model))) * aNormal;
	//Fragment Positions
	FragPosition = vec3(model * vec4(aPos, 1.0));
	vs_out.texCoords = aTexCoords;
}