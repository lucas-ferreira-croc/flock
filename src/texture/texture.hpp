#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <glad/gl.h>
#include <string>
#include <vector>

class Texture
{
public:
	Texture();
	Texture(std::vector<std::string> faces);
	Texture(GLenum textureTarget, std::string filepath);
	Texture(int width, int height);
	~Texture();

	bool loadTexture();
	bool loadTextureA();
	bool loadTextureFromData(unsigned char*  data);
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
