/**
 * @file ssao.frag
 * @brief SSAO calculation shader
 * @author LuNingfang
 */

#version 450 core

out float FragColor;

in vec2 vTexCoord;

uniform sampler2D uPositionTex;
uniform sampler2D uNormalTex;
uniform sampler2D uNoiseTex;

uniform vec3 uSamples[64];
uniform mat4 uProjection;

uniform float uRadius;
uniform float uBias;
uniform int uKernelSize;
uniform vec2 uNoiseScale;

void main()
{
    vec3 fragPos = texture(uPositionTex, vTexCoord).xyz;
    vec3 normal = normalize(texture(uNormalTex, vTexCoord).xyz);
    vec3 randomVec = normalize(texture(uNoiseTex, vTexCoord * uNoiseScale).xyz);
    
    // Check if this is a valid fragment (has geometry)
    if (length(fragPos) < 0.001)
    {
        FragColor = 1.0;
        return;
    }
    
    // Create TBN matrix for orienting sample hemisphere
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    
    float occlusion = 0.0;
    int validSamples = 0;
    
    for (int i = 0; i < uKernelSize; i++)
    {
        // Get sample position in view space
        vec3 samplePos = TBN * uSamples[i];
        samplePos = fragPos + samplePos * uRadius;
        
        // Project sample position to screen space
        vec4 offset = vec4(samplePos, 1.0);
        offset = uProjection * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;
        
        // Check bounds
        if (offset.x < 0.0 || offset.x > 1.0 || offset.y < 0.0 || offset.y > 1.0)
        {
            continue;
        }
        
        // Get sample depth from G-Buffer
        float sampleDepth = texture(uPositionTex, offset.xy).z;
        
        // Range check and accumulate occlusion
        float rangeCheck = smoothstep(0.0, 1.0, uRadius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + uBias ? 1.0 : 0.0) * rangeCheck;
        validSamples++;
    }
    
    if (validSamples > 0)
    {
        occlusion = 1.0 - (occlusion / float(validSamples));
    }
    else
    {
        occlusion = 1.0;
    }
    
    FragColor = occlusion;
}
