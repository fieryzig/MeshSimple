#pragma once

#include <base.h>
#include <Object.h>
#include <vector>
#include <Camera.h>

class Scene
{
public:
	void render(bool wireframe = false) {
		for (auto obj : objects) {
			obj->render(wireframe, camera);
		}
	}

	void add_object(std::shared_ptr<Object> obj_ptr) {
		objects.push_back(obj_ptr);
	}

	std::vector<std::shared_ptr<Object>> objects;
	std::shared_ptr<Camera> camera;
};