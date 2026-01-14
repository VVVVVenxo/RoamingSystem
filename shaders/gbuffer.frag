#version 450 core

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;

in vec3 vViewPos;
in vec3 vViewNormal;

void main()
{
    // Store view-space position
    gPosition = vec4(vViewPos, 1.0);
    
    // Store view-space normal
    gNormal = vec4(normalize(vViewNormal), 1.0);
}
