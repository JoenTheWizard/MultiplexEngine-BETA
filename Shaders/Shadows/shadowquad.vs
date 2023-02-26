#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTextCoords;
out vec2 TextureCoords;
void main()
{
	TextureCoords = aTextCoords;
	gl_Position = vec4(aPos.xy * 0.5, aPos.z,1.0);
}