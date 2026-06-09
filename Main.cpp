#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

/*
* OpenGL doesn't provide us defaults for the vertex and fragment shader, so we need to write them ourselves.
* Using source code provided on learnopengl.com, we can write a simple vertex shader and a simple fragment shader to display a color on the screen.
*/


/*
The main purpose of the vertex shader is to transform 3D coordinates into different 3D coordinates 
and the vertex shader allows us to do some basic processing on the vertex attributes.

The output of the vertex shader stage is optionally passed to the geometry shader. 
The geometry shader takes as input a collection of vertices that form a primitive and has the ability to generate other shapes by emitting new vertices to form new (or other) primitive(s). 
In this example case, it generates a second triangle out of the given shape. 
*/
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

/*
The main purpose of the fragment shader is to calculate the final color of a pixel,
and this is usually the stage where all the advanced OpenGL effects occur. 
Usually the fragment shader contains data about the 3D scene that it can use to calculate the final pixel color (like lights, shadows, color of the light and so on). 
*/
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f);\n"
"}\n\0";

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) 
		glfwSetWindowShouldClose(window, true);
}

int main() 
{
	//initialize GLFW library
	glfwInit();

	//tell GLFW we want to use OpenGL 3.3 core profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	//tell GLFW we want to use the core profile - only modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 

	//vertices of the triangle we want to draw
	//equilateral triangle with side length of 1, centered at the origin
	GLfloat vertices[] = 
	{
		-0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,	//left
		0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,		//right
		0.0f, 0.5f * float(sqrt(3))* 2/3, 0.0f		//top
	};


	//Create a windowed mode window and its OpenGL context
	GLFWwindow* window = glfwCreateWindow(800, 800, "OpenGL Window", nullptr, nullptr);


	if(window == nullptr) 
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	//introduce window's context to the current thread
	glfwMakeContextCurrent(window);

	// Load OpenGL function pointers using GLAD
	gladLoadGL();

	//set the viewport size
	glViewport(0, 0, 800, 800);


	//setup and compile the vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	//gpu doesn't understand shader source code, so we need to compile it first
	glCompileShader(vertexShader);

	//setup and compile the fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);

	//create a shader program and link the vertex and fragment shader to it
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	//delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


	// Generate a vertex buffer object (VBO) and a vertex array object (VAO) to store the vertex data and the vertex attribute configuration
	// For sending the data between CPU to GPU in batches, we use buffer objects. 
	// A vertex buffer object (VBO) is a buffer object that contains vertex data.
	// A vertex array object (VAO) is an object that stores the vertex attribute configuration and pointer to which VBO to use.
	GLuint VBO, VAO;

	//make sure that the VAO is created before the VBO, since we need to bind the VAO before binding the VBO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	//bind the VAO first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	//bind the VBO to the GL_ARRAY_BUFFER target, which means that any buffer operations we perform on the GL_ARRAY_BUFFER target will affect this VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//copy the vertex data into the buffer's memory. The last argument specifies how we want to use the data.
	//GL_STATIC_DRAW means that the data will most likely not change at all or very rarely
	//GL_DYNAMIC_DRAW means that the data will be changed a lot
	//GL_STREAM_DRAW means that the data will be changed every time it is drawn
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//specify how OpenGL should interpret the vertex data and how it should be stored in the vertex attribute arrays
	//vertex attribute is a way to communicating with the vertex shader from outside.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//unbind so we don't accidentally modify the VAO and VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind the VBO
	glBindVertexArray(0); //unbind the VAO


	//FONT Buffer - the buffer from which the information is being read to be displayed on the screen
	//BACK Buffer - the buffer to which the information is being written to be displayed on the screen
	//specifies the color of window's background
	glClearColor(0.02f, 0.13f, 0.17f, 1.0f); //back buffer with the color we wan

	//clear the back buffer and specify the new color to it
	glClear(GL_COLOR_BUFFER_BIT); //front buffer

	//swap the back buffer with the front buffer to display the new color
	glfwSwapBuffers(window);

	//render loop - keep running until the user closes the window
	while(!glfwWindowShouldClose(window)) 
	{
		//check for use input (ESC Key) to close the window
		processInput(window);

		glClearColor(0.02f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwSwapBuffers(window);

		//if we don't poll events, the window will freeze or become unresponsive
		//take care of all GLFW events
		glfwPollEvents();

	}

	//optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);


	//destroy the window and its context
	glfwDestroyWindow(window);

	//terminate GLFW, clearing any resources allocated by GLFW
	glfwTerminate();

	return 0;
}