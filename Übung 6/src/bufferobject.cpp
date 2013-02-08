
#include <GL/glew.h>

#include "bufferobject.h"


BufferObject::BufferObject(
    const GLenum target
,   const GLenum usage)

:   m_buffer(-1)
,   m_target(target)
,   m_usage(usage)

,	m_count(0)
,	m_size(0)

,	m_type(GL_NONE)
{
}

BufferObject::~BufferObject()
{
	if(GPUQuery::isBuffer(m_buffer))
	{
		glDeleteBuffers(1, &m_buffer);
		glError();
	}
}

const GLuint BufferObject::buffer()
{
    if(!GPUQuery::isBuffer(m_buffer))
        generateBuffer();

    return m_buffer;
}

const GLenum BufferObject::target() const
{
    return m_target;
}

const GLenum BufferObject::usage() const
{
    return m_usage;
}

void BufferObject::bind()
{
    if(!GPUQuery::isBuffer(m_buffer))
        generateBuffer();

    glBindBuffer(m_target, m_buffer);
    glError();
}

void BufferObject::bind(const GLint attributeLocation)
{
    if(-1 == attributeLocation)
        return;

	assert(m_target == GL_ARRAY_BUFFER);

    bind();

	glEnableVertexAttribArray(attributeLocation);
    glError();
	glVertexAttribPointer(attributeLocation, m_size, m_type, GL_FALSE, 0, 0);
	glError();
}

void BufferObject::draw(const GLenum mode)
{
	assert(m_target == GL_ELEMENT_ARRAY_BUFFER);

	bind();

	glDrawElements(mode, m_count, m_type, 0);
	glError();

	release();
}

void BufferObject::release(const GLint attributeLocation)
{
    if(-1 == attributeLocation)
        return;

    if(!GPUQuery::isBuffer(m_buffer))
        return;

    glDisableVertexAttribArray(attributeLocation);
    glError();

    glBindBuffer(m_target, 0);
    glError();
}

void BufferObject::release()
{
    if(!GPUQuery::isBuffer(m_buffer))
        return;

    glBindVertexArray(0);

    glBindBuffer(m_target, 0);
    glError();
}

void BufferObject::generateBuffer()
{
    if(GPUQuery::isBuffer(m_buffer))
        return;

    glGenBuffers(1, &m_buffer);
    glError();
}