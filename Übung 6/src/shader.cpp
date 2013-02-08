
#include <GL/glew.h>

#include "shader.h"
#include "gpuquery.h"
#include "program.h"

#include <map>
#include <cassert>
#include <iostream>


const Shader::t_typeStrings Shader::typeStrings = initializeTypeStrings();

const Shader::t_typeStrings Shader::initializeTypeStrings()
{
    t_typeStrings typeStrings;

    typeStrings[GL_TESS_EVALUATION_SHADER] = "GL_TESS_EVALUATION_SHADER";
    typeStrings[GL_TESS_CONTROL_SHADER]    = "GL_TESS_CONTROL_SHADER";
    typeStrings[GL_COMPUTE_SHADER]         = "GL_COMPUTE_SHADER";
    typeStrings[GL_FRAGMENT_SHADER]        = "GL_FRAGMENT_SHADER";
    typeStrings[GL_GEOMETRY_SHADER]        = "GL_GEOMETRY_SHADER";
    typeStrings[GL_VERTEX_SHADER]          = "GL_VERTEX_SHADER";

    return typeStrings;
}

Shader::Shader(const GLenum type)

:	m_type(type)
,	m_shader(0)
,   m_compiled(false)
{
	if(typeStrings.end() == typeStrings.find(type))
        std::cerr << "Shader type " << type << " is not supported." << std::endl;

    m_shader = glCreateShader(type);
    glError();
}
    
Shader::~Shader()
{
	if(GPUQuery::isShader(m_shader))
    {
        glDeleteShader(m_shader);
        glError();

        m_shader = 0;
    }
}

const bool Shader::setSource(
    const std::string & source
,   const bool update)
{
    m_source = source;

    const GLchar * chr(source.c_str());

    glShaderSource(m_shader, 1, &chr, nullptr);
    glError();
    glCompileShader(m_shader);
    glError();

    GLint status(GL_FALSE);
    glGetShaderiv(m_shader, GL_COMPILE_STATUS, &status);

    m_compiled = (GL_TRUE == status);
    m_log = "";

    if(!m_compiled)
    {
        GLint maxLength(0);
        GLint logLength(0);

		glGetShaderiv(m_shader, GL_INFO_LOG_LENGTH, &maxLength);
        glError();

        GLchar *log = new GLchar[maxLength];
        glGetShaderInfoLog(m_shader, maxLength, &logLength, log);
        glError();

        m_log = log;

		t_typeStrings::const_iterator i(typeStrings.find(type()));
		if(typeStrings.end() != i)
			std::cerr << "Compiling shader of type " << i->second << " failed." << std::endl;
		else
			std::cerr << "Compiling shader of type " << type() << " failed." << std::endl;

		if(!m_log.empty())
			std::cerr << m_log;
    }

    if(update)
        this->update();

    return isCompiled();
}

const std::string & Shader::source() const
{
    return m_source;
}

void Shader::update()
{
	t_programs::const_iterator i(m_programs.begin());
	const t_programs::const_iterator iEnd(m_programs.end());

	for(; i != iEnd; ++i)
		(*i)->invalidate();
}

const GLenum Shader::type() const
{
    return m_type;
}

const GLuint Shader::shader() const
{
    return m_shader;
}

const bool Shader::isCompiled() const
{
    return m_compiled;
}

const Shader::t_programs & Shader::programs() const
{
	return m_programs;
}

Shader::t_programs & Shader::programs()
{
	return m_programs;
}