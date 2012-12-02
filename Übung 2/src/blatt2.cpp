#include <cassert>
#include <cstdio>
#include <cmath>
#include <vector>
#include <hash_map>
#include "GL/glew.h"
#include "GL/glut.h"
#include "cgmath/vec3.hpp"
#include "cgmath/vec2.hpp"
#include "utils.hpp"

#include "lib3ds/lib3ds.h"

//using namespace cgmath;
typedef cgmath::vec3<float> vec3;

///////////////////////////////////////////////////////////////////////////////
//
// Declare graphics state
//
Lib3dsFile* city3DSFile;
Lib3dsFile* reflective3DSFile;

const char* cityFileName = "./data/megacity/city.3ds";
const char* reflectiveFileName  = "./data/sphere.3ds";
//const char* reflectiveFileName  = "./data/reflective/bunny.3ds";
//const char* reflectiveFileName  = "./data/reflective/cube.3ds";

//
// Cube Map Textures
//
GLuint g_staticCubeMap;
GLuint g_dynamicCubeMapColor;
GLuint g_renderBufferDepth; 
GLuint g_facesFBO[6];

//
// Shader Programs
//
GLhandleARB g_skyboxProgram;
GLhandleARB g_texturingProgram;
GLhandleARB g_environmentProgram;
GLhandleARB g_mprtcProgram;

//
// Camera stuff
//
struct Camera
{
    vec3 position;
    vec3 direction;
    vec3 up;

} g_camera;

Camera g_fboCameras[6];
Camera g_reflectiveCamera;

enum CubeMapFaces
{
    LEFT   = 0,
    RIGHT  = 1,
    TOP    = 2,
    BOTTOM = 3,
    FRONT  = 4,
    BACK   = 5
};

///////////////////////////////////////////////////////////////////////////////
//
// Init state variables
//
float        s_angle = 1.0f;
unsigned int s_width  = 800;
unsigned int s_height = 600;
unsigned int s_fboSize = 512;
bool         s_use_animation = false;

void
initShader(void)
{
    ///////////////////////////////////////////////////////////////////////////
    //
    // Create skybox shader
    //
    GLhandleARB skybox_vertex_shader;
    GLhandleARB skybox_fragment_shader;

    //
    // Create and load resources
    //
    g_skyboxProgram          = glCreateProgramObjectARB();
    skybox_vertex_shader     = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    skybox_fragment_shader   = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

    std::cout << "Load skybox shader:" << std::endl;

    loadShaderFromFile(skybox_vertex_shader, "./shader/skybox.vert");
    loadShaderFromFile(skybox_fragment_shader, "./shader/skybox.frag");

    //
    // Compile
    //
    glCompileShaderARB(skybox_vertex_shader);
    GET_GLERROR(0);
    printInfoLog(skybox_vertex_shader);
    glCompileShaderARB(skybox_fragment_shader);
    GET_GLERROR(0);
    printInfoLog(skybox_fragment_shader);

    //
    // Link
    //
    glAttachObjectARB(g_skyboxProgram, skybox_vertex_shader);
    glAttachObjectARB(g_skyboxProgram, skybox_fragment_shader);
    glLinkProgramARB(g_skyboxProgram);
    GET_GLERROR(0);
    printInfoLog(g_skyboxProgram);

    ///////////////////////////////////////////////////////////////////////////
    //
    // Setup skybox shader
    //

    //
    // Sampler
    //
    glUseProgramObjectARB(g_skyboxProgram);
	GET_GLERROR(0);
    int staticCubeMapLocation = glGetUniformLocationARB(g_skyboxProgram, "staticCubeMap");
    GET_GLERROR(0);
    glUniform1iARB(staticCubeMapLocation, 0);
    GET_GLERROR(0);


    ///////////////////////////////////////////////////////////////////////////
    //
    // Create texturing shader
    //
    GLhandleARB texturing_vertex_shader;
    GLhandleARB texturing_fragment_shader;

    //
    // Create and load resources
    //
    g_texturingProgram          = glCreateProgramObjectARB();
    texturing_vertex_shader     = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    texturing_fragment_shader   = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

    std::cout << "Load texturing shader:" << std::endl;

    loadShaderFromFile(texturing_vertex_shader, "./shader/texturing.vert");
    loadShaderFromFile(texturing_fragment_shader, "./shader/texturing.frag");

    //
    // Compile
    //
    glCompileShaderARB(texturing_vertex_shader);
    GET_GLERROR(0);
    printInfoLog(texturing_vertex_shader);
    glCompileShaderARB(texturing_fragment_shader);
    GET_GLERROR(0);
    printInfoLog(texturing_fragment_shader);

    //
    // Link
    //
    glAttachObjectARB(g_texturingProgram, texturing_vertex_shader);
    glAttachObjectARB(g_texturingProgram, texturing_fragment_shader);
    glLinkProgramARB(g_texturingProgram);
    GET_GLERROR(0);
    printInfoLog(g_texturingProgram);

    ///////////////////////////////////////////////////////////////////////////
    //
    // Setup texturing shader
    //
    //
    // Sampler
    //
    glUseProgramObjectARB(g_texturingProgram);
    GET_GLERROR(0);
    int textureMapLocation = glGetUniformLocationARB(g_texturingProgram, "texture0");
    GET_GLERROR(0);
    glUniform1iARB(textureMapLocation, 0);
    GET_GLERROR(0);


    ///////////////////////////////////////////////////////////////////////////
    //
    // Create environment shader
    //
    GLhandleARB environment_vertex_shader;
    GLhandleARB environment_fragment_shader;

    //
    // Create and load resources
    //
    g_environmentProgram          = glCreateProgramObjectARB();
    environment_vertex_shader     = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    environment_fragment_shader   = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

    std::cout << "Load environment shader:" << std::endl;

    loadShaderFromFile(environment_vertex_shader, "./shader/environment.vert");
    loadShaderFromFile(environment_fragment_shader, "./shader/environment.frag");

    //
    // Compile
    //
    glCompileShaderARB(environment_vertex_shader);
    GET_GLERROR(0);
    printInfoLog(environment_vertex_shader);
    glCompileShaderARB(environment_fragment_shader);
    GET_GLERROR(0);
    printInfoLog(environment_fragment_shader);

    //
    // Link
    //
    glAttachObjectARB(g_environmentProgram, environment_vertex_shader);
    glAttachObjectARB(g_environmentProgram, environment_fragment_shader);
    glLinkProgramARB(g_environmentProgram);
    GET_GLERROR(0);
    printInfoLog(g_environmentProgram);

    ///////////////////////////////////////////////////////////////////////////
    //
    // Setup environment shader
    //

    //
    // Sampler
    //
    glUseProgramObjectARB(g_environmentProgram);
    GET_GLERROR(0);
    int dynamicCubeMapLocation = glGetUniformLocationARB(g_environmentProgram, "dynamicCubeMap");
    GET_GLERROR(0);
    glUniform1iARB(dynamicCubeMapLocation, 0);
    GET_GLERROR(0);

    return;
}


void
resetShader(void)
{
    glDeleteObjectARB(g_skyboxProgram);  
    glDeleteObjectARB(g_environmentProgram);  
    glDeleteObjectARB(g_mprtcProgram);  

    return;
}


void
initTexture(void)
{
    ///////////////////////////////////////////////////////////////////////////
    //
    // Init static cube map texture
    //
    glGenTextures( 1, &g_staticCubeMap);

    //
    // Lösung Aufgabe 1.1....
    //

	FILE* file;
	char* data;
	int depth = 24;
	int width = 512;
	int height = 512;

	data = (char*) malloc(width * height * depth);

	GET_GLERROR(0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, g_staticCubeMap);
	GET_GLERROR(0);

	file = fopen("data/skybox/skybox_left.raw", "rb");
	fread(data, width * height * depth, 1, file);
	fclose(file);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	GET_GLERROR(0);

	file = fopen("data/skybox/skybox_right.raw", "rb");
	fread(data, width * height * depth, 1, file);
	fclose(file);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	GET_GLERROR(0);

	file = fopen("data/skybox/skybox_top.raw", "rb");
	fread(data, width * height * depth, 1, file);
	fclose(file);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	GET_GLERROR(0);

	file = fopen("data/skybox/skybox_bottom.raw", "rb");
	fread(data, width * height * depth, 1, file);
	fclose(file);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	GET_GLERROR(0);

	file = fopen("data/skybox/skybox_back.raw", "rb");
	fread(data, width * height * depth, 1, file);
	fclose(file);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	GET_GLERROR(0);

	file = fopen("data/skybox/skybox_front.raw", "rb");
	fread(data, width * height * depth, 1, file);
	fclose(file);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	GET_GLERROR(0);

	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	GET_GLERROR(0);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	GET_GLERROR(0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	GET_GLERROR(0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	GET_GLERROR(0);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	GET_GLERROR(0);

	free(data);

    return;
}

void
resetFrameBufferObjects(void)
{
    glDeleteRenderbuffers(1, &g_renderBufferDepth);
    glDeleteFramebuffers(6, g_facesFBO);
    GET_GLERROR(0);

    glDeleteTextures(1, &g_dynamicCubeMapColor);
    GET_GLERROR(0);

    return;
}


void 
initFrameBufferObjects(void)
{
    ///////////////////////////////////////////////////////////////////////////
    //
    // Init dynamic cube map texture 
    //
    glGenTextures( 1, &g_dynamicCubeMapColor);

    //
    // Aufgabe 2.1 ...
    //

	glBindTexture(GL_TEXTURE_CUBE_MAP, g_dynamicCubeMapColor);
	GET_GLERROR(0);

	for(GLenum side = GL_TEXTURE_CUBE_MAP_POSITIVE_X; side < GL_TEXTURE_CUBE_MAP_POSITIVE_X + 6; ++side)
	{
		glTexImage2D(side, 0, GL_RGB, s_fboSize, s_fboSize, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	}

	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	GET_GLERROR(0);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	GET_GLERROR(0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	GET_GLERROR(0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	GET_GLERROR(0);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	GET_GLERROR(0);

    ///////////////////////////////////////////////////////////////////////////
    //
    // Init multi-pass render to texture FBOs
    //

    // Init shared 2D depth buffer resource, required for depth test ect.
    //
    glGenRenderbuffers(1, &g_renderBufferDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, g_renderBufferDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, s_fboSize, s_fboSize);
    GET_GLERROR(0);

    // Init OpenGL frame buffer resources
    //
    glGenFramebuffers(6, g_facesFBO);

    //
    // Aufgabe 2.2 ...
    //

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, g_renderBufferDepth);
	for(int i = 0; i <= 5; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, g_facesFBO[i]);
		GLenum side = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, side, g_dynamicCubeMapColor, 0);
	}

    return;
}


void 
updateFBOCameras(Camera camera)
{
    //
    // Aufgabe 2.3
    //

	g_fboCameras[RIGHT].position = camera.position;
	g_fboCameras[RIGHT].direction = vec3(-1.0, 0.0, 0.0);
	g_fboCameras[RIGHT].up = vec3(0.0, -1.0, 0.0);

	g_fboCameras[LEFT].position = camera.position;
	g_fboCameras[LEFT].direction = vec3(1.0, 0.0, 0.0);
	g_fboCameras[LEFT].up = vec3(0.0, -1.0, 0.0);
	
	g_fboCameras[BOTTOM].position = camera.position;
	g_fboCameras[BOTTOM].direction = vec3(0.0, -1.0, 0.0);
	g_fboCameras[BOTTOM].up = vec3(0.0, 0.0, -1.0);

	g_fboCameras[TOP].position = camera.position;
	g_fboCameras[TOP].direction = vec3(0.0, 1.0, 0.0);
	g_fboCameras[TOP].up = vec3(0.0, 0.0, 1.0);

	g_fboCameras[BACK].position =  camera.position;
	g_fboCameras[BACK].direction = vec3(0.0, 0.0, -1.0);
	g_fboCameras[BACK].up = vec3(0.0, -1.0, 0.0);

	g_fboCameras[FRONT].position = camera.position;
	g_fboCameras[FRONT].direction = vec3(0.0, 0.0, 1.0);
	g_fboCameras[FRONT].up = vec3(0.0, -1.0, 0.0);

    return;
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

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(p[0], p[1], p[2], t[0], t[1], t[2], u[0], u[1], u[2]);
    
    return;
}


void 
renderSkyBox(void)
{
    // Activate Shader program
    //
    glUseProgramObjectARB(g_skyboxProgram);
    GET_GLERROR(0);

    // Bind Textures
    //
    glEnable(GL_TEXTURE_CUBE_MAP_ARB);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, g_staticCubeMap);
    GET_GLERROR(0);

    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glRotatef(static_cast<GLfloat>(s_angle), 0.0f, 1.0f, 0.0f);

    //
    // Lösung Aufgabe 1.2...
    //

	glBegin(GL_QUADS);
		glVertex3f(5.0, 5.0, 5.0);
		glVertex3f(5.0, 5.0, -5.0);
		glVertex3f(5.0, -5.0, -5.0);
		glVertex3f(5.0, -5.0, 5.0);
	glEnd();

	glBegin(GL_QUADS);
		glVertex3f(-5.0, 5.0, 5.0);
		glVertex3f(-5.0, 5.0, -5.0);
		glVertex3f(-5.0, -5.0, -5.0);
		glVertex3f(-5.0, -5.0, 5.0);
	glEnd();

	glBegin(GL_QUADS);
		glVertex3f(5.0, 5.0, 5.0);
		glVertex3f(5.0, 5.0, -5.0);
		glVertex3f(-5.0, 5.0, -5.0);
		glVertex3f(-5.0, 5.0, 5.0);
	glEnd();

	glBegin(GL_QUADS);
		glVertex3f(5.0, -5.0, 5.0);
		glVertex3f(5.0, -5.0, -5.0);
		glVertex3f(-5.0, -5.0, -5.0);
		glVertex3f(-5.0, -5.0, 5.0);
	glEnd();

	glBegin(GL_QUADS);
		glVertex3f(5.0, 5.0, 5.0);
		glVertex3f(-5.0, 5.0, 5.0);
		glVertex3f(-5.0, -5.0, 5.0);
		glVertex3f(5.0, -5.0, 5.0);
	glEnd();

	glBegin(GL_QUADS);
		glVertex3f(5.0, 5.0, -5.0);
		glVertex3f(-5.0, 5.0, -5.0);
		glVertex3f(-5.0, -5.0, -5.0);
		glVertex3f(5.0, -5.0, -5.0);
	glEnd();    

    glPopMatrix();

    // De-activate Shader program and reset matrix and attribute state
    //
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    glUseProgramObjectARB(0);

    return;
}


void
renderCityScene(void)
{
    glActiveTexture(GL_TEXTURE0);
    GET_GLERROR(0);
    glUseProgramObjectARB(g_texturingProgram);
    GET_GLERROR(0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glRotatef(static_cast<GLfloat>(s_angle), 0.0f, 1.0f, 0.0f);

    render3DS(city3DSFile);
    GET_GLERROR(0);

    glPopMatrix();

    glUseProgramObjectARB(0);
    GET_GLERROR(0);

    return;
}


void
renderReflectiveObject(void)
{
    glUseProgramObjectARB(g_environmentProgram);
    GET_GLERROR(0);

    // Transfer Camera Position
    //
    glUseProgramObjectARB(g_environmentProgram);
    GET_GLERROR(0);
    int cameraPostionLocation = glGetUniformLocationARB(g_environmentProgram, "v3WS_CameraPosition");
    GET_GLERROR(0);
    glUniform3fARB(
        cameraPostionLocation,
        g_camera.position[0],
        g_camera.position[1],
        g_camera.position[2]
        );
    GET_GLERROR(0);


    glPushAttrib(GL_ENABLE_BIT);

    glEnable(GL_TEXTURE_CUBE_MAP_ARB);
    glActiveTexture(GL_TEXTURE0);
    GET_GLERROR(0);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, g_dynamicCubeMapColor);
    GET_GLERROR(0);

    render3DS(reflective3DSFile);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glUseProgramObjectARB(0);

    glPopAttrib();

    glUseProgramObjectARB(0);
    GET_GLERROR(0);

    return;
}


void 
renderToCubeMapMultiPass(void)
{
    updateFBOCameras(g_reflectiveCamera);

    for(int pass = 0; pass < 6; ++pass)
    {
        // Setup transforms and fbos
        //
        // 1. Adapt projection and orientation transformation
        //
        glViewport(0, 0, s_fboSize, s_fboSize);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(90, 1.0, 0.1, 20.0);

        // 2. Adapt orientation of virtual camera
        //
        setCameraTransform(g_fboCameras[pass]);

        // 3. Activate FBO
        //
        glBindFramebuffer(GL_FRAMEBUFFER, g_facesFBO[pass]);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Render scene
        //
        renderSkyBox();
        renderCityScene();

        // Unbind FBO resources
        //
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }//endfor

    return;
}


void
on_reshape(int w, int h)
{
    // Notify 
    //
    s_width = w;
    s_height = h;

    return;
}


void
on_display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //
    // 1. Render-To-Cube-Map
    //
    renderToCubeMapMultiPass();

    //
    // 2. Adapt projection and camera to scene configuration
    //
    glViewport(0, 0, (GLsizei)s_width, (GLsizei)s_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 1.0*s_width/s_height, 0.1, 20);
    setCameraTransform(g_camera);
    //glRotatef(static_cast<GLfloat>(s_angle), 0.0f, 1.0f, 0.0f);

    //
    // 3. Render Skybox
    //
    renderSkyBox();

    //
    // 4. Render Scene into Framebuffer
    //
    renderCityScene();

    //
    // 5. Render Bunny
    //
    renderReflectiveObject();

    //
    // 5. Swap
    //
    glutSwapBuffers();

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
    case '-':
        {
            if(s_fboSize > 4)
            {
                s_fboSize = s_fboSize / 2;
                resetFrameBufferObjects();
                initFrameBufferObjects();

                printf("Cube-Map Texture Size: %d\n", s_fboSize);
            }
            break;
        }

    case '+':
        {
            if(s_fboSize < 4096)
            {
                s_fboSize = s_fboSize * 2;
                resetFrameBufferObjects();
                initFrameBufferObjects();

                printf("Cube-Map Texture Size: %d\n", s_fboSize);
            }
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

    }//endswitch

    glutPostRedisplay();

    return;
}


void
on_timer(int value)
{
    if(s_use_animation)
    {
        s_angle += value;

    }//endif

    //
    // Post for redisplay and trigger callback again
    //
    glutPostRedisplay();
    glutTimerFunc(50, on_timer, 1);

    return;
}


int
main(int argc, char** argv)
{
    ///////////////////////////////////////////////////////////////////////////
    //
    // Init Window System
    //
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE | GLUT_MULTISAMPLE);
    glutInitWindowSize(s_width, s_height);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Environment Mapping");
    glewInit();

    ///////////////////////////////////////////////////////////////////////////
    //
    // Load 3DS Files
    //
    std::cout << "Loading City Model..." << std::endl;
    city3DSFile = lib3ds_file_open(cityFileName);
    if (!city3DSFile)
    {
        fprintf(stderr, "***ERROR***\nLoading file failed: %s\n", cityFileName);
        exit(1);
    }

    if (!city3DSFile->nodes)
        lib3ds_file_create_nodes_for_meshes(city3DSFile);

    static char datapath[256];
    strcat(datapath, "./data/megacity");

    //
    // Init textures
    //
    for (int i = 0; i < city3DSFile->nmaterials; ++i) {
        Lib3dsMaterial *mat = city3DSFile->materials[i];
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

    lib3ds_file_eval(city3DSFile, 0);

    std::cout << "Loading Reflective Model..." << std::endl;
    reflective3DSFile = lib3ds_file_open(reflectiveFileName);
    if (!reflective3DSFile)
    {
        fprintf(stderr, "***ERROR***\nLoading file failed: %s\n", reflectiveFileName);
        exit(1);
    }

    if (!reflective3DSFile->nodes)
        lib3ds_file_create_nodes_for_meshes(reflective3DSFile);

    lib3ds_file_eval(reflective3DSFile, 0);

    //
    // Init Camera
    //

    // Scene Camera
    //
    g_camera.position  = vec3(0.25f, 0.15f, 0.0);
    g_camera.direction = vec3(0.0, 0.0, 0.0);
    g_camera.up = vec3(0.0, 1.0, 0.0);

    // Cube map reference camera
    //
    g_reflectiveCamera.position  = vec3(0.0, 0.0, 0.0);
    g_reflectiveCamera.direction = vec3(0.0, 0.0, 1.0);
    g_reflectiveCamera.up        = vec3(0.0, 1.0, 0.0);

    ///////////////////////////////////////////////////////////////////////////
    //
    // Init Resources
    //
    initTexture();
    initShader();
    initFrameBufferObjects();

	glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_TEXTURE_CUBE_MAP_ARB);

    glutDisplayFunc(on_display);
    glutReshapeFunc(on_reshape);
    glutKeyboardFunc(on_keyboard);
    glutTimerFunc(1, on_timer, 1);
    
    glutMainLoop();

    ///////////////////////////////////////////////////////////////////////////
    //
    // Free Resources
    //
    lib3ds_file_free(city3DSFile);    
    lib3ds_file_free(reflective3DSFile);

    return 0;
}
