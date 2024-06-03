#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "shader.h"


Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
    }

    // 2. compile shaders
    unsigned int vertex = CompileShader(GL_VERTEX_SHADER, vertexCode);
    unsigned int fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentCode);

    // shader Program
    m_RendererID = glCreateProgram();
    glAttachShader(m_RendererID, vertex);
    glAttachShader(m_RendererID, fragment);
    glLinkProgram(m_RendererID);
    checkCompileErrors(m_RendererID, "program");

    glDeleteShader(vertex);
    glDeleteShader(fragment);

}

Shader::Shader(const std::string& filepath) : m_RendererID(0)
{
    ShaderProgramSource source = ParseShader(filepath);
    m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
    glDeleteProgram(m_RendererID);
}

void Shader::Bind() const
{
    glUseProgram(m_RendererID);
}

void Shader::Unbind() const
{
    glUseProgram(0);
}

// private helper functions
//
ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE = -01, VERTEX = 0, FRAMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                type = ShaderType::FRAMENT;
            }
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }
    return { ss[0].str(), ss[1].str() };
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, NULL);
    glCompileShader(id);

    checkCompileErrors(id, (GL_VERTEX_SHADER ? "vertex" : "fragment"));

    return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    unsigned int program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);
    checkCompileErrors(program, "program");

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int Shader::GetUniformLocation(const std::string& name)
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
        return m_UniformLocationCache[name];

    int location = glGetUniformLocation(m_RendererID, name.c_str());
    if (location == -1)
        std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;

    m_UniformLocationCache[name] = location;
    return location;
}

void Shader::checkCompileErrors(GLuint shader, std::string type)
{
    GLint success;
    GLchar infoLog[1024];
    if (type != "program")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" 
                << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" 
                << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

// Public set uniforms
//
void Shader::setBool(const std::string& name, bool value)
{
    glUniform1i(GetUniformLocation(name), (int)value);
}

void Shader::setUniform1i(const std::string& name, int value)
{
    glUniform1i(GetUniformLocation(name), value);
}

void Shader::setUniform1f(const std::string& name, float value)
{
    glUniform1f(GetUniformLocation(name), value);
}

void Shader::setUniform2fv(const std::string& name, const glm::vec2& value)
{
    glUniform2fv(GetUniformLocation(name), 1, &value[0]);
}

void Shader::setUniform3fv(const std::string& name, const glm::vec3& value)
{
    glUniform3fv(GetUniformLocation(name), 1, &value[0]);
}

void Shader::setUniform4fv(const std::string& name, const glm::vec4& value)
{
    glUniform4fv(GetUniformLocation(name), 1, &value[0]);
}

void Shader::setUniformMatrix2fv(const std::string& name, const glm::mat2& mat)
{
    glUniformMatrix2fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setUniformMatrix3fv(const std::string& name, const glm::mat3& mat)
{
    glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setUniformMatrix4fv(const std::string& name, const glm::mat4& mat)
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}
