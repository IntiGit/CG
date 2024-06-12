#include <iostream>
#include <vector>
#include <sstream>

#include <GL/glew.h>
//#include <GL/gl.h> // OpenGL header not necessary, included by GLEW
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/string_cast.hpp>

#include "AbstractCube.h"
#include "CubeSharp.h"
#include "CubeSmooth.h"
#include "Sphere.h"

#define WINDOW_WIDTH  600
#define WINDOW_HEIGHT 600
// GLUT window id/handle
int glutID = 0;

//cg::AbstractCube* cubeLeft   = nullptr;
//cg::AbstractCube* cubeRight  = nullptr;

//cg::AbstractCube* manipulate = nullptr;

Sphere* sun = nullptr;
Sphere* planet = nullptr;
Sphere* moon = nullptr;
Sphere* moon_moon = nullptr;


Material red(
	glm::vec3(1.0f, 0.0f, 0.0f),	//Farbe
	glm::vec3(0.1f, 0.1f, 0.1f),	//Farbe Ambient
	glm::vec3(0.9f, 0.0f, 0.0f),	//Farbe Diffuse
	glm::vec3(1, 1, 1),				//Farbe Specular
	8.0f);							//Shininess

Material green(
	glm::vec3(1.0f, 0.0f, 0.0f),	//Farbe
	glm::vec3(0.1f, 0.1f, 0.1f),	//Farbe Ambient
	glm::vec3(0.1f, 0.9f, 0.0f),	//Farbe Diffuse
	glm::vec3(1, 1, 1),				//Farbe Specular
	8.0f);							//Shininess

Material blue(
	glm::vec3(1.0f, 0.0f, 0.0f),	//Farbe
	glm::vec3(0.1f, 0.1f, 0.1f),	//Farbe Ambient
	glm::vec3(0.0f, 0.1f, 0.9f),	//Farbe Diffuse
	glm::vec3(1, 1, 1),				//Farbe Specular
	8.0f);							//Shininess

Material violet(
	glm::vec3(1.0f, 0.0f, 0.0f),	//Farbe
	glm::vec3(0.1f, 0.1f, 0.1f),	//Farbe Ambient
	glm::vec3(0.9f, 0.0f, 0.8f),	//Farbe Diffuse
	glm::vec3(1, 1, 1),				//Farbe Specular
	8.0f);							//Shininess

glm::mat4x4 view;
glm::mat4x4 projection;

float zNear = 0.1f;
float zFar = 100.0f;

glm::vec3 eye(0.0f, 0.0f, 12.0f);
glm::vec3 center(0.0f, 0.0f, 0.0f);
glm::vec3 up(0.0f, 1.0f, 0.0f);

bool wireframe = GL_FALSE;
bool showNormals = GL_FALSE;
bool flatShading = GL_FALSE;

/*
Release resources on termination.
*/
void release()
{
	//delete cubeRight;
	//delete cubeLeft;
	delete sun;
	delete planet;
	delete moon;
	delete moon_moon;
}

unsigned  lightIndex = 0;
glm::vec4 lights[2] = {
	{ 0.0f, 1.0f, 0.0f, 0.0f }, //Richtungslicht
	{ eye.x,  eye.y, eye.z, 1.0f } //Punktlicht
};

void initSpheres() {
	sun = new Sphere(1.0f, red, flatShading);
	planet = new Sphere(0.55f, green, flatShading);
	moon = new Sphere(0.25f, blue, flatShading);
	moon_moon = new Sphere(0.15f, violet, flatShading);

	planet->translate({ 4.0f,0.0f,0.0f });
	moon->translate({ 5.7f,0.0f,0.0f });
	moon_moon->translate({ 6.35f,0.0f,0.0f });

	moon->rotateAround(-45.0f, { 0.0f,0.0f,1.0f }, planet->getSphereCenter());
	moon_moon->rotateAround(-45.0f, { 0.0f,0.0f,1.0f }, planet->getSphereCenter());

	planet->rotateAroundSelf(-45.0f, { 0.0f,0.0f,1.0f });
	moon->rotateAroundSelf(-45.0f, { 0.0f,0.0f,1.0f });
	moon_moon->rotateAroundSelf(-45.0f, { 0.0f,0.0f,1.0f });

	planet->setChild(moon);
	moon->setChild(moon_moon);
}
/*
 Initialization. Should return true if everything is ok and false if something went wrong.
 */
bool init()
{
	// OpenGL stuff. Set "background" color and enable depth testing.
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glEnable    (GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Construct view matrix.
	view = glm::lookAt(eye, center, up);

	// Create cube objects.
	try
	{
		//cubeLeft   = new cg::CubeSharp(glm::vec3(1.0f, 0.1f, 0.1f));
		//cubeRight  = new cg::CubeSmooth(glm::vec3(0.1f, 0.1f, 1.0f));

		//manipulate = cubeLeft;
		initSpheres();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return false;
	}

	// Manipulate models.
	//auto& modelLeft = cubeLeft->getModel();
	//modelLeft = glm::translate(modelLeft, glm::vec3(-2.5, 0.0f, 0.0f));

	//auto& modelRight = cubeRight->getModel();
	//modelRight = glm::translate(modelRight, glm::vec3(2.5, 0.0f, 0.0f));

	// Set light (only an example).
	//cubeLeft ->setLightVector(lights[lightIndex]);
	//cubeRight->setLightVector(lights[lightIndex]);
	sun->setLightVector(lights[lightIndex]);
	planet->setLightVector(lights[lightIndex]);
	moon->setLightVector(lights[lightIndex]);
	moon_moon->setLightVector(lights[lightIndex]);

	std::cout << "Use space to switch between cubes" << std::endl;
	std::cout << "Use x, y, z to rotate the currently selected cube" << std::endl;
	std::cout << "Use + and - to scale the currently selected cube" << std::endl;
	std::cout << "Use n to enable / disable normals for the currently selected cube" << std::endl;

	return true;
}

/*
 Rendering.
 */
void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//cubeLeft ->render(view, projection);
	//cubeRight->render(view, projection);
	planet->rotateAroundSelf(45.0f, { 0.0f,0.0f,1.0f });
	moon->rotateAroundSelf(45.0f, { 0.0f,0.0f,1.0f });
	moon_moon->rotateAroundSelf(45.0f, { 0.0f,0.0f,1.0f });

	moon_moon->rotateAround(1.4, { 1.0f,1.0f,0.0f }, moon->getSphereCenter());
	moon->rotateAround(0.8, { 1.0f,1.0f,0.0f }, planet->getSphereCenter());
	planet->rotateAround(0.1, { 0.0f,1.0f,0.0f }, sun->getSphereCenter());

	planet->rotateAroundSelf(-45.0f, { 0.0f,0.0f,1.0f });
	moon->rotateAroundSelf(-45.0f, { 0.0f,0.0f,1.0f });
	moon_moon->rotateAroundSelf(-45.0f, { 0.0f,0.0f,1.0f });

	sun->rotateAroundSelf(0.1f, { 0.0f,1.0f,0.0f });
	planet->rotateAroundSelf(0.3f, { 1.0f,1.0f,0.0f });

	sun->render(view, projection, wireframe, showNormals);
	planet->render(view, projection, wireframe, showNormals);
	moon->render(view, projection, wireframe, showNormals);
	moon_moon->render(view, projection, wireframe, showNormals);
}
/*
 Display callback.
 */
void glutDisplay ()
{
   render();
   glutSwapBuffers();
}

/*
 Resize callback.
 */
void glutResize (int width, int height)
{
	height = height < 1 ? 1 : height;
	glViewport(0, 0, width, height);

	// Construct projection matrix.
	projection = glm::perspective(45.0f, (float) width / height, zNear, zFar);
}

/*
 Callback for char input.
 */
void glutKeyboard (unsigned char keycode, int x, int y)
{
	//auto& model = sphere->getModel();

	switch (keycode)
	{
	case '+':
		eye.z -= 1.0f;
		lights[1].z -= 1.0f;
		sun->setLightVector(lights[lightIndex]);
		planet->setLightVector(lights[lightIndex]);
		moon->setLightVector(lights[lightIndex]);
		moon_moon->setLightVector(lights[lightIndex]);
		view = glm::lookAt(eye, center, up);
		break;
	case '-':
		eye.z += 1.0f;
		lights[1].z += 1.0f;
		sun->setLightVector(lights[lightIndex]);
		planet->setLightVector(lights[lightIndex]);
		moon->setLightVector(lights[lightIndex]);
		moon_moon->setLightVector(lights[lightIndex]);
		view = glm::lookAt(eye, center, up);
		break;
	case 'w':
		wireframe = !wireframe;
		break;
	case 'n':
		showNormals = !showNormals;
		break;
	case 's':
		flatShading = !flatShading;
		initSpheres();
		sun->setLightVector(lights[lightIndex]);
		planet->setLightVector(lights[lightIndex]);
		moon->setLightVector(lights[lightIndex]);
		moon_moon->setLightVector(lights[lightIndex]);
		break;
	case 'l':
		lightIndex = 1 - lightIndex;
		sun->setLightVector(lights[lightIndex]);
		planet->setLightVector(lights[lightIndex]);
		moon->setLightVector(lights[lightIndex]);
		moon_moon->setLightVector(lights[lightIndex]);
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
    // GLUT: Initialize freeglut library (window toolkit).
    glutInitWindowSize    (WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(40,40);
    glutInit(&argc, argv);

    // GLUT: Create a window and opengl context (version 4.3 core profile).
    glutInitContextVersion(4, 3);
    glutInitContextFlags  (GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
    glutInitDisplayMode   (GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

    glutCreateWindow("Planetensystem");
    glutID = glutGetWindow();
	  
    // GLEW: Load opengl extensions
    glewExperimental = GL_TRUE;
    GLenum result = glewInit();

    if (result != GLEW_OK) {
       return -1;
    }

    // GLUT: Set callbacks for events.
    glutReshapeFunc(glutResize);
    glutDisplayFunc(glutDisplay);
    glutIdleFunc   (glutDisplay); // redisplay when idle

    glutKeyboardFunc(glutKeyboard);

    // init vertex-array-objects.
    {
       bool result = init();
       if (!result) {
	 release();
	 return -2;
       }
    }

    // GLUT: Loop until the user closes the window
    // rendering & event handling
    glutMainLoop ();

    // Cleanup everything on termination.
    release();
	
    return 0;
}
