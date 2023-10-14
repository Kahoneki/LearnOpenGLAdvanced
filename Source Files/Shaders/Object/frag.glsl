#version 330 core
#define NR_POINT_LIGHTS 4

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct DirLight {
	vec3 direction;

	//Strengths
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	//Strengths
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Spotlight {
	vec3 position;
	vec3 direction;
	float innerCutoffAngle;
	float outerCutoffAngle;

	//Strengths
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

in vec2 TexCoords;
in vec3 fragPos;
in vec3 normal;

uniform Material material;

uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Spotlight spotlight;

uniform vec3 lightPos;
uniform vec3 viewPos;

out vec4 FragColour;



//Forward declarations
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotlight(Spotlight light, vec3 normal, vec3 fragPos, vec3 viewDir);



void main()
{
	//Properties
	vec3 norm = normalize(normal);
	vec3 viewDir = normalize(viewPos - fragPos);

	vec3 result;

	//Directional Lights
	result = CalcDirLight(dirLight, norm, viewDir);

	//Point lights
	for (int i=0; i < NR_POINT_LIGHTS; i++)
		result += CalcPointLight(pointLights[i], norm, fragPos, viewDir);

	//Spotlight
	//result += CalcSpotlight(spotlight, norm, fragPos, viewDir);

	FragColour = vec4(result, 1.0);
}



vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDirFromFrag = normalize(-light.direction); //Direction from frag to the light source

	//Diffuse shading
	float diff = max(dot(normal, lightDirFromFrag), 0.0);

	//Specular shading
	vec3 reflectDir = reflect(-lightDirFromFrag, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	
	//Combine results
	vec3 ambient  = light.ambient * vec3(texture(material.diffuse, TexCoords));
	vec3 diffuse  = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

	return (ambient + diffuse + specular);
}



vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDirFromFrag = normalize(light.position - fragPos); //Direction from frag to the light source
	
	//Diffuse shading
	float diff = max(dot(normal, lightDirFromFrag), 0.0);

	//Specular shading
	vec3 reflectDir = reflect(-lightDirFromFrag, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	//Attenuation
	float distanceLightTravelled = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant +
							   light.linear * distanceLightTravelled +
							   light.quadratic * (distanceLightTravelled * distanceLightTravelled));
	
	//Combine results
	vec3 ambient  = light.ambient * vec3(texture(material.diffuse, TexCoords));
	vec3 diffuse  = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}


vec3 CalcSpotlight(Spotlight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDirFromFrag = normalize(light.position - fragPos); //Direction from frag to the light source

	//Diffuse shading
	float diff = max(dot(normal, lightDirFromFrag), 0.0);

	//Specular shading
	vec3 reflectDir = reflect(-lightDirFromFrag, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	//Attenuation
	float distanceLightTravelled = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant +
							   light.linear * distanceLightTravelled +
							   light.quadratic * (distanceLightTravelled * distanceLightTravelled));

	//Spotlight intensity
	float theta = dot(lightDirFromFrag, normalize(-light.direction));
	float epsilon = light.innerCutoffAngle - light.outerCutoffAngle;
	float intensity = clamp((theta - light.outerCutoffAngle) / epsilon, 0.0, 1.0);

	//Combine results
	vec3 ambient  = light.ambient * vec3(texture(material.diffuse, TexCoords));
	vec3 diffuse  = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;

	return (ambient + diffuse + specular);

}