#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <fstream>

#include <glew.h>
#include <glfw3.h>

GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path) {
	//Allocate shaders
 	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

 	//Read vertex shader code
 	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
 	if (VertexShaderStream.is_open()) 
	{
 		std::string Line = "";
		while (getline(VertexShaderStream, Line))
		{
			VertexShaderCode += "\n" + Line;
		}
 		VertexShaderStream.close();
	}
	else 
	{
 		printf("Cannot open vertex shader\n");
 		getchar();
 		return 0;
	}

 	// Read fragment shader code 
 	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
 	if (FragmentShaderStream.is_open()) 
	{
 		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
		{
			FragmentShaderCode += "\n" + Line;
		}
 		FragmentShaderStream.close();

	}
	else
	{
		printf("Cannot open fragment shader\n");
		getchar();
		return 0;
	}

	//Pointers for error conditions
 	GLint Result = GL_FALSE;
	int InfoLogLength;

 	//Compile vertex shader
 	printf("Compiling shader : %s\n", vertex_file_path);
 	char const * VertexSourcePointer = VertexShaderCode.c_str();
 	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
 	glCompileShader(VertexShaderID);

 	//Check vertex shader
 	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
 	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
 	if (InfoLogLength > 0) 
	{
 		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
 		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
 		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

 	//Compile fragment shader
 	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
 	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
 	glCompileShader(FragmentShaderID);

 	//Check fragment shader
 	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
 	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
 	if (InfoLogLength > 0) 
	{
 		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
 		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
 		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

 	//Link the program
 	printf("Linking program\n");
 	GLuint ProgramID = glCreateProgram();
 	glAttachShader(ProgramID, VertexShaderID);
 	glAttachShader(ProgramID, FragmentShaderID);
 	glLinkProgram(ProgramID);

 	//Check the program
 	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
 	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
 	if (InfoLogLength > 0) 
	{
 		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
 		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
 		printf("%s\n", &ProgramErrorMessage[0]);
	}

	//Send shaders to OpenGL
 	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);
 	glDeleteShader(VertexShaderID);
 	glDeleteShader(FragmentShaderID);
 	return ProgramID;
}