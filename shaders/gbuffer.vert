/**
 * @file gbuffer.vert
 * @brief G-Buffer vertex shader (for SSAO)
 * @author LuNingfang
 */

#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;

out vec3 vViewPos;
out vec3 vViewNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

void main()
{
    vec4 viewPos = uView * uModel * vec4(aPos, 1.0);
    vViewPos = viewPos.xyz;
    
    mat3 normalMatrix = transpose(inverse(mat3(uView * uModel)));
    vViewNormal = normalize(normalMatrix * aNormal);
    
    gl_Position = uProjection * viewPos;
}
