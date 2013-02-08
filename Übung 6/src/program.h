
#pragma once

#include <set>
#include <string>

#include <GL/freeglut.h>
#include <glm/glm.hpp>


class Shader;

class Program
{
public:
	typedef std::set<Shader *> t_shaders;

public:
    Program();
    virtual ~Program();

    const bool use() const;
    const bool release() const;

    const GLuint program() const;

    const bool attach(Shader * shader);
    const bool detach(Shader * shader);

	const t_shaders & shaders();

    const bool link() const;
    const bool isLinked() const;

	void invalidate();

    // location access

    // TODO: location cache

    const GLint attributeLocation(const std::string & name) const;
    const GLint uniformLocation(const std::string & name) const;

    // value access

    void setUniform(
        const std::string & name
    ,   const int value) const;

    void setUniform(
        const std::string & name
    ,   const unsigned int value) const;

    void setUniform(
        const std::string & name
    ,   const float value) const;

    void setUniform(
        const std::string & name
    ,   const glm::vec2 & vec) const;

    void setUniform(
        const std::string & name
    ,   const glm::ivec2 & vec) const;

    void setUniform(
        const std::string & name
    ,   const glm::vec3 & vec) const;

    void setUniform(
        const std::string & name
    ,   const glm::ivec3 & vec) const;

    void setUniform(
        const std::string & name
    ,   const glm::vec4 & vec) const;

    void setUniform(
        const std::string & name
    ,   const glm::ivec4 & vec) const;

    void setUniform(
        const std::string & name
    ,   const glm::mat3 & mat
    ,   const bool transpose = false) const;

    void setUniform(
        const std::string & name
    ,   const glm::mat4 & mat
    ,   const bool transpose = false) const;

protected:
    GLuint m_program;

    mutable std::string m_log;

	mutable bool m_linked;
    mutable bool m_dirty;

    t_shaders m_shaders;
};