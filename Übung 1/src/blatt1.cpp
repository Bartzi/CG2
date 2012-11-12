#include <cassert>
#include <cstdio>
#include <cmath>
#include <vector>
#include <string>
#include <hash_map>
#include "GL/glew.h"
#include "GL/glut.h"
#include "cgmath/vec3.hpp"
#include "cgmath/vec2.hpp"
#include "utils.hpp"


typedef cgmath::vec3<float> vec3;
typedef cgmath::vec2<float> vec2;
typedef cgmath::vec3<unsigned> uvec3;
typedef cgmath::vec3<unsigned> uvec2;

struct face {
    uvec3 v;
    uvec3 n;
};


struct AABB {
	float xMin;
	float xMax; 
	float yMin;
	float yMax;
	float zMin;
	float zMax;
};

///////////////////////////////////////////////////////////////////////////////
//
// Declare graphics state
//
AABB g_sceneAABB;

GLhandleARB g_terrainProgram;

std::vector<vec3> g_vertices;
std::vector<vec3> g_normals;
std::vector<face> g_faces;

GLfloat* g_sceneVBOVertices;
GLfloat* g_sceneVBONormals;
int g_size_vertex;
int g_size_normal;

GLuint g_sceneVBO;

GLuint g_texTerrain;
GLuint g_texGradient;

///////////////////////////////////////////////////////////////////////////////
//
// Init state variables
//
double angle = 90;

int s_width  = 800;
int s_height = 600;

bool s_draw_vbo = true;
bool s_draw_array = false;
bool s_use_animation = false;

AABB computeAABB(const std::vector<vec3>& verticies)
{
    //
    // Aufgabe 1.a
    //
	AABB aabb;
	aabb.xMin = aabb.yMin = aabb.zMin = FLT_MAX; 
	aabb.xMax = aabb.yMax = aabb.zMax = FLT_MIN; 

    //
    // AABB Berechnung implementieren...
    //
	for (int i = 0; i < verticies.size(); i++)
	{
		if (verticies[i].x < aabb.xMin)
			aabb.xMin = verticies[i].x;
		else if (verticies[i].x > aabb.xMax)
			aabb.xMax = verticies[i].x;

		if (verticies[i].y < aabb.yMin)
			aabb.yMin = verticies[i].y;
		else if (verticies[i].y > aabb.yMax)
			aabb.yMax = verticies[i].y;

		if (verticies[i].z < aabb.zMin)
			aabb.zMin = verticies[i].z;
		else if (verticies[i].z > aabb.zMax)
			aabb.zMax = verticies[i].z;
	}


#ifdef _DEBUG

	//
	// Print AABBB
	//
	std::cout << "Scene AABB: " << std::endl;
	std::cout << "  X: [ " << aabb.xMin << " | " << aabb.xMax << " ]" << std::endl;
	std::cout << "  Y: [ " << aabb.yMin << " | " << aabb.yMax << " ]" << std::endl;
	std::cout << "  Z: [ " << aabb.zMin << " | " << aabb.zMax << " ]" << std::endl;

#endif

	return aabb;
}

bool
load_obj(const char *path)
{
    const char *s_sep = " \t\n\r";

    FILE *f = fopen(path, "rt");
    if (!f)
        return false;

    bool result = true;
    char buf[256];
    while (result && !feof(f)) {
        strcpy(buf, "");
        fgets(buf, 256, f);

        char *token = strtok( buf, s_sep);
        if (token) {
            switch (token[0]) {
case 'v': {
    if ((token[1] == 0) || (token[1] == 'n') || (token[1] == 't')) {
        char type = token[1];

        token = strtok( NULL, s_sep);
        int i = 0;
        int r = 3;
        if(type == 't') r = 2;
        float v[3];
        while (token && (i < r)) {
            v[i] = static_cast<float>(atof(token));
            token = strtok( NULL, s_sep);
            ++i;
        }

        if ((i != r) || token) {
            result = false;
            break;
        }

        switch(type)
        {
        case 0: 
            {
                g_vertices.push_back(vec3(v[0],v[1],v[2]));
                break;
            }
        case 'n':
            {
                g_normals.push_back(vec3(v[0],v[1],v[2]));
                break;
            }
        default: break;
        }

    }
    break;
          }

case 'f': {
    face f;
    for (int i = 0; i < 3; ++i) {
        token = strtok( NULL, s_sep);
        if (!token) {
            result = false;
            break;
        }

        char *p = strchr(token, '/');
        char *q = strrchr(token, '/');
        if (!p || !q) {
            result = false;
            break;
        }

        *p = 0;
        ++q;

        f.v[i] = atoi(token) - 1;
        f.n[i] = atoi(q) - 1;

        if ((f.v[i] < 0) || (f.n[i] < 0)) {
            result = false;
            break;
        }
    }
    if (result) {
        g_faces.push_back(f);
    }
    break;
          }
            }
        }
    }

    fclose(f);

	//
	// Compute scene AABB
	//
	g_sceneAABB = computeAABB(g_vertices);

    return result;
}


void
initShader(void)
{
    ///////////////////////////////////////////////////////////////////////////
    //
    // Create earth shader
    //
    GLhandleARB terrain_vertex_shader;
    GLhandleARB terrain_fragment_shader;

    //
    // Create and load resources
    //
    g_terrainProgram          = glCreateProgramObjectARB();
    terrain_vertex_shader     = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    terrain_fragment_shader   = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

    loadShaderFromFile(terrain_vertex_shader, "shader/terrain.vert");
    loadShaderFromFile(terrain_fragment_shader, "shader/terrain.frag");

    //
    // Compile
    //
    glCompileShaderARB(terrain_vertex_shader);
    GET_GLERROR(0);
    printInfoLog(terrain_vertex_shader);

    glCompileShaderARB(terrain_fragment_shader);
    GET_GLERROR(0);
    printInfoLog(terrain_fragment_shader);

    //
    // Link
    //
    glAttachObjectARB(g_terrainProgram, terrain_vertex_shader);
    glAttachObjectARB(g_terrainProgram, terrain_fragment_shader);
    glLinkProgramARB(g_terrainProgram);
    GET_GLERROR(0);
    printInfoLog(g_terrainProgram);

    ///////////////////////////////////////////////////////////////////////////
    //
    // Setup terrain shader
    //

    //
    // Sampler
    //
    glUseProgramObjectARB(g_terrainProgram);
	GET_GLERROR(0);

	float test;
    //
    // Aufgabe 1.b - Begin
    //
	
	int samplerTerrain = glGetUniformLocationARB(g_terrainProgram, "samplerTerrain");
	GET_GLERROR(0);
	int samplerGradient = glGetUniformLocationARB(g_terrainProgram, "samplerGradient");
	GET_GLERROR(0);

	glUniform1i(samplerTerrain, g_texTerrain);
	GET_GLERROR(0);
	glUniform1i(samplerGradient, g_texGradient);
	GET_GLERROR(0);
    //
    // Aufgabe 1.b - End
    //

	//
	// Bounding Box
	//
	int LLFLocation = glGetUniformLocationARB(g_terrainProgram, "v3LLF");
	GET_GLERROR(0);
	int URBLocation = glGetUniformLocationARB(g_terrainProgram, "v3URB");
	GET_GLERROR(0);

	vec3 LLF = vec3(g_sceneAABB.xMin, g_sceneAABB.yMin, g_sceneAABB.zMin);
	vec3 URB = vec3(g_sceneAABB.xMax, g_sceneAABB.yMax, g_sceneAABB.zMax);

    glUniform3fARB(LLFLocation, g_sceneAABB.xMin, g_sceneAABB.yMin, g_sceneAABB.zMin);
	GET_GLERROR(0);
	glUniform3fARB(URBLocation, g_sceneAABB.xMax, g_sceneAABB.yMax, g_sceneAABB.zMax);
	GET_GLERROR(0);
    return;
}


void
resetShader(void)
{
    glDeleteObjectARB(g_terrainProgram);  

    return;
}


void
initTexture(void)
{
    g_texTerrain  = loadTextureRAW2D("data/terrain_872x810.raw", false, 872, 810, 3);
    g_texGradient = loadTextureRAW1D("data/gradient_512x1.raw", false, 512, 3);

    return;
}


void
on_reshape(int w, int h)
{
    //
    // Notify & Reset framebuffer object
    //
    s_width = w;
    s_height = h;

    //
    // Adapt projection and orientation transformation
    //
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 1.0*w/h, 1, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(2.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 2.0, 0.0);

    return;
}


void 
initVertexBufferObject(void)
{
    g_size_vertex   = g_faces.size()*3*3;
    g_size_normal   = g_faces.size()*3*3;

    g_sceneVBOVertices  = new GLfloat[g_size_vertex];
    g_sceneVBONormals   = new GLfloat[g_size_normal];

    std::cout << "faces: "          <<  g_faces.size()  << std::endl;
    std::cout << "vbo_vertices: "   <<  g_size_vertex     << std::endl;
    std::cout << "vbo_normals: "    <<  g_size_normal     << std::endl;

	//
    // Collapse indexed mesh
    //
    for (unsigned j = 0; j < g_faces.size(); ++j)
    {
        for (unsigned i = 0; i < 3; ++i)
        {
            // per vertex of face do...
            g_sceneVBOVertices[j*9+i*3+0]  = g_vertices[g_faces[j].v[i]][0];
            g_sceneVBOVertices[j*9+i*3+1]  = g_vertices[g_faces[j].v[i]][1];
            g_sceneVBOVertices[j*9+i*3+2]  = g_vertices[g_faces[j].v[i]][2];

            g_sceneVBONormals[j*9+i*3+0]   = g_normals[g_faces[j].n[i]][0];
            g_sceneVBONormals[j*9+i*3+1]   = g_normals[g_faces[j].n[i]][1];
            g_sceneVBONormals[j*9+i*3+2]   = g_normals[g_faces[j].n[i]][2];

        }//endfor

    }//endfor

    glGenBuffers(1, &g_sceneVBO);
    glBindBuffer(GL_ARRAY_BUFFER, g_sceneVBO);
    GET_GLERROR(0);

    // reserve buffer but not initialize it...
    glBufferData(GL_ARRAY_BUFFER, (g_size_vertex+g_size_normal) * sizeof(GLfloat), 0, GL_STATIC_DRAW);
    GET_GLERROR(0);

    // copy vertices starting from 0 offset
    glBufferSubData(GL_ARRAY_BUFFER, 0, g_size_vertex* sizeof(GLfloat), g_sceneVBOVertices);                             
    GET_GLERROR(0);

    // copy normals after vertices
    glBufferSubData(GL_ARRAY_BUFFER, g_size_vertex* sizeof(GLfloat) , g_size_normal* sizeof(GLfloat), g_sceneVBONormals);
    GET_GLERROR(0);

    return;
}


void 
resetVertexBufferObject(void)
{
    glDeleteBuffers(1, &g_sceneVBO);
    return;
}


void
on_display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //
    // Apply model transformation
    //
    glPushMatrix();
    glRotatef(static_cast<GLfloat>(angle), 0.0f, 1.0f, 0.0f);
	glRotatef(-45, 1.0f, 0.0f, 0.0f);

	//
	// Bind Textures
	//
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_texTerrain);
    GET_GLERROR(0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_1D, g_texGradient);
    GET_GLERROR(0);

	//
	// Use Program
	//
    glUseProgramObjectARB(g_terrainProgram);
    GET_GLERROR(0);

    if(s_draw_vbo)
    {
        // bind VBOs with IDs and set the buffer offsets of the bound VBOs
        // When buffer object is bound with its ID, all pointers in gl*Pointer()
        // are treated as offset instead of real pointer.
        glBindBuffer(GL_ARRAY_BUFFER, g_sceneVBO);

        // enable vertex arrays
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_VERTEX_ARRAY);

        // before draw, specify arrays with their offsets
        glNormalPointer(GL_FLOAT, 0, (void*)(g_size_vertex* sizeof(GLfloat)));
        glTexCoordPointer(2, GL_FLOAT, 0, (void*)((g_size_vertex+g_size_normal)* sizeof(GLfloat)));
        glVertexPointer(3, GL_FLOAT, 0, 0);

        glDrawArrays(GL_TRIANGLES, 0, g_faces.size()*3);

        glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);

        // it is good idea to release VBOs with ID 0 after use.
        // Once bound with 0, all pointers in gl*Pointer() behave as real
        // pointer, so, normal vertex array operations are re-activated
        glBindBuffer(GL_ARRAY_BUFFER, 0);

    } else {

        if(s_draw_array)
        {
            //
            // Enable vertex arrays
            //
            glEnableClientState(GL_NORMAL_ARRAY);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glEnableClientState(GL_VERTEX_ARRAY);

            //
            // Specify vertex arrays
            //
            glNormalPointer(GL_FLOAT, 0, g_sceneVBONormals);
            glVertexPointer(3, GL_FLOAT, 0, g_sceneVBOVertices);
    
            //
            // Draw 
            //
            glDrawArrays(GL_TRIANGLES, 0, g_faces.size()*3);

            //
            // Enable vertex arrays
            //
            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            glDisableClientState(GL_NORMAL_ARRAY);

        } else {

            //
            // Draw using intermediate mode
            //
            glBegin(GL_TRIANGLES);
            {
                for (unsigned j = 0; j < g_faces.size(); ++j)
                {
                    for (unsigned i = 0; i < 3; ++i)
                    {
                        glNormal3fv(&g_normals[g_faces[j].n[i]][0]);
                        glVertex3fv(&g_vertices[g_faces[j].v[i]][0]);

                    }//endfor

                }//endfor
            }
            glEnd();

        }//endif

    }//endif

    glUseProgramObjectARB(0);
    glPopMatrix();

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
    case 'i':
        {
            printf("DRAW: intermediate mode\n");
            s_draw_vbo    = false;
            s_draw_array  = false;

            break;
        }
    case 'v':
        {
            printf("DRAW: vbo mode\n");
            s_draw_vbo            = true;
            s_draw_array          = false;

            break;
        }
    case 'a':
        {
            printf("DRAW: vertex array mode\n");
            s_draw_vbo = false;
            s_draw_array = true;

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

void loadTerrain(void)
{
    //
    // Aufgabe 1.a ...
    //
	load_obj("data/terrain.obj");

    return;
}

int main(int argc, char** argv)
{
    loadTerrain();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE | GLUT_MULTISAMPLE);
    glutInitWindowSize(s_width, s_height);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Terrain");

    glewInit();
    initVertexBufferObject();
    initTexture();
    initShader();

	glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    glutDisplayFunc(on_display);
    glutReshapeFunc(on_reshape);
    glutKeyboardFunc(on_keyboard);
    glutTimerFunc(1, on_timer, 1);
    
    glutMainLoop();

    return 0;
}
