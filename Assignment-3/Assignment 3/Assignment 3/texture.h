#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image\stb_image.h"

class Texture
{
public:

	Texture(const char* textureFilePath) 
	{
		textureID = 0;
		glGenTextures(1, &textureID);
		assert(textureID != 0);

		glBindTexture(GL_TEXTURE_2D, textureID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


		int width, height, nrChannels;
		unsigned char* data = stbi_load(textureFilePath, &width, &height, &nrChannels, 0);
		if (!data)
		{
			std::cerr << "Error::Texture class could not load texture file path:" << textureFilePath << std::endl;
			return;
		}

		GLenum format = 0;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	~Texture() 
	{

	}

	unsigned int getTextureID() 
	{
		return textureID;
	}

private:
	unsigned int textureID;
};
#endif 

