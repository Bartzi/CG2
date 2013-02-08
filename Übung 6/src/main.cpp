

// INCLUDES

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>

#include "gpuquery.h"
#include "exercise6.h"
#include "fileassociatedshader.h"

#include <iostream>
#include <cmath>


// GLOBALS

int g_width  = 800;
int g_height = 450;

const glm::vec3 g_eye   (0.f, 0.f,-1.f);
const glm::vec3 g_center(0.f, 0.f, 0.f);
const glm::vec3 g_up    (0.f, 1.f, 0.f);


// TYPEDEFS

// EXERCISE 6 - BEGIN TASKS


// FORWARD DECLARATIONS


void printGLInfo();

void on_reshape(
	int width
,	int height);

void on_display();

void on_close();

void on_keyboard(
	unsigned char key
,	int x
,	int y);

void on_special(
	int key
,	int x
,	int y);

void on_mousemove(
	int x
,	int y);

void on_timer(int value);



// MAIN

Exercise6 * g_e6(nullptr);

int main(int argc, char** argv)
{
	// OpenGL and Glut Setup

	glutInit(&argc, argv);

    glutInitContextVersion(3, 1);
    //glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
    //glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(g_width, g_height);
    glutInitWindowPosition(0, 0);

    glutCreateWindow("Computer Graphics 2 - Exercise 6 v2");

	// Glut Setup

    glutCloseFunc(on_close);
    glutDisplayFunc(on_display);
    glutReshapeFunc(on_reshape);
    glutKeyboardFunc(on_keyboard);
	glutSpecialFunc(on_special);
	glutPassiveMotionFunc(on_mousemove);
    glutTimerFunc(1, on_timer, 1);

    // Glew Init

	glewInit();
    glError();

	// Info

	printGLInfo();

	// Scene Setup

    glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);

    // Initialze Exercises

    g_e6 = new Exercise6;
    g_e6->initialize();

	// Enter Main Loop

    glutMainLoop();

    return 0;
}

void printGLInfo()
{
    std::cout << "Vendor: " << GPUQuery::vendor() << std::endl;
    std::cout << "Renderer: " << GPUQuery::renderer() << std::endl << std::endl;

	std::cout << "Context Version: " 
		<< GPUQuery::queryi(GL_MAJOR_VERSION) << "."
		<< GPUQuery::queryi(GL_MINOR_VERSION) << std::endl << std::endl;
}

// ON GLUT

void on_reshape(int width, int height)
{
    // Notify & Reset framebuffer object

    g_width = width;
    g_height = height;

    if(g_e6)
        g_e6->resize(g_width, g_height);
}

void on_display()
{
	g_e6->draw();

    glutSwapBuffers();
    glError();
}

void on_close()
{
    delete g_e6;
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

    glutPostRedisplay();
    return;
}

void on_special(
	int key
,	int x
,	int y)
{
    switch (key)
    {
	case GLUT_KEY_F5:
		FileAssociatedShader::reloadAll();
		if(g_e6)
			g_e6->postShaderRelinked();
		break;

	default:
		break;
    }

    glutPostRedisplay();
    return;
}

void on_mousemove(
	int x
,	int y)
{
	if(g_e6)
		g_e6->mouseMove(x, y);
}

void on_timer(int value)
{
    // Post for redisplay and trigger callback again

    glutPostRedisplay();
    glutTimerFunc(1, on_timer, 1);

    return;
}
