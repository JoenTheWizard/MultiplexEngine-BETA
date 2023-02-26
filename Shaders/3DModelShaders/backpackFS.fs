#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPosition;

//Setting textures from mtl for model
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

struct SpotLight {
	vec3 position;
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
};

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcPhongLighting(vec3 normal, vec3 fragPos, vec3 viewPos);

uniform SpotLight sl;
uniform vec3 viewPosition;

uniform bool isLighted;
//Reflection refraction model
vec3 CalcReflection(vec3 fragPos, vec3 normal, vec3 viewPos, samplerCube reflector);
uniform bool isReflect;
uniform samplerCube skybox;

uniform vec3 weaponColor;
void main()
{    
    //FragColor = texture(texture_diffuse1, TexCoords);
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPosition - FragPosition);
	
	vec3 result = (isLighted) ? CalcSpotLight(sl, norm, FragPosition, viewDir) : //spotlight
		(isReflect) ? CalcReflection(FragPosition, Normal, viewPosition, skybox) : //reflection
		CalcPhongLighting(norm, FragPosition, viewPosition); //basic phong model
	FragColor = vec4(result, 1.0);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0f);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}

vec3 CalcPhongLighting(vec3 normal, vec3 fragPos, vec3 viewPos)
{
	vec3 lightColor = vec3(1, 1, 1);
	float ambientStrength = 0.6;
	vec3 ambient = ambientStrength * lightColor;
	
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(vec3(1,-0.5,5) - fragPos);
	
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
	
	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;
	
	return (ambient + diffuse + specular) * weaponColor;
}

vec3 CalcReflection(vec3 fragPos, vec3 normal, vec3 viewPos, samplerCube reflector)
{
	vec3 I = normalize(fragPos - viewPos);
	vec3 R = refract(I, normalize(normal), 1/1.52);

	return texture(reflector,R).rgb;
}