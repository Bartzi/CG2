
#pragma once

#include <GL/freeglut.h>

#include <string>
#include <map>
#include <set>


class Program;

class Shader
{
public:
	typedef std::set<Program *> t_programs;

public:
    Shader(const GLenum type);
    virtual ~Shader();

    const bool setSource(
        const std::string & source
    ,   const bool update = true);
    const std::string & source() const;

    void update();

    const GLenum type() const;
    const GLuint shader() const;

    const bool isCompiled() const;

	const t_programs & programs() const;
	t_programs & programs();

protected:
    const GLenum m_type;
    GLuint m_shader;

    std::string m_source;
    std::string m_log;
    
	bool m_compiled;

	t_programs m_programs;

protected:
    typedef std::map<GLenum, std::string> t_typeStrings;
    static const t_typeStrings typeStrings;

    static const t_typeStrings initializeTypeStrings();
};