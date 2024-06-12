#include "Material.h"

Material::Material(glm::vec3 clr, glm::vec3 surfKa, glm::vec3 surfKd, glm::vec3 surfKs, float surfShininess) :
	color(clr), surfaceKa(surfKa), surfaceKd(surfKd), surfaceKs(surfKs), surfaceShininess(surfShininess){}