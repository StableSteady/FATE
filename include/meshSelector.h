#pragma once

#include <vector>

struct Model;

class MeshSelector {
public:
	MeshSelector();
	void setModel(const Model* model);
	void draw();

	std::vector<int> selectedMeshes;

private:
	const Model* model;
};
