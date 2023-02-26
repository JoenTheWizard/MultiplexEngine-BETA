#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float time;
uniform float gamma;

//vec4 KernalTest(sampler2D textureScrn);
//vec4 WaterShaders(sampler2D water);
//const float offset = 1.0/300.0;
void main()
{
	vec3 fg = pow(texture(screenTexture, TexCoords).rgb, vec3(1.0/gamma));
	FragColor = vec4(fg,1.0);
	//FragColor = WaterShaders(screenTexture);
	//FragColor = KernalTest(screenTexture);
}

//vec4 KernalTest(sampler2D textureScrn)
//{
//   vec2 offsets[9] = vec2[](
//      vec2(-offset, offset), // top-left
//      vec2(0.0f, offset), // top-center
//      vec2(offset, offset), // top-right
//      vec2(-offset, 0.0f),   // center-left
//      vec2(0.0f, 0.0f),   // center-center
//      vec2(offset, 0.0f),   // center-right
//      vec2(-offset, -offset), // bottom-left
//      vec2(0.0f, -offset), // bottom-center
//      vec2(offset, -offset)  // bottom-right    
//      );
//
//   float kernel[9] = float[](
//      -1, -1, -1,
//      -1, 9, -1,
//      -1, -1, -1
//      );
//
//   vec3 sampleTex[9];
//   for (int i = 0; i < 9; i++)
//   {
//      sampleTex[i] = vec3(texture(textureScrn, TexCoords.st + offsets[i]));
//   }
//   vec3 col = vec3(0.0);
//   for (int i = 0; i < 9; i++)
//      col += sampleTex[i] * kernel[i];
//   return vec4(col, 1.0);
//}
//vec4 WaterShaders(sampler2D water)
//{
//   vec2 tc = TexCoords.xy;
//   vec2 p = -1.0 + 2.0 * tc;
//   float len = length(p);
//   vec2 uv = tc + (p / len) * cos(len * 12.0 - time * 4.0) * 0.009;
//
//   vec3 col = vec3(texture(water, uv));
//
//   return vec4(col, 1.0);
//}