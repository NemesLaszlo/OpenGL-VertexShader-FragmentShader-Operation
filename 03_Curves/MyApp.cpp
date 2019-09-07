#include "MyApp.h"

#include <math.h>
#include <vector>

#include <array>
#include <list>
#include <tuple>

#include "imgui\imgui.h"

#include "ObjParser_OGL3.h"

CMyApp::CMyApp(void)
{
	m_camera.SetView(glm::vec3(3, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}


CMyApp::~CMyApp(void)
{
	
}

bool CMyApp::Init()
{
	// törlési szín legyen fehér
	glClearColor(1,1,1, 1);

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST); // mélységi teszt bekapcsolása (takarás)
	//glDepthMask(GL_FALSE); // kikapcsolja a z-pufferbe történő írást - https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glDepthMask.xml

	glLineWidth(4.0f); // a vonalprimitívek vastagsága: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glLineWidth.xhtml
	//glEnable(GL_LINE_SMOOTH);

	// átlátszóság engedélyezése
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // meghatározza, hogy az átlátszó objektum az adott pixelben hogyan módosítsa a korábbi fragmentekből oda lerakott színt: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBlendFunc.xhtml

	// a raszterizált pontprimitívek mérete
	glPointSize(15.0f);

	//
	// shaderek betöltése
	//

	// így is létre lehetne hozni:

	// a shadereket tároló program létrehozása
	m_program.Init({ 
		{ GL_VERTEX_SHADER, "myVert.vert"},
		{ GL_FRAGMENT_SHADER, "myFrag.frag"}
	},
	{ 
		{ 0, "vs_in_pos" },					// VAO 0-as csatorna menjen a vs_in_pos-ba
		{ 1, "vs_in_normal" },				// VAO 1-es csatorna menjen a vs_in_normal-ba
	});

	m_program.LinkProgram();

	std::vector<glm::vec3> pos;
	pos.resize(((N + 1)*(M + 1)));
	std::vector<glm::vec3> norm;
	norm.resize(((N + 1)*(M + 1)));

	for (int i = 0; i <= N; ++i)
		for (int j = 0; j <= M; ++j)
		{
			float u = i / (float)N;
			float v = j / (float)M;

			pos[i + j * (N + 1)] = glm::vec3(v, 0, u);
			norm[i + j * (N + 1)] = glm::vec3(0, 1, 0);
		}

	std::vector<int> indices;
	indices.resize((3 * 2 * (N)*(M)));
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < M; ++j)
		{
			indices[6 * i + j * 3 * 2 * (N)+0] = (i)+(j)*	(N + 1);
			indices[6 * i + j * 3 * 2 * (N)+1] = (i + 1) + (j)*	(N + 1);
			indices[6 * i + j * 3 * 2 * (N)+2] = (i)+(j + 1)*(N + 1);
			indices[6 * i + j * 3 * 2 * (N)+3] = (i + 1) + (j)*	(N + 1);
			indices[6 * i + j * 3 * 2 * (N)+4] = (i + 1) + (j + 1)*(N + 1);
			indices[6 * i + j * 3 * 2 * (N)+5] = (i)+(j + 1)*(N + 1);
		}

	m_gpuBufferPos.BufferData(pos);
	m_gpuBufferNormal.BufferData(norm);
	m_gpuBufferIndices.BufferData(indices);

	// geometria VAO-ban való regisztrálása
	m_vao.Init(
		{
			{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, m_gpuBufferPos },	// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferPos GPU pufferben vannak
			{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, m_gpuBufferNormal },	// 1-es attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferNormal GPU pufferben vannak
		},
		m_gpuBufferIndices
		);

	// textúra betöltése
	m_textureEarth.FromFile("earth.jpg");


	// kamera
	m_camera.SetProj(45.0f, 640.0f / 480.0f, 0.01f, 1000.0f);

	return true;
}

void CMyApp::Clean()
{
}

void CMyApp::Update()
{
	static Uint32 last_time = SDL_GetTicks();
	float delta_time = (SDL_GetTicks() - last_time) / 1000.0f;

	pos.x += vel.x/10.0f;
	pos.z += vel.y/10.0f;

	m_camera.SetView(pos,pos+glm::vec3(1, 0, 0), glm::vec3(0, cosf(angle), sinf(angle)));
	std::cout << angle << std::endl;
	m_camera.Update(delta_time);

	last_time = SDL_GetTicks();
}

void CMyApp::Render()
{
	// töröljük a frampuffert (GL_COLOR_BUFFER_BIT) és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_program.Use();

	m_program.SetTexture("texImage", 0, m_textureEarth);

	glm::mat4 matWorld = glm::translate(glm::vec3(0, 0, 0));
	m_program.SetUniform("world", matWorld);
	m_program.SetUniform("worldIT", glm::transpose(glm::inverse(matWorld)));
	m_program.SetUniform("MVP", m_camera.GetViewProj()*matWorld);

	m_vao.Bind();
	glDrawElements(GL_TRIANGLES, (3 * 2 * N * M), GL_UNSIGNED_INT, 0);
	m_vao.Unbind(); 

	

	

	// végetért a 3D színtér rajzolása
	m_program.Unuse();

}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{
	//m_camera.KeyboardDown(key);
	if (key.keysym.sym == SDLK_w)
	{
		vel.x += 1;
	}

	if (key.keysym.sym == SDLK_s)
	{
		vel.x -= 1;
	}

	if (key.keysym.sym == SDLK_a)
	{
		vel.y -= 1;
	}

	if (key.keysym.sym == SDLK_d)
	{
		vel.y += 1;
	}

	if (key.keysym.sym == SDLK_SPACE)
	{
		vel = glm::vec2(0, 0);
	}

	if (key.keysym.sym == SDLK_q)
	{
		angle -= 0.1f;
	}

	if (key.keysym.sym == SDLK_e)
	{
		angle += 0.1f;
	}
}

void CMyApp::KeyboardUp(SDL_KeyboardEvent& key)
{
}

void CMyApp::MouseMove(SDL_MouseMotionEvent& mouse)
{
	//m_camera.MouseMove(mouse);
}

void CMyApp::MouseDown(SDL_MouseButtonEvent& mouse)
{
}

void CMyApp::MouseUp(SDL_MouseButtonEvent& mouse)
{
}

void CMyApp::MouseWheel(SDL_MouseWheelEvent& wheel)
{
}

// a két paraméterbe az új ablakméret szélessége (_w) és magassága (_h) található
void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h );

	m_camera.Resize(_w, _h);
}

