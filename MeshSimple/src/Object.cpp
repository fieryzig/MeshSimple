#include "Object.h"

void Object::render(bool wireframe, std::shared_ptr<Camera> camera)
{
	if (wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	shader->Use();
	glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), 800.f / 600.f, 0.1f, 100.0f);
	shader->setMat4("projection", projection);
	glm::mat4 view = camera->GetViewMatrix();
	shader->setMat4("view", view);
	glm::mat4 world(1);
	//world = glm::translate(world, glm::vec3(0.5, 0.5, 0));
	world = glm::rotate(world, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
	shader->setMat4("model", world);


	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, model->indices.size(), GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_TRIANGLES, 0, 9);
	glBindVertexArray(0);
}

void Object::render_init()
{
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(this->VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, model->vertices.size() * 4, model->vertices.data(), GL_STATIC_DRAW);
		
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, model->indices.size() * 4, model->indices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
	
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
}

void Object::update_mesh()
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, model->vertices.size() * 4, model->vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, model->indices.size() * 4, model->indices.data(), GL_STATIC_DRAW);
}
