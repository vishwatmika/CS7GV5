#ifndef LOADER_H_
#define LOADER_H_

#include <iostream>
#include <string> 
#include <fstream>
#include <functional>
#include <algorithm>
#include <sstream>
#include <vector>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

struct vertex {
	std::vector<float> v;
	void normalize() {
		float magnitude = 0.0f;
		for (int i = 0; i < v.size(); i++)
			magnitude += pow(v[i], 2.0f);
		magnitude = sqrt(magnitude);
		for (int i = 0; i < v.size(); i++)
			v[i] /= magnitude;
	}
	vertex operator-(vertex v2) {
		vertex v3;
		if (v.size() != v2.v.size()) {
			v3.v.push_back(0.0f);
			v3.v.push_back(0.0f);
			v3.v.push_back(0.0f);
		}
		else {
			for (int i = 0; i < v.size(); i++)
				v3.v.push_back(v[i] - v2.v[i]);
		}
		return v3;
	}
	vertex cross(vertex v2) {
		vertex v3;
		if (v.size() != 3 || v2.v.size() != 3) {
			v3.v.push_back(0.0f);
			v3.v.push_back(0.0f);
			v3.v.push_back(0.0f);
		}
		else {
			v3.v.push_back(v[1] * v2.v[2] - v[2] * v2.v[1]);
			v3.v.push_back(v[2] * v2.v[0] - v[0] * v2.v[2]);
			v3.v.push_back(v[0] * v2.v[1] - v[1] * v2.v[0]);
		}
		return v3;
	}
};

struct face {
	std::vector<int> vertex;
	std::vector<int> texture;
	std::vector<int> normal;
};

class LoadObj {
private:
	std::vector<vertex> vertices;
	std::vector<vertex> normals;
	std::vector<vertex> texcoords;
	std::vector<vertex> parameters;
	std::vector<face> faces;
	int numvertices;
	std::vector<float> floatVertices;
	std::vector<float> neutral;
	std::vector<float> floatNormals;
	std::vector<float> floatTex;
protected:
public:
	LoadObj();
	void LoadObject(std::string filename);
	float* getVertices();
	float* getNormals();
	float* getTexcoords();
	int getNumVertices();
	std::vector<vertex> getAllVertices();
	std::vector<float> returnfloatVertices();
	void modify(int pos, glm::vec3 nv);
	void blendMesh(std::vector<float> vert_m1, float w1);
	void blendMesh(	std::vector<float> vert_m1, std::vector<float> vert_m2, float w1, 
					std::vector<float> vert_m3, std::vector<float> vert_m4, float w2, 
					std::vector<float> vert_m5, std::vector<float> vert_m6, float w3, 
					std::vector<float> vert_m7, std::vector<float> vert_m8, float w4, 
					std::vector<float> vert_m9, float w5, 
					std::vector<float> vert_m10, float w6);
};

#endif  // LOADER_H_