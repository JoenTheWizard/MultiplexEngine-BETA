#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTexture;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 Normals;
out vec3 FragPos;
out vec2 TextCoords;
out vec4 ClipSpace;

out VS_OUT {
	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
} vs_out;
uniform vec3 viewPosition;
void main()
{
	gl_Position = projection * view * model * vec4(aPos,1.0);
	
	Normals = aNormal;
	FragPos = vec3(model * vec4(aPos,1.0));
	TextCoords = aTexture.xy;
	ClipSpace = gl_Position;
	
	//Tangent and Bitangent calculations
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 T = normalize(normalMatrix * aTangent);
	vec3 N = normalize(normalMatrix * aNormal);
	T = normalize(T - dot(T,N)*N);
	vec3 B = cross(N,T);
	
	mat3 TBN = transpose(mat3(T,B,N));
	vs_out.TangentLightPos = TBN * vec3(5,5,5);
	vs_out.TangentViewPos = TBN * viewPosition;
	vs_out.TangentFragPos = TBN * FragPos;
}