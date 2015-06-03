#pragma once
#include <VrLib/VrLib.h>

#include <GL\glew.h>
#include <gl\GL.h>
#include <glm\glm.hpp>

#include <map>
#include <string>

#pragma warning(push)
#pragma warning(disable : 4251)

class TASubTexture
{
	std::string filename;
	glm::vec2 startPosition;
	glm::vec2 dimensions;

public:
	TASubTexture(std::string filename, glm::vec2 startPosition, glm::vec2 dimensions);
	~TASubTexture();

	std::string getFilename();
	glm::vec2 getStartPosition();
	glm::vec2 getDimensions();
};

class TextureAtlas
{
	GLuint textureID;
	std::map<std::string, TASubTexture*> subTextures;
	int currentX, currentY, maxY;

public:
	TextureAtlas();
	~TextureAtlas();

	TASubTexture* addTexture(std::string filename);
	int getTextureSize();
	void bind();
};

#pragma warning(pop)