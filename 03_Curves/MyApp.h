#pragma once

// C++ includes
#include <memory>

// GLEW
#include <GL/glew.h>

// SDL
#include <SDL.h>
#include <SDL_opengl.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

#include "ProgramObject.h"
#include "BufferObject.h"
#include "VertexArrayObject.h"
#include "TextureObject.h"

#include "Mesh_OGL3.h"
#include "gCamera.h"

#include <vector>

class CMyApp
{
public:
	CMyApp(void);
	~CMyApp(void);

	bool Init();
	void Clean();

	void Update();
	void Render();

	void KeyboardDown(SDL_KeyboardEvent&);
	void KeyboardUp(SDL_KeyboardEvent&);
	void MouseMove(SDL_MouseMotionEvent&);
	void MouseDown(SDL_MouseButtonEvent&);
	void MouseUp(SDL_MouseButtonEvent&);
	void MouseWheel(SDL_MouseWheelEvent&);
	void Resize(int, int);
protected:

	// shaderekhez sz�ks�ges v�ltoz�k
	ProgramObject		m_program;		// shaderek programja

	Texture2D			m_textureEarth;

	VertexArrayObject	m_vao;			// VAO objektum
	IndexBuffer			m_gpuBufferIndices;		// indexek
	ArrayBuffer			m_gpuBufferPos;	// poz�ci�k t�mbje
	ArrayBuffer			m_gpuBufferNormal;	// norm�lisok t�mbje

	gCamera				m_camera;

	static const int N = 200;
	static const int M = 200;

	glm::vec3 pos = glm::vec3(-3,0,0);
	glm::vec2 vel;
	float angle = 0.0f;

};

