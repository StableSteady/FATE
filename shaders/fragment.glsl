#version 330 core
in vec2 texCoord;
out vec4 FragColor;

uniform sampler2D aTexture;
uniform float alphaThreshold;

void main()
{
    vec4 color = texture2D(aTexture, texCoord);
    if (color.a < alphaThreshold)
        discard;
    FragColor = color;
} 