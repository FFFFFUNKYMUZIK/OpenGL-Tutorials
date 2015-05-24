#define GLFW_DLL
#define GLFW_INCLUDE_GLU
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3dll.lib")

// ========== ���� �׷��Ƚ� ������Ʈ ==========
// ���̴� ���α׷� ������Ʈ
GLuint gShaderProgram;

// ���� ���� ������Ʈ
GLuint gVertexBufferObject;
std::vector<glm::vec3> gVertices;

// ========== �Լ� ���� ==========
const std::string ReadStringFromFile(const std::string& filename);
GLuint CreateShader(GLenum shaderType, const std::string& source);
bool CheckShader(GLuint shader);
bool CheckProgram(GLuint program);

bool InitApp();
void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void WindowSizeChangeCallback(GLFWwindow* window, int width, int height);
void Render();


int main() {
	// GLFW ������ �ڵ鷯
	GLFWwindow* window = NULL;

	// ���� �ڵ鷯 ���
	glfwSetErrorCallback(ErrorCallback);

	// GLFW �ʱ�ȭ
	if (!glfwInit())
		exit(EXIT_FAILURE);

	// ������ ����
	window = glfwCreateWindow(640, 480, "Tutorial 01: Basic OpenGL", NULL, NULL);
	if (!window) {
		// ���� ������ ������ �����ߴٸ� ���ø����̼� ����
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// ���ؽ�Ʈ ����
	glfwMakeContextCurrent(window);

	// swap ���� ����
	glfwSwapInterval(1);

	// Ű �ڵ鷯 ���
	glfwSetKeyCallback(window, KeyCallback);
	// ������ ������ ���� �ڵ鷯 ���
	glfwSetWindowSizeCallback(window, WindowSizeChangeCallback);

	// GLEW �ʱ�ȭ
	glewInit();

	// ���ø����̼��� �ʱ�ȭ
	if (!InitApp()) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// �� ������ ����. �����찡 ����Ǳ� ������ �ݺ��Ѵ�.
	while (!glfwWindowShouldClose(window)) {
		// ������
		Render();

		// ���� ���� ��ü. (�׸� ����� ���÷����ϴ� ���)
		glfwSwapBuffers(window);

		// ������ �̺�Ʈ (Ű ��Ʈ��ũ ��) ����.
		glfwPollEvents();
	}

	// ������ ����
	glfwDestroyWindow(window);

	// GLFW ����
	glfwTerminate();

	return 0;
}


bool InitApp() {
	// ---------- OpenGL ���� �ʱ�ȭ ----------
	// Ŭ���� ����(����) ����
	glClearColor(1.f, 1.f, 1.f, 1.f);

	glEnable(GL_DEPTH_TEST);

	glDisable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);


	// ---------- ���̴� ���� �� ������ ----------
	// ���̴� ���� �б�
	std::string vertShaderSource = ReadStringFromFile("BasicShader.glvs");
	std::string fragShaderSource = ReadStringFromFile("BasicShader.glfs");

	// ���̴� ������Ʈ ����
	GLuint vertShaderObj = CreateShader(GL_VERTEX_SHADER, vertShaderSource);
	GLuint fragShaderObj = CreateShader(GL_FRAGMENT_SHADER, fragShaderSource);

	// ���̴� ���α׷� ������Ʈ ����
	gShaderProgram = glCreateProgram();

	// ���̴� ���α׷��� ���ؽ� �� �����׸�Ʈ ���̴� ���
	glAttachShader(gShaderProgram, vertShaderObj);
	glAttachShader(gShaderProgram, fragShaderObj);

	// ���̴� ���α׷��� ���̴� ��ŷ(������ ������) �׸��� Ȯ��
	glLinkProgram(gShaderProgram);
	if (!CheckProgram(gShaderProgram)) {
		glDeleteProgram(gShaderProgram);
		return false;
	}

	// ���� ���̴� ���
	glDetachShader(gShaderProgram, vertShaderObj);
	glDetachShader(gShaderProgram, fragShaderObj);

	// ���̴� ����
	glDeleteShader(vertShaderObj);
	glDeleteShader(fragShaderObj);


	// ---------- ���� ��Ʈ����Ʈ ���� ���� ----------
	// ���� ����
	gVertices = {
		glm::vec3(-0.5f, -0.5f, 0.f), glm::vec3(0.5f, -0.5f, 0.f), glm::vec3(0.5f, 0.5f, 0.f)
	};

	// ���� ���� ����
	glGenBuffers(1, &gVertexBufferObject);

	// ���� ���� ���ε� �� ������ ���
	glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, gVertices.size()*sizeof(glm::vec3), &gVertices[0], GL_STATIC_DRAW);

	return true;
}

const std::string ReadStringFromFile(const std::string& filename) {
	std::ifstream f(filename);

	if (!f.is_open())
		return "";

	return std::string(std::istreambuf_iterator<char>(f),
		std::istreambuf_iterator<char>());
}

GLuint CreateShader(GLenum shaderType, const std::string& source) {
	GLuint shader = glCreateShader(shaderType);
	if (shader == 0)
		return 0;

	// set shader source
	const char* raw_str = source.c_str();
	glShaderSource(shader, 1, &raw_str, NULL);

	// compile shader object
	glCompileShader(shader);

	// check compilation error
	if (!CheckShader(shader)){
		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

bool CheckShader(GLuint shader) {
	GLint state;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &state);
	if (GL_FALSE == state){
		int infologLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLength);
		if (infologLength > 1){
			int charsWritten = 0;
			char *infoLog = new char[infologLength];
			glGetShaderInfoLog(shader, infologLength, &charsWritten, infoLog);
			std::cout << infoLog << std::endl;
			delete[] infoLog;
		}
		return false;
	}
	return true;
}

bool CheckProgram(GLuint program) {
	GLint state;
	glGetProgramiv(program, GL_LINK_STATUS, &state);
	if (GL_FALSE == state) {
		int infologLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLength);
		if (infologLength > 1){
			int charsWritten = 0;
			char *infoLog = new char[infologLength];
			glGetProgramInfoLog(program, infologLength, &charsWritten, infoLog);
			std::cout << infoLog << std::endl;
			delete[] infoLog;
		}
		return false;
	}
	return true;
}

void ErrorCallback(int error, const char* description) {
	std::cout << description << std::endl;
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void WindowSizeChangeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void Render() {
	// ���� �����
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// gShaderProgram�� ���̴� ���α׷��� ����Ұ���
	glUseProgram(gShaderProgram);

	// ���۸� ���ε�
	glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
	// location 0 ���� �Ӽ��� Ȱ��ȭ (���̴� �ڵ� ����)
	glEnableVertexAttribArray(0);
	// location 0 ���� �Ӽ��� ������
	glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, NULL);

	// ������ �׸�
	glDrawArrays(GL_TRIANGLES, 0, gVertices.size());

	// ���ε� ����
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Ȱ��ȭ�� ���� �Ӽ� ��Ȱ��ȭ
	glDisableVertexAttribArray(0);

	// ���̴� ���α׷� ��� ����
	glUseProgram(0);
}