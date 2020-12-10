///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// constant values
const vec3 c_v3Blue = vec3(0.0, 0.43, 0.69);   // default surface color

// shader input
flat varying vec4 v_v4Lighting;                // lighting properties (xyz = light direction, w = height offset for smooth shores)


void FragmentMain()
{
    vec2 v2ScreenCoord = gl_FragCoord.xy * u_v4Resolution.zw;

    // lookup normal map (multiple times) and depth map
    vec3  v3BumpNormal1 = coreTexture2D(0, v_av2TexCoord[0]).xyz;
    vec3  v3BumpNormal2 = coreTexture2D(0, v_av2TexCoord[1]).xyz;
    float v1Depth       = coreTexture2D(3, v2ScreenCoord).r;

    // calculate dot-3 bump factor
    vec3  v3MathLightDir = normalize(v_v4Lighting.xyz);
    vec3  v3BumpNormal   = (v3BumpNormal1 + v3BumpNormal2) * 2.0 - 2.0;
          v3BumpNormal   = normalize(vec3(v3BumpNormal.xy, v3BumpNormal.z / 2.0));
    float v1BumpFactor   = dot(v3MathLightDir, v3BumpNormal);

    // set distortion vector and lookup reflection texture
    vec2 v2Distortion = v3BumpNormal.xy * 0.022 * DistortionStrength(v2ScreenCoord);
    vec3 v3Reflection = coreTexture2D(1, v2ScreenCoord + v2Distortion).rgb;

    // calculate dot-3 reflection factor
    vec3  v3MathViewDir = normalize(v_v3ViewDir);
    vec3  v3ReflNormal  = normalize(v3MathLightDir + v3MathViewDir);
    float v1ReflFactor  = max(0.0, dot(v3ReflNormal, v3BumpNormal));
          v1ReflFactor  = 0.6 * min(coreGGX(v1ReflFactor, 0.008), 1.0);

    // adjust depth value
    v1Depth = smoothstep(0.64, 0.735, v1Depth) * 0.8 * (1.0 + v1ReflFactor) + v_v4Lighting.w;
    v1Depth = max(v1Depth - (0.5 - 0.5 * smoothstep(0.3, 0.4, v1Depth)), 0.0);

    // lookup refraction texture
    vec3 v3Refraction = coreTexture2D(2, v2ScreenCoord + v2Distortion * v1Depth).rgb;

    // adjust reflection value
    v3Reflection = mix(c_v3Blue, v3Reflection, max(0.0, dot(v3BumpNormal, v3MathViewDir) - 0.3)) + vec3(v1ReflFactor);

    // draw final color
    gl_FragColor = vec4(mix(v3Refraction, v3Reflection, v1Depth) * (0.85 + 0.2 * v1BumpFactor), 1.0);
}