/*
*	COMP 371 - Assignment 2
*
*	Umaid Malik 27576110
*	Daniel Thibault-Shea 40073133
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
#include "Sphere.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


using namespace glm;

const int numbObjInScene = 6;                           // make sure to update this if you add more models!!!

const int numGridLines = 100;                           // how many gridlines (going one way)

glm::vec3 initialCameraPosition(0.6f, 1.0f, 1.5f);
glm::vec3 initialcameraLookAt(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

glm::vec3 center(0.0f, 0.0f, 0.0f);

// light position
glm::vec3 lightPos(0.0f, 3.0f, 0.0f);

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
glm::mat4 modelScalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
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

GLuint worldMatrixLocation;

struct TexturedColoredVertex
{
    TexturedColoredVertex(vec3 _position, vec3 _color, vec2 _uv, vec3 _vertexNormal) : position(_position), color(_color), uv(_uv), vertexNormal(_vertexNormal){}

    vec3 position;
    vec3 color;
    vec2 uv;
    vec3 vertexNormal;
};

// function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, GLuint shaderProgram);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void setProjectionMatrix(int shaderProgram, mat4 projectionMatrix);
void setViewMatrix(int shaderProgram, mat4 viewMatrix);
void setWorldMatrix(int shaderProgram, mat4 worldMatrix);
int compileAndLinkShaders(const char* vertexShaderSource, const char* fragmentShaderSource);
GLuint loadTexture(const char *filename);


// models
int createTexturedCubeVertexArrayObject(TexturedColoredVertex* texturedCubeVertexArray, GLuint size);
int createPointLightSource(glm::vec3* lightSource, GLuint size);
void model_ground(float groundSize, GLuint textureLocation, GLuint texture, bool isTextureOn);
void model_A7(GLuint textureLocation, GLuint texture_1, GLuint texture_2, bool isTextureOn);
void model_O9(GLuint textureLocation, GLuint texture_1, GLuint texture_2, bool isTextureOn);
void model_S0(GLuint textureLocation, GLuint texture_1, GLuint texture_2, bool isTextureOn);
void model_M6(GLuint textureLocation, GLuint texture_1, GLuint texture_2, bool isTextureOn);
void model_N7(GLuint textureLocation, GLuint texture_1, GLuint texture_2, bool isTextureOn);

int createGridlines(int numGridlines, vec3* gridLinesVertices, GLuint size, vec3 lineColor, float sizeOfGrid);
void drawGridlines();
int createLine(vec3* lineVertices);
void drawAxisLines(GLuint shaderProgram);

// control function for the models 
void modelControl(GLFWwindow* window, unsigned int* shaderProgram, const float& initSize, float& modelSize, glm::vec3& initPos,
	glm::vec3& modelPos, glm::vec3& rotation, glm::mat4& rotX, glm::mat4& rotY, glm::mat4& rotZ, const float ANGLE, bool KEY_PRESSED);

GLFWwindow* setupWindow();

// end of function protoypes


const char* getVertexShaderSource()
{
	return
		"#version 330 core\n"
		"layout (location = 0) in vec3 aPos;"
		"layout (location = 1) in vec3 aColor;"
		"layout (location = 3) in vec3 aNormal;"
		""
		"uniform mat4 worldMatrix;"
		"uniform mat4 viewMatrix = mat4(1.0);" // default value for view matrix (identity)
		"uniform mat4 projectionMatrix = mat4(1.0);"
		""
		"out vec3 vertexColor;"
		"void main()"
		"{"
		"   vertexColor = aColor;"
		"	mat4 modelViewProjection = projectionMatrix * viewMatrix * worldMatrix;"
		"   gl_Position = modelViewProjection * vec4(aPos.x, aPos.y, aPos.z, 1.0);"
		"}";
}

const char* getFragmentShaderSource()
{
	return
		"#version 330 core\n"
		"uniform vec3 objectColor;"
		"out vec4 FragColor;"
		"in vec3 vertexColor;"
		"void main()"
		"{"		
		" FragColor = vec4(vertexColor.r * objectColor.r, vertexColor.g * objectColor.r, vertexColor.b * objectColor.r, 1.0f);"
		"}";
}


const char* getTexturedVertexShaderSource()
{
    return
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;"
        "layout (location = 1) in vec3 aColor;"
        "layout (location = 2) in vec2 aUV;"
		"layout (location = 3) in vec3 aNormal;"
        ""
        "uniform mat4 worldMatrix;"
        "uniform mat4 viewMatrix = mat4(1.0);"  // default value for view matrix (identity)
        "uniform mat4 projectionMatrix = mat4(1.0);"
        ""
        "out vec3 vertexColor;"
        "out vec2 vertexUV;"
		"out vec3 Normal;"
		"out vec3 FragPos;"
        ""
        "void main()"
        "{"
		"   FragPos = vec3(worldMatrix * vec4(aPos, 1.0));"
        "   vertexColor = aColor;"
		"   vertexUV = aUV;"
		"   Normal = mat3(transpose(inverse(worldMatrix))) * aNormal;"
        "   mat4 modelViewProjection = projectionMatrix * viewMatrix * worldMatrix;"
        "   gl_Position = modelViewProjection * vec4(aPos, 1.0);"
		
        "}";
}

const char* getTexturedFragmentShaderSource()
{
    return
        "#version 330 core\n"
		"out vec4 FragColor;"
		"in vec3 Normal;"
        "in vec3 vertexColor;"
        "in vec2 vertexUV;"
		"in vec3 FragPos;"
        "uniform sampler2D textureSampler;"
		"uniform vec3 lightColor;"
		"uniform vec3 objectColor;"
		"uniform vec3 lightPos;"
		"uniform vec3 viewPos;"
        ""
        "void main()"
        "{"
		// ambient
		"   float ambientStrength = 0.60;"
		"   vec3 ambient = ambientStrength * lightColor;"
		// diffuse
		"	vec3 norm = normalize(Normal);"
		"	vec3 lightDir = normalize(lightPos - FragPos);"
		"	float diff = max(dot(norm, lightDir), 0.0);"
		"	vec3 diffuse = 1.5 * diff * lightColor;"
		// specular
		"	float specularStrength = 1.6;"
		"	vec3 viewDir = normalize(viewPos - FragPos);"
		"	vec3 reflectDir = reflect(-lightDir, norm);"
		"	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);"
		"	vec3 specular = specularStrength * spec * lightColor;"
		//
		"   vec3 phongModel = (ambient + diffuse + specular) * objectColor;"
        "   FragColor = texture(textureSampler, vertexUV) * vec4(phongModel, 1.0);"// * vec4(vertexColor, 1.0f)"
        "}";
}

const char* getLightVertexShaderSource()
{
	return
		"#version 330 core\n"
		"layout (location = 0)\n"
		"in vec3 aPos;"
		"uniform mat4 worldMatrix;"
		"uniform mat4 viewMatrix = mat4(1.0);"
		"uniform mat4 projectionMatrix = mat4(1.0);"
		"void main()"
		"{"
		"		gl_Position = projectionMatrix * viewMatrix * worldMatrix * vec4(aPos, 1.0);"
		"}";
}

const char* getLightFragmentShaderSource()
{
	return
		"#version 330 core\n"
		"out vec4 FragColor;"
		"void main()"
		"{"
		"	FragColor = vec4(1.0);"
		"}";
}



GLFWwindow* setupWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TURE);
	glfwWindowHint(GL_DEPTH_TEST, 24);

	GLFWwindow* window = glfwCreateWindow(1024, 768, "COMP 371 - Team 6 - Part 2", NULL, NULL);
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
    GLuint colorShaderProgram = compileAndLinkShaders(getVertexShaderSource(), getFragmentShaderSource());
    GLuint texturedShaderProgram = compileAndLinkShaders(getTexturedVertexShaderSource(), getTexturedFragmentShaderSource());
	GLuint lightShaderProgram = compileAndLinkShaders(getLightVertexShaderSource(), getLightFragmentShaderSource());
	

    // Load Textures
#if defined(PLATFORM_OSX)
    GLuint groundTexture = loadTexture("../Assignment 2/assets/textures/tiles.bmp");
    GLuint metalTexture = loadTexture("../Assignment 2/assets/textures/metal.jpg");
#else
    GLuint groundTexture = loadTexture("../Assignment 2/assets/textures/tiles.bmp");
    GLuint metalTexture = loadTexture("../Assignment 2/assets/textures/metal.jpg");
    GLuint boxTexture = loadTexture("../Assignment 2/assets/textures/box.jpg");
#endif

	// defines vertices and color for the cube
    TexturedColoredVertex texturedCubeVertexArray[] = {

		// cube made of 12 triangles

		                      // postion				 // color                   // Texture UV           // normal
        TexturedColoredVertex(vec3(0.0f, 0.0f, 0.0f),    vec3(0.5f, 0.5f, 0.5f),    vec2(0.0f, 0.0f),    vec3(0.0f, 0.0f, -1.0f)),
        TexturedColoredVertex(vec3(0.1f, 0.0f, 0.0f),    vec3(0.5f, 0.5f, 0.5f),    vec2(0.0f, 1.0f),    vec3(0.0f, 0.0f, -1.0f)),
        TexturedColoredVertex(vec3(0.1f, 0.1f, 0.0f),    vec3(0.4f, 0.4f, 0.4f),    vec2(1.0f, 1.0f),    vec3(0.0f, 0.0f, -1.0f)),
                                                         
        TexturedColoredVertex(vec3(0.0f, 0.0f, 0.0f),    vec3(0.5f, 0.5f, 0.5f),    vec2(0.0f, 0.0f),    vec3(0.0f, 0.0f, -1.0f)),
        TexturedColoredVertex(vec3(0.1f, 0.1f, 0.0f),    vec3(0.4f, 0.4f, 0.4f),    vec2(1.0f, 1.0f),    vec3(0.0f, 0.0f, -1.0f)),
        TexturedColoredVertex(vec3(0.0f, 0.1f, 0.0f),    vec3(0.25f, 0.25f, 0.25f), vec2(1.0f, 0.0f),    vec3(0.0f, 0.0f, -1.0f)),
                                                         
        TexturedColoredVertex(vec3(0.1f, 0.0f, 0.0f),    vec3(0.5f, 0.5f, 0.5f),    vec2(1.0f, 1.0f),    vec3(1.0f, 0.0f, 0.0f)),
        TexturedColoredVertex(vec3(0.1f, 0.0f, 0.1f),    vec3(0.7f, 0.7f, 0.7f),    vec2(0.0f, 0.0f),    vec3(1.0f, 0.0f, 0.0f)),
        TexturedColoredVertex(vec3(0.1f, 0.1f, 0.0f),    vec3(0.4f, 0.4f, 0.4f),    vec2(0.0f, 1.0f),    vec3(1.0f, 0.0f, 0.0f)),
                                                         
        TexturedColoredVertex(vec3(0.1f, 0.1f, 0.0f),    vec3(0.4f, 0.4f, 0.4f),    vec2(1.0f, 1.0f),    vec3(1.0f, 0.0f, 0.0f)),
        TexturedColoredVertex(vec3(0.1f, 0.0f, 0.1f),    vec3(0.7f, 0.7f, 0.7f),    vec2(1.0f, 0.0f),    vec3(1.0f, 0.0f, 0.0f)),
        TexturedColoredVertex(vec3(0.1f, 0.1f, 0.1f),    vec3(0.7f, 0.7f, 0.7f),    vec2(0.0f, 0.0f),    vec3(1.0f, 0.0f, 0.0f)),
                                                         
        TexturedColoredVertex(vec3(0.0f, 0.0f, 0.1f),    vec3(0.2f, 0.2f, 0.2f),    vec2(1.0f, 1.0f),    vec3(0.0f, 0.0f, 1.0f)),
        TexturedColoredVertex(vec3(0.1f, 0.1f, 0.1f),    vec3(0.7f, 0.7f, 0.7f),    vec2(0.0f, 0.0f),    vec3(0.0f, 0.0f, 1.0f)),
        TexturedColoredVertex(vec3(0.1f, 0.0f, 0.1f),    vec3(0.7f, 0.7f, 0.7f),    vec2(1.0f, 0.0f),    vec3(0.0f, 0.0f, 1.0f)),
                                                         
        TexturedColoredVertex(vec3(0.0f, 0.0f, 0.1f),    vec3(0.2f, 0.2f, 0.2f),    vec2(1.0f, 1.0f),    vec3(0.0f, 0.0f, 1.0f)),
        TexturedColoredVertex(vec3(0.0f, 0.1f, 0.1f),    vec3(0.4f, 0.4f, 0.4f),    vec2(0.0f, 1.0f),    vec3(0.0f, 0.0f, 1.0f)),
        TexturedColoredVertex(vec3(0.1f, 0.1f, 0.1f),    vec3(0.7f, 0.7f, 0.7f),    vec2(0.0f, 0.0f),    vec3(0.0f, 0.0f, 1.0f)),
                                                         
        TexturedColoredVertex(vec3(0.0f, 0.0f, 0.0f),    vec3(0.5f, 0.5f, 0.5f),    vec2(0.0f, 1.0f),    vec3(-1.0f, 0.0f, 0.0f)),
        TexturedColoredVertex(vec3(0.0f, 0.1f, 0.0f),    vec3(0.25f, 0.25f, 0.25f), vec2(0.0f, 0.0f),    vec3(-1.0f, 0.0f, 0.0f)),
        TexturedColoredVertex(vec3(0.0f, 0.0f, 0.1f),    vec3(0.2f, 0.2f, 0.2f),    vec2(1.0f, 0.0f),    vec3(-1.0f, 0.0f, 0.0f)),
                                                         
        TexturedColoredVertex(vec3(0.0f, 0.1f, 0.0f),    vec3(0.25f, 0.25f, 0.25f), vec2(1.0f, 1.0f),    vec3(-1.0f, 0.0f, 0.0f)),
        TexturedColoredVertex(vec3(0.0f, 0.1f, 0.1f),    vec3(0.4f, 0.4f, 0.4f),    vec2(0.0f, 1.0f),    vec3(-1.0f, 0.0f, 0.0f)),
        TexturedColoredVertex(vec3(0.0f, 0.0f, 0.1f),    vec3(0.2f, 0.2f, 0.2f),    vec2(1.0f, 0.0f),    vec3(-1.0f, 0.0f, 0.0f)),
                                                         
        TexturedColoredVertex(vec3(0.0f, 0.0f, 0.0f),    vec3(0.5f, 0.5f, 0.5f),    vec2(1.0f, 1.0f),    vec3(0.0f, -1.0f, 0.0f)),
        TexturedColoredVertex(vec3(0.1f, 0.0f, 0.1f),    vec3(0.7f, 0.7f, 0.7f),    vec2(0.0f, 0.0f),    vec3(0.0f, -1.0f, 0.0f)),
        TexturedColoredVertex(vec3(0.1f, 0.0f, 0.0f),    vec3(0.5f, 0.5f, 0.5f),    vec2(1.0f, 0.0f),    vec3(0.0f, -1.0f, 0.0f)),
                                                         
        TexturedColoredVertex(vec3(0.0f, 0.0f, 0.0f),    vec3(0.5f, 0.5f, 0.5f),    vec2(0.0f, 0.0f),    vec3(0.0f, -1.0f, 0.0f)),
        TexturedColoredVertex(vec3(0.0f, 0.0f, 0.1f),    vec3(0.2f, 0.2f, 0.2f),    vec2(1.0f, 1.0f),    vec3(0.0f, -1.0f, 0.0f)),
        TexturedColoredVertex(vec3(0.1f, 0.0f, 0.1f),    vec3(0.7f, 0.7f, 0.7f),    vec2(0.0f, 1.0f),    vec3(0.0f, -1.0f, 0.0f)),
                                                         
        TexturedColoredVertex(vec3(0.0f, 0.1f, 0.0f),    vec3(0.25f, 0.25f, 0.25f), vec2(1.0f, 1.0f),    vec3(0.0f, 1.0f, 0.0f)),
        TexturedColoredVertex(vec3(0.1f, 0.1f, 0.0f),    vec3(0.4f, 0.4f, 0.4f),    vec2(1.0f, 0.0f),    vec3(0.0f, 1.0f, 0.0f)),
        TexturedColoredVertex(vec3(0.1f, 0.1f, 0.1f),    vec3(0.7f, 0.7f, 0.7f),    vec2(0.0f, 0.0f),    vec3(0.0f, 1.0f, 0.0f)),
                                                         
        TexturedColoredVertex(vec3(0.0f, 0.1f, 0.0f),    vec3(0.25f, 0.25f, 0.25f), vec2(1.0f, 1.0f),    vec3(0.0f, 1.0f, 0.0f)),
        TexturedColoredVertex(vec3(0.1f, 0.1f, 0.1f),    vec3(0.7f, 0.7f, 0.7f),    vec2(0.0f, 0.0f),    vec3(0.0f, 1.0f, 0.0f)),
        TexturedColoredVertex(vec3(0.0f, 0.1f, 0.1f),    vec3(0.4f, 0.4f, 0.4f),    vec2(0.0f, 1.0f),    vec3(0.0f, 1.0f, 0.0f))
	}; 
	

	
	

	glm::vec3 pointLightSource[]{
		// position					// color
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f),
	};

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

	VAO[5] = createPointLightSource(pointLightSource, sizeof(pointLightSource));

	// bind to nothing so we don't inadvertantly modify something
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

    // set global matrices for each shader
	projectionMatrix = glm::perspective(90.0f, 1024.0f / 768.0f, 0.0005f, 500.0f);
    setProjectionMatrix(colorShaderProgram, projectionMatrix);
    setProjectionMatrix(texturedShaderProgram, projectionMatrix);
	setProjectionMatrix(lightShaderProgram, projectionMatrix);

	glm::mat4 viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
    setViewMatrix(colorShaderProgram, viewMatrix);
	setViewMatrix(texturedShaderProgram, viewMatrix);
    setViewMatrix(lightShaderProgram, viewMatrix);
	


	
	GLuint colorLocation = glGetUniformLocation(colorShaderProgram, "objectColor");
	GLuint textureLocation = glGetUniformLocation(texturedShaderProgram, "textureSampler");
	GLuint lightLocation = glGetUniformLocation(lightShaderProgram, "lightPos");
	glUseProgram(lightShaderProgram);
	
	


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

    // @TODO (Dan): put the model parameters into the array and update control function
    typedef struct ModelParameters {

        const float init_Size = 1.0f;
        float model_Size = 0.0f;
        glm::vec3 initPos = { 0.0f, 0.0f, 0.0f };
        glm::vec3 model_Position = { 0.0f, 0.0f, 0.0f };
        glm::vec3 theta = { 0.0f, 0.0f, 0.0f };
        glm::mat4 rotation_X = identityMatrix;
        glm::mat4 rotation_Y = identityMatrix;
        glm::mat4 rotation_Z = identityMatrix;
    };
	
    ModelParameters modelParamList[numbObjInScene];

	// initial model parameters
	
	// model A7											// model O9											// model S0											
    const float init_A7_Size = 1.0f;					const float init_O9_Size = 1.0f;					const float init_S0_Size = 1.0f;	
    float model_A7_Size = 0.0f;							float model_O9_Size = 0.0f;							float model_S0_Size = 0.0f;
    glm::vec3 initPos_A7(-4.0f, 0.5f, -4.0f);			glm::vec3 initPos_O9(4.0f, 0.5f, -4.0f);			glm::vec3 initPos_S0(-4.0f, 0.5f, 4.0f);
    glm::vec3 model_A7_Position(0.0f, 0.0f, 0.0f);		glm::vec3 model_O9_Position(0.0f, 0.0f, 0.0f);		glm::vec3 model_S0_Position(0.0f, 0.0f, 0.0f);
    glm::vec3 A7_theta(0.0f, 0.0f, 0.0f);				glm::vec3 O9_theta(0.0f, 0.0f, 0.0f);				glm::vec3 S0_theta(0.0f, 0.0f, 0.0f);				
    glm::mat4 A7_rotation_X;							glm::mat4 O9_rotation_X;							glm::mat4 S0_rotation_X;							
    glm::mat4 A7_rotation_Y;							glm::mat4 O9_rotation_Y;							glm::mat4 S0_rotation_Y;							
    glm::mat4 A7_rotation_Z;							glm::mat4 O9_rotation_Z;							glm::mat4 S0_rotation_Z;							
	
    // model M6                                         // model N7										    // model ground									
    const float init_M6_Size = 1.0f;                    const float init_N7_Size = 1.0f;                    const float init_ground_Size = 1.0f;
    float model_M6_Size = 0.0f;                         float model_N7_Size = 0.0f;                         float model_ground_Size = 1.0f;
    glm::vec3 initPos_M6(4.0f, 0.5f, 4.0f);             glm::vec3 initPos_N7(0.0f, 0.3f, 0.0f);             glm::vec3 initPos_ground(0.0f, 0.0f, 0.0f);
    glm::vec3 model_M6_Position(0.0f, 0.0f, 0.0f);      glm::vec3 model_N7_Position(0.0f, 0.0f, 0.0f);      glm::vec3 model_ground_Position(0.0f, 0.0f, 0.0f);
    glm::vec3 M6_theta(0.0f, 0.0f, 0.0f);               glm::vec3 N7_theta(0.0f, 0.0f, 0.0f);               glm::vec3 ground_theta(0.0f, 0.0f, 0.0f);
    glm::mat4 M6_rotation_X;                            glm::mat4 N7_rotation_X;                            glm::mat4 ground_rotation_X;
    glm::mat4 M6_rotation_Y;                            glm::mat4 N7_rotation_Y;                            glm::mat4 ground_rotation_Y;
    glm::mat4 M6_rotation_Z;                            glm::mat4 N7_rotation_Z;                            glm::mat4 ground_rotation_Z;

	const float ANGLE = 5.0f; // set rotation snap to 5 degrees

    Sphere sphere1(25.0f, 36, 18, false);    // radius, sectors, stacks, non-smooth (flat) shading

	// render loop
	while (!glfwWindowShouldClose(window))
	{

		//  background color
		//	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClearColor(0.28f, 0.68f, 0.82f, 1.0f);	// sky blue color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glPointSize(10);		// increased point size so that it is easily visible
		glLineWidth(2);
		glBindVertexArray(VAO[0]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);

		deltaTime = glfwGetTime() - lastFrameTime;
		lastFrameTime += deltaTime;

		
		glUniform3fv(glGetUniformLocation(texturedShaderProgram, "objectColor"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
		glUniform3fv(glGetUniformLocation(texturedShaderProgram, "lightColor"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
		glUniform3fv(glGetUniformLocation(texturedShaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
		glUniform3fv(glGetUniformLocation(texturedShaderProgram, "viewPos"), 1, glm::value_ptr(cameraPosition));

		

        // use texturedShaderProgram for the textured models
        if (textures_on) {
            textureLocation = glGetUniformLocation(texturedShaderProgram, "textureSampler");
            glUseProgram(texturedShaderProgram);
            glActiveTexture(GL_TEXTURE0); // this is the GL "reset" texture, use to flush previous texture
        }
        else {
            colorLocation = glGetUniformLocation(colorShaderProgram, "objectColor");
            glUseProgram(colorShaderProgram);
        }
		
		
        modelControl(window, &texturedShaderProgram, init_ground_Size, model_ground_Size, initPos_ground, model_ground_Position, ground_theta, ground_rotation_X, ground_rotation_Y, ground_rotation_Z, ANGLE, false);
		//glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(0.5f, 1.0f, 0.5f)));
		//glUniform3fv(glGetUniformLocation(texturedShaderProgram, "objectColor"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
		model_ground(10.0f, textureLocation, groundTexture, textures_on);

		// beginning of model A7
		modelControl(window, &texturedShaderProgram, init_A7_Size, model_A7_Size, initPos_A7, model_A7_Position, A7_theta, A7_rotation_X, A7_rotation_Y, A7_rotation_Z, ANGLE, ONE_KEY_PRESSED);
		//glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.0f)));
		//glUniform3fv(glGetUniformLocation(texturedShaderProgram, "objectColor"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
		model_A7(textureLocation, boxTexture, metalTexture, textures_on);
		// end of model_A7();

		

		// beginning of model O9
		modelControl(window, &texturedShaderProgram, init_O9_Size, model_O9_Size, initPos_O9, model_O9_Position, O9_theta, O9_rotation_X, O9_rotation_Y, O9_rotation_Z, ANGLE, TWO_KEY_PRESSED);
		//glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(1.0f, 0.60f, 0.90f)));
		//glUniform3fv(glGetUniformLocation(texturedShaderProgram, "objectColor"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
		model_O9(textureLocation, boxTexture, metalTexture, textures_on);
		// end of model O9

		// beginning of model S0
		modelControl(window, &texturedShaderProgram, init_S0_Size, model_S0_Size, initPos_S0, model_S0_Position, S0_theta, S0_rotation_X, S0_rotation_Y, S0_rotation_Z, ANGLE, THREE_KEY_PRESSED);
		//glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.5f)));
		//glUniform3fv(glGetUniformLocation(texturedShaderProgram, "objectColor"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
		model_S0(textureLocation, boxTexture, metalTexture, textures_on);
		// end of model S0

		// beginning of model M6
		modelControl(window, &texturedShaderProgram, init_M6_Size, model_M6_Size, initPos_M6, model_M6_Position, M6_theta, M6_rotation_X, M6_rotation_Y, M6_rotation_Z, ANGLE, FOUR_KEY_PRESSED);
		//glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(1.0f, 0.0f, 0.0f)));
		//glUniform3fv(glGetUniformLocation(texturedShaderProgram, "objectColor"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
		model_M6(textureLocation, boxTexture, metalTexture, textures_on);
		// end of model M6

        // beginning of model N7
		modelControl(window, &texturedShaderProgram, init_N7_Size, model_N7_Size, initPos_N7, model_N7_Position, N7_theta, N7_rotation_X, N7_rotation_Y, N7_rotation_Z, ANGLE, FIVE_KEY_PRESSED);
		//glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(0.0f, 1.0f, 1.0f)));
		//glUniform3fv(glGetUniformLocation(texturedShaderProgram, "objectColor"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
		model_N7(textureLocation, boxTexture, metalTexture, textures_on);
        // end of moedel N7

		worldMatrix = worldOrientationMatrix;

		


		drawAxisLines(texturedShaderProgram);
		
		
		drawGridlines();
		
		// light
		glBindVertexArray(VAO[5]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);
		worldMatrix = glm::translate(glm::mat4(1.0f), lightPos);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_POINTS, 0, 1);

		worldMatrix = identityMatrix;
		
		
		worldMatrix = worldOrientationMatrix;

        
	  
		
		// teardown: check and call events and swap the buffers
		glBindVertexArray(0);
		glfwSwapBuffers(window);
		glfwPollEvents();

		// Detect inputs
		processInput(window, texturedShaderProgram);

	}

	// de-allocate all resources
	glDeleteVertexArrays(1, VAO);
	glDeleteBuffers(1, VBO);
	glDeleteProgram(colorShaderProgram);
    glDeleteProgram(texturedShaderProgram);
	glDeleteProgram(lightShaderProgram);

	glfwTerminate();
	return 0;
}


// resize window 
void framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow * window, GLuint shaderProgram)
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

		GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
		glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
	}

	// orthographic - projection transform
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		projectionMatrix = glm::ortho(-4.0f, 4.0f,      // left/right
			-3.0f, 3.0f,	  // bottom/top
			-100.0f, 100.0f);  // near/far 

		GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
		glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
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
		GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
		
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "viewMatrix"), 1, GL_FALSE, &viewMatrix[0][0]);
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

		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

		viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
		GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);

	}


	// mouse scroll
	glfwSetScrollCallback(window, scroll_callback); // receives mouse scroll as input
   
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

	if ((DOWN_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_DOWN))) {

		worldRotation.y -= glm::radians(worldOrientation_ANGLE);

		worldOrientation_Y = {
								  cos(worldRotation.y),  0, sin(worldRotation.y), 0,
											 0,  1,            0, 0,
								 -sin(worldRotation.y),  0, cos(worldRotation.y), 0,
											 0,  0,            0, 1,
		};
		worldOrientationMatrix = worldOrientation_X * worldOrientation_Y;
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	}	DOWN_KEY = glfwGetKey(window, GLFW_KEY_DOWN);

	if ((UP_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_UP))) {

		worldRotation.y += glm::radians(worldOrientation_ANGLE);

		worldOrientation_Y = {
								  cos(worldRotation.y),  0, sin(worldRotation.y), 0,
											 0,  1,            0, 0,
								 -sin(worldRotation.y),  0, cos(worldRotation.y), 0,
											 0,  0,            0, 1,
		};
		worldOrientationMatrix = worldOrientation_X * worldOrientation_Y;
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	}	UP_KEY = glfwGetKey(window, GLFW_KEY_UP);

	if ((LEFT_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_LEFT))) {

		worldRotation.x -= glm::radians(worldOrientation_ANGLE);

		worldOrientation_X = {
								  1,            0,             0, 0,
								  0, cos(worldRotation.x), -sin(worldRotation.x), 0,
								  0, sin(worldRotation.x),  cos(worldRotation.x), 0,
								  0,            0,             0, 1,
		};
		worldOrientationMatrix = worldOrientation_X * worldOrientation_Y;
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	}	LEFT_KEY = glfwGetKey(window, GLFW_KEY_LEFT);



	if ((RIGHT_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_RIGHT))) {

		worldRotation.x += glm::radians(worldOrientation_ANGLE);

		worldOrientation_X = {
								  1,            0,             0, 0,
								  0, cos(worldRotation.x), -sin(worldRotation.x), 0,
								  0, sin(worldRotation.x),  cos(worldRotation.x), 0,
								  0,            0,             0, 1,
		};
		worldOrientationMatrix = worldOrientation_X * worldOrientation_Y;
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	}	RIGHT_KEY = glfwGetKey(window, GLFW_KEY_RIGHT);

	
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
					0.05f, 500.0f);	// near and far (near > 0)

				GLuint projectionMatrixLocation = glGetUniformLocation(compileAndLinkShaders(getTexturedVertexShaderSource(), getTexturedFragmentShaderSource()), "projectionMatrix");
				glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
			}
		}
		// scroll down to zoom out 
		if (yoffset > 0) {
			if (fov >= 10.0f)
			{
				fov = fov - 5.0;
				projectionMatrix = glm::perspective(glm::radians(fov),			// field of view in degrees
					1024.0f / 768.0f,	// aspect ratio
					0.05f, 500.0f);	// near and far (near > 0)

				GLuint projectionMatrixLocation = glGetUniformLocation(compileAndLinkShaders(getTexturedVertexShaderSource(), getTexturedFragmentShaderSource()), "projectionMatrix");
				glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
			}
		}
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (action == GLFW_RELEASE) return;

	if (key == GLFW_KEY_TAB)
		TAB_KEY = !TAB_KEY;
}


void model_A7(GLuint textureLocation, GLuint texture_1, GLuint texture_2, bool isTextureOn)
{
	// set object origin position
	objectTranslationtionMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.20f, 0.0));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);


    // Start of A
    if (isTextureOn) {
        glBindTexture(GL_TEXTURE_2D, texture_1);
        glUniform1i(textureLocation, 0);                // Set our Texture sampler to user Texture Unit 0
    }

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	shearingMatrix =
	{
		1.0, 0.0, 0.0, 0.0,
		0.35, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0,
	};
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	shearingMatrix =
	{
		1.0, 0.0, 0.0, 0.0,
		-0.35, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0,
	};
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.4f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	shearingMatrix = glm::mat4(1.0f);
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 0.75f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.15f, 0.15f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

    // Start of 7
    if (isTextureOn) {
        glBindTexture(GL_TEXTURE_2D, texture_2);
        glUniform1i(textureLocation, 0);                // Set our Texture sampler to user Texture Unit 0
    }

	shearingMatrix =
	{
		1.0, 0.0, 0.0, 0.0,
		0.3, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0,
	};
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 4.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.7f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	shearingMatrix = glm::mat4(1.0f);
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 1.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.62f, 0.4f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
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

void model_O9(GLuint textureLocation, GLuint texture_1, GLuint texture_2, bool isTextureOn) {

	// set object origin position
	objectTranslationtionMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.20f, 0.0));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

	// beginning of letter O
    if (isTextureOn) {
        glBindTexture(GL_TEXTURE_2D, texture_1);
        glUniform1i(textureLocation, 0);                // Set our Texture sampler to user Texture Unit 0
    }

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.4f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 0.75f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.1f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 0.75f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.1f, 0.425f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	// end of letter O

	// beginning of number 9

    if (isTextureOn) {
        glBindTexture(GL_TEXTURE_2D, texture_2);
        glUniform1i(textureLocation, 0);                // Set our Texture sampler to user Texture Unit 0
    }

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(4.0f, 0.75f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.62f, 0.425f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 0.75f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.62f, 0.175f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 0.75f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.62f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//vertical
	//right
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.92f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.75f, -2.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.62f, 0.425f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
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

void model_S0(GLuint textureLocation, GLuint texture_1, GLuint texture_2, bool isTextureOn) {

	// set object origin position
	objectTranslationtionMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.3f, 0.0));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

	// S
    if (isTextureOn) {
        glBindTexture(GL_TEXTURE_2D, texture_1);
        glUniform1i(textureLocation, 0);                // Set our Texture sampler to user Texture Unit 0
    }

	shearingMatrix = glm::mat4(1.0f);
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(4.0f, 1.0f, 1.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	shearingMatrix = glm::mat4(1.0f);
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.1f, -0.2f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.3f, 0.0f));
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 1.0f, 1.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	shearingMatrix = glm::mat4(1.0f);
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.3, -0.5f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.1f, -0.6f, 0.0f));
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(4.0f, 1.0f, 1.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	// ZERO
    if (isTextureOn) {
        glBindTexture(GL_TEXTURE_2D, texture_2);
        glUniform1i(textureLocation, 0);                // Set our Texture sampler to user Texture Unit 0
    }

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	shearingMatrix = glm::mat4(1.0f);
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.6f, -0.5f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	shearingMatrix = glm::mat4(1.0f);
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, -0.5f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 1.0f, 1.0f));
	shearingMatrix = glm::mat4(1.0f);
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.7f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 1.0f, 1.0f));
	shearingMatrix = glm::mat4(1.0f);
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.7f, -0.6f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
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

void model_M6(GLuint textureLocation, GLuint texture_1, GLuint texture_2, bool isTextureOn) {

	// set object origin position
	objectTranslationtionMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.2f, 0.0));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

	//draw letter M

    if (isTextureOn) {
        glBindTexture(GL_TEXTURE_2D, texture_1);
        glUniform1i(textureLocation, 0);                // Set our Texture sampler to user Texture Unit 0
    }

	//left upright for letter M
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//left diagonal for letter M
	shearingMatrix =
	{
		1.0, 0.0, 0.0, 0.0,
		0.35, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0,
	};
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.2f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//right diagonal for letter M
	shearingMatrix =
	{
		1.0, 0.0, 0.0, 0.0,
		-0.35, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0,
	};
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	//translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//right upright for letter M
	shearingMatrix = glm::mat4(1.0f);
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.4f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	//end of M

	//draw 6
    if (isTextureOn) {
        glBindTexture(GL_TEXTURE_2D, texture_2);
        glUniform1i(textureLocation, 0);                // Set our Texture sampler to user Texture Unit 0
    }

		//vertical segments
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.8f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.6f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//horizontal segments
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 1.0f, 1.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 1.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.6f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 1.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.6f, 0.4f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	//end 6	

	// reset world matrix, rotation matrix, and model scaling matrix after we're done with it for this object.
	// also reset shearing matrix and scaling matrix, so that they work correctly with model_A7().
	shearingMatrix =
	{
		1.0, 0.0, 0.0, 0.0,
		0.35, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0,
	};
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	modelScalingMatrix = identityMatrix;
	modelRotationMatrix = identityMatrix;
	modelShearingMatrix = identityMatrix;
	worldMatrix = identityMatrix;

    if (isTextureOn)
        glActiveTexture(GL_TEXTURE0); // this is the GL "reset" texture, use to flush previous texture
}

void model_N7(GLuint textureLocation, GLuint texture_1, GLuint texture_2, bool isTextureOn)
{
    const float letterSpacing = 0.25f;
    const float left_of_origin = -0.5f;

    translationMatrix = identityMatrix;
    shearingMatrix = identityMatrix;
    scalingMatrix = scale(identityMatrix, glm::vec3(1.0f, 5.0f, 1.0f));

    if (isTextureOn) {
        glBindTexture(GL_TEXTURE_2D, texture_1);
        glUniform1i(textureLocation, 0);                // Set our Texture sampler to user Texture Unit 0
    }

    // Left leg of 'N'
    translationMatrix[3][0] = left_of_origin;
    partMatrix = translationMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Right leg of 'N'
    translationMatrix[3][0] = left_of_origin + 0.35f;
    partMatrix = translationMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Diagonal of 'N'
    shearingMatrix[1][0] = -0.7f;
    partMatrix = translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    if (isTextureOn) {
        glBindTexture(GL_TEXTURE_2D, texture_2);
        glUniform1i(textureLocation, 0);                // Set our Texture sampler to user Texture Unit 0
    }

    // Diagonal base of '7'
    translationMatrix[3][0] = left_of_origin + 0.35f + letterSpacing;
    shearingMatrix[1][0] = 0.5f;
    partMatrix = translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Horizontal top of '7'
    scalingMatrix = scale(identityMatrix, vec3(1.0f, 2.5f, 1.0f));
    shearingMatrix[1][0] = 0.0f;
    translationMatrix[3][0] = left_of_origin + 0.6f + letterSpacing;
    translationMatrix[3][1] = 0.4f;
    mat4 seven_top_rotate = rotate(identityMatrix, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
    partMatrix = translationMatrix * seven_top_rotate * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);


    // reset world matrix, rotation matrix, and model scaling matrix after we're done with it for this object
    modelScalingMatrix = identityMatrix;
    modelRotationMatrix = identityMatrix;
	modelShearingMatrix = identityMatrix;
    worldMatrix = identityMatrix;

    if (isTextureOn)
        glActiveTexture(GL_TEXTURE0); // this is the GL "reset" texture, use to flush previous texture
}

void model_ground(float groundSize, GLuint textureLocation, GLuint texture, bool isTextureOn) {

    const float ground_final = 5 * groundSize;

    translationMatrix = identityMatrix;
    scalingMatrix = scale(identityMatrix, glm::vec3(ground_final, 0.0f, ground_final));

    if (isTextureOn) {
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(textureLocation, 0);                // Set our Texture sampler to user Texture Unit 0
    }

    translationMatrix[3][0] = -ground_final / 2 / 10;
    translationMatrix[3][2] = -ground_final / 2 / 10;
    partMatrix = translationMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    if (isTextureOn)
        glActiveTexture(GL_TEXTURE0); // this is the GL "reset" texture, use to flush previous texture
}

void modelControl(GLFWwindow* window, unsigned int* shaderProgram, const float& initSize, float& modelSize, glm::vec3& initPos,
	glm::vec3& modelPos, glm::vec3& rotation, glm::mat4& rotX, glm::mat4& rotY, glm::mat4& rotZ, const float ANGLE, bool KEY_PRESSED) {

	rotation = { rotation.x, rotation.y, rotation.z };
	modelRotationMatrix = rotX * rotY * rotZ;
	modelTranslationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(initPos.x + modelPos.x, initPos.y + modelPos.y, initPos.z + modelPos.z));
	modelScalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(modelSize + initSize, modelSize + initSize, modelSize + initSize));
	worldMatrixLocation = glGetUniformLocation(*shaderProgram, "worldMatrix");
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

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
			std::cout << "Model position randomized - X: " << randomPos_X << " Z: " << randomPos_Z << std::endl;
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

		rotX = {
								  1,            0,             0, 0,
								  0, cos(rotation.x), -sin(rotation.x), 0,
								  0, sin(rotation.x),  cos(rotation.x), 0,
								  0,            0,             0, 1,
		};

		rotY = {
								  cos(rotation.y),  0, sin(rotation.y), 0,
											 0,  1,            0, 0,
								 -sin(rotation.y),  0, cos(rotation.y), 0,
											 0,  0,            0, 1,
		};

		rotZ = {
					   cos(rotation.z), -sin(rotation.z), 0, 0,
					   sin(rotation.z),  cos(rotation.z), 0, 0,
								  0,             0, 1, 0,
								  0,             0, 0, 1,
		};


		modelTranslationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(initPos.x + modelPos.x, initPos.y + modelPos.y, initPos.z + modelPos.z));
		modelScalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(modelSize + initSize, modelSize + initSize, modelSize + initSize));
		modelRotationMatrix = rotX * rotY * rotZ;
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);


	}


}

void drawAxisLines(GLuint shaderProgram) {
	
	
	GLuint textureLocation = glGetUniformLocation(shaderProgram, "objectColor");
    // red line
    glLineWidth(5);
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glUniform3fv(textureLocation, 1, glm::value_ptr(glm::vec3(1.0f, 0.0f, 0.0f)));
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
    glDrawArrays(GL_LINES, 0, 2);

    // green line
    glBindVertexArray(VAO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glUniform3fv(textureLocation, 1, glm::value_ptr(glm::vec3(0.0f, 1.0f, 0.0f)));
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
    glDrawArrays(GL_LINES, 0, 2);

    // blue line
    glBindVertexArray(VAO[3]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
	glUniform3fv(textureLocation, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 1.0f)));
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_LINES, 0, 2);
}

void drawGridlines() {
    glLineWidth(1);
    glBindVertexArray(VAO[4]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
    glDrawArrays(GL_LINES, 0, 2 * 2 * numGridLines);
}


void setProjectionMatrix(int shaderProgram, mat4 projectionMatrix)
{
    glUseProgram(shaderProgram);
    GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
}

void setViewMatrix(int shaderProgram, mat4 viewMatrix)
{
    glUseProgram(shaderProgram);
    GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
}

void setWorldMatrix(int shaderProgram, mat4 worldMatrix)
{
    glUseProgram(shaderProgram);
    GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
}


int compileAndLinkShaders(const char* vertexShaderSource, const char* fragmentShaderSource)
{
    // compile and link shader program
    // return shader program id
    // ------------------------------------

    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

GLuint loadTexture(const char *filename)
{
    // Step1 Create and bind textures
    GLuint textureId = 0;
    glGenTextures(1, &textureId);
    assert(textureId != 0);


    glBindTexture(GL_TEXTURE_2D, textureId);

    // Step2 Set filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Step3 Load Textures with dimension data
    int width, height, nrChannels;
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (!data)
    {
        std::cerr << "Error::Texture could not load texture file:" << filename << std::endl;
        return 0;
    }

    // Step4 Upload the texture to the PU
    GLenum format = 0;
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,
        0, format, GL_UNSIGNED_BYTE, data);

    // Step5 Free resources
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureId;
}


int createGridlines(int numGridlines, vec3* gridLinesVertices, GLuint size, vec3 lineColor, float sizeOfGrid) {

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


int createTexturedCubeVertexArrayObject(TexturedColoredVertex* texturedCubeVertexArray, GLuint size) {

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
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedColoredVertex), (void*)sizeof(vec3));       // colour rgb
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedColoredVertex), (void*)(2 * sizeof(vec3)));   // texture uv
    glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedColoredVertex), (void*)(3 * sizeof(vec3)));       // normal
	glEnableVertexAttribArray(3);

    return vertexArrayObject;
}

int createPointLightSource(glm::vec3* lightSource, GLuint size) {
	
	GLuint vertexArrayObject;
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	GLuint vertexBufferObject;
	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, size, lightSource, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)sizeof(vec3));
	glEnableVertexAttribArray(1);

	return vertexArrayObject;
}


int createLine(vec3* lineVertices) {

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