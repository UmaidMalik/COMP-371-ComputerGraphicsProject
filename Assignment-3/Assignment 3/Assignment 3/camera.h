#ifndef CAMERA_H
#define CAMERA_H

#include <algorithm>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include <vector>

#include "shader.h"

class Camera
{
public:

	Camera(GLFWwindow* window, Shader shaderProgram, float deltaTime)
	{

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

	}
	


private:
	float cameraSpeed = 1.0f;
	float cameraSpeedFast = 4 * cameraSpeed;

	float cameraHorizontalAngle = 90.0f;
	float cameraVerticalAngle = 0.0f;

	glm::vec3 initialCameraPosition = glm::vec3(0.6f, 1.0f, 1.5f);
	glm::vec3 initialcameraLookAt = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);


	glm::vec3 cameraPosition = initialCameraPosition;
	glm::vec3 cameraLookAt = initialcameraLookAt;

	glm::mat4 identityMatrix = glm::mat4(1.0f);

	glm::mat4 viewMatrix = identityMatrix;

	double lastMousePosX, lastMousePosY;
};





#endif

/*

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

*/