#include <base.h>
#include <Scene.h>
#include <Object.h>
#include <Model.h>
#include <Shader.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <thread>


auto MainCamera = std::make_shared<Camera>(glm::vec3(0,0,1.5f));
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void processInput(GLFWwindow *window, float deltaTime);
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

int main() 
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(1280, 720, "MeshSimple", nullptr, nullptr);
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
	ImGui::StyleColorsDark();
	

	
	glEnable(GL_DEPTH_TEST);
	
	//glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	auto object_ptr = std::make_shared<Object>();
	object_ptr->model = std::make_shared<Model>("flamingo.obj");
	
	object_ptr->shader = std::make_shared<Shader>("vertexshader.glsl", "fragshader.glsl");
	object_ptr->render_init();

	std::shared_ptr<Scene> scene_ptr = std::make_shared<Scene>();
	scene_ptr->camera = MainCamera;
	scene_ptr->add_object(object_ptr);

	float delTime = 0.0f, lastTime = 0.0f, lastHit = 0.0f;
	bool wireframe = true, gui_flag = true;
	float ratio = 1.0f, last_ratio = 1.0f;
	char save_buf[20] = "out.obj", open_buf[20] = "flamingo.obj";
	while (!glfwWindowShouldClose(window))
	{
		float curTime = glfwGetTime();
		delTime = curTime - lastTime;
		lastTime = curTime;
		
		glfwPollEvents();
		
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("MeshSimple", &gui_flag);
		ImGui::Text("Mesh Simple Demo");
		ImGui::SliderFloat("Ratio", &ratio, 0.0f, 1.0f);
		
		if (ImGui::Button("Simplify")) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			object_ptr->model->simplify(1.0f - ratio);
			object_ptr->update_mesh();
		}
		ImGui::InputText("[Open] Filename", open_buf, IM_ARRAYSIZE(open_buf));
		if (ImGui::Button("Open")) {
			object_ptr->model = std::make_shared<Model>(open_buf);
			object_ptr->render_init();
		}
		ImGui::InputText("[Save] Filename", save_buf, IM_ARRAYSIZE(save_buf));
		if (ImGui::Button("Save")) {
			object_ptr->model->save(save_buf);
		}
		if (ImGui::Button("Close")) {
			glfwSetWindowShouldClose(window, true);
		}
		ImGui::Text("FPS: %f", 1.0f / delTime);
		ImGui::Text("Faces: %d\n", object_ptr->model->indices.size() / 3);
		ImGui::End();

		ImGui::Render();
		int width, height;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &width, &height);
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		glViewport(0, 0, width, height);
		processInput(window, delTime);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
			float now = glfwGetTime();
			if (now - lastHit > 0.5) {
				wireframe ^= 1;
				lastHit = now;
			}
		}
		scene_ptr->render(wireframe);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		glfwMakeContextCurrent(window);
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (MainCamera->firstMouse)
	{
		MainCamera->lastX = xpos;
		MainCamera->lastY = ypos;
		MainCamera->firstMouse = false;
	}

	float xoffset = xpos - MainCamera->lastX;
	float yoffset = MainCamera->lastY - ypos; // reversed since y-coordinates go from bottom to top

	MainCamera->lastX = xpos;
	MainCamera->lastY = ypos;

	MainCamera->ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	MainCamera->ProcessMouseScroll(yoffset);
}

void processInput(GLFWwindow *window, float deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		MainCamera->ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		MainCamera->ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		MainCamera->ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		MainCamera->ProcessKeyboard(RIGHT, deltaTime);
}