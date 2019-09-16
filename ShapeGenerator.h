#pragma once

#define _USE_MATH_DEFINES

#include <cmath>
#include <vector>
#include <iostream>
#include "Model.h"


class IcosoSphere
{
public:
	IcosoSphere(float radius = 1.0f, int subDiv = 0)
	{
		this->radius = radius;
		subdivision = subDiv;
	}
	Model buildSphere();

private:
	std::vector<float> computeIcosahedronVertices();
	void computeFaceNormal(const float v1[3], const float v2[3], const float v3[3], float n[3]);
	float computeScaleForLength(const float v[3], float length);
	void computeVertexNormal(const float v[3], float normal[3]);
	void computeHalfVertex(const float v1[3], const float v2[3], float length, float newV[3]);
	void computeHalfTexCoord(const float t1[2], const float t2[2], float newT[2]);

	void addVertices(const float v1[3], const float v2[3], const float v3[3]);
	void addNormals(float n1[3], float n2[3], float n3[3]);
	void addIndices(unsigned int i1, unsigned int i2, unsigned int i3);

	void subdivideVerticesFlat();
	void buildVerticesFlat();

	int subdivision;
	float radius;

	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<unsigned int> indices;
};
