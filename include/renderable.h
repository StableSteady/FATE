#pragma once

struct Model;
struct Mesh;

struct Renderable {
	Renderable(Model* model, Mesh* mesh);
	~Renderable();
	Renderable(Renderable&& renderable) noexcept;
	Renderable(const Renderable&) = delete;
	Renderable& operator=(const Renderable&) = delete;
	void setVertexData();
	void setTextureData(Model* model);
	void bind() const;
	void draw() const;
	void unbind() const;

	Mesh* mesh;
	unsigned int VAO, VBO, EBO;
	unsigned int texture;
};