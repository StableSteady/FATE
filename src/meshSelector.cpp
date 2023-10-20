#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "meshSelector.h"
#include "model.h"

MeshSelector::MeshSelector() : model(nullptr) {
}

void MeshSelector::setModel(const Model* model) {
	this->model = model;
	selectedMeshes.clear();
	selectedMeshes.resize(model->meshes.size());
}

void MeshSelector::draw() {
	ImGui::Begin("Mesh Selector");
	if (!model) {
		ImGui::Text("No model selected.");
	} else {
		for (int i = 0; i < selectedMeshes.size(); ++i) {
			ImGui::Checkbox(model->meshes[i].name.c_str(), (bool*)&selectedMeshes[i]);
		}
	}
	ImGui::End();
}
