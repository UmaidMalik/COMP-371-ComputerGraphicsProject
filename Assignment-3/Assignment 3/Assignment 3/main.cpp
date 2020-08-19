/*
*	COMP 371 - Assignment 2
*
*	Umaid Malik 27576110
*	Noor Alali
*	Vishal Senewiratne
*	Dominic Hart
*
*	CONTROLS:

*		VIEW SELECTION CONTROLS
*		==============================================================
*
*       ESCAPE              : QUIT PROGRAM
*
*		==============================================================
*		X					: TOGGLE TEXTURE ON/OFF
*
*		Q					: PERSPECTIVE PROJECTION
*
*		E					: ORTHOGRAPHIC PROJECTION
*
*		T					: RENDER TRIANGLES
*
*		L					: RENDER LINES
*
*		P					: RENDER POINTS
*
*		C					: HOLD TO DISABLE BACKFACE CULLING
*
*		HOME				: RETURN TO INITIAL POSTION
*
*		MOUSE SCROLL UP		: ZOOM IN
*
*		MOUSE SCROLL DOWN	: ZOOM OUT
*
*
*		PAN AND TILT (MOUSE MOVEMENT) WHILE HOLDING RIGHT MOUSE BUTTON
*		==============================================================
*
*		W		: MOVE FORWARD
*
*		S		: MOVE BACKWARD
*
*		A		: MOVE LEFT
*
*		D		: MOVE RIGHT
*
*		X		: MOVE NEGATIVE Y-DIRECTION
*
*		SPACE	: MOVE POSITIVE Y-DIRECTION
*
*		SHIFT	: INCREASE MOVEMENT SPEED
*
*		==============================================================
*
*
*		MODEL CONTROL
*		ENTER EITHER 1, 2, 3, 4 OR 5 TO SELECT MODELS - 0 WILL SELECT ALL MODELS - LEFT MOUSE CLICK WILL DESELECT ALL MODELS
*		====================================================================================================================
*
*		TAB						: TOGGLE BETWEEN INCREMENTAL OR CONTINUOUS MOVEMENT
*
*		W						: MOVE MODEL IN +Z DIRECTION
*
*		S						: MOVE MODEL IN -Z DIRECTION
*
*		A						: MOVE MODEL IN -X DIRECTION
*
*		D						: MOVE MODEL IN +X DIRECTION
*
*		Z						: MOVE MODEL IN -Y DIRECTION
*
*		SPACE					: MOVE MODEL IN +Y DIRECTION
*
*		R						: RESET MODEL TO INITIAL POSITION
*
*		SHIFT + R				: RESET MODEL TO INITIAL SIZE
*
*		LCTRL + SHIFT + R		: RESET MODEL ORIENTATION
*
*		U						: UPSCALE MODEL
*
*		J						: DOWNSCALE MODEL
*
*		T						: ROTATE MODEL +X DIRECTION
*
*		Y						: ROTATE MODEL -X DIRECTION
*
*		G						: ROTATE MODEL +Y DIRECTION
*
*		H						: ROTATE MODEL -Y DIRECTION
*
*		B						: ROTATE MODEL +Z DIRECTION
*
*		N						: ROTATE MODEL -Z DIRECTION
*
*		SHIFT + W/A/S/D/X/SPACE	: INCREASE MOVEMENT SPEED
*
*		SHIFT + U/J				: INCREASE SCALING SPEED
*
*		ALT						: RANDOMIZE THE POSITION OF THE MODEL WITHIN THE GRID AREA
*
*		ARROW UP				: +Y ORIENTATION
*
*       ARROW DOWN				: -Y ORIENTATION
*
*		ARROW LEFT				: -X ORIENTATION
*
*		ARROW RIGHT				: +X ORIENTATION
*/


#include <iostream>
#include <algorithm>

#define GLEW_STATIC 1 

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

// include class for the assets
#include "shader.h"
#include "texture.h"
#include "camera.h"

using namespace glm;

const int numbObjInScene = 5;                           // make sure to update this if you add more models!!!

const int numGridLines = 100;                           // how many gridlines (going one way)

glm::vec3 initialCameraPosition(0.6f, 1.0f, 1.5f);
glm::vec3 initialcameraLookAt(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

glm::vec3 center(0.0f, 0.0f, 0.0f);


glm::vec3 cameraPosition = initialCameraPosition;
glm::vec3 cameraLookAt = initialcameraLookAt;

glm::mat4 identityMatrix = glm::mat4(1.0f);

glm::mat4 projectionMatrix = identityMatrix;
glm::mat4 viewMatrix = identityMatrix;
glm::mat4 worldMatrix = identityMatrix;

glm::mat4 rotationMatrix = identityMatrix;
glm::mat4 scalingMatrix = identityMatrix;
glm::mat4 shearingMatrix = identityMatrix;
glm::mat4 translationMatrix = identityMatrix;
glm::mat4 objectTranslationtionMatrix;
glm::mat4 partMatrix;

glm::mat4 modelTranslationMatrix;
glm::mat4 modelScalingMatrix;
glm::mat4 modelShearingMatrix;
glm::mat4 modelRotationMatrix;

// matrix for world oriention
glm::mat4 worldOrientationMatrix = identityMatrix;
glm::mat4 worldOrientation_X;
glm::mat4 worldOrientation_Y;
//

// used for rotation angle and rotation snap for world orientation set to 15 degrees
glm::vec3 worldRotation;
const float worldOrientation_ANGLE = 15.0f;

float cameraHorizontalAngle = 90.0f;
float cameraVerticalAngle = 0.0f;
float cameraSpeed = 1.0f;
float cameraSpeedFast = 4 * cameraSpeed;
float deltaTime;
float fov = 90.0f;

double lastMousePosX, lastMousePosY;

// model parameters
float modelSpeed = 1.0f;
float shearingScale = 1.0f; // while the model is moving, it is sheared. When moving the model with (shift + movem

bool ONE_KEY_PRESSED = false;
bool TWO_KEY_PRESSED = false;
bool THREE_KEY_PRESSED = false;
bool FOUR_KEY_PRESSED = false;
bool FIVE_KEY_PRESSED = false;

bool UP_KEY = false;
bool DOWN_KEY = false;
bool RIGHT_KEY = false;
bool LEFT_KEY = false;

bool T_KEY = GLFW_RELEASE;
bool Y_KEY = GLFW_RELEASE;
bool G_KEY = GLFW_RELEASE;
bool H_KEY = GLFW_RELEASE;
bool B_KEY = GLFW_RELEASE;
bool N_KEY = GLFW_RELEASE;
bool X_KEY = GLFW_RELEASE;      // toggle textures

bool TAB_KEY = GLFW_RELEASE;
bool W_KEY = GLFW_RELEASE;
bool A_KEY = GLFW_RELEASE;
bool S_KEY = GLFW_RELEASE;
bool D_KEY = GLFW_RELEASE;
bool SPACE_KEY = GLFW_RELEASE;
bool Z_KEY = GLFW_RELEASE;
bool U_KEY = GLFW_RELEASE;
bool J_KEY = GLFW_RELEASE;

bool textures_on = true;        // toggles textures on/off; textures are on initially

bool ALT_KEY = GLFW_RELEASE; // key used to randomize the position of the model

float gridUnit = 0.1f; // used for incremental model translation



struct TexturedColoredVertex
{
	TexturedColoredVertex(vec3 _position, /*vec3 _color,*/ vec2 _uv, vec3 _vertexNormal) : position(_position), /*color(_color),*/ uv(_uv), vertexNormal(_vertexNormal) {}

	vec3 position;
	//vec3 color;
	vec2 uv;
	vec3 vertexNormal;
};

// function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, Shader shaderProgram);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);



// models
int createTexturedCubeVertexArrayObject(TexturedColoredVertex* texturedCubeVertexArray, GLuint size);
void model_ground(float groundSize, Shader shaderProgram, Texture texture, bool isTextureOn);
void model_A7(Shader shaderProgram, Texture texture_1, Texture texture_2, bool isTextureOn);
void model_O9(Shader shaderProgram, Texture texture_1, Texture texture_2, bool isTextureOn);
void model_S0(Shader shaderProgram, Texture texture_1, Texture texture_2, bool isTextureOn);
void model_M6(Shader shaderProgram, Texture texture_1, Texture texture_2, bool isTextureOn);
void model_N7(Shader shaderProgram, Texture texture_1, Texture texture_2, bool isTextureOn);

int createGridlines(int numGridlines, vec3* gridLinesVertices, GLuint size, vec3 lineColor, float sizeOfGrid);
void drawGridlines(Shader shaderProgram);
int createLine(vec3* lineVertices);
void drawAxisLines(Shader shaderProgram);

// control function for the models 
void modelControl(GLFWwindow* window, Shader shaderProgram, const float& initSize, float& modelSize, glm::vec3& initPos,
	glm::vec3& modelPos, glm::vec3& rotation, glm::mat4& rotX, glm::mat4& rotY, glm::mat4& rotZ, const float ANGLE, bool KEY_PRESSED, glm::vec3& color);

GLFWwindow* setupWindow();

// end of function protoypes

GLFWwindow* setupWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TURE);
	glfwWindowHint(GL_DEPTH_TEST, 24);
	
	GLFWwindow* window = glfwCreateWindow(1024, 768, "COMP 371 - Team 6 - Assignment 3", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	return window;
}

GLuint VAO[numbObjInScene];	// vertexArrayObject
GLuint VBO[numbObjInScene];	// vertexBufferObject



int main()
{
	// Setup a window
	GLFWwindow* window = setupWindow();

	// Compile and link shaders
	Shader texturedShaderProgram("../assets/shaders/textured_vertex.glsl", "../assets/shaders/textured_fragment.glsl");
	Shader shadowShaderProgram("../assets/shaders/shadow_vertex.glsl", "../assets/shaders/shadow_fragment.glsl");


	// Load Textures
#if defined(PLATFORM_OSX)
	Texture groundTexture("../assets/textures/tiles.bmp");
	Texture metalTexture("../assets/textures/metal.jpg");
	Texture boxTexture("../assets/textures/box.jpg");
#else 
	Texture groundTexture("../assets/textures/tiles.bmp");
	Texture metalTexture("../assets/textures/metal.jpg");
	Texture boxTexture("../assets/textures/box.jpg");

	// face 1
	Texture knight_1("../assets/textures/face_1/hollow_knight_art_1.png");
	Texture knight_2("../assets/textures/face_1/hollow_knight_art_2.png");
	Texture knight_3("../assets/textures/face_1/hollow_knight_art_3.png");
	Texture knight_4("../assets/textures/face_1/hollow_knight_art_4.png");
	Texture knight_5("../assets/textures/face_1/hollow_knight_art_5.png");
	Texture knight_6("../assets/textures/face_1/hollow_knight_art_6.png");
	Texture knight_7("../assets/textures/face_1/hollow_knight_art_7.png");
	Texture knight_8("../assets/textures/face_1/hollow_knight_art_8.png");
	Texture knight_9("../assets/textures/face_1/hollow_knight_art_9.png");

	// face 2
	Texture color_1("../assets/textures/face_2/color_1.png");
	Texture color_2("../assets/textures/face_2/color_2.png");
	Texture color_3("../assets/textures/face_2/color_3.png");
	Texture color_4("../assets/textures/face_2/color_4.png");
	Texture color_5("../assets/textures/face_2/color_5.png");
	Texture color_6("../assets/textures/face_2/color_6.png");
	Texture color_7("../assets/textures/face_2/color_7.png");
	Texture color_8("../assets/textures/face_2/color_8.png");
	Texture color_9("../assets/textures/face_2/color_9.png");

	// face 3
	Texture shape_A_1("../assets/textures/face_3/shape_A_1.png");
	Texture shape_A_2("../assets/textures/face_3/shape_A_2.png");
	Texture shape_A_3("../assets/textures/face_3/shape_A_3.png");
	Texture shape_A_4("../assets/textures/face_3/shape_A_4.png");
	Texture shape_A_5("../assets/textures/face_3/shape_A_5.png");
	Texture shape_A_6("../assets/textures/face_3/shape_A_6.png");
	Texture shape_A_7("../assets/textures/face_3/shape_A_7.png");
	Texture shape_A_8("../assets/textures/face_3/shape_A_8.png");
	Texture shape_A_9("../assets/textures/face_3/shape_A_9.png");

	// face 4
	Texture shape_B_1("../assets/textures/face_4/shape_B_1.png");
	Texture shape_B_2("../assets/textures/face_4/shape_B_2.png");
	Texture shape_B_3("../assets/textures/face_4/shape_B_3.png");
	Texture shape_B_4("../assets/textures/face_4/shape_B_4.png");
	Texture shape_B_5("../assets/textures/face_4/shape_B_5.png");
	Texture shape_B_6("../assets/textures/face_4/shape_B_6.png");
	Texture shape_B_7("../assets/textures/face_4/shape_B_7.png");
	Texture shape_B_8("../assets/textures/face_4/shape_B_8.png");
	Texture shape_B_9("../assets/textures/face_4/shape_B_9.png");

	// face 5
	Texture shape_C_1("../assets/textures/face_5/shape_C_1.jpg");
	Texture shape_C_2("../assets/textures/face_5/shape_C_2.jpg");
	Texture shape_C_3("../assets/textures/face_5/shape_C_3.jpg");
	Texture shape_C_4("../assets/textures/face_5/shape_C_4.jpg");
	Texture shape_C_5("../assets/textures/face_5/shape_C_5.jpg");
	Texture shape_C_6("../assets/textures/face_5/shape_C_6.jpg");
	Texture shape_C_7("../assets/textures/face_5/shape_C_7.jpg");
	Texture shape_C_8("../assets/textures/face_5/shape_C_8.jpg");
	Texture shape_C_9("../assets/textures/face_5/shape_C_9.jpg");

	// face 6
	Texture science_1("../assets/textures/face_6/science_1.jpg");
	Texture science_2("../assets/textures/face_6/science_2.jpg");
	Texture science_3("../assets/textures/face_6/science_3.jpg");
	Texture science_4("../assets/textures/face_6/science_4.jpg");
	Texture science_5("../assets/textures/face_6/science_5.jpg");
	Texture science_6("../assets/textures/face_6/science_6.jpg");
	Texture science_7("../assets/textures/face_6/science_7.jpg");
	Texture science_8("../assets/textures/face_6/science_8.jpg");
	Texture science_9("../assets/textures/face_6/science_9.jpg");

#endif

	
	// defines vertices and color for the cube 
	TexturedColoredVertex texturedCubeVertexArray[] = {

		// cube made of 12 triangles

							  // postion				 // Texture UV           // normal
		TexturedColoredVertex(vec3(0.0f, 0.0f, 0.0f),    vec2(0.0f, 0.0f),    vec3(0.0f, 0.0f, -1.0f)),
		TexturedColoredVertex(vec3(0.1f, 0.0f, 0.0f),    vec2(-1.0f, 0.0f),    vec3(0.0f, 0.0f, -1.0f)),
		TexturedColoredVertex(vec3(0.1f, 0.1f, 0.0f),    vec2(-1.0f, -1.0f),    vec3(0.0f, 0.0f, -1.0f)),

		TexturedColoredVertex(vec3(0.0f, 0.0f, 0.0f),    vec2(0.0f, 0.0f),    vec3(0.0f, 0.0f, -1.0f)),
		TexturedColoredVertex(vec3(0.1f, 0.1f, 0.0f),    vec2(-1.0f, -1.0f),    vec3(0.0f, 0.0f, -1.0f)),
		TexturedColoredVertex(vec3(0.0f, 0.1f, 0.0f),    vec2(0.0f, -1.0f),    vec3(0.0f, 0.0f, -1.0f)),

		TexturedColoredVertex(vec3(0.1f, 0.0f, 0.0f),    vec2(1.0f, 0.0f),    vec3(1.0f, 0.0f, 0.0f)),
		TexturedColoredVertex(vec3(0.1f, 0.0f, 0.1f),    vec2(0.0f, 0.0f),    vec3(1.0f, 0.0f, 0.0f)),
		TexturedColoredVertex(vec3(0.1f, 0.1f, 0.0f),    vec2(1.0f, -1.0f),    vec3(1.0f, 0.0f, 0.0f)),

		TexturedColoredVertex(vec3(0.1f, 0.1f, 0.0f),    vec2(0.0f, 0.0f),    vec3(1.0f, 0.0f, 0.0f)),
		TexturedColoredVertex(vec3(0.1f, 0.0f, 0.1f),    vec2(-1.0f, 1.0f),  vec3(1.0f, 0.0f, 0.0f)),
		TexturedColoredVertex(vec3(0.1f, 0.1f, 0.1f),    vec2(-1.0f, 0.0f),   vec3(1.0f, 0.0f, 0.0f)),

		TexturedColoredVertex(vec3(0.0f, 0.0f, 0.1f),    vec2(0.0f, 0.0f),    vec3(0.0f, 0.0f, 1.0f)),
		TexturedColoredVertex(vec3(0.1f, 0.1f, 0.1f),    vec2(1.0f, -1.0f),    vec3(0.0f, 0.0f, 1.0f)),
		TexturedColoredVertex(vec3(0.1f, 0.0f, 0.1f),    vec2(1.0f, 0.0f),    vec3(0.0f, 0.0f, 1.0f)),

		TexturedColoredVertex(vec3(0.0f, 0.0f, 0.1f),    vec2(0.0f, 0.0f),  vec3(0.0f, 0.0f, 1.0f)),
		TexturedColoredVertex(vec3(0.0f, 0.1f, 0.1f),    vec2(0.0f, -1.0f),   vec3(0.0f, 0.0f, 1.0f)),
		TexturedColoredVertex(vec3(0.1f, 0.1f, 0.1f),    vec2(1.0f, -1.0f),    vec3(0.0f, 0.0f, 1.0f)),

		TexturedColoredVertex(vec3(0.0f, 0.0f, 0.0f),    vec2(0.0f, 1.0f),   vec3(-1.0f, 0.0f, 0.0f)),
		TexturedColoredVertex(vec3(0.0f, 0.1f, 0.0f),    vec2(0.0f, 0.0f),    vec3(-1.0f, 0.0f, 0.0f)),
		TexturedColoredVertex(vec3(0.0f, 0.0f, 0.1f),    vec2(1.0f, 1.0f),  vec3(-1.0f, 0.0f, 0.0f)),

		TexturedColoredVertex(vec3(0.0f, 0.1f, 0.0f),    vec2(0.0f, 0.0f),    vec3(-1.0f, 0.0f, 0.0f)),
		TexturedColoredVertex(vec3(0.0f, 0.1f, 0.1f),    vec2(1.0f, 0.0f),   vec3(-1.0f, 0.0f, 0.0f)),
		TexturedColoredVertex(vec3(0.0f, 0.0f, 0.1f),    vec2(1.0f, 1.0f),  vec3(-1.0f, 0.0f, 0.0f)),

		TexturedColoredVertex(vec3(0.0f, 0.0f, 0.0f),    vec2(-1.0f, 1.0f),    vec3(0.0f, -1.0f, 0.0f)),
		TexturedColoredVertex(vec3(0.1f, 0.0f, 0.1f),    vec2(0.0f, 0.0f),    vec3(0.0f, -1.0f, 0.0f)),
		TexturedColoredVertex(vec3(0.1f, 0.0f, 0.0f),    vec2(0.0f, 1.0f),    vec3(0.0f, -1.0f, 0.0f)),

		TexturedColoredVertex(vec3(0.0f, 0.0f, 0.0f),    vec2(0.0f, 0.0f),    vec3(0.0f, -1.0f, 0.0f)),
		TexturedColoredVertex(vec3(0.0f, 0.0f, 0.1f),    vec2(0.0f, -1.0f),   vec3(0.0f, -1.0f, 0.0f)),
		TexturedColoredVertex(vec3(0.1f, 0.0f, 0.1f),    vec2(1.0f, -1.0f),  vec3(0.0f, -1.0f, 0.0f)),

		TexturedColoredVertex(vec3(0.0f, 0.1f, 0.0f),    vec2(-1.0f, -1.0f),    vec3(0.0f, 1.0f, 0.0f)),
		TexturedColoredVertex(vec3(0.1f, 0.1f, 0.0f),    vec2(0.0f, -1.0f),    vec3(0.0f, 1.0f, 0.0f)),
		TexturedColoredVertex(vec3(0.1f, 0.1f, 0.1f),    vec2(0.0f, 0.0f),    vec3(0.0f, 1.0f, 0.0f)),

		TexturedColoredVertex(vec3(0.0f, 0.1f, 0.0f),    vec2(-1.0f, -1.0f),    vec3(0.0f, 1.0f, 0.0f)),
		TexturedColoredVertex(vec3(0.1f, 0.1f, 0.1f),    vec2(0.0f, 0.0f),    vec3(0.0f, 1.0f, 0.0f)),
		TexturedColoredVertex(vec3(0.0f, 0.1f, 0.1f),    vec2(-1.0f, 0.0f),    vec3(0.0f, 1.0f, 0.0f))
	};
	
	
	// Setup texture and framebuffer for creating shadow map

	// Variable for strong index to framebuffer used for shadow mapping
	GLuint depthMapFBO;
	// Get the framebuffer
	glGenFramebuffers(1, &depthMapFBO);
	// Dimensions of the shadow texture
	const unsigned SHADOW_WIDTH = 24, SHADOW_HEIGHT = 24;
	// Variable storing index to texture used for shadow mapping
	GLuint depthMap;
	// Get the texture
	glGenTextures(1, &depthMap);
	// Bind the texture 
	glBindTexture(GL_TEXTURE_2D, depthMap);
	// Create the texture and specify its attributes
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	// Set texture sample parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Bdin the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	// Attach the depth map textures to the depth map framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	// set texture unit # for shadow map
	//setShadowMapTexture(lightShaderProgram.getShaderProgramID(), 0);
	glDrawBuffer(GL_NONE);

	const float unitLength = 10.0f / (float)numGridLines;   // henceforth considered a unit of length
	const float axisLineLength = 5 * unitLength;            // axis lines are the length of n grid squares

	// red axis line
	glm::vec3 redLine[] = {
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(axisLineLength, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
	};

	// green axis line
	glm::vec3 greenLine[] = {
		glm::vec3(0.0f,  0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, axisLineLength, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
	};

	// blue axis line
	glm::vec3 blueLine[] = {
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, axisLineLength), glm::vec3(0.0f, 0.0f, 1.0f),
	};
	

	glGenVertexArrays(numbObjInScene, VAO);
	glGenBuffers(numbObjInScene, VBO);


	// cube
	VAO[0] = createTexturedCubeVertexArrayObject(texturedCubeVertexArray, sizeof(texturedCubeVertexArray));

	// axis lines
	VAO[1] = createLine(redLine);
	VAO[2] = createLine(greenLine);
	VAO[3] = createLine(blueLine);

	// grid lines
	glm::vec3 gridLines[8 * numGridLines];
	VAO[4] = createGridlines(numGridLines, gridLines, sizeof(gridLines), vec3(1.0f, 1.0f, 0.0f), 10.0f);

	

	// bind to nothing so we don't inadvertantly modify something
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// set global matrices for each shader
	projectionMatrix = glm::perspective(90.0f, 1024.0f / 768.0f, 0.0005f, 500.0f);
	texturedShaderProgram.useProgram();
	texturedShaderProgram.setMat4("projectionMatrix", projectionMatrix);
	shadowShaderProgram.useProgram();
	shadowShaderProgram.setMat4("projectionMatrix", projectionMatrix);


	viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
	texturedShaderProgram.useProgram();
	texturedShaderProgram.setMat4("viewMatrix", viewMatrix);
	shadowShaderProgram.useProgram();
	shadowShaderProgram.setMat4("viewMatrix", viewMatrix);


	float lightAngleOuter = cos(radians(67.5));
	float lightAngleInner = cos(radians(62.5));
	// Set light cutoff angles
	texturedShaderProgram.useProgram();
	texturedShaderProgram.setInt("light_cutoff_inner", lightAngleInner);
	
	texturedShaderProgram.useProgram();
	texturedShaderProgram.setInt("light_cutoff_outer", lightAngleOuter);
	

	// Set light color 
	texturedShaderProgram.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

	// Set object color
	texturedShaderProgram.setVec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));

	// Variables to be used in rotation
	float angle = 0.0f;
	float rotationSpeed = 90.0f;  // degrees per second
	float lastFrameTime = glfwGetTime();

	// read mouse position into variables
	glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);

	// Enable Backface culling
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);     // clockwise polygons are considered as front-facing

	// z-Buffer
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	


	// initial model parameters

	// model A7											// model O9											// model S0											
	const float init_A7_Size = 1.0f;					const float init_O9_Size = 1.0f;					const float init_S0_Size = 1.0f;
	float model_A7_Size = 0.0f;							float model_O9_Size = 0.0f;							float model_S0_Size = 0.0f;
	glm::vec3 initPos_A7(-4.0f, 0.5f, -4.0f);			glm::vec3 initPos_O9(4.0f, 0.5f, -4.0f);			glm::vec3 initPos_S0(-4.0f, 0.5f, 4.0f);
	glm::vec3 model_A7_Position(0.0f, 0.0f, 0.0f);		glm::vec3 model_O9_Position(0.0f, 0.0f, 0.0f);		glm::vec3 model_S0_Position(0.0f, 0.0f, 0.0f);
	glm::vec3 A7_theta(0.0f, 0.0f, 0.0f);				glm::vec3 O9_theta(0.0f, 0.0f, 0.0f);				glm::vec3 S0_theta(0.0f, 0.0f, 0.0f);
	glm::mat4 A7_rotation_X;                			glm::mat4 O9_rotation_X;							glm::mat4 S0_rotation_X;
	glm::mat4 A7_rotation_Y;                 			glm::mat4 O9_rotation_Y;							glm::mat4 S0_rotation_Y;
	glm::mat4 A7_rotation_Z;                    		glm::mat4 O9_rotation_Z;							glm::mat4 S0_rotation_Z;
	glm::vec3 A7_color(1.0, 1.0f, 0.0f);				glm::vec3 O9_color(1.0, 0.6f, 0.9f);				glm::vec3 S0_color(0.0, 0.0f, 0.5f);

	// model M6                                         // model N7										    // model ground									
	const float init_M6_Size = 1.0f;                    const float init_N7_Size = 1.0f;                    const float init_ground_Size = 1.0f;
	float model_M6_Size = 0.0f;                         float model_N7_Size = 0.0f;                         float model_ground_Size = 1.0f;
	glm::vec3 initPos_M6(4.0f, 0.5f, 4.0f);             glm::vec3 initPos_N7(0.0f, 0.3f, -4.0f);             glm::vec3 initPos_ground(0.0f, 0.0f, 0.0f);
	glm::vec3 model_M6_Position(0.0f, 0.0f, 0.0f);      glm::vec3 model_N7_Position(0.0f, 0.0f, 0.0f);      glm::vec3 model_ground_Position(0.0f, 0.0f, 0.0f);
	glm::vec3 M6_theta(0.0f, 0.0f, 0.0f);               glm::vec3 N7_theta(0.0f, 0.0f, 0.0f);               glm::vec3 ground_theta(0.0f, 0.0f, 0.0f);
	glm::mat4 M6_rotation_X;                            glm::mat4 N7_rotation_X;                            glm::mat4 ground_rotation_X;
	glm::mat4 M6_rotation_Y;                            glm::mat4 N7_rotation_Y;                            glm::mat4 ground_rotation_Y;
	glm::mat4 M6_rotation_Z;                            glm::mat4 N7_rotation_Z;                            glm::mat4 ground_rotation_Z;
	glm::vec3 M6_color(1.0, 0.0f, 0.0f);				glm::vec3 N7_color(0.0, 1.0f, 1.0f);				glm::vec3 ground_color(0.5f, 1.0f, 0.5f);

	const float ANGLE = 5.0f; // set rotation snap to 5 degrees

	

	// render loop
	while (!glfwWindowShouldClose(window))
	{
	
		

		deltaTime = glfwGetTime() - lastFrameTime;
		lastFrameTime += deltaTime;

		// light parameters
		vec3 lightPosition = vec3(-0.1f, 3.0f, -0.1f);
		vec3 lightFocus(0.1f, 0.0f, 0.1f);      // the point in 3D space the light "looks" at
		vec3 lightDirection = normalize(lightFocus - lightPosition);

		float lightNearPlane = 0.1f;
		float lightFarPlane = 10.0f;

		texturedShaderProgram.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
		texturedShaderProgram.setVec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));
		
		mat4 lightProjectionMatrix = perspective(90.0f, (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, lightNearPlane, lightFarPlane);
		mat4 lightViewMatrix = lookAt(lightPosition, lightFocus, vec3(0.0f, 1.0f, 0.0f));
		mat4 lightSpaceMatrix = lightProjectionMatrix * lightViewMatrix;

		// Set light space matrix on both shaders
		shadowShaderProgram.setMat4("light_view_proj_matrix", lightSpaceMatrix);
		texturedShaderProgram.setMat4("light_view_proj_matrix", lightSpaceMatrix);

		// Set light far and near planes on scene shader
		texturedShaderProgram.setInt("light_near_plane", lightNearPlane);
		texturedShaderProgram.setInt("light_far_plane", lightFarPlane);

		// Set light position on scene shader
		texturedShaderProgram.setVec3("lightPos", lightPosition);

		// Set light direction on scene shader
		texturedShaderProgram.setVec3("lightDir", lightDirection);



		texturedShaderProgram.setMat4("worldMatrix", worldMatrix);
		shadowShaderProgram.setMat4("worldMatrix", worldMatrix);
		
		texturedShaderProgram.setMat4("viewMatrix", viewMatrix);
	

		texturedShaderProgram.setVec3("viewPos", cameraPosition);


		// use texturedShaderProgram for the textured models
		if (textures_on) {
			texturedShaderProgram.useProgram();
			glActiveTexture(GL_TEXTURE0); // this is the GL "reset" texture, use to flush previous texture
		}
		else {
			texturedShaderProgram.useProgram();
		}
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 0.2f, 0.4f, 1.0f); // Midnight blue
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);

		glPointSize(10);		// increased point size so that it is easily visible
		glLineWidth(2);
		glBindVertexArray(VAO[0]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		
		texturedShaderProgram.setFloat("ambientStrength", 0.3f);
		modelControl(window, texturedShaderProgram, init_ground_Size, model_ground_Size, initPos_ground, model_ground_Position, ground_theta, ground_rotation_X, ground_rotation_Y, ground_rotation_Z, ANGLE, false, ground_color);
		model_ground(10.0, texturedShaderProgram, groundTexture, textures_on);
			
		// beginning of model A7
		modelControl(window, texturedShaderProgram, init_A7_Size, model_A7_Size, initPos_A7, model_A7_Position, A7_theta, A7_rotation_X, A7_rotation_Y, A7_rotation_Z, ANGLE, ONE_KEY_PRESSED, A7_color);
		model_A7(texturedShaderProgram, boxTexture, metalTexture, textures_on);
		// end of model_A7();
		// beginning of model O9
		modelControl(window, texturedShaderProgram, init_O9_Size, model_O9_Size, initPos_O9, model_O9_Position, O9_theta, O9_rotation_X, O9_rotation_Y, O9_rotation_Z, ANGLE, TWO_KEY_PRESSED, O9_color);
		model_O9(texturedShaderProgram, boxTexture, metalTexture, textures_on);
		// end of model O9
		// beginning of model S0
		modelControl(window, texturedShaderProgram, init_S0_Size, model_S0_Size, initPos_S0, model_S0_Position, S0_theta, S0_rotation_X, S0_rotation_Y, S0_rotation_Z, ANGLE, THREE_KEY_PRESSED, S0_color);
		model_S0(texturedShaderProgram, boxTexture, metalTexture, textures_on);
		// end of model S0
		// beginning of model M6
		modelControl(window, texturedShaderProgram, init_M6_Size, model_M6_Size, initPos_M6, model_M6_Position, M6_theta, M6_rotation_X, M6_rotation_Y, M6_rotation_Z, ANGLE, FOUR_KEY_PRESSED, M6_color);
		model_M6(texturedShaderProgram, boxTexture, metalTexture, textures_on);
		// end of model M6
		// beginning of model N7
		modelControl(window, texturedShaderProgram, init_N7_Size, model_N7_Size, initPos_N7, model_N7_Position, N7_theta, N7_rotation_X, N7_rotation_Y, N7_rotation_Z, ANGLE, FIVE_KEY_PRESSED, N7_color);
		model_N7(texturedShaderProgram, boxTexture, metalTexture, textures_on);
		// end of moedel N7

		// light cube
		texturedShaderProgram.setVec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));
		texturedShaderProgram.setFloat("ambientStrength", 100.0f);
		worldMatrix = worldOrientationMatrix * translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f));
		texturedShaderProgram.setMat4("worldMatrix", worldMatrix);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		
		// draw rubiks cube
		texturedShaderProgram.setFloat("ambientStrength", 1.0f);
		modelTranslationMatrix = translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, -2.0f));
		modelScalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 3.0f, 3.0f));
		modelRotationMatrix = identityMatrix;

		// cube 1
		partMatrix = translate(glm::mat4(1.0f), glm::vec3(0.05f, 0.05f, 0.05f));
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
		texturedShaderProgram.setMat4("worldMatrix", worldMatrix);

		glBindTexture(GL_TEXTURE_2D, shape_A_9.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 30, 6);
	
		glBindTexture(GL_TEXTURE_2D, color_3.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 12, 6);

		glBindTexture(GL_TEXTURE_2D, science_1.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 6, 6);

		// cube 2
		partMatrix = translate(glm::mat4(1.0f), glm::vec3(0.05f, 0.05f, -0.05f));
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
		texturedShaderProgram.setMat4("worldMatrix", worldMatrix);

		glBindTexture(GL_TEXTURE_2D, shape_A_6.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 30, 6);

		glBindTexture(GL_TEXTURE_2D, science_2.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 6, 6);

		// cube 3
		partMatrix = translate(glm::mat4(1.0f), glm::vec3(0.05f, 0.05f, -0.15f));
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
		texturedShaderProgram.setMat4("worldMatrix", worldMatrix);

		glBindTexture(GL_TEXTURE_2D, shape_A_3.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 30, 6);

		glBindTexture(GL_TEXTURE_2D, science_3.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 6, 6);

		glBindTexture(GL_TEXTURE_2D, shape_C_1.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// cube 4
		partMatrix = translate(glm::mat4(1.0f), glm::vec3(-0.05f, 0.05f, 0.05f));
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
		texturedShaderProgram.setMat4("worldMatrix", worldMatrix);

		glBindTexture(GL_TEXTURE_2D, shape_A_8.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 30, 6);

		glBindTexture(GL_TEXTURE_2D, color_2.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 12, 6);

		// cube 5
		partMatrix = translate(glm::mat4(1.0f), glm::vec3(-0.05f, 0.05f, -0.05f));
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
		texturedShaderProgram.setMat4("worldMatrix", worldMatrix);

		glBindTexture(GL_TEXTURE_2D, shape_A_5.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 30, 6);

		//cube 6
		partMatrix = translate(glm::mat4(1.0f), glm::vec3(-0.05f, 0.05f, -0.15f));
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
		texturedShaderProgram.setMat4("worldMatrix", worldMatrix);

		glBindTexture(GL_TEXTURE_2D, shape_A_2.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 30, 6);

		glBindTexture(GL_TEXTURE_2D, shape_C_2.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// cube 7
		partMatrix = translate(glm::mat4(1.0f), glm::vec3(-0.15f, 0.05f, 0.05f));
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
		texturedShaderProgram.setMat4("worldMatrix", worldMatrix);

		glBindTexture(GL_TEXTURE_2D, shape_A_7.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 30, 6);

		glBindTexture(GL_TEXTURE_2D, color_1.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 12, 6);

		glBindTexture(GL_TEXTURE_2D, knight_3.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 18, 6);

		// cube 8
		partMatrix = translate(glm::mat4(1.0f), glm::vec3(-0.15f, 0.05f, -0.05f));
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
		texturedShaderProgram.setMat4("worldMatrix", worldMatrix);

		glBindTexture(GL_TEXTURE_2D, shape_A_4.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 30, 6);

		glBindTexture(GL_TEXTURE_2D, knight_2.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 18, 6);

		// cube 9
		partMatrix = translate(glm::mat4(1.0f), glm::vec3(-0.15f, 0.05f, -0.15f));
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
		texturedShaderProgram.setMat4("worldMatrix", worldMatrix);

		glBindTexture(GL_TEXTURE_2D, shape_A_1.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 30, 6);

		glBindTexture(GL_TEXTURE_2D, knight_1.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 18, 6);

		glBindTexture(GL_TEXTURE_2D, shape_C_3.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// cube 10 
		partMatrix = translate(glm::mat4(1.0f), glm::vec3(0.05f, -0.05f, 0.05f));
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
		texturedShaderProgram.setMat4("worldMatrix", worldMatrix);

		glBindTexture(GL_TEXTURE_2D, color_6.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 12, 6);

		glBindTexture(GL_TEXTURE_2D, science_4.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 6, 6);

		// cube 11
		partMatrix = translate(glm::mat4(1.0f), glm::vec3(0.05f, -0.05f, -0.05f));
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
		texturedShaderProgram.setMat4("worldMatrix", worldMatrix);

		glBindTexture(GL_TEXTURE_2D, science_5.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 6, 6);

		// cube 12
		partMatrix = translate(glm::mat4(1.0f), glm::vec3(0.05f, -0.05f, -0.15f));
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
		texturedShaderProgram.setMat4("worldMatrix", worldMatrix);

		glBindTexture(GL_TEXTURE_2D, science_6.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 6, 6);

		glBindTexture(GL_TEXTURE_2D, shape_C_4.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// cube 13
		partMatrix = translate(glm::mat4(1.0f), glm::vec3(-0.05f, -0.05f, 0.05f));
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
		texturedShaderProgram.setMat4("worldMatrix", worldMatrix);

		glBindTexture(GL_TEXTURE_2D, color_5.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 12, 6);

		// cube 14
		partMatrix = translate(glm::mat4(1.0f), glm::vec3(-0.05f, -0.05f, -0.15f));
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
		texturedShaderProgram.setMat4("worldMatrix", worldMatrix);

		glBindTexture(GL_TEXTURE_2D, shape_C_5.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);


		// cube 15
		partMatrix = translate(glm::mat4(1.0f), glm::vec3(-0.15f, -0.05f, 0.05f));
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
		texturedShaderProgram.setMat4("worldMatrix", worldMatrix);

		glBindTexture(GL_TEXTURE_2D, color_4.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 12, 6);

		glBindTexture(GL_TEXTURE_2D, knight_6.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 18, 6);

		// cube 16
		partMatrix = translate(glm::mat4(1.0f), glm::vec3(-0.15f, -0.05f, -0.05f));
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
		texturedShaderProgram.setMat4("worldMatrix", worldMatrix);

		glBindTexture(GL_TEXTURE_2D, knight_5.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 18, 6);

		// cube 17
		partMatrix = translate(glm::mat4(1.0f), glm::vec3(-0.15f, -0.05f, -0.15f));
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
		texturedShaderProgram.setMat4("worldMatrix", worldMatrix);

		glBindTexture(GL_TEXTURE_2D, knight_4.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 18, 6);

		glBindTexture(GL_TEXTURE_2D, shape_C_6.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// cube 18
		partMatrix = translate(glm::mat4(1.0f), glm::vec3(0.05f, -0.15f, 0.05f));
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
		texturedShaderProgram.setMat4("worldMatrix", worldMatrix);

		glBindTexture(GL_TEXTURE_2D, color_9.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 12, 6);

		glBindTexture(GL_TEXTURE_2D, science_7.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 6, 6);

		glBindTexture(GL_TEXTURE_2D, shape_B_3.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 24, 6);

		// cube 19 
		partMatrix = translate(glm::mat4(1.0f), glm::vec3(0.05f, -0.15f, -0.05f));
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
		texturedShaderProgram.setMat4("worldMatrix", worldMatrix);

		glBindTexture(GL_TEXTURE_2D, science_8.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 6, 6);

		glBindTexture(GL_TEXTURE_2D, shape_B_6.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 24, 6);

		// cube 20
		partMatrix = translate(glm::mat4(1.0f), glm::vec3(0.05f, -0.15f, -0.15f));
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
		texturedShaderProgram.setMat4("worldMatrix", worldMatrix);

		glBindTexture(GL_TEXTURE_2D, science_9.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 6, 6);

		glBindTexture(GL_TEXTURE_2D, shape_B_9.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 24, 6);

		glBindTexture(GL_TEXTURE_2D, shape_C_7.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// cube 21
		partMatrix = translate(glm::mat4(1.0f), glm::vec3(-0.05f, -0.15f, 0.05f));
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
		texturedShaderProgram.setMat4("worldMatrix", worldMatrix);

		glBindTexture(GL_TEXTURE_2D, color_8.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 12, 6);

		glBindTexture(GL_TEXTURE_2D, shape_B_2.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 24, 6);

		// cube 22
		partMatrix = translate(glm::mat4(1.0f), glm::vec3(-0.05f, -0.15f, -0.05f));
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
		texturedShaderProgram.setMat4("worldMatrix", worldMatrix);

		glBindTexture(GL_TEXTURE_2D, shape_B_5.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 24, 6);

		// cube 23
		partMatrix = translate(glm::mat4(1.0f), glm::vec3(-0.05f, -0.15f, -0.15f));
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
		texturedShaderProgram.setMat4("worldMatrix", worldMatrix);

		glBindTexture(GL_TEXTURE_2D, shape_B_8.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 24, 6);

		glBindTexture(GL_TEXTURE_2D, shape_C_8.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// cube 24
		partMatrix = translate(glm::mat4(1.0f), glm::vec3(-0.15f, -0.15f, 0.05f));
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
		texturedShaderProgram.setMat4("worldMatrix", worldMatrix);

		glBindTexture(GL_TEXTURE_2D, color_7.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 12, 6);

		glBindTexture(GL_TEXTURE_2D, knight_9.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 18, 6);
		
		glBindTexture(GL_TEXTURE_2D, shape_B_1.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 24, 6);

		// cube 25
		partMatrix = translate(glm::mat4(1.0f), glm::vec3(-0.15f, -0.15f, -0.05f));
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
		texturedShaderProgram.setMat4("worldMatrix", worldMatrix);

		glBindTexture(GL_TEXTURE_2D, knight_8.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 18, 6);

		glBindTexture(GL_TEXTURE_2D, shape_B_4.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 24, 6);

		// cube 26
		partMatrix = translate(glm::mat4(1.0f), glm::vec3(-0.15f, -0.15f, -0.15f));
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
		texturedShaderProgram.setMat4("worldMatrix", worldMatrix);

		glBindTexture(GL_TEXTURE_2D, knight_7.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 18, 6);

		glBindTexture(GL_TEXTURE_2D, shape_B_7.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 24, 6);

		glBindTexture(GL_TEXTURE_2D, shape_C_9.getTextureID());
		texturedShaderProgram.setInt("texture_1", 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);


		worldMatrix = worldOrientationMatrix;
		texturedShaderProgram.setFloat("ambientStrength", 2.0f);
		drawAxisLines(texturedShaderProgram);
		texturedShaderProgram.setFloat("ambientStrength", 1.0f);
		drawGridlines(texturedShaderProgram);
		
		

		glBindVertexArray(0);

	
		



	




		// teardown: check and call events and swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

		// Detect inputs
		processInput(window, texturedShaderProgram);

	}

	// de-allocate all resources
	glDeleteVertexArrays(1, VAO);
	glDeleteBuffers(1, VBO);
	glDeleteProgram(texturedShaderProgram.shaderProgram);
	glDeleteProgram(shadowShaderProgram.shaderProgram);

	glfwTerminate();
	return 0;
}


// resize window 
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window, Shader shaderProgram)
{

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// Press L to render model in LINE mode
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	// Press T to render model in TRIANGLE mode
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	// Pess P to render model in POINT mode
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}


	// perspective - projection transform
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		projectionMatrix = glm::perspective(90.0f,			// field of view in degrees
			1024.0f / 768.0f,	// aspect ratio
			0.005f, 500.0f);	// near and far (near > 0)
		shaderProgram.setMat4("projectionMatrix", projectionMatrix);
	}

	// orthographic - projection transform
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		projectionMatrix = glm::ortho(-4.0f, 4.0f,      // left/right
			-3.0f, 3.0f,	  // bottom/top
			-100.0f, 100.0f);  // near/far 
		shaderProgram.setMat4("projectionMatrix", projectionMatrix);
	}


	// Press and hold C to disable backface culling
	glEnable(GL_CULL_FACE);
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !GLFW_RELEASE)
	{
		glDisable(GL_CULL_FACE);
	}

	// toggle all textures on/off
	if ((X_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_X)))	// rotate in +z axis
		textures_on = !textures_on;
	X_KEY = glfwGetKey(window, GLFW_KEY_X);

	
	
	bool fastCam = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
	float currentCameraSpeed = (fastCam) ? cameraSpeedFast : cameraSpeed;

	if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) || (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != GLFW_RELEASE)) {

		// disable mouse cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		// detect mouse input
		double mousePosX, mousePosY;
		glfwGetCursorPos(window, &mousePosX, &mousePosY);

		// change in position
		double dx = mousePosX - lastMousePosX;
		double dy = mousePosY - lastMousePosY;

		lastMousePosX = mousePosX;
		lastMousePosY = mousePosY;

		// conversion to sperical coordinates
		const float cameraAngularSpeed = 15.0f;	// mouse speed
		cameraHorizontalAngle -= dx * cameraAngularSpeed * deltaTime;
		cameraVerticalAngle -= dy * cameraAngularSpeed * deltaTime;

		// limit range of vertical camera angle
		cameraVerticalAngle = std::max(-85.0f, std::min(85.0f, cameraVerticalAngle));
		if (cameraHorizontalAngle > 360)
		{
			cameraHorizontalAngle -= 360;
		}
		else if (cameraHorizontalAngle < -360)
		{
			cameraHorizontalAngle += 360;
		}

		// conversion to radians
		float theta = glm::radians(cameraHorizontalAngle);
		float phi = glm::radians(cameraVerticalAngle);

		cameraLookAt = glm::vec3(cosf(phi) * cosf(theta), sinf(phi), -cosf(phi) * sinf(theta));
		glm::vec3 cameraSideVector = glm::cross(cameraLookAt, glm::vec3(0.0f, 1.0f, 0.0f));

		glm::normalize(cameraSideVector);

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // move camera to the left
		{
			cameraPosition -= cameraSideVector * deltaTime * currentCameraSpeed;
		}

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // move camera to the right
		{
			cameraPosition += cameraSideVector * deltaTime * currentCameraSpeed;
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // move camera forward
		{
			cameraPosition += cameraLookAt * deltaTime * currentCameraSpeed;
		}

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // move camera back
		{
			cameraPosition -= cameraLookAt * deltaTime * currentCameraSpeed;
		}

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) // move camera forward
		{
			cameraPosition.y += currentCameraSpeed * deltaTime;
		}

		if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) // move camera down
		{
			cameraPosition.y -= currentCameraSpeed * deltaTime;
		}


		viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
		shaderProgram.setMat4("viewMatrix", viewMatrix);
	}
	

	// enable mouse cursor if right mouse button is released
	if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)) == GLFW_RELEASE)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	// reset to original world position and orientation
	if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS)
	{
		// reset camera position and lookAt
		cameraPosition = initialCameraPosition;
		cameraLookAt = initialcameraLookAt;

		// world angle/orientation
		worldOrientation_X = identityMatrix;
		worldOrientation_Y = identityMatrix;
		worldOrientationMatrix = identityMatrix;
		worldRotation = { 0.0f, 0.0f, 0.0f };

		//shaderProgram.setMat4("worldMatrix", worldMatrix);

		viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
		shaderProgram.setMat4("viewMatrix", viewMatrix);
	}


	// mouse scroll for zoom in/out
	glfwSetScrollCallback(window, scroll_callback); // receives mouse scroll as input
	shaderProgram.setMat4("projectionMatrix", projectionMatrix);

	glfwSetKeyCallback(window, key_callback);


	// press 0 mouse button to select all models
	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
	{
		ONE_KEY_PRESSED = true;
		TWO_KEY_PRESSED = true;
		THREE_KEY_PRESSED = true;
		FOUR_KEY_PRESSED = true;
		FIVE_KEY_PRESSED = true;
	}

	// press right mouse button to deselect all models
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		ONE_KEY_PRESSED = false;
		TWO_KEY_PRESSED = false;
		THREE_KEY_PRESSED = false;
		FOUR_KEY_PRESSED = false;
		FIVE_KEY_PRESSED = false;
	}

	// press 1 to select model A7
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		ONE_KEY_PRESSED = true;
		TWO_KEY_PRESSED = false;
		THREE_KEY_PRESSED = false;
		FOUR_KEY_PRESSED = false;
		FIVE_KEY_PRESSED = false;
	}

	// press 2 to select model O9
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		ONE_KEY_PRESSED = false;
		TWO_KEY_PRESSED = true;
		THREE_KEY_PRESSED = false;
		FOUR_KEY_PRESSED = false;
		FIVE_KEY_PRESSED = false;
	}

	// press 3 to select model S0 
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	{
		ONE_KEY_PRESSED = false;
		TWO_KEY_PRESSED = false;
		THREE_KEY_PRESSED = true;
		FOUR_KEY_PRESSED = false;
		FIVE_KEY_PRESSED = false;
	}

	// press 4 to select model M6
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
	{
		ONE_KEY_PRESSED = false;
		TWO_KEY_PRESSED = false;
		THREE_KEY_PRESSED = false;
		FOUR_KEY_PRESSED = true;
		FIVE_KEY_PRESSED = false;
	}

	// press 5 to select model N7
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
	{
		ONE_KEY_PRESSED = false;
		TWO_KEY_PRESSED = false;
		THREE_KEY_PRESSED = false;
		FOUR_KEY_PRESSED = false;
		FIVE_KEY_PRESSED = true;
	}


	// while in model control, holding shift will increase movement speed or scaling speed
	if ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
	{
		modelSpeed = 6.0;
		shearingScale = 2.0f;
	}

	if ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_RELEASE))
	{
		modelSpeed = 1.0;
		shearingScale = 1.0f;
	}

	if ((DOWN_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_DOWN)))
	{
		worldRotation.y -= glm::radians(worldOrientation_ANGLE);
	}	
	DOWN_KEY = glfwGetKey(window, GLFW_KEY_DOWN);

	if ((UP_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_UP))) 
	{
		worldRotation.y += glm::radians(worldOrientation_ANGLE);
	}	
	UP_KEY = glfwGetKey(window, GLFW_KEY_UP);

	if ((LEFT_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_LEFT))) 
	{	
		worldRotation.x -= glm::radians(worldOrientation_ANGLE);
	}	
	LEFT_KEY = glfwGetKey(window, GLFW_KEY_LEFT);

	if ((RIGHT_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_RIGHT))) 
	{
		worldRotation.x += glm::radians(worldOrientation_ANGLE);
	}	
	RIGHT_KEY = glfwGetKey(window, GLFW_KEY_RIGHT);
	
	worldOrientation_Y[0][0] = cos(worldRotation.y);
	worldOrientation_Y[0][2] = sin(worldRotation.y);
	worldOrientation_Y[2][0] = -sin(worldRotation.y);
	worldOrientation_Y[2][2] = cos(worldRotation.y);

	worldOrientation_X[1][1] = cos(worldRotation.x);
	worldOrientation_X[1][2] = -sin(worldRotation.x);
	worldOrientation_X[2][1] = sin(worldRotation.x);
	worldOrientation_X[2][2] = cos(worldRotation.x);

	worldOrientationMatrix = worldOrientation_X * worldOrientation_Y;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{

	if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		|| (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != GLFW_RELEASE))
	{

		// scroll up to zoom in 
		if (yoffset < 0) {
			if (fov <= 120.0f)
			{
				fov = fov + 5.0;
				projectionMatrix = glm::perspective(glm::radians(fov),			// field of view in degrees
					1024.0f / 768.0f,	// aspect ratio
					0.005f, 500.0f);	// near and far (near > 0)
			}
		}
		// scroll down to zoom out 
		if (yoffset > 0) {
			if (fov >= 10.0f)
			{
				fov = fov - 5.0;
				projectionMatrix = glm::perspective(glm::radians(fov),			// field of view in degrees
					1024.0f / 768.0f,	// aspect ratio
					0.005f, 500.0f);	// near and far (near > 0)
			}
		}
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (action == GLFW_RELEASE) return;

	if (key == GLFW_KEY_TAB)
		TAB_KEY = !TAB_KEY;
}

void model_A7(Shader shaderProgram, Texture texture_1, Texture texture_2, bool isTextureOn)
{
	// set object origin position
	objectTranslationtionMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.20f, 0.0));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);

	// Start of A
	if (isTextureOn) {
		glBindTexture(GL_TEXTURE_2D, texture_1.getTextureID());
		shaderProgram.setInt("texture_1", 0);		// Set our Texture sampler to user Texture Unit      
	}

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	shearingMatrix[1][0] = 0.35f;
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	shearingMatrix[1][0] = -0.35f;
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.4f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	shearingMatrix = glm::mat4(1.0f);
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 0.75f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.15f, 0.15f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Start of 7
	if (isTextureOn) {
		glBindTexture(GL_TEXTURE_2D, texture_2.getTextureID());
		shaderProgram.setInt("texture_1", 0);		// Set our Texture sampler to user Texture Unit 0              
	}

	shearingMatrix[1][0] = 0.3f;
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 4.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.7f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	shearingMatrix = identityMatrix;
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 1.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.62f, 0.4f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	// reset world matrix, rotation matrix, and model scaling matrix after we're done with it for this object
	modelScalingMatrix = identityMatrix;
	modelRotationMatrix = identityMatrix;
	modelShearingMatrix = identityMatrix;
	worldMatrix = identityMatrix;

	if (isTextureOn)
		glActiveTexture(GL_TEXTURE0); // this is the GL "reset" texture, use to flush previous texture
	// end of model A7
}

void model_O9(Shader shaderProgram, Texture texture_1, Texture texture_2, bool isTextureOn) {

	// set object origin position
	objectTranslationtionMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.20f, 0.0));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);

	// beginning of letter O
	if (isTextureOn) {
		glBindTexture(GL_TEXTURE_2D, texture_1.getTextureID());
		shaderProgram.setInt("texture_1", 0);		// Set our Texture sampler to user Texture Unit 0
	}

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.4f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 0.75f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.1f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 0.75f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.1f, 0.425f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	// end of letter O

	// beginning of number 9

	if (isTextureOn) {
		glBindTexture(GL_TEXTURE_2D, texture_2.getTextureID());
		shaderProgram.setInt("texture_1", 0);		// Set our Texture sampler to user Texture Unit 0
	}

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(4.0f, 0.75f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.62f, 0.425f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 0.75f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.62f, 0.175f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 0.75f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.62f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	

	//vertical
	//right
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.92f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	
	// vertical
	// left
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.75f, 1.8f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.62f, 0.25f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	// end of number 9


	// reset world matrix, rotation matrix, and model scaling matrix after we're done with it for this object
	modelScalingMatrix = identityMatrix;
	modelRotationMatrix = identityMatrix;
	modelShearingMatrix = identityMatrix;
	worldMatrix = identityMatrix;
	// end of model O9

	if (isTextureOn)
		glActiveTexture(GL_TEXTURE0); // this is the GL "reset" texture, use to flush previous texture
}

void model_S0(Shader shaderProgram, Texture texture_1, Texture texture_2, bool isTextureOn) {

	// set object origin position
	objectTranslationtionMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.3f, 0.0));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);

	// S
	if (isTextureOn) {
		glBindTexture(GL_TEXTURE_2D, texture_1.getTextureID());
		shaderProgram.setInt("texture_1", 0);		// Set our Texture sampler to user Texture Unit 0
	}

	shearingMatrix = glm::mat4(1.0f);
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(4.0f, 1.0f, 1.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	shearingMatrix = glm::mat4(1.0f);
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.1f, -0.2f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.3f, 0.0f));
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 1.0f, 1.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	shearingMatrix = glm::mat4(1.0f);
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.3, -0.5f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.1f, -0.6f, 0.0f));
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(4.0f, 1.0f, 1.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	// ZERO
	if (isTextureOn) {
		glBindTexture(GL_TEXTURE_2D, texture_2.getTextureID());
		shaderProgram.setInt("texture_1", 0);		// Set our Texture sampler to user Texture Unit 0
	}

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	shearingMatrix = glm::mat4(1.0f);
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.6f, -0.5f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	shearingMatrix = glm::mat4(1.0f);
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, -0.5f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 1.0f, 1.0f));
	shearingMatrix = glm::mat4(1.0f);
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.7f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 1.0f, 1.0f));
	shearingMatrix = glm::mat4(1.0f);
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.7f, -0.6f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	// End of model S0

	//Reset 
	modelScalingMatrix = identityMatrix;
	modelRotationMatrix = identityMatrix;
	modelShearingMatrix = identityMatrix;
	worldMatrix = identityMatrix;

	if (isTextureOn)
		glActiveTexture(GL_TEXTURE0); // this is the GL "reset" texture, use to flush previous texturere
}

void model_M6(Shader shaderProgram, Texture texture_1, Texture texture_2, bool isTextureOn) {

	// set object origin position
	objectTranslationtionMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.2f, 0.0));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);

	//draw letter M
	if (isTextureOn) {
		glBindTexture(GL_TEXTURE_2D, texture_1.getTextureID());
		shaderProgram.setInt("texture_1", 0);		// Set our Texture sampler to user Texture Unit 0
	}

	//left upright for letter M
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//left diagonal for letter M
	shearingMatrix[1][0] = 0.35f;
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.2f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//right diagonal for letter M
	shearingMatrix[1][0] = -0.35f;
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//right upright for letter M
	shearingMatrix = identityMatrix;
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.4f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	//end of M

	//draw 6
	if (isTextureOn) {
		glBindTexture(GL_TEXTURE_2D, texture_2.getTextureID());
		shaderProgram.setInt("texture_Sampler", 0);		// Set our Texture sampler to user Texture Unit 0
	}

	//vertical segments
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.8f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.6f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//horizontal segments
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 1.0f, 1.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 1.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.6f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 1.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.6f, 0.4f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	//end 6	

	// reset world matrix, rotation matrix, and model scaling matrix after we're done with it for this object.
	// also reset shearing matrix and scaling matrix, so that they work correctly with model_A7().
	
	shearingMatrix[1][0] = 0.35f;
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));

	modelScalingMatrix = identityMatrix;
	modelRotationMatrix = identityMatrix;
	modelShearingMatrix = identityMatrix;
	worldMatrix = identityMatrix;

	if (isTextureOn)
		glActiveTexture(GL_TEXTURE0); // this is the GL "reset" texture, use to flush previous texture
}

void model_N7(Shader shaderProgram, Texture texture_1, Texture texture_2, bool isTextureOn)
{
	const float letterSpacing = 0.25f;
	const float left_of_origin = -0.5f;

	translationMatrix = identityMatrix;
	shearingMatrix = identityMatrix;
	scalingMatrix = scale(identityMatrix, glm::vec3(1.0f, 5.0f, 1.0f));

	if (isTextureOn) {
		glBindTexture(GL_TEXTURE_2D, texture_1.getTextureID());
		shaderProgram.setInt("texture_1", 0);		// Set our Texture sampler to user Texture Unit 0
	}

	// Left leg of 'N'
	translationMatrix[3][0] = left_of_origin;
	partMatrix = translationMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Right leg of 'N'
	translationMatrix[3][0] = left_of_origin + 0.35f;
	partMatrix = translationMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Diagonal of 'N'
	shearingMatrix[1][0] = -0.7f;
	partMatrix = translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	if (isTextureOn) {
		glBindTexture(GL_TEXTURE_2D, texture_2.getTextureID());
		shaderProgram.setInt("texture_1", 0);		// Set our Texture sampler to user Texture Unit 0
		//glUniform1i(textureLocation, 0);                // Set our Texture sampler to user Texture Unit 0
	}

	// Diagonal base of '7'
	translationMatrix[3][0] = left_of_origin + 0.35f + letterSpacing;
	shearingMatrix[1][0] = 0.5f;
	partMatrix = translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Horizontal top of '7'
	scalingMatrix = scale(identityMatrix, vec3(1.0f, 2.5f, 1.0f));
	shearingMatrix[1][0] = 0.0f;
	translationMatrix[3][0] = left_of_origin + 0.6f + letterSpacing;
	translationMatrix[3][1] = 0.4f;
	mat4 seven_top_rotate = rotate(identityMatrix, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
	partMatrix = translationMatrix * seven_top_rotate * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	// reset world matrix, rotation matrix, and model scaling matrix after we're done with it for this object
	modelScalingMatrix = identityMatrix;
	modelRotationMatrix = identityMatrix;
	modelShearingMatrix = identityMatrix;
	worldMatrix = identityMatrix;

	if (isTextureOn)
		glActiveTexture(GL_TEXTURE0); // this is the GL "reset" texture, use to flush previous texture
}

void model_ground(float groundSize, Shader shaderProgram, Texture texture, bool isTextureOn) {

	const float ground_final = 5 * groundSize;

	translationMatrix = identityMatrix;
	scalingMatrix = scale(identityMatrix, glm::vec3(ground_final, 0.0f, ground_final));

	if (isTextureOn) {
		glBindTexture(GL_TEXTURE_2D, texture.getTextureID());
		shaderProgram.setInt("texture_1", 0);
	}

	translationMatrix[3][0] = -ground_final / 2 / 10;
	translationMatrix[3][2] = -ground_final / 2 / 10;
	partMatrix = translationMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	if (isTextureOn)
		glActiveTexture(GL_TEXTURE0); // this is the GL "reset" texture, use to flush previous texture
}

void modelControl(GLFWwindow * window, Shader shaderProgram, const float& initSize, float& modelSize, glm::vec3 & initPos,
	glm::vec3 & modelPos, glm::vec3 & rotation, glm::mat4 & rotX, glm::mat4 & rotY, glm::mat4 & rotZ, const float ANGLE, bool KEY_PRESSED, glm::vec3 & color) {

	rotation = { rotation.x, rotation.y, rotation.z };
	modelRotationMatrix = rotX * rotY * rotZ;
	modelTranslationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(initPos.x + modelPos.x, initPos.y + modelPos.y, initPos.z + modelPos.z));
	modelScalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(modelSize + initSize, modelSize + initSize, modelSize + initSize));
	shaderProgram.setMat4("worldMatrix", worldMatrix);

	if (KEY_PRESSED)
	{
		switch (TAB_KEY) {
			// continuous model movement
		case (false):
			if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
				modelSize += deltaTime * modelSpeed;
			if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
				modelSize -= deltaTime * modelSpeed;

			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
				modelPos.x -= deltaTime * modelSpeed;
				modelShearingMatrix[1][0] = 0.5f * shearingScale;
			}
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
				modelPos.x += deltaTime * modelSpeed;
				modelShearingMatrix[1][0] = -0.5f * shearingScale;
			}
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
				modelPos.z -= deltaTime * modelSpeed;
				modelShearingMatrix[1][2] = 0.5f * shearingScale;
			}
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
				modelPos.z += deltaTime * modelSpeed;
				modelShearingMatrix[1][2] = -0.5f * shearingScale;
			}
			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
				modelPos.y += deltaTime * modelSpeed;
			if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
				modelPos.y -= deltaTime * modelSpeed;
			break;

			// incremental translation of 1 grid unit length of 0.1;
		case (true):
			if ((U_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_U)))
				modelSize += 0.1f;
			U_KEY = glfwGetKey(window, GLFW_KEY_U);
			if ((J_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_J)))
				modelSize -= 0.1f;
			J_KEY = glfwGetKey(window, GLFW_KEY_J);

			if ((A_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_A))) {
				modelShearingMatrix[1][0] = 0.5f * shearingScale;
				modelPos.x -= gridUnit;

			}	A_KEY = glfwGetKey(window, GLFW_KEY_A);

			if ((D_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_D))) {
				modelShearingMatrix[1][0] = -0.5f * shearingScale;
				modelPos.x += gridUnit;

			}	D_KEY = glfwGetKey(window, GLFW_KEY_D);

			if ((W_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_W))) {
				modelShearingMatrix[1][2] = 0.5f * shearingScale;
				modelPos.z -= gridUnit;

			}	W_KEY = glfwGetKey(window, GLFW_KEY_W);

			if ((S_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_S))) {
				modelShearingMatrix[1][2] = -0.5f * shearingScale;
				modelPos.z += gridUnit;

			}	S_KEY = glfwGetKey(window, GLFW_KEY_S);

			if ((SPACE_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_SPACE))) {
				modelPos.y += gridUnit;

			}	SPACE_KEY = glfwGetKey(window, GLFW_KEY_SPACE);

			if ((Z_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_Z))) {
				modelPos.y -= gridUnit;

			}	Z_KEY = glfwGetKey(window, GLFW_KEY_Z);
			break;
		}


		// defined range range of random numbers (-5.0f to 5.0f)
		float randomPos_X = ((float(rand()) / RAND_MAX) * 10.0f) - 5.0f;
		float randomPos_Z = ((float(rand()) / RAND_MAX) * 10.0f) - 5.0f;
		if ((ALT_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_LEFT_ALT))) {
			std::cout << "Model position randomized - X: " << randomPos_X << " Z: " << randomPos_Z << std::endl; // will display on command window the new position of the model
			modelPos = { randomPos_X - initPos.x, 0.0f, randomPos_Z - initPos.z };

		}	ALT_KEY = glfwGetKey(window, GLFW_KEY_LEFT_ALT);

		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
			modelPos = { 0.0f, 0.0f, 0.0f };
		if ((glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT)) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE))
			modelSize = 0.0f;

		if ((T_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_T)))	// rotate in +y axis
			rotation.x += glm::radians(ANGLE);
		T_KEY = glfwGetKey(window, GLFW_KEY_T);

		if ((Y_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_Y)))	// rotate in -y axis
			rotation.x -= glm::radians(ANGLE);
		Y_KEY = glfwGetKey(window, GLFW_KEY_Y);

		if ((G_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_G))) 	// rotate in +x axis
			rotation.y += glm::radians(ANGLE);
		G_KEY = glfwGetKey(window, GLFW_KEY_G);

		if ((H_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_H))) 	// rotate in -x axis
			rotation.y -= glm::radians(ANGLE);
		H_KEY = glfwGetKey(window, GLFW_KEY_H);

		if ((B_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_B)))	// rotate in +z axis
			rotation.z += glm::radians(ANGLE);
		B_KEY = glfwGetKey(window, GLFW_KEY_B);

		if ((N_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_N))) 	// rotate in -z axis
			rotation.z -= glm::radians(ANGLE);
		N_KEY = glfwGetKey(window, GLFW_KEY_N);


		if ((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) && glfwGetKey(window, GLFW_KEY_R)) == GLFW_PRESS)	// reset model orientation
			rotation = { 0.0f, 0.0f, 0.0f };  // set theta back to zero

		//rotX, rotY, rotZ = identityMatrix;
		rotX[1][1] = cos(rotation.x);
		rotX[2][2] = cos(rotation.x);
		rotX[1][2] = -sin(rotation.x);
		rotX[2][1] = sin(rotation.x);

		rotY[0][0] = cos(rotation.y); 
		rotY[2][2] = cos(rotation.y);
		rotY[0][2] = sin(rotation.y);
		rotY[2][0] = -sin(rotation.y);

		rotZ[0][0] = cos(rotation.z);
		rotZ[1][1] = cos(rotation.z);
		rotZ[0][1] = -sin(rotation.z);
		rotZ[1][0] = sin(rotation.z);


		modelTranslationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(initPos.x + modelPos.x, initPos.y + modelPos.y, initPos.z + modelPos.z));
		modelScalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(modelSize + initSize, modelSize + initSize, modelSize + initSize));
		modelRotationMatrix = rotX * rotY * rotZ;
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
		
		shaderProgram.setMat4("worldMatrix", worldMatrix);

		// when textures are off, models will have default textures
	}
	if (!textures_on) {
		shaderProgram.setVec3("objectColor", color);
	}

}

void drawAxisLines(Shader shaderProgram) {


	
	// red line
	glLineWidth(5);
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	shaderProgram.setVec3("objectColor", glm::vec3(1.0f, 0.0f, 0.0f));
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_LINES, 0, 2);

	// green line
	glBindVertexArray(VAO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	shaderProgram.setVec3("objectColor", glm::vec3(0.0f, 1.0f, 0.0f));
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_LINES, 0, 2);

	// blue line
	glBindVertexArray(VAO[3]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
	shaderProgram.setVec3("objectColor", glm::vec3(0.0f, 0.0f, 1.0f));
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_LINES, 0, 2);
}

void drawGridlines(Shader shaderProgram) {
	glLineWidth(1);
	glBindVertexArray(VAO[4]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
	shaderProgram.setMat4("worldMatrix", worldMatrix);
	glDrawArrays(GL_LINES, 0, 2 * 2 * numGridLines);
}

int createGridlines(int numGridlines, vec3 * gridLinesVertices, GLuint size, vec3 lineColor, float sizeOfGrid) {

	glm::vec3 zLineColor = glm::vec3(0.0f, 1.0f, 1.0f);     // set line color for lines running parallel to z-axis
	glm::vec3 xLineColor = glm::vec3(0.0f, 1.0f, 1.0f);		// set line color for lines running parallel to x-axis
	float increment = 0.0f;                                 // how much to move a line over
	const float spacing = 10.0f / (float)numGridlines;      // divide the 2.0 world into the number of gridlines

	for (int i = 0; i < numGridlines; ++i) {
		// lines parallel to z-axis
		gridLinesVertices[i * 8] = glm::vec3(-5.0f + increment, 0.0f, -5.0f);
		gridLinesVertices[i * 8 + 1] = glm::vec3(zLineColor.x, zLineColor.y, zLineColor.z);
		gridLinesVertices[i * 8 + 2] = glm::vec3(-5.0f + increment, 0.0f, 5.0f);
		gridLinesVertices[i * 8 + 3] = glm::vec3(zLineColor.x, zLineColor.y, zLineColor.z);
		// lines parallel to x-axis
		gridLinesVertices[i * 8 + 4] = glm::vec3(-5.0f, 0.0f, -5.0f + increment);
		gridLinesVertices[i * 8 + 5] = glm::vec3(xLineColor.x, xLineColor.y, xLineColor.z);
		gridLinesVertices[i * 8 + 6] = glm::vec3(5.0f, 0.0f, -5.0f + increment);
		gridLinesVertices[i * 8 + 7] = glm::vec3(xLineColor.x, xLineColor.y, xLineColor.z);

		increment += spacing;
	}

	// Create a vertex array
	GLuint vertexArrayObject;
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	// Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
	GLuint vertexBufferObject;
	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, size, gridLinesVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vec3), (void*)0);                   // position xyz
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vec3), (void*)sizeof(vec3));       // colour rgb
	glEnableVertexAttribArray(1);

	return vertexArrayObject;
}


int createTexturedCubeVertexArrayObject(TexturedColoredVertex * texturedCubeVertexArray, GLuint size) {

	// Create a vertex array
	GLuint vertexArrayObject;
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	// Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
	GLuint vertexBufferObject;
	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, size, texturedCubeVertexArray, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedColoredVertex), (void*)0);                   // position xyz
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 32, (void*)sizeof(vec3));       // texture uv
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedColoredVertex), (void*)(2 * sizeof(vec3)));   // normal
	glEnableVertexAttribArray(2);

	return vertexArrayObject;
}



int createLine(vec3 * lineVertices) {

	// Create a vertex array
	GLuint vertexArrayObject;
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	// Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
	GLuint vertexBufferObject;
	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(vec3), lineVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);                   // position xyz
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)sizeof(vec3));       // colour rgb
	glEnableVertexAttribArray(1);

	return vertexArrayObject;
}


