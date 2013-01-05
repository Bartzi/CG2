//
// STL
//
#include <cassert>
#include <cstdio>
#include <cmath>
#include <vector>
#include <string>

//
// GL
//
#include "GL/glew.h"
#include "GL/glut.h"

//
// cgmath
//
#include "cgmath/mat44.hpp"
#include "cgmath/mat44_util.hpp"
#include "cgmath/vec3.hpp"
#include "cgmath/vec2.hpp"
#include "utils.hpp"

typedef cgmath::vec3<float> vec3;
typedef cgmath::vec2<float> vec2;
typedef cgmath::vec3<unsigned> uvec3;
typedef cgmath::vec3<unsigned> uvec2;
typedef cgmath::Mat44<float> mat4;

//
// lib3ds
//
#include "lib3ds/lib3ds.h"

Lib3dsFile* modelFile;
const char* modelFileName = "./data/house/house.3ds";
const char* modelDir = "./data/house";

struct face {
    uvec3 v;
    uvec3 n;
};

struct Camera
{
    vec3 position;
    vec3 direction;
    vec3 up;

} g_camera;

///////////////////////////////////////////////////////////////////////////////
//
// Declare graphics state
//

// shaders
GLhandleARB g_renderProgram;
GLhandleARB g_skyboxProgram;

// FBOs
GLuint g_depthFBO;
GLuint g_cubeMapBuffer;

// RTT
GLuint g_depthMap[4];
GLuint g_cubeMapColor;
GLuint g_cubeMapDepth;

// Static Textures
GLuint g_staticCubeMap;

///////////////////////////////////////////////////////////////////////////////
//
// Init state variables
//
double angle = 0.0;
int shadowMode = 1;
int activeLightSource = 0;

int s_width  = 800;
int s_height = 600;

bool s_use_animation = false;
bool s_render_depthMap = false;

mat4 lightProjection_[4];
mat4 lightModelView_[4];
mat4 lightMatrix_[4];

vec3 llf_ = vec3();
vec3 urb_ = vec3();
vec3 lightDir_[4];

const unsigned int SHADOW_MAP_SIZE_X = 4096;
const unsigned int SHADOW_MAP_SIZE_Y = 4096;

// forward declarations
void setCameraTransform(Camera camera);
void renderSkyBox(void);
void renderScene(bool renderFromLight = false);
void renderDepthMap(void);

void
computeBoundingBox(Lib3dsFile* model)
{
    /**************************************************************
    * Aufgabe 1A - Begin
    ***************************************************************/

    // compute AABB
	float min[3];
	float max[3];

	// call lib3ds function (thanks for that)...
	lib3ds_file_bounding_box_of_objects(model, true, false, false, min, max);
	llf_ = vec3(min[0], min[1], min[2]);
	urb_ = vec3(max[0], max[1], max[2]);
	printf("min: %f, %f, %f\n", min[0], min[1], min[2]);
	printf("max: %f, %f, %f\n", max[0], max[1], max[2]);
    
	/**************************************************************
    * Aufgabe 1A - End
    ***************************************************************/
}

/**************************************************************
* Aufgabe 1B
***************************************************************/

void
renderFromLightSource(unsigned int lightID, unsigned int mapID)
{
    vec3 lightFrom, lightTo, lightUp;
    float sceneDiag = 1.0;

    vec3 lightDir = lightDir_[lightID];

    /**************************************************************
    * Aufgabe 1B - Begin
    ***************************************************************/

    lightFrom = lightDir;
	lightTo = vec3(0.0f, 0.0f, 0.0f);

	/*
	 * für den up-Vektor werden als x und z Koordinate die Werte des Vektors zwischen
	 * lightFrom und lighTo genommen... was nun noch fehlt ist ein y-Wert, sodass
	 * das Punktprodukt von lightVec und dem lightUp = 0 ist.
	 */

	vec3 lightVec = lightTo - lightFrom;

	float yLightUp = (-(lightVec.x * lightVec.x) - (lightVec.z * lightVec.z)) / lightVec.y;
	lightUp = vec3(lightVec.x, yLightUp, lightVec.z).normalized();

	// calc sceneDiag

	sceneDiag = sqrt( pow(llf_.x - urb_.x, 2) + pow(llf_.y - urb_.y, 2) + pow(llf_.z - urb_.z, 2) );

    /**************************************************************
    * Aufgabe 1B - End
    ***************************************************************/

    // Adapt projection and orientation transformation
    glViewport(0, 0, (GLsizei)(SHADOW_MAP_SIZE_X), (GLsizei)(SHADOW_MAP_SIZE_Y));
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-0.5*sceneDiag, 0.5*sceneDiag, -0.5*sceneDiag, 0.5*sceneDiag, -0.5*sceneDiag, 2.0*sceneDiag);

    mat4 lightProjection, lightModelView, lightMatrix;

    /**************************************************************
    * Aufgabe 2A (1/3) - Begin Set lightProjection
    ***************************************************************/

	float projection_m[16];
	glGetFloatv(GL_PROJECTION_MATRIX, projection_m);
	GET_GLERROR(0);
	lightProjection = mat4(projection_m, false);
							

    /**************************************************************
    * Aufgabe 2A (1/3) - End Set lightProjection
    ***************************************************************/

    // Adapt modelview
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    gluLookAt(lightFrom[0], lightFrom[1], lightFrom[2], lightTo[0], lightTo[1], lightTo[2], lightUp[0], lightUp[1], lightUp[2]); 
    
    /**************************************************************
    * Aufgabe 3D - Begin
    ***************************************************************/
    
    glRotatef(angle, 0.0f, 0.0f, 1.0f);

    /**************************************************************
    * Aufgabe 3D - End
    ***************************************************************/
   
    /**************************************************************
    * Aufgabe 2A (2/3) - Begin Set lightModelView
    ***************************************************************/

	float modelview_m[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview_m);
	GET_GLERROR(0);
	lightModelView = mat4(modelview_m, false);

    /**************************************************************
    * Aufgabe 2A (1/3) - End Set lightModelView
    ***************************************************************/

    lightProjection_[mapID] = lightProjection;
    lightModelView_[mapID] = lightModelView;

    // transformation matrix
    static mat4 NDCToTexCoords = mat4(0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.5, 0.5, 0.5, 1.0).transpose();

    /**************************************************************
    * Aufgabe 2A (3/3) - Begin Set lightMatrix
    ***************************************************************/
	mat4 intermediate = lightProjection * lightModelView;
	lightMatrix =  NDCToTexCoords * intermediate;

    /**************************************************************
    * Aufgabe 2A (3/3) - End Set lightMatrix
    ***************************************************************/

    lightMatrix_[mapID] = lightMatrix;

    GET_GLERROR(0);

    // push depth buffer and clear
    glBindFramebufferEXT(GL_FRAMEBUFFER, g_depthFBO);
    GET_GLERROR(0);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, g_depthMap[mapID], 0);
    GET_GLERROR(0);
    glClear(GL_DEPTH_BUFFER_BIT);
    GET_GLERROR(0);

    // render scene
    renderScene(true);
    GET_GLERROR(0);

    // pop depth buffer
    glBindFramebufferEXT(GL_FRAMEBUFFER, 0);

    // revert view to camera
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    GET_GLERROR(0);
}

void
on_display(void)
{
    // clear main framebuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //
    // 0. Render Skybox
    //
    setCameraTransform(g_camera);
    renderSkyBox();

    //
    // 1. Set Light Source Positions
    //
    lightDir_[0] = vec3(1.0, 3.0, 6.0);
    lightDir_[1] = vec3(-1.0, 2.0, 7.0);
    lightDir_[2] = vec3(0.0, 3.0, 3.0);
    lightDir_[3] = vec3(2.0, 3.0, 4.0);

    //
    // 2. Render depth map from light source
    //
    if(shadowMode == 3) {
        renderFromLightSource(0, 0);
        renderFromLightSource(1, 1);
        renderFromLightSource(2, 2);
        renderFromLightSource(3, 3);
    }
    else {
        renderFromLightSource(activeLightSource, 0);
    }

    //
    // 3. Apply depth map and render scene
    //
    glViewport(0, 0, (GLsizei)s_width, (GLsizei)s_height);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    GET_GLERROR(0);

    glUseProgramObjectARB(g_renderProgram);
    GET_GLERROR(0);

    // Transfer Textures
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, g_depthMap[0]);
    GET_GLERROR(0);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, g_depthMap[1]);
    GET_GLERROR(0);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, g_depthMap[2]);
    GET_GLERROR(0);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, g_depthMap[3]);
    GET_GLERROR(0);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, g_cubeMapColor);
    GET_GLERROR(0);
    glActiveTexture(GL_TEXTURE0);
    GET_GLERROR(0);

    //
    // Transfer Uniforms
    //
    // window dimension
    int windowCoordsLocation = glGetUniformLocationARB(g_renderProgram, "windowDimension");
    GET_GLERROR(0);
    glUniform2fARB(
        windowCoordsLocation,
        s_width,
        s_height
        );
    GET_GLERROR(0);

    // modelview-projection matrix for light views
    int lightMatrixLocation0 = glGetUniformLocationARB(g_renderProgram, "lightMatrix0");
    GET_GLERROR(0);
    glUniformMatrix4fvARB(
        lightMatrixLocation0,
        1,
        GL_TRUE,
        (const GLfloat*)lightMatrix_[0].m
        );
    GET_GLERROR(0);

    int lightMatrixLocation1 = glGetUniformLocationARB(g_renderProgram, "lightMatrix1");
    GET_GLERROR(0);
    glUniformMatrix4fvARB(
        lightMatrixLocation1,
        1,
        GL_TRUE,
        (const GLfloat*)lightMatrix_[1].m
        );
    GET_GLERROR(0);

    int lightMatrixLocation2 = glGetUniformLocationARB(g_renderProgram, "lightMatrix2");
    GET_GLERROR(0);
    glUniformMatrix4fvARB(
        lightMatrixLocation2,
        1,
        GL_TRUE,
        (const GLfloat*)lightMatrix_[2].m
        );
    GET_GLERROR(0);

    int lightMatrixLocation3 = glGetUniformLocationARB(g_renderProgram, "lightMatrix3");
    GET_GLERROR(0);
    glUniformMatrix4fvARB(
        lightMatrixLocation3,
        1,
        GL_TRUE,
        (const GLfloat*)lightMatrix_[3].m
        );
    GET_GLERROR(0);

    // bbox
    int frustumXZLocation = glGetUniformLocationARB(g_renderProgram, "frustumXZ");
    GET_GLERROR(0);
    glUniform2fARB(
        frustumXZLocation,
        std::abs(llf_[0] - urb_[0]),
        std::abs(llf_[1] - urb_[1])
        );
    GET_GLERROR(0);

    // texture mode
    int shadowModeLocation = glGetUniformLocationARB(g_renderProgram, "shadowMode");
    GET_GLERROR(0);
    glUniform1iARB(
        shadowModeLocation,
        shadowMode
        );
    GET_GLERROR(0);

    // light source
    int activeLightSourceLocation = glGetUniformLocationARB(g_renderProgram, "activeLightSource");
    GET_GLERROR(0);
    glUniform1iARB(
        activeLightSourceLocation,
        activeLightSource
        );
    GET_GLERROR(0);

    // render scene
    renderScene();
    GET_GLERROR(0);

    if(s_render_depthMap) {
        renderDepthMap();
        GET_GLERROR(0);
    }

    glUseProgramObjectARB(0);

    // pop
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, 0);
    GET_GLERROR(0);

    glutSwapBuffers();

    return;
}

void
initShader(void)
{
    ///////////////////////////////////////////////////////////////////////////
    //
    // Create shader
    //
    GLhandleARB render_vertex_shader;
    GLhandleARB render_fragment_shader;

    GLhandleARB skybox_vertex_shader;
    GLhandleARB skybox_fragment_shader;

    //
    // Create and load resources
    //
    // pcss shader
    g_renderProgram          = glCreateProgramObjectARB();
    render_vertex_shader     = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    render_fragment_shader   = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

    loadShaderFromFile(render_vertex_shader, "shader/applyDepthMap.vert");
    loadShaderFromFile(render_fragment_shader, "shader/applyDepthMap.frag");

    // skybox shader
    g_skyboxProgram          = glCreateProgramObjectARB();
    skybox_vertex_shader     = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    skybox_fragment_shader   = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

    loadShaderFromFile(skybox_vertex_shader, "shader/skybox.vert");
    loadShaderFromFile(skybox_fragment_shader, "shader/skybox.frag");

    //
    // Compile
    //
    // pcss shader
    glCompileShaderARB(render_vertex_shader);
    GET_GLERROR(0);
    printInfoLog(render_vertex_shader);

    glCompileShaderARB(render_fragment_shader);
    GET_GLERROR(0);
    printInfoLog(render_fragment_shader);

    // skybox shader
    glCompileShaderARB(skybox_vertex_shader);
    GET_GLERROR(0);
    printInfoLog(skybox_vertex_shader);

    glCompileShaderARB(skybox_fragment_shader);
    GET_GLERROR(0);
    printInfoLog(skybox_fragment_shader);

    //
    // Link
    //
    // pcss shader
    glAttachObjectARB(g_renderProgram, render_vertex_shader);
    glAttachObjectARB(g_renderProgram, render_fragment_shader);
    glLinkProgramARB(g_renderProgram);
    GET_GLERROR(0);
    printInfoLog(g_renderProgram);

    // skybox shader
    glAttachObjectARB(g_skyboxProgram, skybox_vertex_shader);
    glAttachObjectARB(g_skyboxProgram, skybox_fragment_shader);
    glLinkProgramARB(g_skyboxProgram);
    GET_GLERROR(0);
    printInfoLog(g_skyboxProgram);

    ///////////////////////////////////////////////////////////////////////////
    //
    // Setup shaders
    //

    // Sampler - Depth Map Rendering
    glUseProgramObjectARB(g_renderProgram);
	GET_GLERROR(0);

    int textureMapLocation = glGetUniformLocationARB(g_renderProgram, "textureMap");
    GET_GLERROR(0);
    glUniform1iARB(textureMapLocation, 0);
    GET_GLERROR(0);

    int shadowMapLocation0 = glGetUniformLocationARB(g_renderProgram, "shadowMap0");
    GET_GLERROR(0);
    glUniform1iARB(shadowMapLocation0, 1);
    GET_GLERROR(0);

    int shadowMapLocation1 = glGetUniformLocationARB(g_renderProgram, "shadowMap1");
    GET_GLERROR(0);
    glUniform1iARB(shadowMapLocation1, 2);
    GET_GLERROR(0);

    int shadowMapLocation2 = glGetUniformLocationARB(g_renderProgram, "shadowMap2");
    GET_GLERROR(0);
    glUniform1iARB(shadowMapLocation2, 3);
    GET_GLERROR(0);

    int shadowMapLocation3 = glGetUniformLocationARB(g_renderProgram, "shadowMap3");
    GET_GLERROR(0);
    glUniform1iARB(shadowMapLocation3, 4);
    GET_GLERROR(0);

    int skyBoxColorBufferLocation = glGetUniformLocationARB(g_renderProgram, "skyBoxColor");
	GET_GLERROR(0);
    glUniform1iARB(skyBoxColorBufferLocation, 5);
	GET_GLERROR(0);

    // Sampler - Skybox Rendering
    glUseProgramObjectARB(g_skyboxProgram);
	GET_GLERROR(0);

    int staticCubeMapLocation = glGetUniformLocationARB(g_renderProgram, "staticCubeMap");
    GET_GLERROR(0);
    glUniform1iARB(staticCubeMapLocation, 0);
    GET_GLERROR(0);

    return;
}


void
resetShader(void)
{
    glDeleteObjectARB(g_renderProgram);
    GET_GLERROR(0);
    glDeleteObjectARB(g_skyboxProgram);  
    GET_GLERROR(0);

    return;
}


void 
initTextures(void)
{
    // create RTT - depth maps
    for(int i=0; i < 4; i++) {
        glGenTextures(1, &g_depthMap[i]);
	    GET_GLERROR(0);
        glBindTexture(GL_TEXTURE_2D, g_depthMap[i]);
	    GET_GLERROR(0);

        // initialize texture with default data
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, SHADOW_MAP_SIZE_X, SHADOW_MAP_SIZE_Y, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
        GET_GLERROR(0);

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    ///// Cube Map Color
    glGenTextures( 1, &g_cubeMapColor );
	GET_GLERROR(0);
    glBindTexture( GL_TEXTURE_2D, g_cubeMapColor );
	GET_GLERROR(0);

    // initialize texture with default data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s_width, s_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    GET_GLERROR(0);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);

    ///// Cube Map Depth
    glGenTextures( 1, &g_cubeMapDepth );
	GET_GLERROR(0);
    glBindTexture( GL_TEXTURE_2D, g_cubeMapDepth );
	GET_GLERROR(0);

    // initialize texture with default data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, s_width, s_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
    GET_GLERROR(0);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);

    ///////////////////////////////////////////////////////////////////////////
    //
    // Init static cube map texture
    //
    glGenTextures( 1, &g_staticCubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, g_staticCubeMap);

    unsigned int size = 512;
    GLint        internalFormat = GL_RGB;
    GLenum       format = GL_RGB;
	GLenum  	 type = GL_UNSIGNED_BYTE;

    gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_POSITIVE_X, internalFormat, size, size, format, type, 
        loadTextureRAW("data/skybox/skybox_left.raw", size, size, 3)
        );
    GET_GLERROR(0);
    gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, internalFormat, size, size, format, type,
        loadTextureRAW("data/skybox/skybox_right.raw", size, size, 3)
        );
    GET_GLERROR(0);
    gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, internalFormat, size, size, format, type,
        loadTextureRAW("data/skybox/skybox_back.raw", size, size, 3)
        );
    GET_GLERROR(0);
    gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, internalFormat, size, size, format, type,
        loadTextureRAW("data/skybox/skybox_front.raw", size, size, 3)
        );
    GET_GLERROR(0);
    gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, internalFormat, size, size, format, type,
        loadTextureRAW("data/skybox/skybox_top.raw", size, size, 3)
        );
    GET_GLERROR(0);
    gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, internalFormat, size, size, format, type,
        loadTextureRAW("data/skybox/skybox_bottom.raw", size, size, 3)
        );
    GET_GLERROR(0);

    //
    // Set cube map configuration
    //
    glTexParameterf(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, 0);
    GET_GLERROR(0);

    return;
}


void 
initFrameBufferObjects(void)
{
    // create a buffer ID
    glGenFramebuffersEXT(1, &g_depthFBO);
    GET_GLERROR(0);
    glBindFramebufferEXT(GL_FRAMEBUFFER, g_depthFBO);
    GET_GLERROR(0);

    // bind depth buffers to FBO
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, g_depthMap[0], 0);
    GET_GLERROR(0);

    // there is no draw and read buffer
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // check status
    CheckFramebufferStatus();
    glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
    GET_GLERROR(0);

    // cube map FBO
    glGenFramebuffersEXT(1, &g_cubeMapBuffer);
    GET_GLERROR(0);
    glBindFramebufferEXT(GL_FRAMEBUFFER, g_cubeMapBuffer);
    GET_GLERROR(0);

    // bind color buffer to FBO
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_cubeMapColor, 0);
    GET_GLERROR(0);

    // bind depth buffer to FBO
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, g_cubeMapDepth, 0);
    GET_GLERROR(0);

    // check status
    CheckFramebufferStatus();
    glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
    GET_GLERROR(0);

    return;
}


void
resetFrameBufferObjects(void)
{
    glDeleteFramebuffersEXT(1, &g_depthFBO);
    GET_GLERROR(0);
    glDeleteFramebuffersEXT(1, &g_cubeMapBuffer);
    GET_GLERROR(0);

    for(int i=0; i < 4; i++) {
        glDeleteTextures(1, &g_depthMap[i]);
    }
    glDeleteTextures(1, &g_cubeMapColor);
    glDeleteTextures(1, &g_cubeMapDepth);
    glDeleteTextures(1, &g_staticCubeMap);
    GET_GLERROR(0);

    return;
}


void
on_reshape(int w, int h)
{
    s_width = w;
    s_height = h;

    // Resize textures and buffers
    for(int i=0; i < 4; i++) {
        glBindTexture(GL_TEXTURE_2D, g_depthMap[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, SHADOW_MAP_SIZE_X, SHADOW_MAP_SIZE_Y, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
        GET_GLERROR(0);
    }

    glBindTexture(GL_TEXTURE_2D, g_cubeMapColor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s_width, s_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    GET_GLERROR(0);

    glBindTexture(GL_TEXTURE_2D, g_cubeMapDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, s_width, s_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
    GET_GLERROR(0);

    glBindTexture(GL_TEXTURE_2D, 0);

    //
    // Adapt projection and orientation transformation
    //
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 1.0*w/h, 1, 1000);
    glMatrixMode(GL_MODELVIEW);

    return;
}


void
renderSkyBox(void)
{
    //
    // Bind Textures
    //
    glEnable(GL_TEXTURE_CUBE_MAP_ARB);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, g_staticCubeMap);
    GET_GLERROR(0);

    // Activate Shader program
    //
    glUseProgramObjectARB(g_skyboxProgram);
    GET_GLERROR(0);    

    float size = 10.0f;

    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glRotatef(90, 1.0f, 0.0f, 0.0f);

    // Render skybox geometry using intermediate mode
    glBegin(GL_QUADS);
        // Render the front quad
        glVertex3f(  size, -size, -size );
        glVertex3f( -size, -size, -size );
        glVertex3f( -size,  size, -size );
        glVertex3f(  size,  size, -size );
        // Render the left quad
        glVertex3f(  size, -size,  size );
        glVertex3f(  size, -size, -size );
        glVertex3f(  size,  size, -size );
        glVertex3f(  size,  size,  size );
        // Render the back quad
        glVertex3f( -size, -size,  size );
        glVertex3f(  size, -size,  size );
        glVertex3f(  size,  size,  size );
        glVertex3f( -size,  size,  size );
        // Render the right quad
        glVertex3f( -size, -size, -size );
        glVertex3f( -size, -size,  size );
        glVertex3f( -size,  size,  size );
        glVertex3f( -size,  size, -size );
        // Render the top quad
        glVertex3f( -size,  size, -size );
        glVertex3f( -size,  size,  size );
        glVertex3f(  size,  size,  size );
        glVertex3f(  size,  size, -size );
        // Render the bottom quad
        glVertex3f( -size, -size, -size );
        glVertex3f( -size, -size,  size );
        glVertex3f(  size, -size,  size );
        glVertex3f(  size, -size, -size );

    glEnd();

    glPopMatrix();

    // De-activate Shader program and reset matrix and attribute state
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glUseProgramObjectARB(0);

    return;
}

void
renderDepthMap(void) 
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, 0.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_depthMap[0]); // TODO

    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex2f(0.0, 0.0);
        glTexCoord2f(1.0, 0.0); glVertex2f(1.0, 0.0);
        glTexCoord2f(1.0, 1.0); glVertex2f(1.0, 1.0);
        glTexCoord2f(0.0, 1.0); glVertex2f(0.0, 1.0);
    glEnd();

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
}


void
renderScene(bool renderFromLight)
{
    //
    // Apply model transformation
    //
    if(!renderFromLight) {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
    }

    render3DS(modelFile);

    if(!renderFromLight) {
        glPopMatrix();
    }

    //glFlush();
}

void
setCameraTransform(Camera camera)
{
    //
    // Set view transformation
    //
    vec3 p = camera.position;
    vec3 t = camera.direction;
    vec3 u = camera.up;

    glViewport(0, 0, (GLsizei)s_width, (GLsizei)s_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 1.0*s_width/s_height, 1, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(p[0], p[1], p[2], t[0], t[1], t[2], u[0], u[1], u[2]);

    return;
}

void 
on_keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27: // ESC key
        {
            exit(0);
            break;
        }
    case 'f':
        {
            printf("RE-INITIALIZED FRAMEBUFFER OBJECTS\n");
            resetFrameBufferObjects();
            initFrameBufferObjects();
            break;
        }
    case 'r':
        {
            printf("RELOAD SHADER\n");
            resetShader();
            initShader();

            break;
        }
    case ' ':
        {
            s_use_animation = !s_use_animation;
            printf("SWITCH ANIMATION\n");

            break;
        }
    case 's':
        {
            glBindTexture(GL_TEXTURE_2D, g_depthMap[0]); // TODO
            unsigned char* depthData = (unsigned char*) malloc(sizeof(unsigned char) * SHADOW_MAP_SIZE_X * SHADOW_MAP_SIZE_Y);
            glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, depthData);
            tga_save("out/depthImage.tga", depthData, (unsigned char*) "ALPHA", SHADOW_MAP_SIZE_X, SHADOW_MAP_SIZE_Y, 8);
            glBindTexture(GL_TEXTURE_2D, 0);

            break;
        }
    case '1':
        {
            shadowMode = 0;
            s_render_depthMap = true;
            break;
        }
    case '2':
        {
            shadowMode = 1;
            s_render_depthMap = false;
            break;
        }
    case '3':
        {
            shadowMode = 2;
            s_render_depthMap = false;
            break;
        }
    case '4':
        {
            shadowMode = 3;
            s_render_depthMap = false;
            break;
        }
    case '+':
        {
            activeLightSource = std::min(3, activeLightSource + 1);
            std::cout << "Active Light Source: " << activeLightSource << std::endl;
            break;
        }
    case '-':
        {
            activeLightSource = std::max(0, activeLightSource - 1);
            std::cout << "Active Light Source: " << activeLightSource << std::endl;
            break;
        }

    }//endswitch

    glutPostRedisplay();

    return;
}


void
on_timer(int value)
{
    //
    // Just add up & use simple sin function to normalize between [-1,1]
    //
    if(s_use_animation)
    {
        angle += value;

    }//endif

    //
    // Post for redisplay and trigger callback again
    //
    glutPostRedisplay();
    glutTimerFunc(1, on_timer, 1);

    return;
}

void loadModel(void)
{
    ///////////////////////////////////////////////////////////////////////////
    //
    // Load 3DS Files
    //
    std::cout << "Loading Model..." << std::endl;
    modelFile = lib3ds_file_open(modelFileName);
    if (!modelFile)
    {
        fprintf(stderr, "***ERROR***\nLoading file failed: %s\n", modelFileName);
        exit(1);
    }

    if (!modelFile->nodes)
        lib3ds_file_create_nodes_for_meshes(modelFile);

    computeBoundingBox(modelFile);

    //
    // Init textures
    //
    static char datapath[256];
    strcat(datapath, modelDir);

    for (int i = 0; i < modelFile->nmaterials; ++i) {
        Lib3dsMaterial *mat = modelFile->materials[i];
        if (mat->texture1_map.name[0]) {  /* texture map? */
            Lib3dsTextureMap *tex = &mat->texture1_map;

            char texname[1024];
            PlayerTexture *pt = (PlayerTexture*)calloc(sizeof(*pt),1);
            tex->user_ptr = pt;
            strcpy(texname, datapath);
            strcat(texname, "/");
            strcat(texname, tex->name);

            printf("Loading %s\n", texname);
            if (tga_load(texname, &pt->pixels, &pt->w, &pt->h)) {
                glGenTextures(1, &pt->tex_id);

                glBindTexture(GL_TEXTURE_2D, pt->tex_id);
                gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGBA, pt->w, pt->h, GL_BGRA, GL_UNSIGNED_BYTE, pt->pixels );
                GET_GLERROR(0);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
            } else {
                fprintf(stderr, "Loading '%s' failed!\n", texname);
            }
        }
    }

    lib3ds_file_eval(modelFile, 0);

    return;
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE | GLUT_MULTISAMPLE);
    glutInitWindowSize(s_width, s_height);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Shadow Mapping");

    glewInit();

    // check if gl extensions are available
    bool gl2 = GLEW_ARB_shader_objects
            && GLEW_ARB_vertex_shader
            && GLEW_ARB_fragment_shader
            && GL_EXT_framebuffer_object;

    if(!gl2) {
        std::cout << "Your graphics card or driver is not compatible with OpenGL2. Exiting.";
        return 1;
    }

    //
    // Scene Camera
    //
    g_camera.position  = vec3(-1.4f, -1.7f, 1.6f);
    g_camera.direction = vec3(1.05f, 0.45f, -0.53f);
    g_camera.up = vec3(0.0f, 0.0f, 1.0f);

    //
    // Init Resources
    //
    loadModel();
    initTextures();
    initFrameBufferObjects();
    initShader();
    
	glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    glutDisplayFunc(on_display);
    glutReshapeFunc(on_reshape);
    glutKeyboardFunc(on_keyboard);
    glutTimerFunc(1, on_timer, 1);
    
    glutMainLoop();

    //
    // Free Resources
    //
    lib3ds_file_free(modelFile);    

    return 0;
}
