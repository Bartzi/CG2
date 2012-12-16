



// INCLUDES


#include <cassert>
#include <cstdio>
#include <cmath>
#include <vector>
#include <cstring>
#include <hash_map>
#include <cfloat>

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>

#include "cgmath/vec3.hpp"
#include "cgmath/vec2.hpp"

#include "utils.hpp"
#include "picture.h"

#include <ctime>




// TYPES


typedef cgmath::vec3<float> vec3;
typedef cgmath::vec2<float> vec2;

typedef cgmath::vec3<unsigned> uvec3;
typedef cgmath::vec3<unsigned> uvec2;

// Early Forward Declaration
const vec3 crossProduct(
	const vec3 & a
,	const vec3 & b);

struct face 
{
    uvec3 v;
    uvec3 n;
};


struct AABB 
{
	float xMin;
	float xMax; 
	float yMin;
	float yMax;
	float zMin;
	float zMax;
};


class ModelObject
{
public:
	ModelObject() {};

	std::vector<vec3> m_vertices;
	std::vector<vec3> m_normals;
	std::vector<face> m_faces;

	GLuint m_tex0;
	GLuint m_tex1;

	AABB m_aabb;
};




// GLOBALS


GLhandleARB g_labelProgram;
GLhandleARB g_sceneProgram;

ModelObject * g_cornellboxModelObject = NULL;
ModelObject * g_companionModelObject  = NULL;
ModelObject * g_luxoJrModelObject     = NULL;

int g_width  = 768;
int g_height = 768;

const vec3 g_eyeBak (0.f, .2f,-1.f);
const vec3 g_center (0.f,-.2f, 0.f);
const vec3 g_up     (0.f, 1.f, 0.f);

vec3 g_eye = g_eyeBak;

GLint g_modeUniform(-1);

GLint g_iDtSizeUniform(-1);
vec2 g_iDtSize(0.f, 0.f);


// AB HIER ÄNDERUNGEN NOTWENDIG
// ----------------------------


// EXERCISE 3 - BEGIN TASKS


// TASK 1A - Specify AABB offsets that yield an appropriate labeling for you. 
// -------

const vec3 g_cornellboxLabelOffset( 0.f, -0.3f, 0.f);
const vec3 g_companionLabelOffset ( 0.f, 0.1f, 0.f);
const vec3 g_luxoJrLabelOffset    ( 0.f, -0.3f, -0.25f);


void label(
	ModelObject * model
,	const vec3 & offset)
{
	glUseProgramObjectARB(g_labelProgram);
	GET_GLERROR(0);

	// TASK 1B - Alpha Blending aktivieren/deaktivieren.
	// -------

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// TASK 1A - Rendern und Positionieren eines texturierten Quads als Modellbeschriftung.
	// -------

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, model->m_tex0);
	GET_GLERROR(0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, model->m_tex1);

	glPushMatrix();


	vec3 position = vec3((model->m_aabb.xMax + model->m_aabb.xMin) / 2 + offset.x, model->m_aabb.yMax + offset.y, (model->m_aabb.zMax + model->m_aabb.zMin) / 2 + offset.z);
	vec3 look = vec3(g_eye.x - position.x, 0.0, g_eye.z - position.z);
	look.normalize();

	vec3 minus_z = vec3(0.0, 0.0, -1.0);
	float angle = cgmath::to_deg(acos(dot_prod(look, minus_z)));

	vec3 normal = cross_prod(look, minus_z);
	
	glTranslatef(position.x, position.y, position.z);
	glRotatef(-angle, normal.x, normal.y, normal.z);

	glBegin(GL_QUADS);
		glVertex3f(-0.2, -0.05, 0.0);
		glTexCoord2f(0, 0);
		glVertex3f(0.2, -0.05, 0.0);
		glTexCoord2f(0, 1);
		glVertex3f(0.2, 0.05, 0.0);
		glTexCoord2f(1, 1);
		glVertex3f(-0.2, 0.05, 0.0);
		glTexCoord2f(1, 0);
	glEnd();	

	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, 0);
	GET_GLERROR(0);

	glDisable(GL_BLEND);

	glUseProgramObjectARB(0);
	GET_GLERROR(0);
}


int clamp(int x, int min, int max)
{
	if(x < min)
		return min;
	if(x > max - 1)
		return max - 1;
	
	return x;
}


Picture * distanceTransform(
	const Picture & picture
,	const unsigned int minimalSideLength = 32
,	const unsigned int kernelSize  = 96)
{

	typedef struct struct_point{
		int x;
		int y;

		struct_point(int vx, int vy)
		{
			x = vx; 
			y = vy;
		}

	} Point;

#ifdef _DEBUG
	printf("Computing Distance Transform (%03i%%)\xd", 0);
	float progress = 0;
#endif


	// TASK 2A - Signed Distance Transform durchführen
	// -------

	const int hresW = picture.getWidth();
	const int hresH = picture.getHeight();

	/*
	 * we are performing the dead reckoning signed distance transform
	 * source: "The 'dead reckoning' signed distance transform" by George J. Grevera, 3rd February 2003
	 */

	Picture * distanceField = new Picture(hresW, hresH);
	Point** p = (Point**) malloc(hresW * sizeof(Point));
	float** distance = (float**) malloc(hresW * sizeof(float)); 
	for(int i = 0; i < hresW; i++)
	{
		p[i] = (Point*) malloc(hresH * sizeof(Point));
		distance[i] = (float*) malloc(hresH * sizeof(float));
	}

	float d_diagonal = sqrt(2.0);
	float d_direct = 1.0;

	for(int y = 0; y < picture.getHeight(); y++)
		for(int x = 0; x < picture.getWidth(); x++)
		{
			distance[x][y] = FLT_MAX;
			p[x][y] = Point(-1, -1);
		}

	// initialize borders
	for(int y = 0; y < picture.getHeight(); y++)
		for(int x = 0; x < picture.getWidth(); x++)
		{
			if( (picture.getColor(x - 1, y)[0] != picture.getColor(x, y)[0]) || (picture.getColor(x + 1, y)[0] != picture.getColor(x, y)[0]) || 
				(picture.getColor(x, y - 1)[0] != picture.getColor(x, y)[0]) || (picture.getColor(x, y + 1)[0] != picture.getColor(x, y)[0]))
				distance[x][y] = 0.0;
				p[x][y] = Point(x, y);
		}

#ifdef _DEBUG
		// TODO: Falls ein Fortschritt während der Berechnung gewünscht ist
		// kann hier einfach der Progress pro Schleifendurchlauf aktualisiert werden.
		progress += 10; 
		printf("Computing Distance Transform (%03i%%)\xd", static_cast<int>(progress * 100));
#endif

	// perform first Pass
	for(int y = 0; y < picture.getHeight(); y++)
		for(int x = 0; x < picture.getWidth(); x++)
		{
			if(distance[clamp(x - 1, 0, picture.getWidth())][clamp(y - 1, 0, picture.getHeight())] + d_diagonal < distance[x][y])
			{
				p[x][y] = p[clamp(x - 1, 0, picture.getWidth())][clamp(y - 1, 0, picture.getHeight())];
				distance[x][y] = sqrt(pow(x - p[x][y].x, 2.0) + pow(y - p[x][y].y, 2.0));
			}
			if(distance[x][clamp(y - 1, 0, picture.getHeight())] + d_direct < distance[x][y])
			{
				p[x][y] = p[x][clamp(y - 1, 0, picture.getHeight())];
				distance[x][y] = sqrt(pow(x - p[x][y].x, 2.0) + pow(y - p[x][y].y, 2.0));
			}
			if(distance[clamp(x + 1, 0, picture.getWidth())][clamp(y - 1, 0, picture.getHeight())] + d_diagonal < distance[x][y])
			{
				p[x][y] = p[clamp(x + 1, 0, picture.getWidth())][clamp(y - 1, 0, picture.getHeight())];
				distance[x][y] = sqrt(pow(x - p[x][y].x, 2.0) + pow(y - p[x][y].y, 2.0));
			}
			if(distance[clamp(x - 1, 0, picture.getWidth())][y] + d_direct < distance[x][y])
			{
				p[x][y] = p[clamp(x - 1, 0, picture.getWidth())][y];
				distance[x][y] = sqrt(pow(x - p[x][y].x, 2.0) + pow(y - p[x][y].y, 2.0));
			}
		}

#ifdef _DEBUG
		// TODO: Falls ein Fortschritt während der Berechnung gewünscht ist
		// kann hier einfach der Progress pro Schleifendurchlauf aktualisiert werden.
		progress += 40; 
		printf("Computing Distance Transform (%03i%%)\xd", static_cast<int>(progress * 100));
#endif

	// perform second and final pass
	for(int y = picture.getHeight() - 1; y >= 0; y--)
		for(int x = picture.getWidth() - 1; x >= 0; x--)
		{
			if(distance[clamp(x + 1, 0, picture.getWidth())][y] + d_direct < distance[x][y])
			{
				p[x][y] = p[clamp(x + 1, 0, picture.getWidth())][y];
				distance[x][y] = sqrt(pow(x - p[x][y].x, 2.0) + pow(y - p[x][y].y, 2.0));
			}
			if(distance[clamp(x - 1, 0, picture.getWidth())][clamp(y + 1, 0, picture.getHeight())] + d_diagonal < distance[x][y])
			{
				p[x][y] = p[clamp(x - 1, 0, picture.getWidth())][clamp(y + 1, 0, picture.getHeight())];
				distance[x][y] = sqrt(pow(x - p[x][y].x, 2.0) + pow(y - p[x][y].y, 2.0));
			}
			if(distance[x][clamp(y + 1, 0, picture.getHeight())] + d_direct < distance[x][y])
			{
				p[x][y] = p[x][clamp(y + 1, 0, picture.getHeight())];
				distance[x][y] = sqrt(pow(x - p[x][y].x, 2.0) + pow(y - p[x][y].y, 2.0));
			}
			if(distance[clamp(x + 1, 0, picture.getWidth())][clamp(y + 1, 0, picture.getHeight())] + d_diagonal < distance[x][y])
			{
				p[x][y] = p[clamp(x + 1, 0, picture.getWidth())][clamp(y + 1, 0, picture.getHeight())];
				distance[x][y] = sqrt(pow(x - p[x][y].x, 2.0) + pow(y - p[x][y].y, 2.0));
			}
		}

#ifdef _DEBUG
		// TODO: Falls ein Fortschritt während der Berechnung gewünscht ist
		// kann hier einfach der Progress pro Schleifendurchlauf aktualisiert werden.
		progress += 40; 
		printf("Computing Distance Transform (%03i%%)\xd", static_cast<int>(progress * 100));
#endif

	// maximum distance from a border shall be 80...
	float max = 80.0;

	// indicate inside and outside

	for(int y = picture.getHeight() - 1; y >= 0; y--)
		for(int x = picture.getWidth() - 1; x >= 0; x--)
		{
			ColorRGBA color = picture.getColor(x, y);
			distance[x][y] = distance[x][y] / max;
			if(color[0] == 0.0)
				distance[x][y] = -distance[x][y];
			if(distance[x][y] < -1)
				distance[x][y] = -1;
			float distance_field_color = (distance[x][y] + 1) / 2;
			distanceField->setColor(x, y, ColorRGBA(distance_field_color, distance_field_color, distance_field_color, 1.0));
		}

#ifdef _DEBUG
	printf("Computing Distance Transform (100%%)\n"); 
#endif

	// free not anymore needed ressources
	free(p);
	free(distance);

	return distanceField;
}

// END EXERCISE 3 - TASKS

// ---------------------------------------
// AB HIER KEINE ÄNDERUNGEN MEHR NOTWENDIG




// FORWARD DECLARATIONS


void createModelObjects();
void initalizeShaders();
void setMode(const int mode);
void generateDistanceFields();
void deleteShaders();
void updateLookAt();

void render(ModelObject * model);
void label(
	ModelObject * model
,	const vec3 & offset);

const AABB computeAABB(const std::vector<vec3> & vertices);

const bool loadModelObjectFromObjFile(
	const char * filePath
,	ModelObject * model);

void on_reshape(
	int width
,	int height);

void on_display(void);

void on_keyboard(
	unsigned char key
,	int x
,	int y);

void on_mousemove(
	int x
,	int y);

void on_timer(int value);




// MAIN


int main(int argc, char** argv)
{
	// OpenGL Setup

    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(g_width, g_height);
    glutInitWindowPosition(0, 0);

    glutCreateWindow("Labeling, Alpha-Blending und Distance Fields - Blatt 3");

	glewInit();

	// Scene Setup

	createModelObjects();
	generateDistanceFields();

	initalizeShaders();

	glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);

	// Glut Setup

    glutDisplayFunc(on_display);
    glutReshapeFunc(on_reshape);
    glutKeyboardFunc(on_keyboard);
	glutPassiveMotionFunc(on_mousemove);
    glutTimerFunc(1, on_timer, 1);

	// Enter Main Loop

    glutMainLoop();

	deleteShaders();

    return 0;
}




// SCENE SETUP


void generateDistanceFields()
{
	// load textures

	Picture textureCornellBox("./data/cornellbox.png");
	Picture textureCompanion("./data/companion_cube.png");
	Picture textureLuxoJr("./data/luxo_jr.png");

	// generate OpenGL textures

	g_cornellboxModelObject->m_tex0 = loadRGBATexture(textureCornellBox.getCharPixels()
		, false, textureCornellBox.getWidth(), textureCornellBox.getHeight());
	g_companionModelObject->m_tex0  = loadRGBATexture(textureCompanion.getCharPixels()
		, false, textureCompanion.getWidth(), textureCompanion.getHeight());
	g_luxoJrModelObject->m_tex0     = loadRGBATexture(textureLuxoJr.getCharPixels()
		, false, textureLuxoJr.getWidth(), textureLuxoJr.getHeight());


	// little timer for basic performance measurment
	// note: this should be done in release mode, not least because the
	// distanceTransform functions produce massive amount of console 
	// output with the progress logging... (disabled in release)

#ifndef _DEBUG
	printf("Computing Distance Transforms\n");
#endif

	const clock_t t0(clock());

	Picture distanceCornellBox = *distanceTransform(textureCornellBox);
	Picture distanceCompanion  = *distanceTransform(textureCompanion);
	Picture distanceLuxoJr     = *distanceTransform(textureLuxoJr);

	const clock_t t1(clock());
	const double deltaT = static_cast<double>(t1 - t0) / CLOCKS_PER_SEC;

	printf("Computation took %f seconds.\n\n", deltaT);

	g_iDtSize = vec2(1.f / distanceCompanion.getWidth(), 1.f / distanceCompanion.getHeight());

	// generate OpenGL textures

	g_cornellboxModelObject->m_tex1 = loadRGBATexture(distanceCornellBox.getCharPixels()
		, false, distanceCornellBox.getWidth(), distanceCornellBox.getHeight());
	g_companionModelObject->m_tex1  = loadRGBATexture(distanceCompanion.getCharPixels()
		, false, distanceCompanion.getWidth(), distanceCompanion.getHeight());
	g_luxoJrModelObject->m_tex1     = loadRGBATexture(distanceLuxoJr.getCharPixels()
		, false, distanceLuxoJr.getWidth(), distanceLuxoJr.getHeight());
}


void createModelObjects()
{
#ifndef _DEBUG
	printf("Loading 3d Models\n\n");
#endif

	g_cornellboxModelObject = new ModelObject();

#ifdef _DEBUG
	printf("\nLoading cornellbox.obj...\n");
#endif
    if (!loadModelObjectFromObjFile("./data/cornellbox.obj", g_cornellboxModelObject))
    {
        printf("ERROR: loading cornellbox.obj failed.\n");
        exit(1);
    }

	g_companionModelObject = new ModelObject();

#ifdef _DEBUG
	printf("\nLoading companion_cube.obj...\n");
#endif
	if (!loadModelObjectFromObjFile("./data/companion_cube.obj", g_companionModelObject))
	{
		printf("ERROR: loading companion_cube.obj failed.\n");
		exit(1);
	}

	g_luxoJrModelObject = new ModelObject();

#ifdef _DEBUG
	printf("\nLoading luxo_jr.obj...\n");
#endif
	if (!loadModelObjectFromObjFile("./data/luxo_jr.obj", g_luxoJrModelObject))
	{
		printf("ERROR: loading luxo_jr.obj failed.\n");
		exit(1);
	}

#ifdef _DEBUG
	printf("\n");
#endif

    return;
}


void render(ModelObject * model)
{
	glUseProgramObjectARB(g_sceneProgram);
	GET_GLERROR(0);

	// Draw using intermediate mode

	glBegin(GL_TRIANGLES);
	{
		for (unsigned j = 0; j < model->m_faces.size(); ++j)
		{
			for (unsigned i = 0; i < 3; ++i)
			{
				glNormal3fv(&(model->m_normals [model->m_faces[j].n[i]][0]));
				glVertex3fv(&(model->m_vertices[model->m_faces[j].v[i]][0]));
			}
		}
	}
	glEnd();

	glUseProgramObjectARB(0);
	GET_GLERROR(0);
}


void initalizeShaders()
{
	printf("Initializing Shaders\n");


	// Setup shader for rendering the scene including all model objects

	g_sceneProgram = glCreateProgramObjectARB();

	const GLhandleARB model_vertex_shader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	loadShaderFromFile(model_vertex_shader, "shader/scene.vert");
	
	glCompileShaderARB(model_vertex_shader);
	GET_GLERROR(0);
	printInfoLog(model_vertex_shader);

	const GLhandleARB model_fragment_shader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
	loadShaderFromFile(model_fragment_shader, "shader/scene.frag");
	
	glCompileShaderARB(model_fragment_shader);
	GET_GLERROR(0);
	printInfoLog(model_fragment_shader);

	glAttachObjectARB(g_sceneProgram, model_vertex_shader);
	GET_GLERROR(0);
	glAttachObjectARB(g_sceneProgram, model_fragment_shader);
	GET_GLERROR(0);

	glLinkProgramARB(g_sceneProgram);
	GET_GLERROR(0);
	printInfoLog(g_sceneProgram);


	// Setup distance field shader

    g_labelProgram = glCreateProgramObjectARB();

	const GLhandleARB labelFragmentShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
    loadShaderFromFile(labelFragmentShader, "shader/label.frag");

	glCompileShaderARB(labelFragmentShader);
    GET_GLERROR(0);
    printInfoLog(labelFragmentShader);

	glAttachObjectARB(g_labelProgram, labelFragmentShader);
	GET_GLERROR(0);
    glLinkProgramARB(g_labelProgram);
    GET_GLERROR(0);
    printInfoLog(g_labelProgram);


	// Sampler

	glUseProgramObjectARB(g_labelProgram);
	GET_GLERROR(0);

    const GLint txtSamplerUniform = glGetUniformLocationARB(g_labelProgram, "txtSampler");
	GET_GLERROR(0);
    glUniform1iARB(txtSamplerUniform, 0);
	GET_GLERROR(0);

    const GLint dtSamplerUniform = glGetUniformLocationARB(g_labelProgram, "dtSampler");
	GET_GLERROR(0);
    glUniform1iARB(dtSamplerUniform, 1);
	GET_GLERROR(0);

	g_iDtSizeUniform = glGetUniformLocationARB(g_labelProgram, "iDtSize");
	GET_GLERROR(0);
	glUniform2fARB(g_iDtSizeUniform, g_iDtSize.x, g_iDtSize.y);
	GET_GLERROR(0);

	g_modeUniform = glGetUniformLocationARB(g_labelProgram, "mode");
	GET_GLERROR(0);
	glUniform1iARB(g_modeUniform, 0);
	GET_GLERROR(0);

    glUseProgramObjectARB(0);

    return;
}


void setMode(const int mode)
{
	glUseProgramObjectARB(g_labelProgram);
	GET_GLERROR(0);
	glUniform1iARB(g_modeUniform, mode);
	GET_GLERROR(0);

	glUseProgramObjectARB(0);
}


void deleteShaders()
{
    glDeleteObjectARB(g_labelProgram);  
	glDeleteObjectARB(g_sceneProgram);  

    return;
}




// ON GLUT


void on_reshape(int width, int height)
{
    // Notify & Reset framebuffer object

    g_width = width;
    g_height = height;

    // Adapt projection and orientation transformation

    glViewport(0, 0, static_cast<GLsizei>(g_width), static_cast<GLsizei>(g_height));
    
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

	const GLdouble aspect = static_cast<GLdouble>(g_width) / static_cast<GLdouble>(g_height);
    gluPerspective(60.0, aspect, 0.5, 2.0);

	updateLookAt();
}


void on_display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();

	// render models

	render(g_cornellboxModelObject);
	render(g_companionModelObject);
	render(g_luxoJrModelObject);

	// render labels

	label(g_cornellboxModelObject, g_cornellboxLabelOffset);
	label(g_companionModelObject, g_companionLabelOffset);
	label(g_luxoJrModelObject, g_luxoJrLabelOffset);

	glPopMatrix();
    glutSwapBuffers();
}


void on_keyboard(
	unsigned char key
,	int x
,	int y)
{
    switch (key)
    {
    case 27: // ESC key
		exit(0);
        break;

	default:
		break;
    }

	if(key > 47 && key < 55)
		setMode(key - 48); // set 0 to 6 keys to mode

    glutPostRedisplay();
    return;
}


void on_mousemove(
	int x
,	int y)
{
	// Apply little motion based on mouse position, just 
	// for better sensation of label placement and depth.

	g_eye = g_eyeBak + vec3(x - g_width * 0.5f, y - g_height * 0.5f, 0.0) * 0.0001f; 

	updateLookAt();
}


void on_timer(int value)
{
    // Post for redisplay and trigger callback again

    glutPostRedisplay();
    glutTimerFunc(1, on_timer, 1);

    return;
}




// Utility Functions


void updateLookAt()
{
	// update camera look at

	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(g_eye.x, g_eye.y, g_eye.z, 
		g_center.x, g_center.y, g_center.z, g_up.x, g_up.y, g_up.z);
}


const AABB computeAABB(const std::vector<vec3> &vertices)
{
	AABB aabb;
	aabb.xMin = aabb.yMin = aabb.zMin =  FLT_MAX; 
	aabb.xMax = aabb.yMax = aabb.zMax = -FLT_MAX; 

	for(unsigned int i = 0; i < vertices.size(); ++i)
	{
		vec3 vertex =  vertices[i];
		aabb.xMin = std::min(aabb.xMin, vertex[0]);
		aabb.xMax = std::max(aabb.xMax, vertex[0]);
		aabb.yMin = std::min(aabb.yMin, vertex[1]);
		aabb.yMax = std::max(aabb.yMax, vertex[1]);
		aabb.zMin = std::min(aabb.zMin, vertex[2]);
		aabb.zMax = std::max(aabb.zMax, vertex[2]);
	}

#ifdef _DEBUG

	std::cout << "AABB: " << std::endl;
	std::cout << "  X: [ " << aabb.xMin << " | " << aabb.xMax << " ]" << std::endl;
	std::cout << "  Y: [ " << aabb.yMin << " | " << aabb.yMax << " ]" << std::endl;
	std::cout << "  Z: [ " << aabb.zMin << " | " << aabb.zMax << " ]" << std::endl;

#endif

	return aabb;
}


const vec3 crossProduct(
	const vec3 & a
,	const vec3 & b)
{
	return vec3(
		a[1]*b[2] - a[2]*b[1],
		a[2]*b[0] - a[0]*b[2],
		a[0]*b[1] - a[1]*b[0]);
}


const bool loadModelObjectFromObjFile(
	const char * filePath
,	ModelObject * model) 
{
    const char * s_sep = " \t\n\r";

    FILE * f = fopen(filePath, "rt");
    if (!f)
        return false;

    bool result = true;
    char buf[256];

    while (result && !feof(f)) 
	{
        strcpy(buf, "");
        fgets(buf, 256, f);

        char *token = strtok( buf, s_sep);
        if (token) 
		{
            switch (token[0]) 
			{
			case 'v': 
			{
				if (token[1] == 0 || token[1] == 'n' || token[1] == 't') 
				{
					char type = token[1];
					token = strtok(NULL, s_sep);
					
					int i = 0;
					int r = 3;
					
					if(type == 't') 
						r = 2;
        
					float v[3];
					
					while (token && i < r) 
					{
						v[i] = static_cast<float>(atof(token));
						token = strtok( NULL, s_sep);
						++i;
					}

					if (i != r || token) 
					{
						result = false;
						break;
					}

					switch(type)
					{
					case 0:
						model->m_vertices.push_back(vec3(v[0],v[1],v[2]));
						break;

					case 'n':
						model->m_normals.push_back(vec3(v[0],v[1],v[2]));
						break;

					default: break;
					}
				}
				break;
			} // case 'v':

			case 'f': 
			{
				face f;
				for (int i = 0; i < 3; ++i) 
				{
					token = strtok( NULL, s_sep);
					if (!token) 
					{
						result = false;
						break;
					}

					char * p = strchr(token, '/');
					char * q = strrchr(token, '/');
					if (!p || !q) 
					{
						result = false;
						break;
					}

					*p = 0;
					++q;

					f.v[i] = atoi(token) - 1;
					f.n[i] = atoi(q) - 1;

					if ((f.v[i] < 0) || (f.n[i] < 0)) 
					{
						result = false;
						break;
					}
				}
				if (result) 
					model->m_faces.push_back(f);

				break;
			} // case 'f':
			} // switch (token[0])
		}
	}

    fclose(f);


	model->m_aabb = computeAABB(model->m_vertices);

    return result;
}
