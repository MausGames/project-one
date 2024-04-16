///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// layout qualifiers
#if defined(CORE_GL_conservative_depth)
    layout(depth_unchanged) out float gl_FragDepth;
#endif

// constant values
const vec3  c_v3Blue         = vec3(0.85, 0.85, 0.85) * 0.9 + vec3(0.0, 0.43, 0.69) * 0.1;   // default surface color
const float c_v1ThreshIgnore = 0.6980;   // 
const float c_v1ThreshTrace  = 0.6962;   // 
const float c_v1MaxHeight    = 20.0;     // 

// shader input
flat varying vec4 v_v4Lighting;          // lighting properties (xyz = light direction, w = height offset for smooth shores)


float GetReflFactor(const in vec3 v3ReflNormal, in vec3 v3BumpNormal)
{
    // 
    v3BumpNormal = normalize(vec3(v3BumpNormal.xy, v3BumpNormal.z * 0.7));

    // calculate dot-3 reflection factor
    float v1ReflFactor = max(dot(v3ReflNormal, v3BumpNormal), 0.0);
          v1ReflFactor = 0.75 * min(coreGGX(v1ReflFactor, 0.005), 1.0);

    return v1ReflFactor;
}

vec3 GetBumpNormal(const in vec2 v2Offset)
{
    // lookup normal map
    return (coreTexture2D(0, v_av2TexCoord[0] + v2Offset).xyz) * 2.0 - 1.0;
}

float GetDepthValue(const in vec2 v2ScreenCoord)
{
#if defined(GL_ES)

    float A = u_v4Resolution.z;
    return (coreTextureBase2D(3, v2ScreenCoord + vec2(  A, 0.0)).r +
            coreTextureBase2D(3, v2ScreenCoord + vec2( -A, 0.0)).r +
            coreTextureBase2D(3, v2ScreenCoord + vec2(0.0,   A)).r +
            coreTextureBase2D(3, v2ScreenCoord + vec2(0.0,  -A)).r) * 0.25;
#else

    // 
    return coreTextureBase2D(3, v2ScreenCoord).r;

#endif
}

void FragmentMain()
{
    vec2 v2ScreenCoord = gl_FragCoord.xy * u_v4Resolution.zw;

    // lookup depth map
    float v1Depth = GetDepthValue(v2ScreenCoord);

    if(v1Depth > c_v1ThreshIgnore)
    {
        discard;   // too deep, do not even try (# performance boost)
    }

    // 
    vec3 v3MathLightDir = normalize(v_v4Lighting.xyz);
    vec3 v3MathViewDir  = normalize(v_v3ViewDir);

    // 
    vec3  v3BumpNormal = vec3(0.0, 0.0, 1.0);
    float v1Intensity  = 0.15;
    float v1Height     = 0.0;
    float v1ReflFactor = 0.0;

    if(v1Depth > c_v1ThreshTrace)
    {
    #if (_CORE_QUALITY_ >= 1)
        const float v1StepSize = 3.0;
    #else
        const float v1StepSize = 6.0;
    #endif

        // 
        vec3 v3Step = v1StepSize * v3MathViewDir * vec3(-u_v4Resolution.z, u_v4Resolution.w, 1.0);

        // 
        for(int i = 0; i < 10; ++i)
        {
            // 
            v2ScreenCoord += v3Step.xy;
            v1Height      += v3Step.z;

            // 
            v1Depth = GetDepthValue(v2ScreenCoord);

            if(v1Depth <= c_v1ThreshTrace)
            {
                break;     // visible from the side
            }
            if(v1Height >= c_v1MaxHeight)
            {
                discard;   // too far away
            }
        }
    }
    else
    {
        // 
        v3BumpNormal = GetBumpNormal(vec2(0.0));
        v1Intensity = 1.0;

        // 
        vec3 v3ReflNormal = normalize(v3MathLightDir + v3MathViewDir);

        // 
        const float A = 0.001;
        v1ReflFactor  = (GetReflFactor(v3ReflNormal, v3BumpNormal)                           +
                         GetReflFactor(v3ReflNormal, GetBumpNormal(vec2(  A, 0.0)))          +
                         GetReflFactor(v3ReflNormal, GetBumpNormal(vec2( -A, 0.0)))          +
                         GetReflFactor(v3ReflNormal, GetBumpNormal(vec2(0.0,   A)))          +
                         GetReflFactor(v3ReflNormal, GetBumpNormal(vec2(0.0,  -A)))          +
                         GetReflFactor(v3ReflNormal, GetBumpNormal(vec2(  A,   A) * 0.7071)) +
                         GetReflFactor(v3ReflNormal, GetBumpNormal(vec2( -A,   A) * 0.7071)) +
                         GetReflFactor(v3ReflNormal, GetBumpNormal(vec2( -A,  -A) * 0.7071)) +
                         GetReflFactor(v3ReflNormal, GetBumpNormal(vec2(  A,  -A) * 0.7071))) * 0.25;
    }

    // calculate dot-3 bump factor
          v3BumpNormal = normalize(vec3(v3BumpNormal.xy, v3BumpNormal.z * 0.3));
    float v1BumpFactor = dot(v3MathLightDir, v3BumpNormal);

    // set distortion vector and lookup reflection texture
    vec2 v2Distortion = v3BumpNormal.xy * 0.012 * DistortionStrength(v2ScreenCoord);
    vec3 v3Reflection = coreTextureBase2D(1, v2ScreenCoord + v2Distortion).rgb;
    
    float v1Border = (1.0 - smoothstep(-0.001, 0.0025, (c_v1ThreshTrace - v1Depth) + 0.03 * (v2Distortion.x + v2Distortion.y))) * max(1.0 - 0.05 * v1Height, 0.0) + max(v1BumpFactor - 0.7, 0.0);

    // adjust depth value
    v1Depth = smoothstep(0.64, 0.735, v1Depth) * 0.9 + v_v4Lighting.w;
    v1Depth = max(v1Depth - (0.5 - 0.5 * smoothstep(0.3, 0.4, v1Depth)), 0.0);

    // lookup refraction texture
    vec3 v3Refraction = coreTextureBase2D(2, v2ScreenCoord + v2Distortion * v1Depth).rgb;

    // adjust reflection value
    v3Reflection = mix(c_v3Blue * v1Intensity, v3Reflection, max(dot(v3BumpNormal, v3MathViewDir) - 0.4, 0.0)) + vec3(v1ReflFactor);

    // draw final color
    //float v1Light = (0.85 + 0.15 * v2ScreenCoord.x);
    gl_FragColor  = vec4(mix(min(mix(v3Refraction, v3Reflection, v1Depth + 0.2) * (v1BumpFactor * 0.1 + 0.95), 1.0), vec3(1.0), v1Border), 1.0);
}