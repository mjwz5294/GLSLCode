/* Learn to use the AntTweakBar GUI For OpenGL
 * Step 1 : include head file
 * Step 2 : initialization
 * Step 3 : create a tweak bar and add variable
 * Step 4 : Draw your tweak bar
 * Step 5 : handle events, and window size change
 * reference: http://anttweakbar.sourceforge.net/doc/tools:anttweakbar:howto
 * Example: example/TwSimpleGLUT.c
*/

/* the program use AntTweakBar GUI to control the inner color
 * in the demo of GLSL4.0 cookbook.
*/
/// OpenGL Library
#include "gl/glew.h"
#include "gl/freeglut.h"

/// Mathematics Library for matrix manipulation
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

/// Shader Class 
#include "GLShader.h"

#include "Camera.h"

/// OpenGL Texture Library
#include "SOIL.h"
#pragma comment(lib, "soil")
// step 1: AntTweakBar GUI head file
#include "AntTweakBar.h"



using namespace glm;

// the innerColor variable
vec3 innerColor = vec3(125.0,10.0,125.0);//default black.
GLShader *shader;
GLint ps;
int width =320;
int height =320;

/// for camera
RenderSystem::Camera camera;
vec3 eye_position = vec3(125.0,10.0,125.0);
vec3 eye_direction = vec3(0.0,0.0,1.0);
vec3 eye_up = vec3(0.0,1.0,0.0);
int xOrigin = -1;
int yOrigin = -1;
float xAngle;
float yAngle;
GLuint quadVao;
int count =0;
GLuint eb;


unsigned char * LoadHeightMap(int &width, int &height)
{
	/* load an image as a heightmap, forcing greyscale (so channels should be 1) */
	int channels;
	unsigned char* ht_map = SOIL_load_image
		(
		"data/heightmap01.bmp",
		&width, &height, &channels,
		SOIL_LOAD_L
		);
	printf("heightmap\n");
	return ht_map;
}
vec3 get_eye_direction(vec3 eye_direction,float xAngle,float yAngle)
{
	yAngle = yAngle>0?-0.1:0.1;
	float x,y,z;
	float a,b,c,k,cosalpha, sinalpha;
	vec3 rotate_axis = glm::cross(eye_direction ,eye_up);
	rotate_axis = eye_up;
	rotate_axis = glm::normalize(rotate_axis);
	
	a = rotate_axis.x;
	b = rotate_axis.y;
	c = rotate_axis.z;
	cosalpha = cos(xAngle);
	sinalpha = sin(yAngle);
	k = 1 - cosalpha;

	x = (a*a*k+cosalpha)*eye_direction.x  
		+ (a*b*k-c*sinalpha)*eye_direction.y
		+ (a*c*k+b*sinalpha)*eye_direction.z;
	y = (a*b*k+c*sinalpha)*eye_direction.x
		+(b*b*k+cosalpha)*eye_direction.y
		+(b*c*k-a*sinalpha)*eye_direction.z;
	z = (a*c*k-b*sinalpha)*eye_direction.x
		+(b*c*k+a*sinalpha)*eye_direction.y
		+(c*c*k+cosalpha)*eye_direction.z;
	return vec3(x,y,z);
}
void init()
{
	camera.perspective(45.0f,(float)width/height,1.0f,1000.0f);
	camera.lookat(vec3(125.0,10.0,125.0),glm::vec3(0.0),glm::vec3(0.0,1.0,0.0));
	int width, height;
	shader = new GLShader("shader/HeightMap.vert","shader/HeightMap.frag");
	unsigned char * ht_mp = LoadHeightMap(width, height);
	vec3 * data = new vec3[width*height];
	for(int i=0;i<width;i++)
		for(int j=0;j<height;j++)
		{
			data[i*width+j].x = i;
			data[i*width+j].y = (float)ht_mp[i*width+j]/15.0;
			data[i*width+j].z = j;
		}
	count = width * height;
	int *index = new int[(width-1)*(height-1)*2*3];
	for(int i=0;i<width-1;i++)
		for(int j=0;j<height-1;j++){
			int k = i*(width-1)+j;
			index[6*k] = index[6*k+3] = i*width+j;
			index[6*k+1] = index[6*k+5] = (i+1)*width+j+1;
			index[6*k+2] = i*width+j+1;
			index[6*k+4] = (i+1)*width+j;
			
		}

	glGenVertexArrays(1,&quadVao);
	glBindVertexArray(quadVao);
	GLuint ab;
	glGenBuffers(1,&ab);
	glBindBuffer(GL_ARRAY_BUFFER,ab);
	glBufferData(GL_ARRAY_BUFFER,width*height*sizeof(vec3),data,GL_STATIC_DRAW);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);
	glEnableVertexAttribArray(0);

	count = (width-1)*(height-1)*2*3;
	glGenBuffers(1,&eb);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,eb);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,count*sizeof(int),index,GL_STATIC_DRAW);
	glBindVertexArray(0);
	
	free(data);
	free(index);
	ps = shader->getProgram();
	glClearColor(0.0,0.0,0.0,1.0);
	glEnable(GL_DEPTH_TEST);
	

}

void shape(int w, int h)
{
	if( w == 0)
		w =h;
	glViewport(0,0,w,h);
	width = w;
	height = h;

	/// step 5: handle events and window size changes
	TwWindowSize(w,h);
}
void keyboard(unsigned char key, int x, int y)
{
	switch(key){
	case 27:
		exit(0);
			break;
	case 'w':
		camera.moveForward(1.0);
		break;
	case 's':
		camera.moveForward(-1.0);
		break;
	case 'a':
		camera.rotate(1);
		break;
	case 'd':
		camera.rotate(-1);
		break;
	}
	/// step 5: handle events and window size changes
	TwEventKeyboardGLUT(key,x,y);
	glutPostRedisplay();
}
void mouseButton(int button, int state, int x, int y) {

	// only start motion if the left button is pressed
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			xOrigin = x;
			yOrigin = y;
	}else{
		xOrigin = -1;
		yOrigin = -1;
	}
	/// step 5: handle events and window size changes
	TwEventMouseButtonGLUT(button,state,x,y);
}
void mouseMove(int x, int y) {

	// this will only be true when the left button is down
	if (xOrigin >= 0) {

		xAngle = x - xOrigin;
		xOrigin = x;
		yAngle = y - yOrigin;
		yOrigin = y;
		eye_direction = get_eye_direction(eye_direction,xAngle,yAngle);
		eye_direction = glm::normalize(eye_direction);
	}
		/// step 5: handle events and window size changes
	TwEventMouseMotionGLUT(x,y);
	glutPostRedisplay();
}
void display()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
	glm::mat4 projection_matrix = camera.getProjectionMatrix();
	glm::mat4 view_matrix = camera.getViewMatrix();//,vec3(0.0,0.0,0.0),vec3(0.0,1.0,0.0));
	shader->begin();
	shader->setUniform("projection_matrix",projection_matrix);
	shader->setUniform("view_matrix",view_matrix);
	//glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);
	glBindVertexArray(quadVao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,eb);
	//glDrawArrays(GL_POINTS,0,count);
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, ((GLubyte *)NULL + (0)));

	shader->end();
	/// Step 4: Draw your tweak bar
	// It must be called just before the frame buffer is presented(swapped).
	TwDraw();
	glutSwapBuffers();

}
    
int main(int argc, char **argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH);

	glutInitWindowPosition(100,100);
	glutInitWindowSize(320,320);
	glutCreateWindow("OpenGL Shading Langue Demo");
	GLenum err = glewInit();
	if(0 != err)
	{
		printf("Cannot init Glew \n");
		exit(0);
	}
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(shape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove);
	//glutIdleFunc(display);
	/// step 2: initializtion
	// After OpenGL initializations and the creation of 
	// your graphic window, initialize AntTweakBar
	TwInit(TW_OPENGL,NULL);
	// or for core profile
	// TwInit(TW_OPENGL_CORE, NULL);

	/// step 3: Create a tweak bar and add variables
	// Create a tweak bar
	TwBar  *myBar;
	myBar = TwNewBar("NameOffMyTweakBar");

	// Add some variables
	TwAddVarRW(myBar,"width",TW_TYPE_INT32,&width,"");
	TwAddVarRW(myBar,"innerColor",TW_TYPE_COLOR3F,&innerColor, "");

	//glutIdleFunc(display);
	glutMainLoop();
	return 0;
}