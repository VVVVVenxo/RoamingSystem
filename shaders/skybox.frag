#version 450 core

out vec4 FragColor;

in vec3 vTexCoord;

uniform samplerCube uSkybox;

void main()
{
    FragColor = texture(uSkybox, vTexCoord);
}
