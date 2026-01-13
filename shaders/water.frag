#version 450 core

out vec4 FragColor;

in vec4 vClipSpace;
in vec2 vTexCoord;
in vec3 vToCamera;
in vec3 vFromLight;
in vec3 vWorldPos;

uniform sampler2D uReflectionTexture;
uniform sampler2D uRefractionTexture;
uniform sampler2D uDudvMap;
uniform sampler2D uNormalMap;
uniform sampler2D uDepthMap;

uniform float uTime;
uniform float uWaveStrength;
uniform float uShineDamper;
uniform float uReflectivity;
uniform vec3 uWaterColor;
uniform vec3 uLightColor;
uniform float uLightIntensity;
uniform bool uUseTextures;

// Fog parameters
uniform vec3 uCameraPos;
uniform vec3 uFogColor;
uniform float uFogDensity;
uniform bool uFogEnabled;

const float waveSpeed = 0.03;
const float near = 0.1;
const float far = 1000.0;

void main()
{
    // Convert clip space to NDC, then to texture coordinates
    vec2 ndc = (vClipSpace.xy / vClipSpace.w) / 2.0 + 0.5;
    vec2 reflectTexCoord = vec2(ndc.x, 1.0 - ndc.y);
    vec2 refractTexCoord = vec2(ndc.x, ndc.y);
    
    vec2 totalDistortion = vec2(0.0);
    vec3 normal = vec3(0.0, 1.0, 0.0);
    
    if (uUseTextures)
    {
        // Animated texture coordinates for DuDv sampling
        float moveFactor = uTime * waveSpeed;
        vec2 distortedTexCoord = texture(uDudvMap, vec2(vTexCoord.x + moveFactor, vTexCoord.y)).rg * 0.1;
        distortedTexCoord = vTexCoord + vec2(distortedTexCoord.x, distortedTexCoord.y + moveFactor);
        
        // Sample DuDv map for distortion
        totalDistortion = (texture(uDudvMap, distortedTexCoord).rg * 2.0 - 1.0) * uWaveStrength;
        
        // Sample normal map
        vec4 normalMapColor = texture(uNormalMap, distortedTexCoord);
        normal = vec3(normalMapColor.r * 2.0 - 1.0, normalMapColor.b * 3.0, normalMapColor.g * 2.0 - 1.0);
        normal = normalize(normal);
    }
    else
    {
        // Procedural distortion without textures
        float wave1 = sin(vTexCoord.x * 20.0 + uTime * 2.0) * 0.5;
        float wave2 = sin(vTexCoord.y * 15.0 + uTime * 1.5) * 0.5;
        totalDistortion = vec2(wave1, wave2) * uWaveStrength * 0.5;
        
        // Procedural normal
        float nx = cos(vTexCoord.x * 20.0 + uTime * 2.0) * 0.3;
        float nz = cos(vTexCoord.y * 15.0 + uTime * 1.5) * 0.3;
        normal = normalize(vec3(nx, 1.0, nz));
    }
    
    // Apply distortion to texture coordinates
    reflectTexCoord += totalDistortion;
    reflectTexCoord.x = clamp(reflectTexCoord.x, 0.001, 0.999);
    reflectTexCoord.y = clamp(reflectTexCoord.y, 0.001, 0.999);
    
    refractTexCoord += totalDistortion;
    refractTexCoord = clamp(refractTexCoord, 0.001, 0.999);
    
    // Sample reflection and refraction textures
    vec4 reflectColor = texture(uReflectionTexture, reflectTexCoord);
    vec4 refractColor = texture(uRefractionTexture, refractTexCoord);
    
    // Fresnel effect - more reflection at grazing angles
    vec3 viewVector = normalize(vToCamera);
    float refractiveFactor = dot(viewVector, vec3(0.0, 1.0, 0.0));
    refractiveFactor = pow(refractiveFactor, 0.5);
    refractiveFactor = clamp(refractiveFactor, 0.0, 1.0);
    
    // Specular highlights with dynamic sun color
    vec3 reflectedLight = reflect(normalize(vFromLight), normal);
    float specular = max(dot(reflectedLight, viewVector), 0.0);
    specular = pow(specular, uShineDamper);
    vec3 specularHighlights = uLightColor * specular * uReflectivity * uLightIntensity;
    
    // Mix reflection and refraction based on Fresnel
    FragColor = mix(reflectColor, refractColor, refractiveFactor);
    
    // Add water color tint
    FragColor = mix(FragColor, vec4(uWaterColor, 1.0), 0.2);
    
    // Add specular highlights
    FragColor.rgb += specularHighlights;
    
    // Apply fog
    if (uFogEnabled)
    {
        float distance = length(vWorldPos - uCameraPos);
        float fogFactor = exp(-distance * uFogDensity);
        fogFactor = clamp(fogFactor, 0.0, 1.0);
        FragColor.rgb = mix(uFogColor, FragColor.rgb, fogFactor);
    }
    
    // Slight transparency
    FragColor.a = 0.9;
}
