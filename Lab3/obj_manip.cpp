#include "obj_manip.h"

ObjManip::ObjManip(std::string filename, std::string outfile)
{
	originalObj = filename;
	currentObj = outfile;
	std::ifstream ifs(filename.c_str(), std::ifstream::in);
	std::ofstream ofs(outfile.c_str(), std::ofstream::out);
	std::string line;
	if (!ifs.good())
		std::cout << "Can't open input file";
	if (!ofs.good())
		std::cout << "Can't open output file";
	while (ifs.good() && !ifs.eof() && std::getline(ifs, line)) {
		ofs << line <<'\n';
	}
	ifs.close();
	ofs.close();
}

void ObjManip::modifyObj(int position, glm::vec3 values)
{
	std::ifstream ifs(originalObj.c_str(), std::ifstream::in);
	std::ofstream ofs(currentObj.c_str(), std::ofstream::out);
	std::string line;
	int currentPos = 0;
	if (!ifs.good())
		std::cout << "Can't open input file";
	if (!ofs.good())
		std::cout << "Can't open output file";
	while (ifs.good() && !ifs.eof() && std::getline(ifs, line))
	{
		ofs << line<<'\n';
		currentPos++;
		if (currentPos == position + 2)
		{
			std::string vertex = "v " + std::to_string(values.x) + " " + std::to_string(values.y) + " " + std::to_string(values.z) + " //changed vertex";
			ofs << vertex<<'\n';
			std::getline(ifs, line);
		}
	}
	ifs.close();
	ofs.close();
}
