#version 450 core

out vec4 FragColor;

in vec3 vColor;
in vec2 vTexCoord;

uniform sampler2D uTexture;
uniform bool uUseTexture;

void main()
{
    if (uUseTexture)
    {
        vec4 texColor = texture(uTexture, vTexCoord);
        FragColor = texColor * vec4(vColor, 1.0);
    }
    else
    {
        FragColor = vec4(vColor, 1.0);
    }
}
