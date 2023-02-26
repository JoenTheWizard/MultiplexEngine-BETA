#version 330 core
out vec4 FragColor;

in vec3 Normals;
in vec3 FragPos;
//in vec2 TextCoords;
uniform samplerCube cubemap;
uniform vec3 viewPos;
vec3 CalcReflection(vec3 fragPos, vec3 normal, vec3 viewPos, samplerCube reflector)
{
	vec3 I = normalize(fragPos - viewPos);
	vec3 R = reflect(I, normalize(normal));

	return texture(reflector,R).rgb;
}
vec3 CalcPhongLighting(vec3 normal, vec3 fragPos, vec3 viewPos)
{
	vec3 lightColor = vec3(0.2, 0.75, 0.5);
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
	
	return (ambient + diffuse + specular) * CalcReflection(fragPos, normal, viewPos, cubemap);
}

void main()
{
	vec3 result = CalcReflection(FragPos, Normals, viewPos, cubemap);
	FragColor = vec4(result,1);
}