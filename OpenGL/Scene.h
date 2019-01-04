#pragma once
#include "Object.h"
#include "Renderer.h"
#include "Utility.h"
#include "TexturedQuad.h"

//Enumerated anti-aliasing selection option
enum AntiAliasingOption { None, MSAA, SSAA };

using namespace std;

class Scene
{
protected:
	//Anti-aliasing option
	AntiAliasingOption aaOpt;

	//Actual framebuffer object (FBO)
	GLuint fbo;
	//Colour texture to render into
	GLuint fboColTex;
	//Depth texture to render into
	GLuint fboDepTex;
	//Flags whether FBO is valid
	bool fboOkay;

	//Scene's render quad (the object that will receive the FBO's texture product)
	TexturedQuad* sceneQuad = nullptr;
	//Scene size
	int sceneX = 1440, sceneY = 900;
	//Scene's Objects
	vector<Object*> sceneObjs;
	//Scene's Camera
	Camera* sceneCam = new Camera(sceneX, sceneY, 0.1, 100.0, glm::vec3(0, 1, 2));
	
	//Child-modifiable sceneObj populator (has default scene)
	virtual void configureScene()
	{
		//Attach default shader
		GLuint defaultShader;
		GLSL_ERROR glsl_err = ShaderLoader::createShaderProgram(
			string("Resources\\Shaders\\defaultShader.vs"),
			string("Resources\\Shaders\\defaultShader.fs"),
			&defaultShader);

		//Initialise model of generic sphere
		Model* nModel = new Model("Resources\\Models\\Primitives\\Sphere.obj");

		//Generates default scene of differently scaled spheres
		/*for (int i = 0; i < 25; i++)
		{
			vec3 pos(Utility::rand32(-5, 5), Utility::rand32(-5, 5), Utility::rand32(-5, 5));
			vec3 rot(0.0, 0.0, 0.0);
			vec3 sca(Utility::rand32(0.05, 1.0), Utility::rand32(0.05, 1.0), Utility::rand32(0.05, 1.0));
			Object* tmp = new Object(pos, rot, sca);
			tmp->AttachRenderer(new Renderer(tmp, nModel));
			tmp->renderer->GiveCamera(sceneCam);
			tmp->renderer->GiveShader(defaultShader);
			sceneObjs.push_back(tmp);
		}*/

		vec3 pos(0, 0, 0);
		vec3 rot(0.0, 0.0, 0.0);
		vec3 sca(1, 1, 1);
		Object* tmp = new Object(pos, rot, sca);
		tmp->AttachRenderer(new Renderer(tmp, nModel));
		tmp->renderer->GiveCamera(sceneCam);
		tmp->renderer->GiveShader(defaultShader);
		sceneObjs.push_back(tmp);
	}

private:
	//Setup framebuffer object
	void setupFBO()
	{
		//Generate FBO name
		glGenFramebuffers(1, &fbo);

		//Bind framebuffer to target
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	}

	//Setup textures that will be drawn into via FBO
	void setupTextures()
	{
		//Setup colour buffer texture
		glGenTextures(1, &fboColTex);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, fboColTex);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, 800, 800, GL_TRUE);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		//Setup depth texture
		glGenTextures(1, &fboDepTex);
		glBindTexture(GL_TEXTURE_2D, fboDepTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, 800, 800, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		//Attach textures to FBOs
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, fboColTex, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fboDepTex, 0);
	}

	//Checks if FBO can be used for rendering
	bool checkFBO()
	{
		GLenum demoFBOStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (demoFBOStatus != GL_FRAMEBUFFER_COMPLETE)
		{
			cout << "ERROR: could not successfully create FBO to render texture" << endl;
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			return false;
		}
		else
		{
			cout << "SUCCESS: FBO created" << endl;
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			return true;
		}
	}

public:
	Scene(AntiAliasingOption nAAOpt)
	{
		aaOpt = nAAOpt;
		configureScene();
		setupFBO();
		setupTextures();
		fboOkay = checkFBO();
		//Initial scene's quad with colour texture
		sceneQuad = new TexturedQuad(fboColTex, true);
	}
	~Scene()
	{
		for (Object* obj : sceneObjs) delete obj;
		delete sceneQuad, sceneCam;
	}

	//Renders FBO product into sceneQuad, then displays result
	void Render(Camera* masterCam, float timeDelta)
	{
		if (!fboOkay) return; //Don't bother rendering if the FBO is kaput

		//Binds FBO so all rendering is directed to FBO
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		//Setup AA options
		if (aaOpt == MSAA)
		{
			glEnable(GL_MULTISAMPLE);
		}

		// Clear the screen (in this case, the scene's texture)
		glClearColor(1, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Set viewport to specified scene size
		glViewport(0, 0, sceneX, sceneY);

		//Update scene objects
		for (Object* obj : sceneObjs)
			obj->Update(timeDelta);

		//Direct rendering back to scene/main framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//Render scene's quad
		sceneQuad->render(masterCam->getProjectionMatrix() * masterCam->getViewMatrix());
	}
};