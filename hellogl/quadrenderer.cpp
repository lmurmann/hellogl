#include "quadrenderer.h"
#include <GL/gl3w.h>
#include <cassert>
#include <cstdio>
#include "glprogram.h"

static const char c_textured_vshader[] = "\
#version 150\n\
\n\
in vec3 VertexPosition;\
in vec2 VertexTexCoord;\
\
out vec2 v_vertexTexCoords;\
\
void main(){\
gl_Position = vec4(VertexPosition, 1); \
v_vertexTexCoords = VertexTexCoord; \
}";

static const char c_textured_fshader[] = "\
#version 150\n\
in vec2 v_vertexTexCoords;\
out vec4 outputColor;\
uniform sampler2D tex;\
\
void main() {\
	outputColor = texture(tex, v_vertexTexCoords);\
}";

quadrenderer::quadrenderer() {
	glGenTextures(1, &m_tex);
	m_program = linkProgram(c_textured_vshader, c_textured_fshader);
}


quadrenderer::~quadrenderer() {
	glDeleteTextures(1, &m_tex);
	glDeleteProgram(m_program);
}

void quadrenderer::draw() {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_tex);

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	glUseProgram(m_program);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_tex);

	glBindVertexArray(m_quad.vao);
	glBindFragDataLocation(m_program, 0, "outputColor");
	glBindAttribLocation(m_program, c_position_attr, "VertexPosition");
	glBindAttribLocation(m_program, c_uv_attr, "VertexTexCoord");

	int sampler_loc = glGetUniformLocation(m_program, "tex");
	glUniform1i(sampler_loc, 0);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	GLenum err = glGetError();
	if (err) {
		printf("Error after draw.\n");
		assert(false);
	}
}
void quadrenderer::upload_texture(unsigned char* data, int w, int h, int format, int type) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_tex);

	static const GLint NO_BORDER = 0;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, NO_BORDER, format, type, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	GLenum err = glGetError();
	if (err) {
		printf("Error after texture upload.\n");
		assert(false);
	}
}