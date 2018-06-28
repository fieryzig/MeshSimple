#pragma once

#include <base.h>
#include <Shader.h>
#include <Model.h>
#include <Camera.h>

class Object
{
public:
	void render(bool, std::shared_ptr<Camera>);

	std::shared_ptr<Model> model;
	std::shared_ptr<Shader> shader;

	GLuint VAO, VBO, EBO;

	void render_init();
	void update_mesh();
};