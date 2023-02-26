#version 330 core
out vec4 FragColor;

in vec3 Normals;
in vec3 FragPos;
in vec4 ClipSpace;
in vec2 TextCoords;

vec3 CalcPhongLighting(vec3 normal, vec3 fragPos, vec3 viewPos);

uniform vec3 viewPosition;
uniform vec3 viewDirection;

uniform sampler2D waterText;
uniform sampler2D dudvmap;
uniform samplerCube cubemap;
uniform sampler2D grass;
uniform sampler2D stone;
uniform sampler2D stoneNormal;

uniform float waterVelocity;

//For the flashlight
struct SpotLight {
    vec3 direction;
	
	//Spotlight Variables
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
	bool isFlashlightOn;
};
uniform SpotLight spotLight;
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLightWater(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 lightObj);
vec3 AmbientSetting(vec3 fragPos);

in VS_OUT {
	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
} fs_in;

vec4 WaterShader()
{
	vec2 ndc = (ClipSpace.xy/ClipSpace.w)/2.0+0.5;
	vec2 reflectTextCoords = vec2(ndc.x, -ndc.y);
	
	vec2 distortion1 = texture(dudvmap, vec2(TextCoords.x + waterVelocity, TextCoords.y)).rg*0.1;
	distortion1 = TextCoords + vec2(distortion1.x,distortion1.y+waterVelocity);
	vec2 totalDistortion = (texture(dudvmap,distortion1).rg*2.0-1.0)*0.5;
	
	reflectTextCoords += totalDistortion;
	return texture(waterText,reflectTextCoords);
}
vec3 NormalMapsLighting(vec3 normals, sampler2D diffuseMap)
{
	vec3 lightcol = (FragPos.y < 20) ? texture(diffuseMap,TextCoords).rgb : texture(stone,TextCoords).rgb;
	vec3 ambient = 0.3 * lightcol;
	
	vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
	float diff = max(dot(lightDir, normals), 0.0);
	vec3 diffuse = diff * lightcol;
	
	vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
	vec3 reflectDir = reflect(-lightDir, normals);
	vec3 halfWayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normals, halfWayDir), 0.0), 32.0);
	vec3 specular = vec3(0.2) * spec;
	
	return ambient + diffuse + specular;
}
void main()
{
	vec3 I = normalize(FragPos - viewPosition);
	vec3 R = reflect(I, normalize(Normals));
	
	vec2 distortion1 = (texture(dudvmap, vec2(TextCoords.x + waterVelocity, TextCoords.y)).rg * 2.0 - 1.0) * 0.02;
	vec2 distortion2 = (texture(dudvmap, vec2(-TextCoords.x, TextCoords.y + waterVelocity)).rg * 2.0 - 1.0) * 0.02;
	vec2 totalDistortion = distortion1 + distortion2;
	
	R.xy += totalDistortion;
	
	vec3 fresNorm = normalize(viewPosition - FragPos);
	float fresnelAngle = dot(fresNorm, vec3(0,1,0));
	
	//Normal map for stone and grass
	vec4 normalMapColor = texture(stoneNormal, TextCoords);
	vec3 stoneNorms = vec3(normalMapColor.r * 2.0 - 1.0, normalMapColor.b, normalMapColor.g * 2.0 - 1.0);

	//OUTPUT CALCULATION
	vec3 viewDirec = normalize(viewPosition - FragPos);
	vec3 Result = (spotLight.isFlashlightOn) ?
	CalcSpotLight(spotLight, normalize(stoneNorms), FragPos, viewDirec) : vec3(0);
	Result += NormalMapsLighting(normalize(stoneNorms), grass);
	
	vec3 WaterResult = vec3(mix(texture(cubemap,vec3(R.xy,-R.z)),WaterShader(),clamp(fresnelAngle,0,1)));
	vec4 watNormColor = texture(stoneNormal, totalDistortion);
	vec3 watNorms = normalize(vec3(watNormColor.r * 2.0 - 1.0, watNormColor.b, watNormColor.g * 2.0 - 1.0));
	watNorms.xy += totalDistortion;
	
	WaterResult += (spotLight.isFlashlightOn) ? CalcSpotLightWater(spotLight, watNorms, FragPos, viewDirec, WaterResult) : vec3(0);
	
	FragColor  = (FragPos.y < 4.5 ) ? vec4(WaterResult,1.0) : vec4(Result, 1.0);
}
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightObj = (fragPos.y < 20) ? vec3(texture(grass,TextCoords)) : vec3(texture(stone, TextCoords));
    vec3 lightDir = normalize(viewPosition - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    // attenuation
    float distance = length(viewPosition - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * lightObj;
    vec3 diffuse = light.diffuse * diff * lightObj;
    vec3 specular = light.specular * spec * lightObj;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLightWater(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 lightObj)
{
	vec3 lightDir = normalize(viewPosition - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    // attenuation
    float distance = length(viewPosition - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * lightObj;
    vec3 diffuse = light.diffuse * diff * lightObj;
    vec3 specular = light.specular * spec * lightObj;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}