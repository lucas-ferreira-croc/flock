#include "texture.hpp"
#include "log/logger.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>


Texture::Texture()
	: _width(0), _height(0), _bitDepth(0), _filepath("")
{
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

	glGenTextures(1, &TextureObject);
	glBindTexture(GL_TEXTURE_2D, TextureObject);

	glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(textureTarget, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_data);

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


	glGenTextures(1, &TextureObject);
	glBindTexture(GL_TEXTURE_2D, TextureObject);

	glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(textureTarget, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);

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


	glGenTextures(1, &TextureObject);
	glBindTexture(GL_TEXTURE_2D, TextureObject);

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
	glBindTexture(textureTarget, TextureObject);
}

void Texture::clear()
{
	glDeleteTextures(1, &TextureObject);
	TextureObject = 0;
	_width = 0;
	_height = 0;
	_bitDepth = 0;
	_filepath = (char*)"";
}