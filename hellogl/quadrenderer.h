#pragma once
#include "unitquad.h"
class quadrenderer {
public:
	quadrenderer();
	~quadrenderer();
	void draw();
	// data must hold w*h*3 bytes.
	void upload_texture(unsigned char* data, int w, int h, int format, int type);

	unitquad m_quad;
	unsigned int m_tex;
	unsigned int m_program;
};