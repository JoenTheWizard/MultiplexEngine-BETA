#version 330 core
out vec4 FragColor;
in vec3 positionCurr;
in vec2 TextCoords;
in vec3 Normals;
in vec3 FragPos;
in vec4 FragPosLightSpace;

uniform sampler2D grasstexture;
uniform sampler2D waterTexture;
uniform sampler2D depthMap;
uniform float timer;
uniform bool isWater;
uniform vec3 viewPos;

uniform vec3 lightPos;

uniform bool isCollider;
vec4 WaterShaders(sampler2D water);

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(depthMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

	if (projCoords.z > 1.0) //if the projected values if greater than 1 then shadow value is 0
		shadow = 0.0;
	
    return shadow;
}

vec3 lightingCalc()
{
	vec3 color = texture(grasstexture, TextCoords).rgb;
    vec3 normal = normalize(Normals);
    vec3 lightColor = vec3(0.25);
    // ambient
    vec3 ambient = 0.3 * color;
    // diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // calculate shadow
    float shadow = ShadowCalculation(FragPosLightSpace);                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
	return lighting;
}

void main()
{
	vec4 col = WaterShaders(waterTexture);
	vec4 portalCol = vec4(vec3(col.r /2., col.g * 0, col.b + cos(timer)),1.0);
	
	FragColor = (isWater) ? (isCollider) ? portalCol :
	WaterShaders(waterTexture) : vec4(lightingCalc(),1.0);
}

vec4 WaterShaders(sampler2D water)
{
	vec2 tc = TextCoords.xy;
	vec2 p = -1.0 + 2.0 * tc;
	float len = length(p);
	vec2 uv = tc + (p/len)*cos(len*12.0-timer*4.0)*0.03;
	
	vec3 col = vec3(texture(water, uv));
	
	return vec4(col,1.0);
}