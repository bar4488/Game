//
// Created by bar44 on 10/30/2020.
//

#ifndef GOME_SHADER_H
#define GOME_SHADER_H


#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <string>
#include <unordered_map>
#include <sstream>
#include <utility>
#include "fstream"
#include "iostream"

class Shader {
public:
    unsigned int m_RendererID;
    std::unordered_map<std::string, int> m_UniformLocationCache;

    Shader(std::string vertexPath, std::string fragmentPath);

    ~Shader();

    void Bind() const;

    void Unbind() const;

    bool CompileShader();
    void SetUniform1f(const std::string& name, float value);
    void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
    void SetUniform3f(const std::string& name, float v0, float v1, float v2);
    void SetUniform1i(const std::string& name, int value);
    void SetUniformMatrix4fv(const std::string& name, unsigned int count, bool transpose, float* matrix);
    int GetUniformLocation(const std::string& name);
private:
    const std::string m_FragmentPath;
    const std::string m_VertexPath;
};


#endif //GOME_SHADER_H
