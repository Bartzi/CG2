
#include <GL/glew.h>

#include "framebufferobject.h"

#include "gpuquery.h"
#include "program.h"

#include <iostream>


//h ttp://www.swiftless.com/tutorials/opengl/framebuffer.html

FrameBufferObject::FrameBufferObject(
    const GLenum internalFormat
,   const GLenum format
,   const GLenum type
,   const GLenum attachment
,   const bool   depth)

:   m_fbo(-1)
,   m_texture(-1)
,   m_render(-1)

,	m_size(glm::ivec2(0))

,   m_internal(internalFormat)
,   m_format(format)
,   m_type(type)
,   m_attachment(attachment)
,   m_depth(depth)
{
}

FrameBufferObject::~FrameBufferObject()
{
	if(GPUQuery::isTexture(m_texture))
	{
        glDeleteTextures(1, &m_texture);
		glError();
	}
	if(GPUQuery::isRenderBuffer(m_render))
	{
        glDeleteRenderbuffers(1, &m_render);
		glError();
	}
	if(GPUQuery::isFrameBuffer(m_fbo))
	{
        glDeleteFramebuffers(1, &m_fbo);
		glError();
	}
}

void FrameBufferObject::bind() const
{
    if(!GPUQuery::isFrameBuffer(m_fbo))
        initialize();

    glViewport(0, 0, m_size.x, m_size.y);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glError();
}

void FrameBufferObject::bind(GLenum target) const
{
    if(!GPUQuery::isFrameBuffer(m_fbo))
        initialize();

    glViewport(0, 0, m_size.x, m_size.y);

    glBindFramebuffer(target, m_fbo);
    glError();
}

void FrameBufferObject::release() const
{
    if(!GPUQuery::isFrameBuffer(m_fbo))
        return;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glError();
}

void FrameBufferObject::bindTexture2D(
    Program * minpathram
,   const std::string & uniform
,   const glm::uint slot) const
{
    if(!GPUQuery::isTexture(m_texture))
        initialize();

	glActiveTexture(GL_TEXTURE0 + slot);
	glError();

    glBindTexture(GL_TEXTURE_2D, m_texture);
    glError();

    if(minpathram)
        minpathram->setUniform(uniform, slot);  
}

void FrameBufferObject::releaseTexture2D() const
{
    if(!GPUQuery::isTexture(m_texture))
        return;

    glBindTexture(GL_TEXTURE_2D, 0);
    glError();
}

void FrameBufferObject::initialize() const
{
    if(m_depth) // Initialize Render Buffer for Depth
    {
		glGenRenderbuffers(1, &m_render);
		glError();
    }

    glGenTextures(1, &m_texture);
    glError();

    glGenFramebuffers(1, &m_fbo);
    glError();

	resize();

	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	if(m_depth)
	{
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT
			, GL_RENDERBUFFER, m_render);
		glError();
	}

    glFramebufferTexture2D(GL_FRAMEBUFFER, m_attachment
        , GL_TEXTURE_2D, m_texture, 0);
    glError();

    const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    glError();

    if(GL_FRAMEBUFFER_COMPLETE != status)
        std::cerr << "Frame Buffer Object incomplete." << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glError();
}

void FrameBufferObject::resize(
    const unsigned int width
,   const unsigned int height)
{
    m_size = glm::ivec2(width, height);

    resize();
}

void FrameBufferObject::resize() const
{
	if(m_depth && m_render != -1)
	{
        glBindRenderbuffer(GL_RENDERBUFFER, m_render);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_size.x, m_size.y);
        glError();
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glError();
	}

	if(m_texture != -1)
	{
		glBindTexture(GL_TEXTURE_2D, m_texture);

		glTexImage2D(GL_TEXTURE_2D, 0, m_internal, m_size.x, m_size.y, 0, m_format, m_type, 0);
		glError();

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 

		glBindTexture(GL_TEXTURE_2D, 0);
		glError();
	}
}
