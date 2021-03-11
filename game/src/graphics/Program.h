//
// Created by bar44 on 10/30/2020.
//
#pragma once

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <string>
#include <unordered_map>
#include <sstream>
#include <utility>
#include "fstream"
#include "iostream"
#include "glm/glm.hpp"

class Program {
public:
    unsigned int m_RendererID;
    bool m_Success;
    std::unordered_map<std::string, int> m_UniformLocationCache;

    Program(std::string vertexPath, std::string fragmentPath);

    ~Program();

    void Bind() const;

    void Unbind() const;

    bool CompileProgram();
    template<typename T>
    void SetUniform(const std::string& name, T value);

    template<>
    void SetUniform<int>(const std::string& name, int value);
    template<>
    void SetUniform<float>(const std::string& name, float value);
    template<>
    void SetUniform<glm::vec3>(const std::string& name, glm::vec3 value);
    template<>
    void SetUniform<glm::vec4>(const std::string& name, glm::vec4 value);
    template<>
    void SetUniform<unsigned int>(const std::string& name, unsigned int value);
    template<>
    void SetUniform<glm::mat4>(const std::string& name, glm::mat4 value);
    int GetUniformLocation(const std::string& name);
private:
    const std::string m_FragmentPath;
    const std::string m_VertexPath;
};

template <typename T>
void Program::SetUniform(const std::string& name, T value)
{
    std::cerr << "ERROR: use of set uniform function without a type!\n";
}
