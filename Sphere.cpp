#include "Sphere.h"
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>



Sphere::Sphere(float radius, Material& mat, bool flShading) : material(mat){
	n = 4;
	sphereRadius = radius;
	sphereCenter = { 0.0f, 0.0f, 0.0f };
	flatShading = flShading;
	initShader();
	initObject();
}

Sphere::~Sphere() {

}

void Sphere::initShader()
{
	initShader(programSimple, "shader/simple.vert", "shader/simple.frag");
	flatShading ? initShader(programShaded, "shadedFlat.vert", "shadedFlat.frag")://Flat Shading
				  initShader(programShaded, "shader/shadedGoraud.vert", "shader/shadedGoraud.frag"); //Goraud Shading

	//initShader(programShaded, "shader/shadedPhong.vert", "shader/shadedPhong.frag"); // Phong Shading

	programShaded.use();
	programShaded.setUniform("light", glm::vec3(0, 0, 0));
	programShaded.setUniform("lightI", float(1.0f));
	// Material
	programShaded.setUniform("surfKa", material.surfaceKa);
	programShaded.setUniform("surfKd", material.surfaceKd);
	programShaded.setUniform("surfKs", material.surfaceKs);
	programShaded.setUniform("surfShininess", material.surfaceShininess);
	//
}

void Sphere::initObject()
{
	std::vector<glm::vec3> vertices;

	// Build the Sphere.
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> colors;
	std::vector<glm::vec3> normals;
	std::vector<GLuint> indices;

	approximateSphere(vertices, positions, normals, colors, material.color);

	for (GLushort i = 0; i < positions.size(); i++)
		indices.push_back(i);

	GLuint programId = programShaded.getHandle();
	GLuint pos;

	// Vertex array object.
	glGenVertexArrays(1, &objSphere.vao);
	glBindVertexArray(objSphere.vao);

	// Position buffer.
	glGenBuffers(1, &objSphere.positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, objSphere.positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(), GL_STATIC_DRAW);

	pos = glGetAttribLocation(programId, "position");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Color buffer.
	glGenBuffers(1, &objSphere.colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, objSphere.colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);

	pos = glGetAttribLocation(programId, "color");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Normal buffer.
	glGenBuffers(1, &objSphere.normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, objSphere.normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);

	pos = glGetAttribLocation(programId, "normal");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Index buffer.
	objSphere.indexCount = (GLuint)indices.size();

	glGenBuffers(1, &objSphere.indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objSphere.indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, objSphere.indexCount * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	// Build the normals.
	programId = programSimple.getHandle();

	std::vector<glm::vec3> positions2;
	std::vector<glm::vec3> colors2;
	std::vector<GLuint> indices2;

	const glm::vec3 colorNormal(1.0f, 1.0f, 1.0f);

	for (int i = 0; i < positions.size(); i++) {
		addNormal(positions2, colors2, colorNormal, positions[i], normals[i], 0.5f);
	}

	for (GLushort i = 0; i < positions2.size(); i++)
		indices2.push_back(i);

	// Vertex array object.
	glGenVertexArrays(1, &objNormals.vao);
	glBindVertexArray(objNormals.vao);

	// Position buffer.
	glGenBuffers(1, &objNormals.positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, objNormals.positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, positions2.size() * sizeof(glm::vec3), positions2.data(), GL_STATIC_DRAW);

	pos = glGetAttribLocation(programId, "position");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Color buffer.
	glGenBuffers(1, &objNormals.colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, objNormals.colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, colors2.size() * sizeof(glm::vec3), colors2.data(), GL_STATIC_DRAW);

	pos = glGetAttribLocation(programId, "color");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Index buffer.
	objNormals.indexCount = (GLuint)indices2.size();

	glGenBuffers(1, &objNormals.indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objNormals.indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, objNormals.indexCount * sizeof(GLuint), indices2.data(), GL_STATIC_DRAW);
}

void Sphere::render(const glm::mat4& view, const glm::mat4& projection, bool wireframe, bool showNormals)
{
	glm::mat4 mv = view * model;
	// Create mvp.
	glm::mat4 mvp = projection * mv;

	// Create normal matrix (nm) from model matrix.
	glm::mat3 nm = glm::inverseTranspose(glm::mat3(model));

	// Bind the shader program and set uniform(s).
	programShaded.use();
	programShaded.setUniform("modelviewMatrix", mv);
	programShaded.setUniform("projectionMatrix", projection);
	programShaded.setUniform("normalMatrix", nm);


	// Bind vertex array object so we can render the triangle.
	glBindVertexArray(objSphere.vao);
	
	wireframe ?	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	glDrawElements(GL_TRIANGLES, objSphere.indexCount, GL_UNSIGNED_INT, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindVertexArray(0);

	if (showNormals)
	{
		// Bind the shader program and set uniform(s).
		programSimple.use();
		programSimple.setUniform("mvp", mvp);

		// Bind vertex array object so we can render the triangle.
		glBindVertexArray(objNormals.vao);
		glDrawElements(GL_LINES, objNormals.indexCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}

void Sphere::setLightVector(const glm::vec4& v)
{
	programShaded.use();
	programShaded.setUniform("light", v);
}

void Sphere::initShader(GLSLProgram& program, const std::string& vert, const std::string& frag)
{
	if (!program.compileShaderFromFile(vert.c_str(), cg::GLSLShader::VERTEX))
	{
		throw std::runtime_error("COMPILE VERTEX: " + program.log());
	}

	if (!program.compileShaderFromFile(frag.c_str(), cg::GLSLShader::FRAGMENT))
	{
		throw std::runtime_error("COMPILE FRAGMENT: " + program.log());
	}

	if (!program.link())
	{
		throw std::runtime_error("LINK: " + program.log());
	}
}

void Sphere::addNormal(std::vector<glm::vec3>& positions, std::vector<glm::vec3>& colors, const glm::vec3& color, const glm::vec3& vertex, const glm::vec3& normal, float scale)
{
	positions.push_back(vertex);
	positions.push_back(vertex + normal * scale);

	colors.push_back(color);
	colors.push_back(color);
}

void Sphere::addTriangle(std::vector<glm::vec3>& positions, std::vector<glm::vec3>& normals, std::vector<glm::vec3>& colors,
	const glm::vec3& color, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
{

	// Add vertices, normals and colors to the buffers.
	positions.push_back(a);
	positions.push_back(b);
	positions.push_back(c);

	if (flatShading) {
		glm::vec3 u = glm::normalize(b - a);
		glm::vec3 v = glm::normalize(c - a);

		glm::vec3 normal = glm::normalize(glm::cross(v, u));

		glm::vec3 toCenter = glm::normalize(- 1.0f * a);

		if (glm::dot(toCenter, normal) >= 0) {
			normal = -1.0f * normal;
		}

		normals.push_back(normal);
		normals.push_back(normal);
		normals.push_back(normal);
	}
	else {
		normals.push_back(glm::normalize(a));
		normals.push_back(glm::normalize(b));
		normals.push_back(glm::normalize(c));
	}

	colors.push_back(color);
	colors.push_back(color);
	colors.push_back(color);

	
}

void Sphere::subdivideTriangle(
	std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& positions, std::vector<glm::vec3>& normals, std::vector<glm::vec3>& colors, glm::vec3& color,
	glm::vec3& v1, glm::vec3& v2, glm::vec3& v3, int depth)
{

	if (depth <= 0) {

		glm::vec3 normV1 = glm::normalize(v1) * sphereRadius;
		glm::vec3 normV2 = glm::normalize(v2) * sphereRadius;
		glm::vec3 normV3 = glm::normalize(v3) * sphereRadius;

		vertices.push_back(normV1);
		vertices.push_back(normV2);
		vertices.push_back(normV3);

		addTriangle(positions, normals, colors, color, normV1, normV2, normV3);

	}
	else {
		float step = depth / (depth + 1.0f);
		glm::vec3 L_LEFT = v1;
		glm::vec3 L_RIGHT = v1 + step * (v2 - v1);
		glm::vec3 L_UP = v1 + step * (v3 - v1);
		subdivideTriangle(vertices, positions, normals, colors, color, L_LEFT, L_RIGHT, L_UP, depth - 1);

		glm::vec3 R_LEFT = v2 + step * (v1 - v2);
		glm::vec3 R_RIGHT = v2;
		glm::vec3 R_UP = v2 + step * (v3 - v2);
		subdivideTriangle(vertices, positions, normals, colors, color, R_LEFT, R_RIGHT, R_UP, depth - 1);

		glm::vec3 M_LEFT = v3 + step * (v1 - v3);
		glm::vec3 M_RIGHT = v3 + step * (v2 - v3);
		glm::vec3 M_UP = v3;
		subdivideTriangle(vertices, positions, normals, colors, color, M_LEFT, M_RIGHT, M_UP, depth - 1);

		if (depth == 1) {
			subdivideTriangle(vertices, positions, normals, colors, color, L_RIGHT, R_UP, L_UP, depth - 1);
		}
	}
}

void Sphere::approximateSphere(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& positions, std::vector<glm::vec3>& normals, std::vector<glm::vec3>& colors, glm::vec3& color) {

	subdivideTriangle(vertices, positions, normals, colors, color, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec3{ 1.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, -1.0f, 0.0f }, n);
	subdivideTriangle(vertices, positions, normals, colors, color, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec3{ -1.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, -1.0f, 0.0f }, n);
	subdivideTriangle(vertices, positions, normals, colors, color, glm::vec3{ 1.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 0.0f, -1.0f }, glm::vec3{ 0.0f, -1.0f, 0.0f }, n);
	subdivideTriangle(vertices, positions, normals, colors, color, glm::vec3{ -1.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 0.0f, -1.0f }, glm::vec3{ 0.0f, -1.0f, 0.0f }, n);
	subdivideTriangle(vertices, positions, normals, colors, color, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec3{ -1.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }, n);
	subdivideTriangle(vertices, positions, normals, colors, color, glm::vec3{ -1.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 0.0f, -1.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }, n);
	subdivideTriangle(vertices, positions, normals, colors, color, glm::vec3{ 0.0f, 0.0f, -1.0f }, glm::vec3{ 1.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }, n);
	subdivideTriangle(vertices, positions, normals, colors, color, glm::vec3{ 1.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }, n);

}

glm::mat4& Sphere::getModel()
{
	return model;
}

glm::vec3 Sphere::getSphereCenter() {
	return sphereCenter;
}

//Transformationen

void Sphere::translate(glm::vec3 translation) {
	model = glm::translate(model, translation);
	sphereCenter = glm::translate(glm::mat4(1.0f), translation) * glm::vec4(sphereCenter, 1.0f);
}

void Sphere::rotateAround(float angle, glm::vec3 axis, glm::vec3 center) {
	glm::mat4 translateToOrigin = glm::translate(-center);
	glm::mat4 rotation = glm::rotate(glm::radians(angle), axis);
	glm::mat4 translateBack = glm::translate(center);
	glm::mat4 rotationMatrix = translateBack * rotation * translateToOrigin;

	model = rotationMatrix * model;
	model = glm::rotate(model, glm::radians(-angle), axis);
	sphereCenter = rotationMatrix * glm::vec4(sphereCenter, 1.0f);

	if (child != NULL) {
		child->rotateAround(angle, axis, center);
	}
}

void Sphere::rotateAroundSelf(float angle, glm::vec3 axis) {
	glm::mat4 translateToOrigin = glm::translate(-sphereCenter);
	glm::mat4 rotation = glm::rotate(glm::radians(angle), axis);
	glm::mat4 translateBack = glm::translate(sphereCenter);
	glm::mat4 rotationMatrix = translateBack * rotation * translateToOrigin;

	model = rotationMatrix * model;
	sphereCenter = rotationMatrix * glm::vec4(sphereCenter, 1.0f);
}

Sphere* Sphere::getChild() {
	return child;
}
void Sphere::setChild(Sphere* ch) {
	child = ch;
}