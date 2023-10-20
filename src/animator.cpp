#include <glm/gtx/quaternion.hpp>

#include "animator.h"
#include "model.h"

std::vector<glm::mat4> Animator::getMeshTransforms(Mesh* mesh, Animation* animation, double timeElapsed) {
	this->mesh = mesh;
	this->animation = animation;
	std::vector<glm::mat4> transforms;
	glm::mat4 identity(1.0f);

	double ticks = animation->ticksPerSecond * timeElapsed;
	double animationTicks = std::fmod(ticks, animation->duration);

	calculateMeshTransforms(animation, animationTicks, *mesh->rootNode, identity);

	transforms.resize(mesh->bones.size());
	for (unsigned int i = 0; i < transforms.size(); ++i) {
		transforms[i] = mesh->bones[i].transformation; // TODO Change to move
	}
	return transforms;
}

void Animator::calculateMeshTransforms(const Animation* animation, double animationTicks, const Node& node, const glm::mat4& parentTransform) {
	const auto& nodeAnims = animation->channels;

	auto nodeAnim = std::find_if(nodeAnims.begin(), nodeAnims.end(), [node](const auto& nodeAnim) {
		return nodeAnim.name == node.name;
		});
	glm::mat4 nodeTransform = node.transformation;
	if (nodeAnim != nodeAnims.end()) {
		nodeTransform = getPositionKey(nodeAnim._Ptr, animationTicks) * getRotationKey(nodeAnim._Ptr, animationTicks) * getScalingKey(nodeAnim._Ptr, animationTicks);
	}

	glm::mat4 globalTransform = parentTransform * nodeTransform;

	if (mesh->boneMap.contains(node.name)) {
		mesh->bones[mesh->boneMap[node.name]].transformation = *mesh->globalInverseTransform * globalTransform * mesh->bones[mesh->boneMap[node.name]].offset;
	}

	for (const auto& child : node.children) {
		calculateMeshTransforms(animation, animationTicks, child, globalTransform);
	}
}

glm::mat4 Animator::getPositionKey(const NodeAnim* nodeAnim, double animationTicks) const {
	const auto& positionKeys = nodeAnim->positionKeys;
	for (int i = 1; i < positionKeys.size(); ++i) {
		if (positionKeys[i].time >= animationTicks) {
			double alpha = (animationTicks - positionKeys[i - 1].time) / (positionKeys[i].time - positionKeys[i - 1].time);
			return glm::translate(glm::mat4(1.0f), glm::mix(positionKeys[i - 1].value, positionKeys[i].value, alpha));
		}
	}
	return glm::translate(glm::mat4(1.0f), positionKeys[0].value);
}

glm::mat4 Animator::getScalingKey(const NodeAnim* nodeAnim, double animationTicks) const {
	const auto& scalingKeys = nodeAnim->scalingKeys;
	for (int i = 1; i < scalingKeys.size(); ++i) {
		if (scalingKeys[i].time >= animationTicks) {
			double alpha = (animationTicks - scalingKeys[i - 1].time) / (scalingKeys[i].time - scalingKeys[i - 1].time);
			return glm::scale(glm::mat4(1.0f), glm::mix(scalingKeys[i - 1].value, scalingKeys[i].value, alpha));
		}
	}
	return glm::scale(glm::mat4(1.0f), scalingKeys[0].value);
}

glm::mat4 Animator::getRotationKey(const NodeAnim* nodeAnim, double animationTicks) const {
	const auto& rotationKeys = nodeAnim->rotationKeys;
	for (int i = 1; i < rotationKeys.size(); ++i) {
		if (rotationKeys[i].time >= animationTicks) {
			float alpha = (animationTicks - rotationKeys[i - 1].time) / (rotationKeys[i].time - rotationKeys[i - 1].time);
			return glm::toMat4(glm::slerp(rotationKeys[i - 1].value, rotationKeys[i].value, alpha));
		}
	}
	return glm::toMat4(rotationKeys[0].value);
}
