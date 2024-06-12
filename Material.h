#pragma once
#include <glm/glm.hpp>

class Material {
public:
	Material(glm::vec3 clr, glm::vec3 surfKa, glm::vec3 surfKd, glm::vec3 surfKs, float surfShininess);

	glm::vec3 color;
	glm::vec3 surfaceKa;
	glm::vec3 surfaceKd;
	glm::vec3 surfaceKs;
	float surfaceShininess;
};