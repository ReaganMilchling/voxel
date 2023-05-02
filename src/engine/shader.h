#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <unordered_map>

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

class Shader
{
private:
    unsigned int m_RendererID;
    std::unordered_map<std::string, int> m_UniformLocationCache; //caching uniforms
public:
    Shader(const std::string& filepath);
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();
    void Bind() const;
    void Unbind() const;

    //set uniforms
    void setBool(const std::string& name, bool value);
    void setUniform1i(const std::string& name, int value);
    void setUniform1f(const std::string& name, float value);
    void setUniform2fv(const std::string& name, const glm::vec2& value);
    void setUniform3fv(const std::string& name, const glm::vec3& value);
    void setUniform4fv(const std::string& name, const glm::vec4& value);
    void setUniformMatrix2fv(const std::string& name, const glm::mat2& mat);
    void setUniformMatrix3fv(const std::string& name, const glm::mat3& mat);
    void setUniformMatrix4fv(const std::string& name, const glm::mat4& mat);
private:
    ShaderProgramSource ParseShader(const std::string& filepath);
    unsigned int CompileShader(unsigned int type, const std::string& source);
    unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
    void checkCompileErrors(GLuint shader, std::string type);

    int GetUniformLocation(const std::string& name); //caching uniforms
};