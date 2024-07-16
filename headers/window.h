void initializeWindow() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); //Dont create OpenGL context
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE); //Toggle resizable window
	
	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}