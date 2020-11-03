#ifndef OBJ_MANIP_H_
#define OBJ_MANIP_H_

#include <iostream>
#include <string>
#include <fstream>
#include <conio.h>
#include <sstream>
#include <glm/glm.hpp>


class ObjManip
{
	std::string currentObj;
	std::string originalObj;
public:
	ObjManip(std::string filename, std::string outfile);
	void modifyObj(int position, glm::vec3 values);
};

#endif