#pragma once

// array numbers for vertex position and uv coords.
static const int c_position_attr = 0;
static const int c_uv_attr = 1;

class unitquad
{
public:
	unitquad();
	~unitquad();
	unsigned int m_buffer;
	unsigned int m_indices;
	unsigned int vao;
};

