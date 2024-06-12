#pragma once
#include <iostream>
#include <vector>
#include <algorithm>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "GLSLProgram.h"
#include "GLTools.h"
#include "Object.h"
#include "AbstractCube.h"
#include "Material.h"

using namespace cg;

class Sphere {
public:
	Sphere(float radius, Material& mat, bool flShading);

	~Sphere();

	void initObject();

	void initShader();

	void render(const glm::mat4& view, const glm::mat4& projection, bool wireframe, bool showNormals);

	void setLightVector(const glm::vec4& v);

	void addNormal(std::vector<glm::vec3>& positions, std::vector<glm::vec3>& colors, const glm::vec3& color, const glm::vec3& vertex, const glm::vec3& normal, float scale = 0.5f);

	void addTriangle(std::vector<glm::vec3>& positions, std::vector<glm::vec3>& normals, std::vector<glm::vec3>& colors,
		const glm::vec3& color, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);

	void initShader(GLSLProgram& program, const std::string& vert, const std::string& frag);

	void subdivideTriangle(
		std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& positions, std::vector<glm::vec3>& normals, std::vector<glm::vec3>& colors, glm::vec3& color,
		glm::vec3& v1, glm::vec3& v2, glm::vec3& v3, int depth);

	void approximateSphere(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& positions, std::vector<glm::vec3>& normals, std::vector<glm::vec3>& colors, glm::vec3& color);

	glm::mat4& getModel();

	glm::vec3 getSphereCenter();


	void translate(glm::vec3 translation);
	void rotateAround(float angle, glm::vec3 axis, glm::vec3 center);
	void rotateAroundSelf(float angle, glm::vec3 axis);

	Sphere* getChild();
	void setChild(Sphere* ch);
	
	GLSLProgram programShaded;
	GLSLProgram programSimple;

	Object objSphere;
	Object objNormals;
	glm::mat4 model;

	float sphereRadius;
	unsigned int n;
	bool flatShading;
	glm::vec3 sphereCenter;
	Sphere* child;
	Material material;
};