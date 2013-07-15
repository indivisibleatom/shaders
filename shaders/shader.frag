#version 400

layout(location = 0) out vec4 color;
in vec3 fragmentColor;
in vec2 textureCoord;
uniform sampler2D texture;

void main()
{
    color = texture2D( texture, textureCoord );
    //color = vec4(fragmentColor, 1.0f);
}
