#pragma once

#include <GL/freeglut.h>


class Program;
class FrameBufferObject;

class Rect
{
public:
    Rect();
    virtual ~Rect();

    void draw(
        Program * program
    ,   FrameBufferObject * target = nullptr) const;

protected:
    void initialize() const;

protected:    
    mutable GLuint m_buffer;
};