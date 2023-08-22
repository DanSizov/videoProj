#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include<string>
#include<iostream>
#include<opencv2/opencv.hpp>
#include<GL/glew.h>
#include<stb/stb_image.h>

class TextureManager
{
private:
	std::vector<GLuint> textures;

public:
	TextureManager(){}
	~TextureManager();

	GLuint loadTextureFromFrame(const cv::Mat& frame);
	GLuint loadTextureFromFile(const std::string& filepath);
	void updateTexture(const GLuint& textureID, const cv::Mat& frame);
	void deleteTexture(const GLuint& textureID);
};

#endif // !TEXTURE_MANAGER_H


