#pragma once

struct Model;

class AnimationSelector {
public:
	AnimationSelector();
	void setModel(const Model* model);
	void draw();

	int selectedAnim;
	bool animChanged;
private:
	const Model* model;
};