#version 300 es

uniform mat4 vp;

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Color;
layout(location = 2) in vec3 in_Normal;

out vec3 ex_Color;

vec3 lighting()
{
	vec3 lightPos = vec3(0.0f, 40.0f, 0.0f);
	vec3 viewPos = vec3(0.0f, 0.0f, 1.0f);

	vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
	float ambientStrength = 0.1f;
	float specularStrength = 0.5f;

	// ambient
    vec3 ambient = ambientStrength * lightColor;

	// diffuse	
	//vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - in_Position);
    float diff = max(dot(in_Normal, lightDir), 0.0f);
    vec3 diffuse = diff * lightColor;

	// specular
    vec3 viewDir = normalize(viewPos - in_Position);
    vec3 reflectDir = reflect(-lightDir, in_Normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32.0f);
    vec3 specular = specularStrength * spec * lightColor; 

	return ambient + diffuse;
}

void main(void) 
{
    gl_Position = vp * vec4(in_Position.x, in_Position.y, in_Position.z, 1.0f);

    ex_Color = in_Color * lighting();
}