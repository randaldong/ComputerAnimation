#include "Shader.h"

//-----------------------------------------------Shader-----------------------------------------------
Shader::Shader(const char* shaderFile, ShaderType shaderType)
{
    // Try to read shader codes from the shader file.
    std::string shaderCode = "";
    std::ifstream shaderStream(shaderFile, std::ios::in);
    if (shaderStream.is_open()) {
        std::string Line = "";
        while (getline(shaderStream, Line))
            shaderCode += "\n" + Line;
        shaderStream.close();
    }
    else {
        std::cerr << "Impossible to open " << shaderFile << ". "
            << "Check to make sure the file exists and you passed in the "
            << "right filepath!"
            << std::endl;
        return;
    }

    // Add #define & create shaderID
    switch (shaderType) {
        case eVertex:
            shaderCode = "#define VERTEX_SHADER\n" + shaderCode;
            shaderID = glCreateShader(GL_VERTEX_SHADER);
            break;
        case eFragment:
            shaderCode = "#define FRAGMENT_SHADER\n" + shaderCode;
            shaderID = glCreateShader(GL_FRAGMENT_SHADER);
            break;
    }
    shaderCode = "#version 330 core\n" + shaderCode;

    // Compile Shader.
    std::cerr << "Compiling shader: " << shaderFile << std::endl;
    char const* sourcePointer = shaderCode.c_str();
    glShaderSource(shaderID, 1, &sourcePointer, NULL);
    glCompileShader(shaderID);

    // Check shader compile status.
    GLint isShaderCompiled = GL_FALSE;
    int InfoLogLength;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isShaderCompiled);
    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> shaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(shaderID, InfoLogLength, NULL, shaderErrorMessage.data());
        std::string msg(shaderErrorMessage.begin(), shaderErrorMessage.end());
        std::cerr << msg << std::endl;
        // Delete shader
        glDeleteShader(shaderID);
        shaderID = 0;
        return;
    }
    else {
        if (shaderType == eVertex)
            printf("Successfully compiled vertex shader!\n");
        else if (shaderType == eFragment)
            printf("Successfully compiled fragment shader!\n");
    }
}

Shader::~Shader()
{
    glDeleteShader(shaderID);
    shaderID = 0;
}

//--------------------------------------------ShaderProgram--------------------------------------------
ShaderProgram::ShaderProgram(const char* shaderFile)
{
    // Initialize & create Shader*
    for (int i = 0; i < 2; i++) {
        shaders[i] = nullptr;
    }
    shaders[0] = new Shader(shaderFile, Shader::eVertex);
    shaders[1] = new Shader(shaderFile, Shader::eFragment);

    // Link shader program.
    printf("Linking program\n");
    programID = glCreateProgram();
    for (int i = 0; i < 2; i++) {
        if (shaders[i]) {
            glAttachShader(programID, shaders[i]->shaderID);
        }
    }
    glLinkProgram(programID);

    // Check program link status.
    GLint isProgramLinked = GL_FALSE;
    int InfoLogLength;
    glGetProgramiv(programID, GL_LINK_STATUS, &isProgramLinked);
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(programID, InfoLogLength, NULL, ProgramErrorMessage.data());
        std::string msg(ProgramErrorMessage.begin(), ProgramErrorMessage.end());
        std::cerr << msg << std::endl;
        glDeleteProgram(programID);
        return;
    }
    else {
        printf("Successfully linked program!\n");
    }
}

ShaderProgram::~ShaderProgram()
{
    for (int i = 0; i < 2; i++) {
        glDetachShader(programID, shaders[i]->shaderID);
        delete shaders[i];
    }
    glDeleteProgram(programID);
}

