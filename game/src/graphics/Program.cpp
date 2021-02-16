//
// Created by bar44 on 10/30/2020.
//

#include "Program.h"
#include "glm/vec3.hpp"

Program::Program(std::string vertexPath, std::string fragmentPath): 
    m_RendererID(0), 
    m_FragmentPath(std::move(fragmentPath)), 
    m_VertexPath(std::move(vertexPath)),
    m_Success(false)
{
    CompileProgram();
}

Program::~Program() {
    glDeleteProgram(m_RendererID);
}

void Program::Bind() const {
    glUseProgram(m_RendererID);
}

void Program::Unbind() const {
    glUseProgram(0);
}

bool Program::CompileProgram() {
    std::cout << "compiling shader" << std::endl;

    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(m_VertexPath, std::ios::in);
    if (VertexShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << VertexShaderStream.rdbuf();
        VertexShaderCode = sstr.str();
        VertexShaderStream.close();
    } else {
        std::cout << "\tImpossible to open " << m_VertexPath << "\n";
        return 0;
    }

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(m_FragmentPath, std::ios::in);
    if (FragmentShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << FragmentShaderStream.rdbuf();
        FragmentShaderCode = sstr.str();
        FragmentShaderStream.close();
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    char const *VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer, nullptr);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, nullptr, &VertexShaderErrorMessage[0]);
        printf("\tvertex shader error: %s: %s\n", m_VertexPath.c_str(), &VertexShaderErrorMessage[0]);
        return false;
    }

    // Compile Fragment Shader
    char const *FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, nullptr);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, nullptr, &FragmentShaderErrorMessage[0]);
        printf("\tfragment shader error: %s: %s\n", m_FragmentPath.c_str(), &FragmentShaderErrorMessage[0]);
        return false;
    }

    // Link the program
    printf("\tLinking program...\n");
    m_RendererID = glCreateProgram();
    glAttachShader(m_RendererID, VertexShaderID);
    glAttachShader(m_RendererID, FragmentShaderID);
    glLinkProgram(m_RendererID);

    // Check the program
    glGetProgramiv(m_RendererID, GL_LINK_STATUS, &Result);
    glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(m_RendererID, InfoLogLength, nullptr, &ProgramErrorMessage[0]);
        printf("\tprogram linking error: %s\n", &ProgramErrorMessage[0]);
        return false;
    }

    glDetachShader(m_RendererID, VertexShaderID);
    glDetachShader(m_RendererID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    m_Success = true;
}

int Program::GetUniformLocation(const std::string& name) {
    if(m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) {
        return m_UniformLocationCache[name];
    }

    int location = glGetUniformLocation(m_RendererID, name.c_str());
    if (location == -1) {
        std::cerr << "Warning: uniform named '" << name << "' not found!\n";
        std::cerr << "\tfragment path: " << m_FragmentPath << "\n\tvertex path: " << m_VertexPath << std::endl;
    }
    m_UniformLocationCache[name] = location;
    return location;
}

void Program::SetUniform1f(const std::string &name, float value) {
    glUniform1f(GetUniformLocation(name), value);
}

void Program::SetUniform4f(const std::string &name, float v0, float v1, float v2, float v3) {
    glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void Program::SetUniform3f(const std::string &name, float v0, float v1, float v2) {
    glUniform3f(GetUniformLocation(name), v0, v1, v2);
}

void Program::SetUniformVec3(const std::string &name, glm::vec3 v) {
    glUniform3f(GetUniformLocation(name), v.x, v.y, v.z);
}

void Program::SetUniformMatrix4fv(const std::string &name, unsigned int count, bool transpose, float *matrix) {
    glUniformMatrix4fv(GetUniformLocation(name), count, transpose, matrix);
}

void Program::SetUniform1i(const std::string &name, int value) {
    glUniform1i(GetUniformLocation(name), value);
}

