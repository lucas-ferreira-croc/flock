#include "texture.hpp"
#include "log/logger.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>

Texture::Texture()
	: _width(0), _height(0), _bitDepth(0), _filepath(""), textureTarget(GL_TEXTURE_2D)
{
}

Texture::Texture(std::vector<std::string> faces)
	: _width(0), _height(0), _bitDepth(0), _filepath(""), textureTarget(GL_TEXTURE_CUBE_MAP)
{
	glGenTextures(1, &textureObject);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureObject);

	for(unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &_width, &_height, &_bitDepth, 0);
		if(data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
						 0, GL_SRGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else{
			Logger::err("Cubemap texture faield to load at path : " + faces[i]);
			stbi_image_free(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

}

Texture::Texture(GLenum textureTarget, std::string filepath)
	:  _width(0), _height(0), _bitDepth(0), _filepath(filepath), textureTarget(textureTarget)
{
}

Texture::~Texture()
{
	clear();
}

bool Texture::loadTexture()
{
	stbi_set_flip_vertically_on_load(true);

	unsigned char* tex_data = stbi_load(_filepath.c_str(), &_width, &_height, &_bitDepth, 0);
	if (!tex_data)
	{
		Logger::err("Could not find: " + _filepath);
		Logger::warning("Loading default error texture");

		_filepath = "C:\\dev\\shader\\flock\\assets\\images\\error.png";
		_width = 0;
		_height = 0;
		_bitDepth = 0;
		tex_data = stbi_load(_filepath.c_str(), &_width, &_height, &_bitDepth, 4);

		if(!tex_data)
		{
			Logger::err("Could not load fallback texture");
			return false;
		}
	}

	Logger::log("succesffuly loaded texture: " + _filepath);

	glGenTextures(1, &textureObject);
	glBindTexture(GL_TEXTURE_2D, textureObject);

	glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(textureTarget, 0, GL_SRGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_data);

	glGenerateMipmap(textureTarget);
	glBindTexture(textureTarget, 0);

	stbi_image_free(tex_data);

	return true;
}

bool Texture::loadTextureA()
{
	stbi_set_flip_vertically_on_load(true);

	int force_channels = 4;
	unsigned char* tex_data = stbi_load(_filepath.c_str(), &_width, &_height, &_bitDepth, force_channels);
	if (!tex_data)
	{
		Logger::err("Could not find: " + _filepath);
		Logger::warning("Loading default error texture");

		_filepath = "C:\\dev\\shader\\flock\\assets\\images\\error.png";
		_width = 0;
		_height = 0;
		_bitDepth = 0;
		tex_data = stbi_load(_filepath.c_str(), &_width, &_height, &_bitDepth, force_channels);

		if(!tex_data)
		{
			Logger::err("Could not load fallback texture");
			return false;
		}
	}

	glGenTextures(1, &textureObject);
	glBindTexture(GL_TEXTURE_2D, textureObject);

	glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(textureTarget, 0, GL_SRGB, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);

	glGenerateMipmap(textureTarget);
	glBindTexture(textureTarget, 0);

	stbi_image_free(tex_data);

	return true;
}

bool Texture::loadTextureGrayscale()
{
	stbi_set_flip_vertically_on_load(true);

	unsigned char* tex_data = stbi_load(_filepath.c_str(), &_width, &_height, &_bitDepth, 0);
	if (!tex_data)
	{
		Logger::err("Could not find: " + _filepath);
		Logger::warning("Loading default error texture");

		_filepath = "C:\\dev\\shader\\flock\\assets\\images\\error.png";
		_width = 0;
		_height = 0;
		_bitDepth = 0;
		tex_data = stbi_load(_filepath.c_str(), &_width, &_height, &_bitDepth, 4);
		
		if(!tex_data)
		{
			Logger::err("Could not load fallback texture");
			return false;
		}
	}


	glGenTextures(1, &textureObject);
	glBindTexture(GL_TEXTURE_2D, textureObject);

	glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(textureTarget, 0, GL_RED, _width, _height, 0, GL_RED, GL_UNSIGNED_BYTE, tex_data);

	glGenerateMipmap(textureTarget);
	glBindTexture(textureTarget, 0);

	stbi_image_free(tex_data);

	return true;
}

void Texture::use(GLenum textureUnit)
{
	glActiveTexture(textureUnit);
	glBindTexture(textureTarget, textureObject);
}

void Texture::clear()
{
	glDeleteTextures(1, &textureObject);
	textureObject = 0;
	_width = 0;
	_height = 0;
	_bitDepth = 0;
	_filepath = (char*)"";
}