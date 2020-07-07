/*
*	COMP 371 - Assignment 1
*	Umaid Malik 27576110
*
*
*
*/

#include <iostream>
#include <algorithm>

#define GLEW_STATIC 1 

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


glm::vec3 cameraPosition(0.6f, 1.0f, 1.5f);
glm::vec3 cameraLookAt(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);
glm::vec3 center(0.0f, 0.0f, 0.0f);




float cameraHorizontalAngle = 90.0f;
float cameraVerticalAngle = 0.0f;
float cameraSpeed = 1.0f;
float cameraSpeedFast = 2 * cameraSpeed;

GLuint worldMatrixLocation;

// function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void model_A7();

glm::mat4 identityMatrix = { 1.0, 0.0, 0.0, 0.0,
							 0.0, 1.0, 0.0, 0.0,
							 0.0, 0.0, 1.0, 0.0,
							 0.0, 0.0, 0.0, 1.0, };

glm::mat4 projectionMatrix = identityMatrix;
glm::mat4 viewMatrix = identityMatrix;
glm::mat4 worldMatrix = identityMatrix;

glm::mat4 rotationMatrix = identityMatrix;
glm::mat4 scalingMatrix = identityMatrix;
glm::mat4 translationMatrix;
glm::mat4 partMatrix;

glm::mat4 shearingMatrix = identityMatrix;






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

int main()
{
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
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	// Disable mouse cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Compile and link shaders
	int shaderProgram = compileAndLinkShaders();






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

	glm::vec3 redLine[] = {
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(5.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
	};

	glm::vec3 greenLine[] = {
		glm::vec3(0.0f,  0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
	};

	glm::vec3 blueLine[] = {
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 1.0f),
	};

	/*
	glm::vec3 gridLine[] = {
	//	glm::vec3( on XZ plane), glm::vec3( yellow color );
	//    glm::vec3(one XZ plane),glm::vec3( yellow color );
	}
	*/



	GLuint VAO[4];	// vertexArrayObject
	GLuint VBO[4];	// vertexBufferObject
	glGenVertexArrays(4, VAO);
	glGenBuffers(4, VBO);


	// cube
	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);	// position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));
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

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);



	//glBindVertexArray(VAO[0]);
	//glBindVertexArray(VAO[1]);


	glUseProgram(shaderProgram);

	projectionMatrix = glm::perspective(70.0f, 1024.0f / 768.0f, 0.01f, 100.0f);	

	GLuint projectionMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "projectionMatrix");
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);


	viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);

	GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);

	// Variables to be used later in tutorial
	float angle = 0.0f;
	float rotationSpeed = 30.0f;  // 180 degrees per second
	float lastFrameTime = glfwGetTime();
	double lastMousePosX, lastMousePosY;
	
	glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);


	// Enable Backface culling
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);

	// z-Buffer
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// render loop
	while (!glfwWindowShouldClose(window))
	{






		//  background color
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(VAO[0]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);







		float deltaTime = glfwGetTime() - lastFrameTime;
		lastFrameTime += deltaTime;


		/**/
		angle = (angle + rotationSpeed * deltaTime); // angles in degrees, but glm expects radians (conversion below)
		rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
		worldMatrix = rotationMatrix * partMatrix;

		// call model A7
		model_A7();






		worldMatrix = partMatrix;
		glBindVertexArray(VAO[1]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_LINES, 0, 2);

		glBindVertexArray(VAO[2]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_LINES, 0, 2);

		glBindVertexArray(VAO[3]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_LINES, 0, 2);

		// When debugging press 2 and 3 to get perspective view.


		//glDisableVertexAttribArray(VAO[0]);






		// check and call events and swap the buffers

		// Detect inputs

		glfwSwapBuffers(window);
		//glfwSwapInterval(0);
		glfwPollEvents();

		// input
		processInput(window);

		bool fastCam = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
		float currentCameraSpeed = (fastCam) ? cameraSpeedFast : cameraSpeed;
		
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
			cameraPosition.x -= currentCameraSpeed * deltaTime;
		}

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // move camera to the right
		{
			cameraPosition.x += currentCameraSpeed * deltaTime;
		}

		if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) // move camera up
		{
			cameraPosition.z -= currentCameraSpeed * deltaTime;
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // move camera down
		{
			cameraPosition.z += currentCameraSpeed * deltaTime;
		}

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // move camera up
		{
			cameraPosition.y -= currentCameraSpeed * deltaTime;
		}

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) // move camera down
		{
			cameraPosition.y += currentCameraSpeed * deltaTime;
		}
		
		viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);	
		GLuint viewMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "viewMatrix");
		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
	}

	// de-allocate all resources
	glDeleteVertexArrays(1, VAO);
	glDeleteBuffers(1, VBO);
	glDeleteProgram(shaderProgram);


	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow * window)
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
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	{
		projectionMatrix = glm::perspective(70.0f,			// field of view in degrees
			1024.0f / 768.0f,	// aspect ratio
			0.01f, 100.0f);	// near and far (near > 0)

		GLuint projectionMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "projectionMatrix");
		glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
	}

	// orthographic - projection transform
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
	{
		projectionMatrix = glm::ortho(-4.0f, 4.0f,      // left/right
			-3.0f, 3.0f,	  // bottom/top
			-100.0f, 100.0f);  // near/far (near == 0 is ok for ortho)

		GLuint projectionMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "projectionMatrix");
		glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
	}


	// camera control - view transform
	
	//	GLuint viewMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "viewMatrix");
	//	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);


	// Press and hold C to disable culling
	glEnable(GL_CULL_FACE);
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !GLFW_RELEASE)
	{
		glDisable(GL_CULL_FACE);
	}


}

void model_A7()
{
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



	shearingMatrix =
	{
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0,
	};
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



	shearingMatrix =
	{
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0,
	};
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2.5f, 1.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.52f, 0.4f, 0.0f));
	partMatrix = translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

}