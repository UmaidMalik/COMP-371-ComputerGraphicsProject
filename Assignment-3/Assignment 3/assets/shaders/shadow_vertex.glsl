#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 worldMatrix;
uniform mat4 light_view_proj_matrix;


void main()
	{
		mat4 scale_bias_matrix = mat4(
									vec4(0.5, 0.0, 0.0, 0.0),
                                    vec4(0.0, 0.5, 0.0, 0.0),
                                    vec4(0.0, 0.0, 0.5, 0.0),
                                    vec4(0.5, 0.5, 0.5, 1.0)
									);
									
	gl_Position = 
                    scale_bias_matrix * // bias the depth map coordinates
                    light_view_proj_matrix * worldMatrix * vec4(aPos, 1.0);
	}