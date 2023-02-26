#version 330 core
out vec4 FragColor;

in vec3 Normals;
void main()
{
	FragColor = vec4(0.45, 0.34, 0.01, 1.0) * 0.3;
}