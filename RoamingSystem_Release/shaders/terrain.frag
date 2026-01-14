#version 450 core

out vec4 FragColor;

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vTexCoord;
in float vHeight;
in mat3 vTBN;

// Color textures
uniform sampler2D uGrassTexture;
uniform sampler2D uRockTexture;
uniform sampler2D uSnowTexture;

// Normal maps
uniform sampler2D uGrassNormalMap;
uniform sampler2D uRockNormalMap;
uniform sampler2D uSnowNormalMap;

uniform bool uUseTextures;
uniform bool uUseNormalMaps;
uniform float uNormalMapStrength;

// Parameters
uniform float uMaxHeight;
uniform float uTextureTiling;
uniform vec3 uLightDir;
uniform vec3 uLightColor;
uniform vec3 uAmbientColor;
uniform float uLightIntensity;

// Height thresholds for texture blending
uniform float uGrassMaxHeight;
uniform float uRockMaxHeight;
uniform float uSlopeThreshold;

// Fog parameters
uniform vec3 uCameraPos;
uniform vec3 uFogColor;
uniform float uFogDensity;
uniform bool uFogEnabled;

void main()
{
    vec3 geometryNormal = normalize(vNormal);
    vec2 tiledUV = vTexCoord * uTextureTiling;
    
    // Calculate slope using geometry normal (0 = flat, 1 = vertical)
    float slope = 1.0 - dot(geometryNormal, vec3(0.0, 1.0, 0.0));
    
    // Normalized height
    float normalizedHeight = vHeight / uMaxHeight;
    
    // Calculate blend weights based on height and slope
    float grassWeight = 0.0;
    float rockWeight = 0.0;
    float snowWeight = 0.0;
    
    // Grass: low areas with low slope
    grassWeight = smoothstep(uGrassMaxHeight + 0.1, uGrassMaxHeight - 0.1, normalizedHeight);
    grassWeight *= smoothstep(uSlopeThreshold + 0.1, uSlopeThreshold - 0.1, slope);
    
    // Snow: high areas
    snowWeight = smoothstep(uRockMaxHeight - 0.1, uRockMaxHeight + 0.1, normalizedHeight);
    
    // Rock: steep slopes and mid-height areas
    rockWeight = 1.0 - grassWeight - snowWeight;
    rockWeight = max(rockWeight, smoothstep(uSlopeThreshold - 0.1, uSlopeThreshold + 0.2, slope));
    
    // Normalize weights
    float totalWeight = grassWeight + rockWeight + snowWeight + 0.001;
    grassWeight /= totalWeight;
    rockWeight /= totalWeight;
    snowWeight /= totalWeight;
    
    vec3 albedo;
    vec3 normal = geometryNormal;
    
    if (uUseTextures)
    {
        // Sample color textures
        vec3 grassColor = texture(uGrassTexture, tiledUV).rgb;
        vec3 rockColor = texture(uRockTexture, tiledUV).rgb;
        vec3 snowColor = texture(uSnowTexture, tiledUV).rgb;
        
        // Blend textures
        albedo = grassColor * grassWeight + rockColor * rockWeight + snowColor * snowWeight;
        
        // Sample and blend normal maps
        if (uUseNormalMaps)
        {
            vec3 grassNormal = texture(uGrassNormalMap, tiledUV).rgb * 2.0 - 1.0;
            vec3 rockNormal = texture(uRockNormalMap, tiledUV).rgb * 2.0 - 1.0;
            vec3 snowNormal = texture(uSnowNormalMap, tiledUV).rgb * 2.0 - 1.0;
            
            // Blend normals in tangent space
            vec3 tangentNormal = grassNormal * grassWeight + rockNormal * rockWeight + snowNormal * snowWeight;
            
            // Apply normal map strength
            tangentNormal.xy *= uNormalMapStrength;
            tangentNormal = normalize(tangentNormal);
            
            // Transform to world space using TBN matrix
            normal = normalize(vTBN * tangentNormal);
        }
    }
    else
    {
        // Use procedural colors for testing
        vec3 grassColor = vec3(0.2, 0.5, 0.1);
        vec3 rockColor = vec3(0.4, 0.35, 0.3);
        vec3 snowColor = vec3(0.95, 0.95, 1.0);
        
        albedo = grassColor * grassWeight + rockColor * rockWeight + snowColor * snowWeight;
    }
    
    // Directional lighting with dynamic color
    vec3 lightDir = normalize(-uLightDir);
    float NdotL = max(dot(normal, lightDir), 0.0);
    
    // Ambient and diffuse with dynamic colors
    vec3 ambient = uAmbientColor * albedo;
    vec3 diffuse = uLightIntensity * NdotL * uLightColor * albedo;
    
    vec3 finalColor = ambient + diffuse;
    
    // Apply fog
    if (uFogEnabled)
    {
        float distance = length(vWorldPos - uCameraPos);
        float fogFactor = exp(-distance * uFogDensity);
        fogFactor = clamp(fogFactor, 0.0, 1.0);
        finalColor = mix(uFogColor, finalColor, fogFactor);
    }
    
    FragColor = vec4(finalColor, 1.0);
}
