#version 400

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vertColor;
layout(location = 2) in vec2 textureCoor;
out vec3 fragmentColor;
out vec2 textureCoord;

void main()
{
    gl_Position = vec4(position, 1.0);
    fragmentColor = vertColor;
    textureCoord = textureCoor;
}
