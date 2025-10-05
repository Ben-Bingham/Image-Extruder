#include "ExportObj.h"

#include <fstream>

void ExportObj(const std::string& path, const Mesh& mesh) {
	std::ofstream file;
	file.open(path.c_str());
	file << "#vertices\n";

	for (int i = 0; i < mesh.vertices.size(); i += 8) {
		file << "v " << mesh.vertices[i + 0] << " " << mesh.vertices[i + 1] << " " << mesh.vertices[i + 2] << "\n";
	}

	file << "#faces:\n";

	for (int i = 0; i < mesh.indices.size(); i += 3) {
		file << "f " << (mesh.indices[i + 0] + 1) << " " << (mesh.indices[i + 1] + 1) << " " << (mesh.indices[i + 2] + 1) << "\n";
	}

	file.close();
}