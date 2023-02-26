#version 330 core
out vec4 FragColor;

//OurColor is the interpolated RGB values
in vec3 ourColor;
in vec2 TexCoord;
uniform sampler2D ourTexture;
uniform sampler2D ourTexture2;

//CurrentPosition is changement of each fragment color
in vec3 currentPosition;
void main()
{
   //vec2 newCoord = vec2(currentPosition.x * TexCoord.x, currentPosition.y * TexCoord.y);
   FragColor = mix(texture(ourTexture, TexCoord) * vec4(ourColor, 1.0), texture(ourTexture2, TexCoord), 0.2);
}