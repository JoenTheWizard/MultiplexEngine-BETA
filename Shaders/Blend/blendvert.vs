#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 2) in vec2 aTexture; 

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 Texture;
void main()
{
    gl_Position = projection * view * model * vec4(aPosition, 1.0);
    Texture = aTexture;
}