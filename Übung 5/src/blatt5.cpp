//
// STL
//
#include <cassert>
#include <cstdio>
#include <cmath>
#include <vector>
#include <string>
#include <hash_map>

//
// GL
//
#include "GL/glew.h"
#include "GL/glut.h"

#include "utils.hpp"
#include "octree.hpp"

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

PlyVertex** g_vertices;
OctreeNode* g_octree;
float g_frustum[6][4];

///////////////////////////////////////////////////////////////////////////////
//
// Init state variables
//
int s_width  = 800;
int s_height = 600;

double angle = 0.0;
double zoom = 800.0;

bool zoom_out = false;

bool s_use_animation = false;
bool s_use_zooming = false;
bool s_renderDebug = false;
bool s_outputNumVertices = false;

vec3 llf_ = vec3();
vec3 urb_ = vec3();

void
on_reshape(int w, int h)
{
    s_width = w;
    s_height = h;

    //
    // Adapt projection and orientation transformation
    //
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 1.0*w/h, 0.01, 100);
    glMatrixMode(GL_MODELVIEW);

    return;
}


void
renderScene()
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // translate model to origin before rotation and scaling
    vec3 center = ((llf_+urb_) / 2);

    glTranslatef(center[0], center[1], center[2]);
    glScalef(1.0f - (zoom / 1000.0f), 1.0f - (zoom / 1000.0f), 1.0f - (zoom / 1000.0f));
    glRotatef(angle, 0.0f, 1.0f, 0.0f);
    glTranslatef(-center[0], -center[1], -center[2]);

    // get view frustum of current frame
    applyViewFrustum(g_frustum);

    // render 
    glPointSize(2.0f);
    glBegin(GL_POINTS);
       glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
       unsigned int numPoints = g_octree->renderPoints(g_frustum);
       if(s_outputNumVertices) {
           std::cout << "rendered points: " << numPoints << std::endl;
       }
    glEnd();

    // render AABBs
    if(s_renderDebug) {
        glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
        glBegin(GL_LINES);
            g_octree->renderDebugGeometry();
        glEnd();
    }

    glPopMatrix();
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
    gluPerspective(60, 1.0*s_width/s_height, 0.01, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(p[0], p[1], p[2], t[0], t[1], t[2], u[0], u[1], u[2]);

    return;
}


void
on_display(void)
{
    // clear main framebuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    setCameraTransform(g_camera);

    // render model
    renderScene();
    GET_GLERROR(0);

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
    case 'c':
        {
            s_outputNumVertices = !s_outputNumVertices;

            break;
        }
    case 'd':
        {
            s_renderDebug = !s_renderDebug;

            break;
        }
    case '+':
        {
            RENDERLEVEL = std::min(10u, RENDERLEVEL+1u);

            break;
        }
    case '-':
        {
            RENDERLEVEL = std::max(0, static_cast<int>(RENDERLEVEL)-1);

            break;
        }
    case ' ':
        {
            s_use_animation = !s_use_animation;
            printf("SWITCH ANIMATION\n");

            break;
        }
    case 'z':
        {
            s_use_zooming = !s_use_zooming;
            printf("SWITCH ZOOMING\n");

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

    if(s_use_zooming)
    {
        zoom_out ? zoom += value : zoom -= value;

        if(zoom == 1000.0) zoom_out = false;
        if(zoom == 1.0) zoom_out = true;

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
    std::vector<std::string> filesToLoad;

    filesToLoad.push_back("data/dragon.ply");
    g_vertices = loadPlyFiles(filesToLoad);

    // compute bbox
    float min_x = std::numeric_limits<float>::max();
    float min_y = std::numeric_limits<float>::max();
    float min_z = std::numeric_limits<float>::max();

    float max_x = std::numeric_limits<float>::min();
    float max_y = std::numeric_limits<float>::min();
    float max_z = std::numeric_limits<float>::min();

    for (int i=0; i < nverts; i++) {
        min_x = std::min(min_x, g_vertices[i]->x);
        min_y = std::min(min_y, g_vertices[i]->y);
        min_z = std::min(min_z, g_vertices[i]->z);

        max_x = std::max(max_x, g_vertices[i]->x);
        max_y = std::max(max_y, g_vertices[i]->y);
        max_z = std::max(max_z, g_vertices[i]->z);
    }

    llf_ = vec3(min_x, min_y, min_z);
    urb_ = vec3(max_x, max_y, max_z);

    // build octree
    g_octree = new OctreeNode(Bounds(llf_, urb_), 0);
    OctreeNode::vertexData_ = g_vertices;
    for (int i=0; i < nverts; i++) {
        g_octree->insertVertex(i, vec3(g_vertices[i]->x, g_vertices[i]->y, g_vertices[i]->z));
    }

    g_octree->computeMidpoints();

    return;
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE | GLUT_MULTISAMPLE);
    glutInitWindowSize(s_width, s_height);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Octree Demo");

    glewInit();

    //
    // Init Resources
    //
    loadModel();

    //
    // Scene Camera
    //
    g_camera.direction = (urb_ + llf_) / 2;
    g_camera.position  = g_camera.direction + (g_camera.direction.length() * vec3(1.0f, 1.0f, 1.0f) / 5);
    g_camera.up = vec3(0.0f, 1.0f, 0.0f);
    
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
