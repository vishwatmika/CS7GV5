#include "loader.h"

LoadObj::LoadObj()
{
}

void LoadObj::LoadObject(std::string filename) {
	std::ifstream ifs(filename.c_str(), std::ifstream::in);
	std::string line, key;
	while (ifs.good() && !ifs.eof() && std::getline(ifs, line)) {
		key = "";
		std::stringstream stringstream(line);
		stringstream >> key >> std::ws;

		if (key == "v") { // vertex
			vertex v; float x;
			while (!stringstream.eof()) {
				stringstream >> x >> std::ws;
				v.v.push_back(x);
			}
			vertices.push_back(v);
		}
		else if (key == "vp") { // parameter
			vertex v; float x;
			while (!stringstream.eof()) {
				stringstream >> x >> std::ws;
				v.v.push_back(x);
			}
			parameters.push_back(v);
		}
		else if (key == "vt") { // texture coordinate
			vertex v; float x;
			while (!stringstream.eof()) {
				stringstream >> x >> std::ws;
				v.v.push_back(x);
			}
			texcoords.push_back(v);
		}
		else if (key == "vn") { // normal
			vertex v; float x;
			while (!stringstream.eof()) {
				stringstream >> x >> std::ws;
				v.v.push_back(x);
			}
			v.normalize();
			normals.push_back(v);
		}
		else if (key == "f") { // face
			face f; int v, t, n;
			while (!stringstream.eof()) {
				stringstream >> v >> std::ws;
				f.vertex.push_back(v - 1);
				if (stringstream.peek() == '/') {
					stringstream.get();
					if (stringstream.peek() == '/') {
						stringstream.get();
						stringstream >> n >> std::ws;
						f.normal.push_back(n - 1);
					}
					else {
						stringstream >> t >> std::ws;
						f.texture.push_back(t - 1);
						if (stringstream.peek() == '/') {
							stringstream.get();
							stringstream >> n >> std::ws;
							f.normal.push_back(n - 1);
						}
					}
				}
			}
			faces.push_back(f);
		}
		else {

		}
	}
	ifs.close();
	std::cout << "\n-------------------------------------------------------------------------------------" << std::endl;
	std::cout << "Name: " << filename << std::endl;
	std::cout << "Vertices: " << vertices.size() << std::endl;
	std::cout << "Parameters: " << parameters.size() << std::endl;
	std::cout << "Texture Coordinates: " << texcoords.size() << std::endl;
	std::cout << "Normals: " << normals.size() << std::endl;
	std::cout << "Faces: " << faces.size() << std::endl << std::endl;

	for (int facesIndex = 0; facesIndex < faces.size(); facesIndex++) {
		for (int vertex_index = 0; vertex_index < faces[facesIndex].vertex.size(); vertex_index++) {
			floatVertices.push_back(vertices[faces[facesIndex].vertex[vertex_index]].v[0]);
			floatVertices.push_back(vertices[faces[facesIndex].vertex[vertex_index]].v[1]);
			floatVertices.push_back(vertices[faces[facesIndex].vertex[vertex_index]].v[2]);
		}
		for (int textureIndex = 0; textureIndex < faces[textureIndex].texture.size(); textureIndex++) {
			floatTex.push_back(texcoords[faces[facesIndex].texture[textureIndex]].v[0]);
			floatTex.push_back(texcoords[faces[facesIndex].texture[textureIndex]].v[1]);
		}
		for (int normalIndex = 0; normalIndex < faces[normalIndex].normal.size(); normalIndex++) {
			floatNormals.push_back(normals[faces[facesIndex].normal[normalIndex]].v[0]);
			floatNormals.push_back(normals[faces[facesIndex].normal[normalIndex]].v[1]);
			floatNormals.push_back(normals[faces[facesIndex].normal[normalIndex]].v[2]);
		}
	}
	numvertices = floatVertices.size() / 3;
	std::cout << "Face Vertices: " << floatVertices.size() << std::endl;
	std::cout << "Face Textures: " << floatTex.size() << std::endl;
	std::cout << "Face Normals: " << floatNormals.size() << std::endl;
	neutral = floatVertices;
	//vertices.clear();
	//texcoords.clear();
	//normals.clear();
	//faces.clear();
}

float* LoadObj::getVertices() {
	return floatVertices.data();
}

float* LoadObj::getNormals() {
	return floatNormals.data();
}

float* LoadObj::getTexcoords() {
	return floatTex.data();
}

int LoadObj::getNumVertices() {
	return numvertices;
}

std::vector<vertex> LoadObj::getAllVertices()
{
	return vertices;
}

std::vector<float> LoadObj::returnfloatVertices()
{
	return floatVertices;
}

void LoadObj::modify(int pos, glm::vec3 nv)
{
	vertex new_vertex;
	new_vertex.v.push_back(nv.x);
	new_vertex.v.push_back(nv.y);
	new_vertex.v.push_back(vertices[pos].v[2]/4);
	std::cout << "-------------------------------------------------------------------------------------" << std::endl;
	std::cout << "original vertex: " << vertices[pos].v[0] << " " << vertices[pos].v[1] << " " << vertices[pos].v[2] << std::endl;
	vertices[pos] = new_vertex;
	std::cout << "changed vertex: " << vertices[pos].v[0] << " " << vertices[pos].v[1] << " " << vertices[pos].v[2] << std::endl;
	std::cout << "-------------------------------------------------------------------------------------" << std::endl;
	floatVertices.clear();
	for (int facesIndex = 0; facesIndex < faces.size(); facesIndex++) {
		for (int vertex_index = 0; vertex_index < faces[facesIndex].vertex.size(); vertex_index++) {
			floatVertices.push_back(vertices[faces[facesIndex].vertex[vertex_index]].v[0]);
			floatVertices.push_back(vertices[faces[facesIndex].vertex[vertex_index]].v[1]);
			floatVertices.push_back(vertices[faces[facesIndex].vertex[vertex_index]].v[2]);
		}
	}
}

void LoadObj::blendMesh(std::vector<float> vert_m1, float w1)
{
	std::transform(vert_m1.begin(), vert_m1.end(), neutral.begin(), vert_m1.begin(), std::minus<float>());
	std::transform(vert_m1.begin(), vert_m1.end(), vert_m1.begin(), [&w1](auto& c) {return c * w1; });
	std::transform(neutral.begin(), neutral.end(), vert_m1.begin(), floatVertices.begin(), std::plus<float>());
}

//reference: http://antongerdelan.net/opengl/blend_shapes.html
//reference: https://stackoverflow.com/questions/24175414/glitchy-facial-morph-target-animation-in-opengl-c
void LoadObj::blendMesh(std::vector<float> vert_m1, std::vector<float> vert_m2, float w1,
						std::vector<float> vert_m3, std::vector<float> vert_m4, float w2,
						std::vector<float> vert_m5, std::vector<float> vert_m6, float w3,
						std::vector<float> vert_m7, std::vector<float> vert_m8, float w4,
						std::vector<float> vert_m9, float w5,
						std::vector<float> vert_m10, float w6)
{
	std::transform(vert_m1.begin(), vert_m1.end(), neutral.begin(), vert_m1.begin(), std::minus<float>());
	std::transform(vert_m1.begin(), vert_m1.end(), vert_m1.begin(), [&w1](auto& c) {return c * w1; });

	std::transform(vert_m2.begin(), vert_m2.end(), neutral.begin(), vert_m2.begin(), std::minus<float>());
	std::transform(vert_m2.begin(), vert_m2.end(), vert_m2.begin(), [&w1](auto& c) {return c * w1; });

	std::transform(vert_m3.begin(), vert_m3.end(), neutral.begin(), vert_m3.begin(), std::minus<float>());
	std::transform(vert_m3.begin(), vert_m3.end(), vert_m3.begin(), [&w2](auto& c) {return c * w2; });

	std::transform(vert_m4.begin(), vert_m4.end(), neutral.begin(), vert_m4.begin(), std::minus<float>());
	std::transform(vert_m4.begin(), vert_m4.end(), vert_m4.begin(), [&w2](auto& c) {return c * w2; });

	std::transform(vert_m5.begin(), vert_m5.end(), neutral.begin(), vert_m5.begin(), std::minus<float>());
	std::transform(vert_m5.begin(), vert_m5.end(), vert_m5.begin(), [&w3](auto& c) {return c * w3; });

	std::transform(vert_m6.begin(), vert_m6.end(), neutral.begin(), vert_m6.begin(), std::minus<float>());
	std::transform(vert_m6.begin(), vert_m6.end(), vert_m6.begin(), [&w3](auto& c) {return c * w3; });

	std::transform(vert_m7.begin(), vert_m7.end(), neutral.begin(), vert_m7.begin(), std::minus<float>());
	std::transform(vert_m7.begin(), vert_m7.end(), vert_m7.begin(), [&w4](auto& c) {return c * w4; });

	std::transform(vert_m8.begin(), vert_m8.end(), neutral.begin(), vert_m8.begin(), std::minus<float>());
	std::transform(vert_m8.begin(), vert_m8.end(), vert_m8.begin(), [&w4](auto& c) {return c * w4; });

	std::transform(vert_m9.begin(), vert_m9.end(), neutral.begin(), vert_m9.begin(), std::minus<float>());
	std::transform(vert_m9.begin(), vert_m9.end(), vert_m9.begin(), [&w5](auto& c) {return c * w5; });

	std::transform(vert_m10.begin(), vert_m10.end(), neutral.begin(), vert_m10.begin(), std::minus<float>());
	std::transform(vert_m10.begin(), vert_m10.end(), vert_m10.begin(), [&w6](auto& c) {return c * w6; });

	std::transform(neutral.begin(), neutral.end(), vert_m1.begin(), floatVertices.begin(), std::plus<float>());
	std::transform(floatVertices.begin(), floatVertices.end(), vert_m2.begin(), floatVertices.begin(), std::plus<float>());
	std::transform(floatVertices.begin(), floatVertices.end(), vert_m3.begin(), floatVertices.begin(), std::plus<float>());
	std::transform(floatVertices.begin(), floatVertices.end(), vert_m4.begin(), floatVertices.begin(), std::plus<float>());
	std::transform(floatVertices.begin(), floatVertices.end(), vert_m5.begin(), floatVertices.begin(), std::plus<float>());
	std::transform(floatVertices.begin(), floatVertices.end(), vert_m6.begin(), floatVertices.begin(), std::plus<float>());
	std::transform(floatVertices.begin(), floatVertices.end(), vert_m7.begin(), floatVertices.begin(), std::plus<float>());
	std::transform(floatVertices.begin(), floatVertices.end(), vert_m8.begin(), floatVertices.begin(), std::plus<float>());
	std::transform(floatVertices.begin(), floatVertices.end(), vert_m9.begin(), floatVertices.begin(), std::plus<float>());
	std::transform(floatVertices.begin(), floatVertices.end(), vert_m10.begin(), floatVertices.begin(), std::plus<float>());
}