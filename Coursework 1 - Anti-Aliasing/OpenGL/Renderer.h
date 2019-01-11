#pragma once

#include "Includes.h"
#include "Component.h"
#include "ShapeData.h"

using namespace std;
using namespace glm;

//Object's Renderer component: handles the drawing of the Object in OGL scene
class Renderer : public Component
{
private:
	//Scene's Camera obj
	Camera* cam = nullptr;
	//Object's 3D model
	Model* model = nullptr;
	//Object's reference to shader
	GLuint shader;
	//Object's vertex array object 
	GLuint VAO;
	//Index couple count
	int indices;

	// Constructs the Object's graphical shape VAO (for when geometry is manually specified)
	void construct(ShapeData shape)
	{
		// Create object construction buffers

		GLuint vertexBuffer;
		GLuint colourBuffer;
		GLuint offsetBuffer;
		GLuint indexBuffer;

		// Setup object's VAO
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		// Create VBO for vertex position data
		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, shape.vertices.size() * sizeof(float), &shape.vertices.front(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr); // attribute 0 gets data from bound VBO (so assign vertex position buffer to attribute 0)

		// Create VBO for vertex colour data
		glGenBuffers(1, &colourBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, colourBuffer);
		glBufferData(GL_ARRAY_BUFFER, shape.colours.size() * sizeof(float), &shape.colours.front(), GL_STATIC_DRAW);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, 0, nullptr); // attribute 1 gets colour data

		// Create VBO for offset colour data
		glGenBuffers(1, &offsetBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, colourBuffer);
		glBufferData(GL_ARRAY_BUFFER, shape.offsets.size() * sizeof(float), &shape.offsets.front(), GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, 0, nullptr); // attribute 2 gets offset data

		// Enable vertex position and colour attribute arrays
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		// Create VBO for face index array
		glGenBuffers(1, &indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, shape.indices.size() * sizeof(unsigned short), &shape.indices.front(), GL_STATIC_DRAW);
		indices = shape.indices.size();

		// Free VAO binding
		glBindVertexArray(0);
	}

public:
	//Object's model matrix
	mat4 matrix;

	Renderer(Object* parentRef, ShapeData nShape, Camera* nCam = nullptr, GLuint nShader = 0)
	{
		parent = parentRef;

		//Apply start properties
		matrix = glm::mat4(1.0);
		matrix *= glm::scale(mat4(1.0), vec3(parent->GetScale()));
		matrix *= glm::translate(mat4(1.0), vec3(parent->GetPosition()));
		construct(nShape);
		cam = nCam;
		shader = nShader;
	}
	Renderer(Object* parentRef, Model* nModel, Camera* nCam = nullptr, GLuint nShader = 0)
	{
		parent = parentRef;

		//Apply start properties
		matrix = glm::mat4(1.0);
		matrix *= glm::scale(mat4(1.0), vec3(parent->GetScale()));
		matrix *= glm::translate(mat4(1.0), vec3(parent->GetPosition()));
		model = nModel;
		cam = nCam;
		shader = nShader;
	}
	~Renderer(void)
	{
		parent = nullptr;
		delete model;
	}

	void Compute(float deltaTime)
	{
		glUseProgram(shader);
		glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(matrix));
		glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(cam->getViewMatrix()));
		glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(cam->getProjectionMatrix()));
		//Render whatever is populated
		if (!model)
		{
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_SHORT, nullptr);
		}
		else model->draw(shader);
	}

	void GiveCamera(Camera* nCam)
	{
		cam = nCam;
	}
	void GiveShader(GLuint nShader)
	{
		shader = nShader;
		glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(matrix));
	}
};