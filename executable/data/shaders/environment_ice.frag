//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// layout qualifiers
#if defined(GL_ARB_conservative_depth)
    layout(depth_unchanged) out float gl_FragDepth;
#endif

// constant values
const vec3  c_v3Blue         = vec3(0.85, 0.85, 0.85) * 0.9 + vec3(0.0, 0.43, 0.69) * 0.1;   // default surface color
const float c_v1ThreshIgnore = 0.6980;   // 
const float c_v1ThreshTrace  = 0.6962;   // 
const float c_v1MaxHeight    = 20.0;     // 

// shader input
flat varying vec4 v_v4Lighting;   // lighting properties (xyz = light direction, w = height offset for smooth shores)


float GetReflFactor(const in vec3 v3ReflNormal, in vec3 v3BumpNormal)
{
    // 
    v3BumpNormal = normalize(vec3(v3BumpNormal.xy, v3BumpNormal.z * 0.7));

    // calculate dot-3 reflection factor
    float v1ReflFactor = max(0.0, dot(v3ReflNormal, v3BumpNormal));
          v1ReflFactor = 0.8 * pow(v1ReflFactor, 500.0);

    return v1ReflFactor;
}

vec3 GetBumpNormal(const in vec2 v2Offset)
{
    // lookup normal map
    return (coreTexture2D(0, v_av2TexCoord[0] + v2Offset).xyz) * 2.0 - 1.0;
}

void FragmentMain()
{
    vec2 v2ScreenCoord = gl_FragCoord.xy * u_v4Resolution.zw;

    // lookup depth map
    float v1Depth = coreTexture2D(3, v2ScreenCoord).r;

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
    float v1ReflFactor = 0.0;

    if(v1Depth > c_v1ThreshTrace)
    {
        // 
        vec3 v3Step = 6.0 * v3MathViewDir * vec3(-u_v4Resolution.z, u_v4Resolution.w, 1.0);

        // 
        float v1Height = 0.0;
        while(true)
        {
            // 
            v2ScreenCoord += v3Step.xy;
            v1Height      += v3Step.z;

            // 
            v1Depth = coreTexture2D(3, v2ScreenCoord).r;

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
    vec2 v2Distortion = v3BumpNormal.xy * 0.022;
    vec3 v3Reflection = coreTexture2D(1, v2ScreenCoord + v2Distortion).rgb;

    // adjust depth value
    v1Depth = smoothstep(0.64, 0.735, v1Depth) * 0.9 + v_v4Lighting.w;
    v1Depth = max(v1Depth - (0.5 - 0.5 * smoothstep(0.3, 0.4, v1Depth)), 0.0);

    // lookup refraction texture
    vec3 v3Refraction = coreTexture2D(2, v2ScreenCoord + v2Distortion * v1Depth).rgb;

    // adjust reflection value
    v3Reflection = mix(c_v3Blue * v1Intensity, v3Reflection, max(0.0, dot(v3BumpNormal, v3MathViewDir) - 0.27)) + vec3(v1ReflFactor);

    // draw final color
    float v1Light = (0.85 + 0.15 * v2ScreenCoord.x);
    gl_FragColor  = vec4(min(mix(v3Refraction, v3Reflection, v1Depth + 0.2) * (0.95 + 0.1 * v1BumpFactor), 1.0) * v1Light, 1.0);
}