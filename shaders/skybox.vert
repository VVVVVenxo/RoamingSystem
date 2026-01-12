#version 450 core

layout (location = 0) in vec3 aPos;

out vec3 vTexCoord;

uniform mat4 uView;
uniform mat4 uProjection;

void main()
{
    vTexCoord = aPos;
    vec4 pos = uProjection * uView * vec4(aPos, 1.0);
    // Set depth to 1.0 (farthest) by setting z = w
    gl_Position = pos.xyww;
}
