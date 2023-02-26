#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 objectColor;

uniform vec3 viewPosition;

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;

void main()
{
    //ambient lighting
	float ambientStrength = 0.2;
	
	vec3 ambient = light.ambient * material.ambient;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	
	//diffusion lighting
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diff * material.diffuse);
	
	//specular lighting
	vec3 viewDir = normalize(viewPosition - FragPos);
	vec3 reflectionDirection = reflect(-lightDir, norm);
	
	float spec = pow(max(dot(viewDir, reflectionDirection), 0.0), material.shininess);
	vec3 specular = light.specular * (spec * material.specular);
	
	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result,1.0f);
}