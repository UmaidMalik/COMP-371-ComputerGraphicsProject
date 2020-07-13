/*
*	COMP 371 - Assignment 1
*
*	Umaid Malik 27576110
*	(name & id)
*	(name & id)
*	(name & id)
*	(name & id)
*
*	CONTROLS:
*
*		X		: MOVE NEGATIVE Y-DIRECTION
*
*		SPACE	: MOVE POSITIVE Y-DIRECTION
*
*		Q		: PERSPECTIVE PROJECTION
*
*		E		: ORTHOGRAPHIC PROJECTION
*
*		T		: RENDER TRIANGLES
*
*		L		: RENDER LINES
*
*		P		: RENDER POINTS
*
*		C		: HOLD TO DISABLE BACKFACE CULLING
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
*		SHIFT	: INCREASE MOVEMENT SPEED
*
*		==============================================================
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


const int numbObjInScene = 5;

const int numGridLines = 100;                           // how many gridlines (going one way)

glm::vec3 initialCameraPosition(0.6f, 1.0f, 1.5f);
glm::vec3 initialcameraLookAt(0.0f, 0.0f, -1.0f);
const glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

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
glm::mat4 partMatrix;




float cameraHorizontalAngle = 90.0f;
float cameraVerticalAngle = 0.0f;
float cameraSpeed = 1.0f;
float cameraSpeedFast = 4 * cameraSpeed;
float deltaTime;
float fov = 45.0f;
double lastMousePosX, lastMousePosY;

bool ONE_KEY_PRESSED;
bool TWO_KEY_PRESSED;
bool THREE_KEY_PRESSED;
bool FOUR_KEY_PRESSED;
bool FIVE_KEY_PRESSED;

GLuint worldMatrixLocation;

// function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void model_A7();
GLFWwindow* setupWindow();


const char* getVertexShaderSource()
{
	return
		"#version 330 core\n"
		"layout (location = 0) in vec3 aPos;"
		"layout (location = 1) in vec3 aColor;"
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
		"out vec4 FragColor;"
		"in vec3 vertexColor;"
		"void main()"
		"{"
		" FragColor = vec4(vertexColor.r, vertexColor.g, vertexColor.b, 1.0f);"
		"}";
}

int compileAndLinkShaders()
{
	// compile and link shader program
	// return shader program id

	// vertex shader
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const char* vertexShaderSource = getVertexShaderSource();
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

	//fragment shader
	unsigned int fragmentShader;
	const char* fragmentShaderSource = getFragmentShaderSource();
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
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
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

GLFWwindow* setupWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TURE);
	glfwWindowHint(GL_DEPTH_TEST, 24);

	GLFWwindow* window = glfwCreateWindow(1024, 768, "COMP 371 - Team 6 - Part 1", NULL, NULL);
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
	int shaderProgram = compileAndLinkShaders();

	// defines vertices and color for the cube
	glm::vec3 vertexArray[] = {

		// cube made of 12 triangles

		// postion						// color
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(0.1f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(0.1f, 0.1f, 0.0f), glm::vec3(0.4f, 0.4f, 0.4f),

		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(0.1f, 0.1f, 0.0f), glm::vec3(0.4f, 0.4f, 0.4f),
		glm::vec3(0.0f, 0.1f, 0.0f), glm::vec3(0.25f, 0.25f, 0.25f),

		glm::vec3(0.1f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(0.1f, 0.0f, 0.1f), glm::vec3(0.7f, 0.7f, 0.7f),
		glm::vec3(0.1f, 0.1f, 0.0f), glm::vec3(0.4f, 0.4f, 0.4f),

		glm::vec3(0.1f, 0.1f, 0.0f), glm::vec3(0.4f, 0.4f, 0.4f),
		glm::vec3(0.1f, 0.0f, 0.1f), glm::vec3(0.7f, 0.7f, 0.7f),
		glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.7f, 0.7f, 0.7f),

		glm::vec3(0.0f, 0.0f, 0.1f), glm::vec3(0.20f, 0.20f, 0.20f),
		glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.7f, 0.7f, 0.7f),
		glm::vec3(0.1f, 0.0f, 0.1f), glm::vec3(0.7f, 0.7f, 0.7f),

		glm::vec3(0.0f, 0.0f, 0.1f), glm::vec3(0.20f, 0.20f, 0.20f),
		glm::vec3(0.0f, 0.1f, 0.1f), glm::vec3(0.4f, 0.4f, 0.4f),
		glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.7f, 0.7f, 0.7f),

		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(0.0f, 0.1f, 0.0f), glm::vec3(0.25f, 0.25f, 0.25f),
		glm::vec3(0.0f, 0.0f, 0.1f), glm::vec3(0.20f, 0.20f, 0.20f),

		glm::vec3(0.0f, 0.1f, 0.0f), glm::vec3(0.25f, 0.25f, 0.25f),
		glm::vec3(0.0f, 0.1f, 0.1f), glm::vec3(0.4f, 0.4f, 0.4f),
		glm::vec3(0.0f, 0.0f, 0.1f), glm::vec3(0.20f, 0.20f, 0.20f),

		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(0.1f, 0.0f, 0.1f), glm::vec3(0.7f, 0.7f, 0.7f),
		glm::vec3(0.1f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f),

		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(0.0f, 0.0f, 0.1f), glm::vec3(0.20f, 0.20f, 0.20f),
		glm::vec3(0.1f, 0.0f, 0.1f), glm::vec3(0.7f, 0.7f, 0.7f),

		glm::vec3(0.0f, 0.1f, 0.0f), glm::vec3(0.25f, 0.25f, 0.25f),
		glm::vec3(0.1f, 0.1f, 0.0f), glm::vec3(0.4f, 0.4f, 0.4f),
		glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.7f, 0.7f, 0.7f),

		glm::vec3(0.0f, 0.1f, 0.0f), glm::vec3(0.25f, 0.25f, 0.25f),
		glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.7f, 0.7f, 0.7f),
		glm::vec3(0.0f, 0.1f, 0.1f), glm::vec3(0.4f, 0.4f, 0.4f),
	};

	const float axisLineLength = 5 * (10.0f / (float)numGridLines);  // axis lines are the length of n grid squares

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
	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);	// position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));   // color attribute
	glEnableVertexAttribArray(1);


	// red line
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(redLine), redLine, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));
	glEnableVertexAttribArray(1);


	// green line
	glBindVertexArray(VAO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(greenLine), greenLine, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));
	glEnableVertexAttribArray(1);


	// blue line
	glBindVertexArray(VAO[3]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(blueLine), blueLine, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));
	glEnableVertexAttribArray(1);

	//glBindBuffer(GL_ARRAY_BUFFER, 0);


	// grid lines


	const float spacing = 10.0f / (float)numGridLines;       // divide the 2.0 world into the number of gridlines
	float increment = 0.0f;                                 // how much to move a line over
	const int numDataPoints = 8;                            // this is how many vec3's there are in one gridline (4 vertices with 1 color each)
	glm::vec3 zLineColor = glm::vec3(1.0f, 0.0f, 1.0f);     // set line color for lines running parallel to z-axis
	glm::vec3 xLineColor = glm::vec3(1.0f, 1.0f, 0.0f);     // set line color for lines running parallel to x-axis
	glm::vec3 gridLines[numDataPoints * numGridLines];


	for (int i = 0; i < numGridLines; ++i) {
		// lines parallel to z-axis
		gridLines[i * numDataPoints] = glm::vec3(-5.0f + increment, 0.0f, -5.0f);
		gridLines[i * numDataPoints + 1] = glm::vec3(zLineColor.x, zLineColor.y, zLineColor.z);
		gridLines[i * numDataPoints + 2] = glm::vec3(-5.0f + increment, 0.0f, 5.0f);
		gridLines[i * numDataPoints + 3] = glm::vec3(zLineColor.x, zLineColor.y, zLineColor.z);
		// lines parallel to x-axis
		gridLines[i * numDataPoints + 4] = glm::vec3(-5.0f, 0.0f, -5.0f + increment);
		gridLines[i * numDataPoints + 5] = glm::vec3(xLineColor.x, xLineColor.y, xLineColor.z);
		gridLines[i * numDataPoints + 6] = glm::vec3(5.0f, 0.0f, -5.0f + increment);
		gridLines[i * numDataPoints + 7] = glm::vec3(xLineColor.x, xLineColor.y, xLineColor.z);

		increment += spacing;
	}

	glBindVertexArray(VAO[4]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gridLines), gridLines, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));
	glEnableVertexAttribArray(1);


	// bind to nothing so we don't inadvertantly modify something
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glUseProgram(shaderProgram);

	projectionMatrix = glm::perspective(90.0f, 1024.0f / 768.0f, 0.0005f, 500.0f);

	GLuint projectionMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "projectionMatrix");
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);


	viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);

	GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);

	// Variables to be used later in tutorial
	float angle = 0.0f;
	float rotationSpeed = 90.0f;  // degrees per second
	float lastFrameTime = glfwGetTime();


	// read mouse position
	glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);


	// Enable Backface culling
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);     // clockwise polygons are considered as front-facing

	// z-Buffer
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// render loop
	while (!glfwWindowShouldClose(window))
	{
	
		//  background color
		//	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClearColor(0.28f, 0.68f, 0.82f, 1.0f);	// sky blue color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(VAO[0]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);


		deltaTime = glfwGetTime() - lastFrameTime;
		lastFrameTime += deltaTime;



		angle = (angle + rotationSpeed * deltaTime); // angles in degrees, but glm expects radians (conversion below)

		glm::mat4 modelTranslationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-4.0f, 0.5f, -4.0f));
		glm::mat4 modelScalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));


		// press 1 to select model A7
		bool ONE_KEY_CURRENTLY_PRESSED = glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS;
		if (ONE_KEY_PRESSED || ONE_KEY_CURRENTLY_PRESSED)
		{

			rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
			modelScalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 5.0f));
			worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
			ONE_KEY_PRESSED = ONE_KEY_CURRENTLY_PRESSED;
		}








		//rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));


		//model O 9
		worldMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "worldMatrix");
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		
		//O
		scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
		translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.20f, 0.0f));
		partMatrix = translationMatrix  * scalingMatrix;
		worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
		translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.1f, -0.20f, 0.0f));
		partMatrix = translationMatrix * scalingMatrix;
		worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 0.75f, 1.0f));
		translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.4f, -0.20f, 0.0f));
		partMatrix = translationMatrix * scalingMatrix;
		worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 0.75f, 1.0f));
		translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.4f, 0.225f, 0.0f));
		partMatrix = translationMatrix * scalingMatrix;
		worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//9
		
		scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(4.0f, 0.75f, 1.0f));
		translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.12f, 0.225f, 0.0f));
		partMatrix = translationMatrix * scalingMatrix;
		worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 0.75f, 1.0f));
		translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.12f, -0.025f, 0.0f));
		partMatrix = translationMatrix * scalingMatrix;
		worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 0.75f, 1.0f));
		translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.12f, -0.20f, 0.0f));
		partMatrix = translationMatrix * scalingMatrix;
		worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//vertical
		//right
		scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
		translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.42f, -0.20f, 0.0f));
		partMatrix = translationMatrix * scalingMatrix;
		worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.75f, -2.0f, 1.0f));
		translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.12f, 0.225f, 0.0f));
		partMatrix = translationMatrix * scalingMatrix;
		worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

	
		

		// reset world matrix after we're done with it for this object
		worldMatrix = glm::mat4(1.0f);

		// end of model A7


		// red line
		glLineWidth(5);
		glBindVertexArray(VAO[1]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_LINES, 0, 2);

		// green line
		glBindVertexArray(VAO[2]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_LINES, 0, 2);

		// blue line
		glBindVertexArray(VAO[3]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_LINES, 0, 2);

		// grid lines
		glLineWidth(1);
		glBindVertexArray(VAO[4]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_LINES, 0, 2 * 2 * numGridLines);



		// check and call events and swap the buffers
		glfwSwapBuffers(window);
		//glfwSwapInterval(0);
		glfwPollEvents();

		// Detect inputs
		processInput(window); // input



		// press 2 to select model (your letter) 
		if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		{
			modelScalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
			rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
			worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
		}

		// press 3 to select model (your letter) 
		if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		{
			modelScalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
			rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
			worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
		}

		// press 4 to select model (your letter) 
		if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		{
			modelScalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
			rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
			worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
		}

		// press 5 to select model (your letter) 
		if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
		{
			modelScalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 5.0f));
			rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
			worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
		}

	}

	// de-allocate all resources
	glDeleteVertexArrays(5, VAO);
	glDeleteBuffers(5, VBO);
	glDeleteProgram(shaderProgram);


	glfwTerminate();
	return 0;
}



void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
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

	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{

		viewMatrix = glm::lookAt(cameraPosition, cameraLookAt, cameraUp);	// eye, center, & up are global variables
		GLuint viewMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "viewMatrix");
		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
	}


	// perspective - projection transform
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		projectionMatrix = glm::perspective(90.0f,			// field of view in degrees
			1024.0f / 768.0f,	// aspect ratio
			0.05f, 500.0f);	// near and far (near > 0)

		GLuint projectionMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "projectionMatrix");
		glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
	}

	// orthographic - projection transform
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		projectionMatrix = glm::ortho(-4.0f, 4.0f,      // left/right
			-3.0f, 3.0f,	  // bottom/top
			-100.0f, 100.0f);  // near/far (near == 0 is ok for ortho)

		GLuint projectionMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "projectionMatrix");
		glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
	}


	// Press and hold C to disable culling
	glEnable(GL_CULL_FACE);
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !GLFW_RELEASE)
	{
		glDisable(GL_CULL_FACE);
	}


	bool fastCam = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
	float currentCameraSpeed = (fastCam) ? cameraSpeedFast : cameraSpeed;

	if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) || (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != GLFW_RELEASE)) {

		// disable mouse cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		// first person camera
		double mousePosX, mousePosY;
		glfwGetCursorPos(window, &mousePosX, &mousePosY);

		// change in position
		double dx = mousePosX - lastMousePosX;
		double dy = mousePosY - lastMousePosY;

		lastMousePosX = mousePosX;
		lastMousePosY = mousePosY;

		// conversion to sperical coordinates
		const float cameraAngularSpeed = 5.0f;	// mouse speed
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

		if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) // move camera back
		{
			cameraPosition.y -= currentCameraSpeed * deltaTime;
		}


		viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
		GLuint viewMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "viewMatrix");
		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
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

		viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
		GLuint viewMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "viewMatrix");
		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);


		// reset camera orientation

	}

	// orientation control
	if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) || ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE)) == GLFW_PRESS))
	{
		viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
		GLuint viewMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "viewMatrix");
		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
	}

	// zoom control
	/*if (glfwSetScrollCallback(window, scroll_callback))
	{
		viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
		GLuint viewMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "viewMatrix");
		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
	}
	*/

	// Press = to Zoom In
	if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) {
		//std::cout << fov << std::endl;
		if (fov <= 91.6f)
		{
			fov = fov + 0.4;
			projectionMatrix = glm::perspective(glm::radians(fov),			// field of view in degrees
				1024.0f / 768.0f,	// aspect ratio
				0.05f, 500.0f);	// near and far (near > 0)

			GLuint projectionMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "projectionMatrix");
			glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
		}
	}
	// Press - to Zoom Out
	if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
		//std::cout << fov << std::endl;
		if (fov >= 0.4f)
		{
			fov = fov - 0.4;
			projectionMatrix = glm::perspective(glm::radians(fov),			// field of view in degrees
				1024.0f / 768.0f,	// aspect ratio
				0.05f, 500.0f);	// near and far (near > 0)

			GLuint projectionMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "projectionMatrix");
			glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
		}
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
	
		cameraVerticalAngle = cameraVerticalAngle + 3;
		float theta = glm::radians(cameraHorizontalAngle);
		float phi = glm::radians(cameraVerticalAngle);
		if (cameraVerticalAngle == 270)
		{
			theta = glm::radians(cameraHorizontalAngle+180.0f);
		}
		cameraLookAt = glm::vec3(cosf(phi) * cosf(theta), sinf(phi), cosf(phi) * sinf(theta)); 

		viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
		GLuint viewMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "viewMatrix");
		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
	}

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		
	
		cameraVerticalAngle = cameraVerticalAngle - 3;
		float theta = glm::radians(cameraHorizontalAngle);
		float phi = glm::radians(cameraVerticalAngle);

		cameraLookAt = glm::vec3(-cosf(phi) * cosf(theta), sinf(phi), -cosf(phi) * sinf(theta));

		viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
		GLuint viewMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "viewMatrix");
		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
		
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		
		cameraHorizontalAngle = cameraHorizontalAngle - 3;
		float theta = glm::radians(cameraHorizontalAngle);
		float phi = glm::radians(cameraVerticalAngle);

		cameraLookAt = glm::vec3(cosf(phi) * cosf(theta), sinf(phi), -cosf(phi) * sinf(theta));
		glm::vec3 cameraSideVector = glm::cross(cameraLookAt, glm::vec3(0.0f, 1.0f, 0.0f));

		glm::normalize(cameraSideVector);

		viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
		GLuint viewMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "viewMatrix");
		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		
		cameraHorizontalAngle = cameraHorizontalAngle + 3;
		float theta = glm::radians(cameraHorizontalAngle);
		float phi = glm::radians(cameraVerticalAngle);

		cameraLookAt = glm::vec3(cosf(phi) * cosf(theta), sinf(phi), -cosf(phi) * sinf(theta));
		glm::vec3 cameraSideVector = glm::cross(cameraLookAt, glm::vec3(0.0f, 1.0f, 0.0f));

		glm::normalize(cameraSideVector);

		viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
		GLuint viewMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "viewMatrix");
		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
	}
}	

// commented because I'm still trying to figure the model movement and scaling with input keys
void model_A7()
{
	/*
	worldMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "worldMatrix");
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	shearingMatrix =
	{
		1.0, 0.0, 0.0, 0.0,
		0.3, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0,
	};

	partMatrix = shearingMatrix * scalingMatrix;
	worldMatrix = rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	shearingMatrix =
	{
		1.0, 0.0, 0.0, 0.0,
		-0.3, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0,
	};
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.3f, 0.0f, 0.0f));
	partMatrix = translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	shearingMatrix = glm::mat4(1.0f);
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.1f, 0.75f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.145f, 0.15f, 0.0f));
	partMatrix = translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	shearingMatrix =
	{
		1.0, 0.0, 0.0, 0.0,
		0.3, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0,
	};
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 4.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.55f, 0.0f, 0.0f));
	partMatrix = translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	shearingMatrix = glm::mat4(1.0f);
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2.5f, 1.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.52f, 0.4f, 0.0f));
	partMatrix = translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);




	// reset world matrix after we're done with it for this object
	worldMatrix = glm::mat4(1.0f);
	*/
}


