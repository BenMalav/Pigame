#pragma once 

#include <GLES3/gl3.h> 
#include <iostream>

struct VertData
{
	GLfloat position[3];
	GLfloat color[3];
	GLfloat normal[3];
};

class Model
{
public:

	/*  numV = number of Vertices
		mumI = number of indices */
	Model(int numV, int numI)
	{
		data = new VertData[numV];
		indices = new GLuint[numI];
		numVerts = numV;
		numIndices = numI;

		vbo[0] = 0;
		vao[0] = 0;
		ebo[0] = 0;
	}

	Model(Model&& m) noexcept
	{
		delete[] data;
		delete[] indices;

		data = m.data;
		indices = m.indices;

		m.data = nullptr;
		m.indices = nullptr;

		numVerts = m.numVerts;
		numIndices = m.numIndices;

		vao[0] = m.vao[0];
		vbo[0] = m.vbo[0];
		ebo[0] = m.ebo[0];

		numVerts = m.numVerts;
		numIndices = m.numIndices;
	}

	Model& operator=(Model&& m) noexcept
	{
		delete[] data;
		delete[] indices;

		data = m.data;
		indices = m.indices;

		m.data = nullptr;
		m.indices = nullptr;

		numVerts = m.numVerts;
		numIndices = m.numIndices;

		vao[0] = m.vao[0];
		vbo[0] = m.vbo[0];
		ebo[0] = m.ebo[0];

		numVerts = m.numVerts;
		numIndices = m.numIndices;

		return *this;
	}

	~Model()
	{
		delete[] data;
		delete[] indices;
	}

	VertData* getDataPtr()
	{
		return data;
	}

	GLuint* getIndPtr()
	{
		return indices;
	}

	GLuint getVao()
	{
		return vao[0];
	}

	int getNumIndices()
	{
		return numIndices;
	}

	void genBufferObjects()
	{
		glGenBuffers(1, vbo);
		glGenBuffers(1, ebo);

		glGenVertexArrays(1, vao);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (numIndices * sizeof(GLuint)), indices, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, (sizeof(VertData) * numVerts), data, GL_STATIC_DRAW);

		uint64_t posSize = 3 * sizeof(GL_FLOAT);
		uint64_t colorSize = 3 * sizeof(GL_FLOAT);
		uint64_t normalSize = 3 * sizeof(GL_FLOAT);
		uint64_t stride = posSize + colorSize + normalSize;

		uint64_t posOffset = 0;
		uint64_t colorOffset = posSize;
		uint64_t normalOffset = posSize + colorSize;

		glBindVertexArray(vao[0]);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);

		glVertexAttribPointer(positionAttributeIndex, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<GLvoid*>(posOffset));
		glEnableVertexAttribArray(positionAttributeIndex);

		glVertexAttribPointer(colorAttributeIndex, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<GLvoid*>(colorOffset));
		glEnableVertexAttribArray(colorAttributeIndex);

		glVertexAttribPointer(normalAttributeIndex, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<GLvoid*>(normalOffset));
		glEnableVertexAttribArray(normalAttributeIndex);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void deleteBufferObjects()
	{
		// Delete buffer objects
		glDisableVertexAttribArray(0);
		glDeleteBuffers(1, vbo);
		glDeleteVertexArrays(1, vao);
	}

private:

	GLuint vbo[1], vao[1], ebo[1];

	VertData* data;
	GLuint* indices;

	GLuint numVerts;
	GLuint numIndices;

	GLuint positionAttributeIndex = 0;
	GLuint colorAttributeIndex = 1;
	GLuint normalAttributeIndex = 2;
};