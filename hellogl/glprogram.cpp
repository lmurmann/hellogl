#include "glprogram.h"
#include <gl/gl3w.h>
#include <cstdio>

bool compileStatus(GLuint shader) {
	int ret;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &ret);
	return ret;
}
bool linkStatus(GLuint program) {
	int ret;
	glGetProgramiv(program, GL_LINK_STATUS, &ret);
	return ret;
}

bool compileShader(GLuint handle, GLenum stype, const char* src) {
	int shader_len = strlen(src);
	glShaderSource(handle, 1, &src, &shader_len);
	glCompileShader(handle);
	if (!compileStatus(handle)) {
	    char buff[2048];
	    int nwritten;
		glGetShaderInfoLog(handle, 2048, &nwritten, buff);

		const char* typelabel = stype == GL_VERTEX_SHADER ? "vertex" : (stype == GL_FRAGMENT_SHADER ? "fragment" : "unknown");
		printf("Error in %s shader\n%s\n", typelabel, buff);
		return false;
	}
	return true;
}
int compileShader(GLenum type, const char* src) {
	GLuint handle = glCreateShader(type);
	compileShader(handle, type, src);
	return handle;
}

bool linkProgram(GLuint handle, GLuint vshader, GLuint fshader) {
	glAttachShader(handle, vshader);
	glAttachShader(handle, fshader);
	glLinkProgram(handle);
	if (!linkStatus(handle)) {
	    char buff[2048];
	    int nwritten;
		glGetProgramInfoLog(handle, 2048, &nwritten, buff);
		printf("Program link error:\n%s\n", buff);
		return false;
	}
	return true;
}
int linkProgram(const char* vshader_src, const char* fshader_src) {
	GLuint program = glCreateProgram();
	GLuint vshader = compileShader(GL_VERTEX_SHADER, vshader_src);
	GLuint fshader = compileShader(GL_FRAGMENT_SHADER, fshader_src);
	if (!linkProgram(program, vshader, fshader)) {
		glDeleteProgram(program);
		program = 0;
	}
	glDeleteShader(vshader);
	glDeleteShader(fshader);
	return program;
}