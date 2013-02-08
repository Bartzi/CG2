
#include <GL/glew.h>

#include "rect.h"

#include "gpuquery.h"
#include "program.h"
#include "framebufferobject.h"


Rect::Rect()
:   m_buffer(-1)
{
}    

Rect::~Rect()
{
    if(m_buffer != -1)
        glDeleteBuffers(1, &m_buffer);
}

void Rect::initialize() const
{
    // By default, counterclockwise polygons are taken to be front-facing.
    // http://www.opengl.org/sdk/docs/man/xhtml/glFrontFace.xml

    const GLfloat vertices[] =
    {
        +1.f, -1.f
    ,   +1.f, +1.f
    ,   -1.f, -1.f
    ,   -1.f, +1.f
    };

    glGenBuffers(1, &m_buffer);
    glError();

    glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
    glError();

    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    glError();
}

void Rect::draw(
    Program * program
,   FrameBufferObject * target) const
{
    if(!program)
        return;

    if(!GPUQuery::isBuffer(m_buffer))
        initialize();

    const int a_vertex = program->attributeLocation("a_vertex");

    if(target)
        target->bind();

    glClear(GL_COLOR_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
    glError();

    glVertexAttribPointerARB(a_vertex, 2, GL_FLOAT, 0, 0, 0);
    glError();

    glEnableVertexAttribArrayARB(a_vertex);
    glError();

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glError();

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

    if(target)
        target->release();
}