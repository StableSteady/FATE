#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "constants.h"

struct aiScene;
struct aiMesh;
struct aiNode;
struct aiAnimation;
struct aiNodeAnim;

namespace fs = std::filesystem;

struct Vertex {
	glm::vec3 pos;
	glm::vec2 texCoord;
	int boneIDs[constants::MAX_BONE_INFLUENCE] = { 0 };
	float boneWeights[constants::MAX_BONE_INFLUENCE];
};

struct Bone {
	glm::mat4 offset;
	glm::mat4 transformation;
};


struct PositionKey {
	glm::vec3 value;
	double time;
};

struct RotationKey {
	glm::quat value;
	double time;
};

struct ScalingKey {
	glm::vec3 value;
	double time;
};

struct NodeAnim {
	NodeAnim(const aiNodeAnim* nodeAnim);
	std::string name;
	std::vector<PositionKey> positionKeys;
	std::vector<RotationKey> rotationKeys;
	std::vector<ScalingKey> scalingKeys;
};

struct Animation {
	Animation(const aiAnimation* animation);
	std::string name;
	double duration;
	double ticksPerSecond;
	std::vector<NodeAnim> channels;
};

struct Node {
	Node() = default;
	Node(const aiNode* node);
	std::string name;
	glm::mat4 transformation;
	std::vector<Node> children;
};

struct Mesh {
	Mesh(const aiMesh* mesh, glm::mat4* transform, Node* node);
	void loadVertices(const aiMesh* mesh);
	void loadBones(const aiMesh* mesh);

	std::string name;
	std::vector<Vertex> vertices;
	std::vector<Bone> bones;
	std::vector<unsigned int> indices;
	unsigned int textureID;
	std::unordered_map<std::string, int> boneMap;
	glm::mat4* globalInverseTransform;
	Node* rootNode;
};

struct Model {
	Model(fs::path& path);
	void loadMeshes(const aiScene* scene);
	void loadAnimations(const aiScene* scene);
	void loadTextures(const aiScene* scene, fs::path& modelPath);

	std::vector<Mesh> meshes;
	std::vector<Animation> animations;
	std::vector<fs::path> textures;
	glm::mat4 globalInverseTransform;
	Node rootNode;
};