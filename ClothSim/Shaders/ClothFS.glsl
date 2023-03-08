#version 330 core

// Inputs
in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;

// Uniforms
uniform sampler2D uniTexSmplr;
uniform vec3 uniLightPos;
uniform vec3 uniLightColor;

// Outputs
out vec4 fragColor;

void main()
{
    // Ambient
    float ambientStrength = 0.5f;
    vec3 ambient = ambientStrength * uniLightColor;

    // Diffuse
    vec3 lightDir = normalize(uniLightPos - fragPosition);
    vec3 diffuse = uniLightColor * max(dot(fragNormal, lightDir), 0.0);

    // Specular
    const vec3 eyepos = vec3(0, 0, 0);
    vec3 eyedirn = normalize(eyepos - fragPosition);
    vec3 halfVec = normalize(lightDir + eyedirn);
    vec3 specular = uniLightColor * pow(max(dot(fragNormal, halfVec), 0.0), 81); // shiness = 81

    // Texture color
    // texture() will output the color obtained by sampling the texture with configured conditions
    fragColor = texture(uniTexSmplr, fragTexCoord);
    vec3 objectColor = vec3(fragColor.x, fragColor.y, fragColor.z);
    vec3 finalColor = (ambient + diffuse + specular) * objectColor;
    fragColor = vec4(finalColor, 1.0f); // gamma correction not used, considering the actual performance
}
