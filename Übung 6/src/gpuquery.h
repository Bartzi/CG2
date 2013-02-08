
#pragma once

#include <GL/freeglut.h>

#include <string>


class GPUQuery
{
public:
    static const GLint queryi(const GLenum penum);
    static const std::string querys(const GLenum penum);

    static const std::string glew(const GLenum penum);
    static const std::string glewError(const GLenum penum);

    static const std::string vendor();
    static const std::string renderer();
    static const std::string glewVersion();

    // checks for an opengl error and prints to console if one occurred
    static const bool error(
        const char * file
    ,   const int line);

	static const bool isShader(const GLuint shader);
	static const bool isProgram(const GLuint program);
	static const bool isBuffer(const GLuint buffer);
    static const bool isFrameBuffer(const GLuint framebuffer);
    static const bool isTexture(const GLuint texture);
    static const bool isRenderBuffer(const GLuint renderbuffer);

    // Extension Info
    
    static const bool extensionSupported(const char * extension);

    // Memory Info (return -1 if query is not possible or failed)

    static const GLint totalMemory();
    static const GLint dedicatedMemory();
    static const GLint availableMemory();
    static const GLint evictionCount();
    static const GLint evictedMemory();

protected:
    static const GLint memoryInfo(const GLenum penum);
};

#define glError() GPUQuery::error(__FILE__, __LINE__)
