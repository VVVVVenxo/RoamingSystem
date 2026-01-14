/**
 * @file skybox.frag
 * @brief Skybox fragment shader
 * @author LuNingfang
 */

#version 450 core

out vec4 FragColor;

in vec3 vTexCoord;

uniform samplerCube uSkybox;
uniform vec3 uSkyColor;
uniform float uBlendFactor;

void main()
{
    vec4 texColor = texture(uSkybox, vTexCoord);
    vec3 finalColor = mix(texColor.rgb, uSkyColor, uBlendFactor);
    FragColor = vec4(finalColor, 1.0);
}
