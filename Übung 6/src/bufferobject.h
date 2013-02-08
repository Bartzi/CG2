#pragma once

#include <GL/freeglut.h>

#include "common.h"
#include "gpuquery.h"

#include <vector>


class BufferObject
{
public:
    BufferObject(
        const GLenum target
    ,   const GLenum usage);

    virtual ~BufferObject();

    const GLuint buffer();

    const GLenum target() const;
    const GLenum usage() const;

    template <typename T>
    void data(
		const std::vector<T> & v
	,	const GLenum type
	,	const GLint  size);

    template <typename T>
    void data(
        const T * v
    ,   const GLsizei count
	,	const GLenum type
	,	const GLint size);

    void bind();
    void bind(const GLint attributeLocation);

    void release();
    void release(const GLint attributeLocation);

	void draw(const GLenum mode);

protected:
    void generateBuffer();

protected:
    GLuint m_buffer;

    const GLenum  m_target;
    const GLenum  m_usage;

	GLsizei m_count;
	GLint   m_size;
	GLenum  m_type;
};


template <typename T>
void BufferObject::data(
    const std::vector<T> & v
,	const GLenum type
,	const GLint  size)
{
    bind();

	m_count  = static_cast<GLsizei>(v.size());
	m_type   = type;
	m_size   = size;

	int te = sizeof(T);

	glBufferData(m_target, m_count * sizeof(T), v.data(), m_usage);
	glError();
}

template <typename T>
void BufferObject::data(
    const T * v
,   const GLsizei count
,	const GLenum type
,	const GLint size)
{
    bind();

	m_count = count;
	m_type  = type;
	m_size  = size;

    glBufferData(m_target, m_count * sizeof(T), v, m_usage);
	glError();
}