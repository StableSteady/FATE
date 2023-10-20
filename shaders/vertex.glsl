#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in ivec2 aBoneID;
layout (location = 3) in vec2 aBoneWeight;

out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const int MAX_BONES = 1000;
layout(std140) uniform boneTransforms {
    mat4 transforms[MAX_BONES];
};

void main()
{
    mat4 finalTransform = transforms[aBoneID[0]] * aBoneWeight[0] + transforms[aBoneID[1]] * aBoneWeight[1];
    gl_Position =  projection * view * model * finalTransform * vec4(aPos, 1.0);
    texCoord = aTexCoord;
}