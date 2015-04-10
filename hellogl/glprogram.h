#pragma once
#include <GL/gl3w.h>

bool compileStatus(GLuint shader);
bool linkStatus(GLuint program);

bool compileShader(GLuint handle, GLenum stype, const char* shader);
int compileShader(GLenum type, const char* shader);

bool linkProgram(GLuint handle, GLuint vshader, GLuint fshader);
int linkProgram(const char* vshader, const char* fshader);
