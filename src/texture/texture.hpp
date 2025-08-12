#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <glad/gl.h>
#include <string>

class Texture
{
public:
	Texture();
	Texture(GLenum textureTarget, std::string filepath);
	~Texture();

	bool loadTexture();
	bool loadTextureA();
	bool loadTextureGrayscale();
	void use(GLenum textureUnit);
	void clear();

	std::string getFilepath() { return _filepath; };
	
	GLenum textureTarget;
	unsigned int textureObject;
private:
	int _width, _height, _bitDepth;

	std::string _filepath;
};



#endif
