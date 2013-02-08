
#pragma once

#include <map>
#include <string>
#include <stdlib.h>

class Camera;
class Group;
class Rect;

class Program;
class FrameBufferObject;

class Exercise6
{
public:
    Exercise6();
    virtual ~Exercise6();

	void draw();

    void initialize();
    void resize(
        const int width
    ,   const int height);

	void mouseMove(const int x, const int y);
	void postShaderRelinked();

protected:
	typedef std::map<std::string, FrameBufferObject *> t_samplerByName;

	static void bindSampler(
		const t_samplerByName & sampler
	,	Program * program);

	static void releaseSampler(
		const t_samplerByName & sampler);

protected:
    Camera * m_camera;
    Group  * m_group;
    Rect   * m_rect;

	Program * m_normalDepthProgram;
	
	// Post Processing
    Program * m_blurhProgram;
	Program * m_blurvProgram;
	Program * m_ssaoProgram;
	Program * m_dofProgram;

    FrameBufferObject * m_fboNormalDepth;
	FrameBufferObject * m_fboTemp;
	FrameBufferObject * m_fbo_ssao;
	FrameBufferObject * m_fboTemp2;

	// pixel reading
	float * m_pixelData;

};