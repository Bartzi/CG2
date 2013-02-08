
#include <GL/glew.h>

#include "exercise6.h"

#include "camera.h"
#include "fileassociatedshader.h"
#include "framebufferobject.h"
#include "gpuquery.h"
#include "group.h"
#include "objio.h"
#include "program.h"
#include "rect.h"
#include "node.h"

#include "NodeIDHandler.h"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>


using namespace glm;


Exercise6::Exercise6()
:   m_camera(nullptr)
,   m_group(nullptr)

,	m_normalDepthProgram(nullptr)
,	m_blurhProgram(nullptr)

,	m_fboNormalDepth(nullptr)
,	m_fboTemp(nullptr)
,	m_fboTemp2(nullptr)
,	m_fbo_ssao(nullptr)
,	m_pixelData(nullptr)
{
}

Exercise6::~Exercise6()
{
    delete m_group;

    delete m_normalDepthProgram;
    delete m_blurhProgram;
	delete m_blurvProgram;
	delete m_ssaoProgram;
	delete m_dofProgram;

    delete m_fboNormalDepth;
    delete m_fboTemp; 
	delete m_fboTemp2; 
	delete m_fbo_ssao;

	delete m_pixelData;
}

void Exercise6::initialize()
{
    m_group = ObjIO::groupFromObjFile("data/sibenik.obj");
    if(!m_group)
    {
        std::cerr << "Have you set the Working Directory? (MSVC: $(TargetDir))" << std::endl;
        exit(0);
    }

    // Task1a: Scale m_group to 10x its size:

	glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(10.0));
	m_group->setTransform(scale);

    // Camera Setup

    m_camera = new Camera();

    m_camera->setFovy (45.0f);
    m_camera->setZNear( 1.0f);
    m_camera->setZFar (10.0f);

    m_camera->append(m_group);

    // Task 1a: Adjust Camera LookAt and Field of View Y as suggested.
    
    m_camera->setView(glm::lookAt(
        vec3( -4.5f, -2.f, 0.f), vec3( 2.f, -2.5f, 0.f), vec3( 0.f, 1.f, 0.f)));
		//vec3( 2.f, -2.5f, 0.f), vec3( -4.5f, -2.f, 0.f), vec3( 0.f, 1.f, 0.f)));

  
    // G-Buffer Shader

	m_normalDepthProgram = new Program();
	m_normalDepthProgram->attach(
		new FileAssociatedShader(GL_FRAGMENT_SHADER, "shader/depth.frag"));
	m_normalDepthProgram->attach(
		new FileAssociatedShader(GL_VERTEX_SHADER, "shader/depth.vert"));

    // Task 2: Screen Aligned Quad and Blurring

    m_rect = new Rect();

    // Setup FBOs for NormalDepth Buffer

    // Task 2a: Setup FrameBuffer with floating values.

    // The last parameter, depth, needs to be true for G-Buffer rendering based on
    // Geometry rendering (i.e., scene graph traversal). If false, no depth test is
    // available. That, however, is OK for pure post-processing (i.e., rect rendering).

    m_fboNormalDepth = new FrameBufferObject(
        GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT0, true);
	m_fbo_ssao = new FrameBufferObject(
		GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT0, false);
	m_fboTemp = new FrameBufferObject(
		GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT0, false);
	m_fboTemp2 = new FrameBufferObject(
		GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT0, false);
		//GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT0, false);


    // Task 2c: extend by vertical blur objects...

    Shader * rectVertShader  = new FileAssociatedShader(GL_VERTEX_SHADER,   "shader/rect.vert");
    Shader * blurhFragShader = new FileAssociatedShader(GL_FRAGMENT_SHADER, "shader/blurh.frag");
	Shader * blurvFragShader = new FileAssociatedShader(GL_FRAGMENT_SHADER, "shader/blurv.frag");
	Shader * ssaoFragShader = new FileAssociatedShader(GL_FRAGMENT_SHADER, "shader/ssao.frag");
	Shader * dofFragShader = new FileAssociatedShader(GL_FRAGMENT_SHADER, "shader/dof.frag");

    // Horizontal Blur Program

    m_blurhProgram = new Program();
    m_blurhProgram->attach(blurhFragShader);
    m_blurhProgram->attach(rectVertShader);

	// Vertical Blur Program
	m_blurvProgram = new Program();
	m_blurvProgram->attach(blurvFragShader);
	m_blurvProgram->attach(rectVertShader);

	// ssao program
	m_ssaoProgram = new Program();
	m_ssaoProgram->attach(ssaoFragShader);
	m_ssaoProgram->attach(rectVertShader);

	// dof program
	m_dofProgram = new Program();
	m_dofProgram->attach(dofFragShader);
	m_dofProgram->attach(rectVertShader);		


    // Task 3... probably more initialization ;)

}

void Exercise6::bindSampler(
	const t_samplerByName & sampler
,	Program * program)
{
	t_samplerByName::const_iterator i(sampler.begin());
	const t_samplerByName::const_iterator iEnd(sampler.end());
	
	for(glm::uint slot(0); i != iEnd; ++i, ++slot)
		i->second->bindTexture2D(program, i->first, slot);
}

void Exercise6::releaseSampler(
	const t_samplerByName & sampler)
{
	t_samplerByName::const_iterator i(sampler.begin());
	const t_samplerByName::const_iterator iEnd(sampler.end());

	for(; i != iEnd; ++i)
		i->second->releaseTexture2D();
}

void Exercise6::draw()
{
    // "viewport" is available as vec2
    // "view", "projection", and "transform" are available as mat4 uniforms
	// "znear" and "zfar" are available as floats

    // Helper vars, that simplify render pass configuration.
    // Hint: make target nullptr: then return after draw call -> shows result on screen!
    // Hint: use setUniform of program instances, to setup uniforms -> if a specific type is missing
    // add it or just use ->program() for the GL handle and user plain gl

	t_samplerByName sampler;

	FrameBufferObject * target(nullptr);
	Program * program;


	// Normals and Depth to RGBA

    // Task 2a: outcomment this code

	program = m_normalDepthProgram;
	target  = m_fboNormalDepth; 
	
	m_camera->draw(program, target);

    if(!target) return;
	
    // SSAO

	sampler.clear();
	sampler["source"] = m_fboNormalDepth;

	program = m_ssaoProgram;
	target  = m_fbo_ssao; 

	bindSampler(sampler, program);
	m_rect->draw(program, target);
	releaseSampler(sampler);

    if(!target) return;


	// Horizontal Blur

    // Task 2b: template for single post-processing step - use this for all the other ones, 
    // just reinit sampler, target, and program before calling draw...
	sampler.clear();
	sampler["source"] = m_fbo_ssao;
	
	program = m_blurhProgram;
	target  = m_fboTemp;

	bindSampler(sampler, program);
	
    m_rect->draw(program, target);
    releaseSampler(sampler);

    if(!target) return;


	// Vertical Blur

    // Task 2c: setup vertical blur pass and resources...

	sampler.clear();
	sampler["source"] = m_fboTemp;

	program = m_blurvProgram;
	target = m_fboTemp2;
	//target = nullptr;

	bindSampler(sampler, program);
    m_rect->draw(program, target);
    releaseSampler(sampler);

    if(!target) return;


	// Task 4a: Depth of Field

	sampler.clear();
	sampler["source"] = m_fboTemp2;
	
	program = m_blurhProgram;
	target  = m_fboTemp;

	bindSampler(sampler, program);
	
    m_rect->draw(program, target);
    releaseSampler(sampler);

    if(!target) return;


	sampler.clear();
	sampler["ssao"] = m_fboTemp2;
	sampler["blur"] = m_fboTemp;
	sampler["normalAndDepth"] = m_fboNormalDepth;

	program = m_dofProgram;
	target  = nullptr;

	bindSampler(sampler, program);
    m_rect->draw(program, target);
    releaseSampler(sampler);

    if(!target) return;


    // Task 5a: Picking - remember setting enabling depth flag for rendering G-Buffers (m_camera->draw)...

}

void Exercise6::resize(
    const int width
,   const int height)
{
    m_camera->setViewport(width, height);

    m_fboNormalDepth->resize(width, height);
	m_fboTemp->resize(width, height);
	m_fboTemp2->resize(width, height);
	m_fbo_ssao->resize(width, height);

    // TODO

	postShaderRelinked();
}

void Exercise6::postShaderRelinked()
{
    const float blurIntensity(2.f);

	m_blurhProgram->setUniform("invViewportWidth"
		, blurIntensity / static_cast<float>(m_camera->viewport().x));
	m_blurvProgram->setUniform("invViewportHeight"
		, blurIntensity / static_cast<float>(m_camera->viewport().y));
	m_ssaoProgram->setUniform("invViewportSize"
		,  vec2(1 / static_cast<float>(m_camera->viewport().x), 1 / static_cast<float>(m_camera->viewport().y)));
	m_dofProgram->setUniform("invViewportHeight"
		, blurIntensity / static_cast<float>(m_camera->viewport().y));

    // TODO Task 2 and probably Task 4
}

void Exercise6::mouseMove(const int x, const int y)
{
    // For texture access, use the following offset:
    // x: -1, y: -7 for accurate pixel accessing.

	if(m_fboNormalDepth < 0)
		return;

	if(!m_pixelData)
		m_pixelData = new float[4];
	float realY = m_camera->viewport().y - y;
	m_fboNormalDepth->bind(GL_READ_FRAMEBUFFER);
	glReadPixels(x, realY, 1, 1, GL_RGBA, GL_FLOAT, m_pixelData);
	printf("%d, %d, %f, %f, %f, %f\n", x, realY, m_pixelData[0], m_pixelData[1], m_pixelData[2], m_pixelData[3]);
	printf("viewport: %d, %d\n", m_camera->viewport().x, m_camera->viewport().y);
	m_dofProgram->setUniform("focusDepth", m_pixelData[3]);



    // Additional Taks 4/5: Read Depth Value under mouse for DOF Focus and Picking 

}