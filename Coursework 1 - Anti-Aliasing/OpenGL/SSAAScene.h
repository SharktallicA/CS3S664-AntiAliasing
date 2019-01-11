#pragma once
#include "Object.h"
#include "Renderer.h"
#include "Utility.h"
#include "TexturedQuad.h"

//Enumerated anti-aliasing selection option
enum AntiAliasingOption { None, SSAA, MSAA };

using namespace std;

class SSAAScene
{
protected:
	//Anti-aliasing option
	AntiAliasingOption aaOpt;
	//Anti-aliasing sample rate
	int aaSamples;

	//Actual framebuffer object (FBO)
	GLuint fbo;
	//Colour texture to render into
	GLuint fboColTex;
	//Depth texture to render into
	GLuint fboDepTex;
	//Flags whether FBO is valid
	bool fboOkay;

	//
	GLuint renderQuadVAO;
	//
	GLuint renderQuadVertBuffer;
	//
	GLuint renderQuadTexBuffer;
	//
	GLuint renderQuadShader;

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
		for (int i = 0; i < 5; i++)
		{
			//Generate model
			Model* nModel = new Model("Resources\\Models\\Primitives\\Sphere.obj");
			GLuint texture = TextureLoader::loadTexture("Resources\\Textures\\Ceres.jpg");
			nModel->attachTexture(texture, "texture_diffuse1");

			//Generate object's properties
			vec3 pos(Utility::rand32(-5, 5), Utility::rand32(-5, 5), Utility::rand32(-5, 5));
			vec3 rot(0, 0, 0);
			float scale = Utility::rand32(0.25, 1.0);
			vec3 sca(scale, scale, scale);

			//Create object
			Object* tmp = new Object(pos, rot, sca);
			tmp->AttachRenderer(new Renderer(tmp, nModel));
			tmp->renderer->GiveCamera(sceneCam);
			tmp->renderer->GiveShader(shader);
			sceneObjs.push_back(tmp);
		}
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
		glBindTexture(GL_TEXTURE_2D, fboColTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, sceneX * aaSamples, sceneY * aaSamples, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Setup depth texture
		glGenTextures(1, &fboDepTex);
		glBindTexture(GL_TEXTURE_2D, fboDepTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, sceneX * aaSamples, sceneY * aaSamples, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		
		//Attach textures to FBOs
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboColTex, 0);
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

	//Constructs full-screen quad to render scene into
	void setupRenderQuad()
	{
		//Geometry for render quad
		static float renderQuadVertices[] = {

			-1.0f, -1.0f, 0.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f,
			-1.0f,  1.0f, 0.0f, 1.0f,
			 1.0f,  1.0f, 0.0f, 1.0f
		};

		//Texture coordinates for render quad
		static float renderQuadTextureCoords[] = {

			0.0f, 1.0f,
			1.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 0.0f
		};

		//Setup render quad's VAO
		glGenVertexArrays(1, &renderQuadVAO);
		glBindVertexArray(renderQuadVAO);

		//Setup VBO for geometry
		glGenBuffers(1, &renderQuadVertBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, renderQuadVertBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(renderQuadVertices), renderQuadVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);

		//Setup VBO for texture coordinates
		glGenBuffers(1, &renderQuadTexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, renderQuadTexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(renderQuadTextureCoords), renderQuadTextureCoords, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, 0, (const GLvoid*)0);

		//Enable buffers
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(2);

		//Unbind VAO
		glBindVertexArray(0);

		//Load shader for render quad
		ShaderLoader::createShaderProgram(string("Resources\\Shaders\\textureShader.vs"), string("Resources\\Shaders\\textureShader.fs"), &renderQuadShader);
	}

public:
	//Scene's Camera
	Camera* sceneCam = nullptr;

	SSAAScene(Camera* masterCam, int width, int height, int sampleRate)
	{
		sceneCam = masterCam;
		sceneX = width;
		sceneY = height;
		aaSamples = sampleRate;
		configureScene();
		setupFBO();
		setupTextures();
		fboOkay = checkFBO();
		//Initial scene's quad with colour texture
		setupRenderQuad();
	}
	~SSAAScene()
	{
		for (Object* obj : sceneObjs) delete obj;
		delete sceneCam;
	}

	//Renders FBO product into sceneQuad, then displays result
	void Render(float timeDelta)
	{
		if (!fboOkay) return; //Don't bother rendering if the FBO is kaput

		//Binds FBO so all rendering is directed to FBO
		glBindFramebuffer(GL_FRAMEBUFFER, fbo); // Make sure your multisampled FBO is the read framebuffer

		// Clear the screen (in this case, the scene's texture)
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glViewport(0, 0, sceneX * aaSamples, sceneY * aaSamples);

		//Update scene objects
		for (Object* obj : sceneObjs)
			obj->Update(timeDelta);

		//Direct rendering back to scene/main framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//Setup render quad for viewing
		glm::mat4 T = glm::mat4(1.0);
		static GLint mvpLocation = glGetUniformLocation(renderQuadShader, "mvpMatrix");

		//Prepare to pass scene resolution into shader for SSAA texel size calculation
		static GLint samplesLocation = glGetUniformLocation(renderQuadShader, "sampleSize");
		
		glUseProgram(renderQuadShader);
		glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(T));
		//Pass sample size in
		glUniform1i(samplesLocation, aaSamples);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fboColTex);
		glBindVertexArray(renderQuadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}
};