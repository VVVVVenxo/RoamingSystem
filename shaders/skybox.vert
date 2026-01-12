#version 450 core

layout (location = 0) in vec3 aPos;

out vec3 vTexCoord;

uniform mat4 uView;
uniform mat4 uProjection;
uniform vec4 uClipPlane;

void main()
{
    vTexCoord = aPos;
    
    // For skybox, we need world position for clip plane
    // Skybox vertices are at unit cube, treat as world position for clipping
    vec4 worldPos = vec4(aPos * 1000.0, 1.0); // Scale up for clip plane calculation
    gl_ClipDistance[0] = dot(worldPos, uClipPlane);
    
    vec4 pos = uProjection * uView * vec4(aPos, 1.0);
    // Set depth to 1.0 (farthest) by setting z = w
    gl_Position = pos.xyww;
}
