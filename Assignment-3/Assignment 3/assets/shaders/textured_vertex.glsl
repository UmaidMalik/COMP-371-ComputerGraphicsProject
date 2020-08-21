#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 light_view_proj_matrix;

out vec3 Normal;
out vec3 FragPos;
out vec4 FragPosLightSpace;
out vec2 vertexUV;


        
void main()
	{
		//Normal = transpose(inverse(mat3(worldMatrix))) * aNormal;
		Normal = mat3(worldMatrix) * aNormal;
		FragPos = vec3(worldMatrix * vec4(aPos, 1.0));
		FragPosLightSpace = light_view_proj_matrix * vec4(FragPos, 1.0);
		
		vertexUV = aUV;
		
		mat4 modelViewProjection = projectionMatrix * viewMatrix * worldMatrix;
		gl_Position = modelViewProjection * vec4(aPos, 1.0);
	}