#pragma once

#include <chrono>
#include <vector>

#include <glm/glm.hpp>

struct NodeAnim;
struct Animation;
struct Node;
struct Mesh;

class Animator {
public:
	[[nodiscard]] std::vector<glm::mat4> getMeshTransforms(Mesh* mesh, Animation* animation, double timeElapsed);

	Mesh* mesh;
	Animation* animation;

private:
	void calculateMeshTransforms(const Animation* animation, double animationTick, const Node& node, const glm::mat4& parentTransform);
	[[nodiscard]] glm::mat4 getPositionKey(const NodeAnim* nodeAnim, double animationTick) const;
	[[nodiscard]] glm::mat4 getScalingKey(const NodeAnim* nodeAnim, double animationTick) const;
	[[nodiscard]] glm::mat4 getRotationKey(const NodeAnim* nodeAnim, double animationTick) const;
};