//----------------------------------------Vertex Shader----------------------------------------

#ifdef VERTEX_SHADER

// Inputs
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

// Uniforms
uniform mat4 ModelMtx = mat4(1);
uniform mat4 ModelViewProjectionMtx = mat4(1);

// Outputs
out vec3 fragPosition;
out vec3 fragNormal;

void main()
{
    gl_Position = ModelViewProjectionMtx * vec4(position, 1.0);
	fragPosition = vec3(ModelMtx * vec4(position, 1));
	fragNormal = vec3(ModelMtx * vec4(normal, 0));
}

#endif

//---------------------------------------Fragment Shader---------------------------------------

#ifdef FRAGMENT_SHADER

// Inputs
in vec3 fragPosition;
in vec3 fragNormal;

// Uniforms
uniform vec3 ViewPosition = vec3(0, 0, 9); // camera position
uniform vec3 AmbientColor; // final reflected ambient color
// Render with two different lights
// Both are directional lights, with different colors
uniform vec3 LightDirection_1 = normalize(vec3(1, 1, 1));
uniform vec3 LightColor_1 = vec3(0.63, 0.63, 0.09);
uniform vec3 LightDirection_2 = normalize(vec3(-1, -1, 1));
uniform vec3 LightColor_2 = vec3(0.81, 0.27, 0.81);

// Outputs
out vec4 fragColor;

// Function declaration
vec3 CalDirLight(vec3 norm, vec3 viewDir, vec3 LightDirection, vec3 LightColor, vec3 AmbientColor);

void main()
{
	// properties
    vec3 norm = normalize(fragNormal);
    vec3 viewDir = normalize(ViewPosition - fragPosition);

	// calculate color
	vec3 finalColor = CalDirLight(norm, viewDir, LightDirection_1, LightColor_1, AmbientColor);
	finalColor += CalDirLight(norm, viewDir, LightDirection_2, LightColor_2, AmbientColor);

	// Gamma correction
	float gamma = 2.2;
    fragColor = vec4(pow(finalColor, vec3(1.0/gamma)), 1);
}

vec3 CalDirLight(vec3 norm, vec3 viewDir, vec3 LightDirection, vec3 LightColor, vec3 AmbientColor)
{
	vec3 lightDir = normalize(LightDirection);
	// diffuse shading
	vec3 DiffuseColor = LightColor * max(dot(norm, lightDir), 0.0);
	// specular shading
    vec3 reflectDir = reflect(-lightDir, norm);
	vec3 SpecularColor = LightColor * pow(max(dot(viewDir, reflectDir), 0.0), 9); // shininess = 9
	return AmbientColor + DiffuseColor + SpecularColor;
}

#endif