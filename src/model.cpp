#include <cassert>
#include <cmath>
#include <iostream>
#include <span>
#include <utility>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#include <glm/gtx/quaternion.hpp>

#include "model.h"

static glm::mat4 glmFromAssimp(const aiMatrix4x4& m) { return glm::transpose(glm::make_mat4(&m.a1)); }
static glm::quat glmFromAssimp(const aiQuaternion& q) { return glm::quat(q.w, q.x, q.y, q.z); }
static glm::vec3 glmFromAssimp(const aiVector3D& v) { return glm::vec3(v.x, v.y, v.z); }

void Model::loadMeshes(const aiScene* scene) {
	for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
		meshes.emplace_back(scene->mMeshes[i], &globalInverseTransform, &rootNode);
	}
}

void Model::loadAnimations(const aiScene* scene) {
	for (unsigned int i = 0; i < scene->mNumAnimations; ++i) {
		animations.emplace_back(scene->mAnimations[i]);
	}
}

void Model::loadTextures(const aiScene* scene, fs::path& modelPath) {
	if (!scene->mMaterials) {
		return;
	}
	std::span mMaterials(scene->mMaterials, scene->mNumMaterials);
	for (const auto& material : mMaterials) {
		aiString texPath;
		material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath);
		fs::path directory = modelPath.parent_path();
		directory.append(texPath.C_Str());

		textures.push_back(std::move(directory));
	}
}

Mesh::Mesh(const aiMesh* mesh, glm::mat4* transform, Node* node) {
	name = mesh->mName.C_Str();
	loadVertices(mesh);
	loadBones(mesh);
	textureID = mesh->mMaterialIndex;
	globalInverseTransform = transform;
	rootNode = node;
}

void Mesh::loadVertices(const aiMesh* mesh) {
	vertices.resize(mesh->mNumVertices);
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
		auto& vertex = mesh->mVertices[i];
		vertices[i].pos = { vertex.x, vertex.y, vertex.z };
		if (mesh->mTextureCoords[0]) {
			vertices[i].texCoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
		}
		else {
			vertices[i].texCoord = { 0,0 };
		}

	}

	std::span mFaces(mesh->mFaces, mesh->mNumFaces);
	for (const auto& face : mFaces) {
		for (unsigned int i = 0; i < 3; ++i) {
			// faces are triangles
			indices.push_back(face.mIndices[i]);
		}
	}
}

void Mesh::loadBones(const aiMesh* mesh) {
	assert(mesh->mNumBones <= MAX_BONES);
	bones.resize(mesh->mNumBones);
	for (unsigned int i = 0; i < mesh->mNumBones; ++i) {
		boneMap[mesh->mBones[i]->mName.C_Str()] = i;
		bones[i].offset = glmFromAssimp(mesh->mBones[i]->mOffsetMatrix);
		std::span vertexWeights(mesh->mBones[i]->mWeights, mesh->mBones[i]->mNumWeights);
		for (const auto& [vertexID, vertexWeight] : vertexWeights) {
			for (int j = 0; j < constants::MAX_BONE_INFLUENCE; ++j) {
				if (vertices[vertexID].boneIDs[j] == 0) {
					vertices[vertexID].boneIDs[j] = i;
					vertices[vertexID].boneWeights[j] = vertexWeight;
					break;
				}
			}
		}
	}
}

Animation::Animation(const aiAnimation* anim) {
	name = anim->mName.C_Str();
	duration = anim->mDuration;
	ticksPerSecond = anim->mTicksPerSecond;
	for (unsigned int i = 0; i < anim->mNumChannels; ++i) {
		channels.emplace_back(anim->mChannels[i]);
	}
}

NodeAnim::NodeAnim(const aiNodeAnim* nodeAnim) {
	name = nodeAnim->mNodeName.C_Str();
	positionKeys.resize(nodeAnim->mNumPositionKeys);
	for (unsigned int i = 0; i < positionKeys.size(); ++i) {
		positionKeys[i].time = nodeAnim->mPositionKeys[i].mTime;
		positionKeys[i].value = glmFromAssimp(nodeAnim->mPositionKeys[i].mValue);
	}
	rotationKeys.resize(nodeAnim->mNumRotationKeys);
	for (unsigned int i = 0; i < rotationKeys.size(); ++i) {
		rotationKeys[i].time = nodeAnim->mRotationKeys[i].mTime;
		rotationKeys[i].value = glmFromAssimp(nodeAnim->mRotationKeys[i].mValue);
	}
	scalingKeys.resize(nodeAnim->mNumScalingKeys);
	for (unsigned int i = 0; i < scalingKeys.size(); ++i) {
		scalingKeys[i].time = nodeAnim->mScalingKeys[i].mTime;
		scalingKeys[i].value = glmFromAssimp(nodeAnim->mScalingKeys[i].mValue);
	}
}

Node::Node(const aiNode* node) {
	name = node->mName.C_Str();
	transformation = glmFromAssimp(node->mTransformation);
	std::span mChildren(node->mChildren, node->mNumChildren);

	for (const auto& child : mChildren) {
		children.emplace_back(child);
	}
}

Model::Model(fs::path& path) {
	std::cout << "Loading Model...\n";
	const std::chrono::time_point startTime = std::chrono::system_clock::now();
	auto importFlags = aiProcess_Triangulate;
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.string(), importFlags);
	if (!scene) {
		std::cout << importer.GetErrorString() << '\n';
	} else {
		loadMeshes(scene);
		loadAnimations(scene);
		loadTextures(scene, path);
		rootNode = Node(scene->mRootNode);
		globalInverseTransform = glm::inverse(glmFromAssimp(scene->mRootNode->mTransformation));
	}
	const std::chrono::time_point endTime = std::chrono::system_clock::now();
	std::cout << "Model loaded in " << std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count() << " seconds.\n";
}
