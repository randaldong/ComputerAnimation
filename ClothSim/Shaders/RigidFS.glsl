#version 330 core

// Inputs
in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;

// Uniforms
uniform sampler2D uniTexSmplr;
uniform vec3 uniLightPos;
uniform vec3 uniLightColor;
uniform bool uniIsUseColor;
uniform bool uniIsTransparent;

// Outputs
out vec4 fragColor;

void main()
{
    // Ambient
    float ambientStrength = 0.8f;
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
    vec3 finalColor;
    if (!uniIsUseColor) {
        fragColor = texture(uniTexSmplr, fragTexCoord);
        vec3 objectColor = vec3(fragColor.x, fragColor.y, fragColor.z);
        finalColor = (ambient + diffuse + specular) * objectColor;
    }
    else {
        finalColor = (ambient + diffuse + specular) * uniLightColor;
    }
    if (uniIsTransparent) {
        fragColor = vec4(finalColor, 0.7f); // gamma correction not used, considering the actual performance
    }
    else {
        fragColor = vec4(finalColor, 1.0f); // gamma correction not used, considering the actual performance
    }
}
