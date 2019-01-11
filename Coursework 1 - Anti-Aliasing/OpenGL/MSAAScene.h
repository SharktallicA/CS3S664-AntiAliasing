#pragma once
#include "Object.h"
#include "Renderer.h"
#include "Utility.h"

using namespace std;

class MSAAScene
{
protected:
	//Anti-aliasing sample rate
	int aaSamples;

	//Scene size
	int sceneX, sceneY;
	//Scene's Objects
	vector<Object*> sceneObjs;
	
	//Child-modifiable sceneObj populator (has default scene)
	virtual void configureScene()
	{
		//Attach default shader
		GLuint shader;
		GLSL_ERROR glsl_err = ShaderLoader::createShaderProgram(
			string("Resources\\Shaders\\defaultShader.vs"),
			string("Resources\\Shaders\\defaultShader.fs"),
			&shader);

		//Generates default scene of differently scaled spheres
		for (int i = 0; i < 100; i++)
		{
			static float vertices[20] = {
				0.0f, 2.0f, 0.0f, 1.0f,
				1.0f, 0.0f, 1.0f, 1.0f,
				1.0f, 0.0f, -1.0f, 1.0f,
				-1.0f, 0.0f, -1.0f, 1.0f,
				-1.0f, 0.0f, 1.0f, 1.0f
			};

			float colourRange = 1.0f;
			static float colours[20] = {
				Utility::rand32(0.0f, colourRange) * 0.75, Utility::rand32(0.0f, colourRange) * 0.75, Utility::rand32(0.0f, colourRange) * 0.75, Utility::rand32(0.0f, colourRange) * 0.75,
				Utility::rand32(0.0f, colourRange) * 0.75, Utility::rand32(0.0f, colourRange) * 0.75, Utility::rand32(0.0f, colourRange) * 0.75, Utility::rand32(0.0f, colourRange) * 0.75,
				Utility::rand32(0.0f, colourRange) * 0.75, Utility::rand32(0.0f, colourRange) * 0.75, Utility::rand32(0.0f, colourRange) * 0.75, Utility::rand32(0.0f, colourRange) * 0.75,
				Utility::rand32(0.0f, colourRange) * 0.75, Utility::rand32(0.0f, colourRange) * 0.75, Utility::rand32(0.0f, colourRange) * 0.75, Utility::rand32(0.0f, colourRange) * 0.75,
				Utility::rand32(0.0f, colourRange) * 0.75, Utility::rand32(0.0f, colourRange) * 0.75, Utility::rand32(0.0f, colourRange) * 0.75, Utility::rand32(0.0f, colourRange) * 0.75
			};

			static unsigned short indices[18] = {
				0, 1, 2,
				0, 2, 3,
				0, 3, 4,
				0, 4, 1,
				3, 2, 4,
				1, 4, 2
			};

			ShapeData data;
			data.vertices = vector<float>(begin(vertices), end(vertices));
			data.colours = vector<float>(begin(colours), end(colours));
			data.indices = vector<unsigned short>(begin(indices), end(indices));

			//Generate object's properties
			vec3 pos(Utility::rand32(-10, 10), Utility::rand32(-10, 10), Utility::rand32(-10, 10));
			vec3 rot(Utility::rand32(0, 359), Utility::rand32(0, 359), Utility::rand32(0, 359));
			vec3 sca(Utility::rand32(0.25, 1.0), Utility::rand32(0.25, 1.0), Utility::rand32(0.25, 1.0));

			//Create object
			Object* tmp = new Object(pos, rot, sca);
			tmp->AttachRenderer(new Renderer(tmp, data));
			tmp->renderer->GiveCamera(sceneCam);
			tmp->renderer->GiveShader(shader);
			sceneObjs.push_back(tmp);
		}
	}

public:
	//Scene's Camera
	Camera* sceneCam = nullptr;

	MSAAScene(Camera* masterCam, int width, int height, int sampleRate)
	{
		sceneCam = masterCam;
		sceneX = width;
		sceneY = height;
		aaSamples = sampleRate;
		configureScene();
	}
	~MSAAScene()
	{
		for (Object* obj : sceneObjs) delete obj;
		delete sceneCam;
	}

	//Renders FBO product into sceneQuad, then displays result
	void Render(float timeDelta)
	{
		//Binds default framebuffer for MSAA
		glBindFramebuffer(GL_FRAMEBUFFER, 0); 

		glClearColor(0, 0, 0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//
		glViewport(0, 0, sceneX, sceneY);

		//Update scene objects
		for (Object* obj : sceneObjs)
			obj->Update(timeDelta);
	}
};