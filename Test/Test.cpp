// Test.cpp : Defines the entry point for the application.
//

#include <random>
#include "Test.h"

using namespace std;

GLuint loadAndCompileComputeShader(char *file_path) {
	// Create the shaders
	GLuint ComputeShaderID = glCreateShader(GL_COMPUTE_SHADER);

	// Read the Vertex Shader code from the file
	std::string ComputeShaderCode;
	std::ifstream ComputeShaderStream(file_path, std::ios::in);
	if (ComputeShaderStream.is_open()) {
		std::string Line;
		while (getline(ComputeShaderStream, Line))
			ComputeShaderCode += "\n" + Line;
		ComputeShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory?\n", file_path);
		getchar();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", file_path);
	char const *ComputeSourcePointer = ComputeShaderCode.c_str();
	glShaderSource(ComputeShaderID, 1, &ComputeSourcePointer, nullptr);
	glCompileShader(ComputeShaderID);

	// Check Vertex Shader
	glGetShaderiv(ComputeShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(ComputeShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (Result == GL_FALSE || InfoLogLength > 0) {
		std::vector<char> ComputeShaderErrorMessage(static_cast<unsigned long long int>(InfoLogLength + 1));
		glGetShaderInfoLog(ComputeShaderID, InfoLogLength, nullptr, &ComputeShaderErrorMessage[0]);
		printf("%s\n", &ComputeShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint Program = (GLuint)glCreateProgram();
	glAttachShader(Program, ComputeShaderID);
	glLinkProgram(Program);

	// Check the program
	glGetProgramiv(Program, GL_LINK_STATUS, &Result);
	glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (Result == GL_FALSE || InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(static_cast<unsigned long long int>(InfoLogLength + 1));
		glGetProgramInfoLog(Program, InfoLogLength, nullptr, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
		throw std::runtime_error(&ProgramErrorMessage[0]);
	}

	glValidateProgram(Program);

	glDetachShader(Program, ComputeShaderID);
	glDeleteShader(ComputeShaderID);

	return Program;
}

auto bindBufferAndRun(unsigned int work_group_size, unsigned int numParticles, GLuint Program, GLuint bufferID) {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, bufferID);
	glUseProgram(Program);
	glDispatchCompute(std::ceil(numParticles / work_group_size), 1, 1);
	std::cout << std::ceil(numParticles / work_group_size) << std::endl;
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

int main()
{
    std::cout << GetCurrentWorkingDir() << std::endl;
	cogl::GLWindow mainWindow(0, 4, 5, 1, 1024, 768, 16, 9, "NULL", "data/postProcessing", false);
	mainWindow.enableCapability(GL_VERTEX_PROGRAM_POINT_SIZE);
	mainWindow.enableCapability(GL_PROGRAM_POINT_SIZE);
	mainWindow.setAASamples(0);

	auto vertices = std::vector<cogl::Vertex>{};

	auto radius = 4.0f;
	auto radial_sep = 0.5f;
	float angular_sep = 0.1f / PI;
	auto mean = 0.0f, stddeviation = 1.0f;

	std::mt19937 rng;
	rng.seed(std::random_device()());
	std::normal_distribution<double> normal_dist(mean, stddeviation); // distribution in range [1, 6]

	for (auto i = 1; i < radius/radial_sep; ++i) {
		for (auto j = 0; j < 2 * PI / angular_sep; ++j) {
			vertices.emplace_back();
			vertices.back().x = i * radial_sep * cos(j * angular_sep);
			vertices.back().y = 0.0f;
			vertices.back().z = i * radial_sep * sin(j * angular_sep);
			/*vertices.back().nx = normal_dist(rng);
			vertices.back().ny = normal_dist(rng);
			vertices.back().nz = normal_dist(rng);*/
		}
	}

	std::cout << vertices.size() << std::endl;

	cogl::Camera defaultCamera = cogl::Camera(glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3({ 0.f, 0.f, 0.f }),
		glm::vec3({ 0.0f, 1.0f, 0.0f }), cogl::projection::perspective);
	defaultCamera.changeAR(16.0 / 9.0);
	defaultCamera.changeZFar(1000.0);
	defaultCamera.changeZNear(0.001);

	auto cube_vertices = cogl::MeshRepresentation(vertices);
	auto cube = cogl::Mesh(cube_vertices);
	cube.scaleMesh(0.25f);
	cube.setRenderType(cogl::RenderTypes::Points);

	mainWindow.setMainCamera(defaultCamera);

	cogl::Shader defShader("data/triTest");
	double previousTime = glfwGetTime();
	double otherPreviousTime = glfwGetTime();
	int frameCount = 0;
	int frameCounterDebug = 0;
	glm::vec3 target_on_floor{ 0.0,0.0,0.0 };
	float angular_speed = 0.01f;

	GLuint ComputeShaderProgram;

	try {
		ComputeShaderProgram = loadAndCompileComputeShader("data/computeTest.cs.glsl");
	}
	catch(const std::exception&) {
		std::cin.get();
		return EXIT_FAILURE;
	}

	while (!mainWindow.shouldClose()) {

		bindBufferAndRun(1, cube_vertices.vertices.size(), ComputeShaderProgram, cube.getvertexBufferLabel());
		glUseProgram(0);

		//cube.rotateMesh(PI * angular_speed, glm::vec3({ 0.0f, 1.0f, 0.0f }));
		mainWindow.renderBegin();
		cube.render(defShader, defaultCamera, true);
		mainWindow.renderEnd();
		double currentTime = glfwGetTime();
		if (currentTime - previousTime >= 1.0) {
			// Display the frame count here any way you want.
			mainWindow.setTitle(std::to_string((currentTime - previousTime)*1e3 / frameCount) + "ms | frame: " + std::to_string(frameCounterDebug));
			previousTime = glfwGetTime();
			frameCount = 0;
		}
		frameCount++;
		++frameCounterDebug;
	}
}
