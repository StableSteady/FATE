#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "animationSelector.h"
#include "model.h"

AnimationSelector::AnimationSelector() : selectedAnim(0), animChanged(false), model(nullptr) {
}

void AnimationSelector::setModel(const Model* model) {
	this->model = model;
	selectedAnim = 0;
}

void AnimationSelector::draw() {
	ImGui::Begin("Animation Selector");
	if (!model) {
		ImGui::Text("No model selected.");
	} else {
		static int previousAnim = selectedAnim;
		for (int i = 0; i < model->animations.size(); ++i) {
			ImGui::RadioButton(model->animations[i].name.c_str(), &selectedAnim, i);
		}
		animChanged = previousAnim == selectedAnim ? false : true;
		previousAnim = selectedAnim;
	}
	ImGui::End();
}
