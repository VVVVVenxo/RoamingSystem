#version 450 core

layout (location = 0) in vec3 aPos;

out vec4 vClipSpace;
out vec2 vTexCoord;
out vec3 vToCamera;
out vec3 vFromLight;
out vec3 vWorldPos;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform vec3 uCameraPos;
uniform vec3 uLightDir;
uniform float uTiling;

void main()
{
    vec4 worldPos = uModel * vec4(aPos, 1.0);
    vWorldPos = worldPos.xyz;
    
    vClipSpace = uProjection * uView * worldPos;
    gl_Position = vClipSpace;
    
    // Texture coordinates based on world position
    vTexCoord = vec2(worldPos.x, worldPos.z) * uTiling * 0.01;
    
    // Vector from vertex to camera
    vToCamera = uCameraPos - worldPos.xyz;
    
    // Light direction (pointing from light)
    vFromLight = uLightDir;
}
