#ifndef SHADERSH
#define SHADERSH

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#ifndef GLM_H
#define GLM_H
#include <glm/glm.hpp>
#endif

#include <fstream>
#include <string>
#include <iostream>

//support class for shader manipulations
class Shader {
public:
	unsigned int ID;

	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	void use();

	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;
	void setMat4(const std::string &name, glm::mat4 value) const;
	void setVec3(const std::string& name, glm::vec3 value) const;
};


#endif