#version 330 core
out vec4 FragColor;
in vec2 Texture;

uniform sampler2D grassBlend;
uniform sampler2D transwindow;

uniform bool isWindow;
void main()
{
    vec4 textureColor = texture(grassBlend, Texture);
    // if (textureColor.a < 0.1)
    //     discard;
    FragColor = (isWindow) ? texture(transwindow, Texture) : textureColor;
}