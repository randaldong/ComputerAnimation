#version 330 core

// Inputs
layout (location = 0) in vec3 vsPosition;
layout (location = 1) in vec3 vsNormal;

// Uniforms
uniform mat4 uniModelMtx;
uniform mat4 uniViewMtx;
uniform mat4 uniProjMtx;

// Outputs
out vec3 fragPosition;
out vec3 fragNormal;

void main()
{
    gl_Position = uniProjMtx * uniViewMtx * uniModelMtx * vec4(vsPosition, 1.0f);

    mat4 ModelViewMtx = uniViewMtx * uniModelMtx;

    vec4 fragPosition_w = ModelViewMtx * vec4(vsPosition, 1.0f);
    fragPosition = fragPosition_w.xyz / fragPosition_w.w;

    fragNormal = vec3(transpose(inverse(ModelViewMtx)) * vec4(vsNormal, 0));
}
