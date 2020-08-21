#version 330 core

const float PI = 3.1415926535897932384626433832795;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 lightDir;

uniform sampler2D texture_1;

uniform vec3 objectColor;
    
uniform float ambientStrength = 0.3; 
const float diffuseStrength = 2.9; 
const float specularStrength = 0.4; 

uniform float light_cutoff_outer;
uniform float light_cutoff_inner;
uniform float light_near_plane;
uniform float light_far_plane;

uniform vec3 viewPos;

uniform sampler2D shadowMap;

uniform bool drawTexture = true;

in vec3 FragPos;
in vec4 FragPosLightSpace;
in vec3 Normal;

in vec2 vertexUV;


in vec4 gl_FragCoord;

out vec4 FragResult;

vec3 ambientColor(vec3 light_color) 
{
	return ambientStrength * light_color;
}

vec3 diffuseColor(vec3 light_color, vec3 light_pos)
{
	vec3 light_dir = normalize(light_pos - FragPos);
    return diffuseStrength * light_color * max(dot(normalize(Normal), lightDir), 0.0f);
}

vec3 specularColor(vec3 light_color, vec3 light_pos)
{
	vec3 light_dir = normalize(light_pos - FragPos);
    vec3 view_dir = normalize(viewPos - FragPos);
    vec3 reflect_light_dir = reflect(-lightDir, normalize(Normal));
    return specularStrength * light_color * pow(max(dot(reflect_light_dir, view_dir), 0.0f), 32);
}

float shadowScalar()
{
	vec3 normalized_device_coord = FragPosLightSpace.xyz / FragPosLightSpace.w;
	
	normalized_device_coord = normalized_device_coord * 0.5 + 0.5;
	
	float closest_depth = texture(shadowMap, normalized_device_coord.xy).r;
	
	float current_depth = normalized_device_coord.z;
	
	float bias = max(0.05 * (1.0 - dot(Normal, lightDir)), 0.005);  // bias applied in depth map: see shadow_vertex.glsl
    return ((current_depth - bias) > closest_depth) ? 1.0 : 0.0;
}

float spotlightScalar()
{
	float theta = dot(normalize(FragPos - lightPos), lightDir);
    
    if (theta > light_cutoff_inner) {
        return 1.0;
    } else if (theta > light_cutoff_outer) {
        return (1.0 - cos(PI * (theta - light_cutoff_outer) / (light_cutoff_inner - light_cutoff_outer))) / 2.0;
    } else {
        return 0.0;
    }
}

	void main()
	{
		float scalar = shadowScalar() * spotlightScalar();

		// ambient
		vec3 ambient = ambientColor(lightColor);

		// diffuse
		vec3 diffuse = scalar * diffuseColor(lightColor, lightPos);

		// specular
		vec3 specular = scalar * specularColor(lightColor, lightPos);

		// phongModel
		vec3 phongModel = (ambient + diffuse + specular) * objectColor;
		
		if (drawTexture)
			FragResult = texture(texture_1, vertexUV) * vec4(phongModel, 1.0);
		else
			FragResult = vec4(phongModel, 1.0);
		
	}