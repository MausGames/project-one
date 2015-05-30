//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// constant values
const vec3 c_v3Blue = vec3(0.0, 0.43, 0.69);   // default surface color

// shader input
varying float v_v1Smooth;   // height offset for smooth shores


void FragmentMain()
{
    vec2 v2ScreenCoord = gl_FragCoord.xy * u_v4Resolution.zw;

    // lookup normal map (multiple times) and depth map
    vec3  v3BumpNormal1 = normalize(coreTexture2D(0, v_av2TexCoord[0].st).xyz * 2.0 - 1.0);
    vec3  v3BumpNormal2 = normalize(coreTexture2D(0, v_av2TexCoord[1].st).xyz * 2.0 - 1.0);
    vec3  v3BumpNormal3 = normalize(coreTexture2D(0, v_av2TexCoord[2].st).xyz * 2.0 - 1.0);
    float v1Depth       = coreTexture2D(3, v2ScreenCoord).r;

    // calculate dot-3 bump factor
    vec3  v3MathLightDir = normalize(v_av4LightDir[0].xyz);
    vec3  v3BumpNormal   = normalize(v3BumpNormal1 + v3BumpNormal2 + v3BumpNormal3);
    float v1BumpFactor   = dot(v3MathLightDir, v3BumpNormal);

    // set distortion vector and lookup reflection texture
    vec2 v2Distortion      = v3BumpNormal.xy * 0.022;
    vec3 v3AboveReflection = coreTexture2D(1, v2ScreenCoord + v2Distortion).rgb;

    // calculate dot-3 reflection factor
    vec3  v3MathViewDir = normalize(v_v3ViewDir);
    vec3  v3ReflNormal  = normalize((2.0 * v1BumpFactor * v3BumpNormal) - v3MathLightDir);
    float v1ReflFactor  = max(0.0, dot(v3MathViewDir, v3ReflNormal));
          v1ReflFactor  = 0.72 * pow(v1ReflFactor, 62.0);

    // adjust depth value
    v1Depth = smoothstep(0.64, 0.735, v1Depth) * 0.8 * (1.0 + v1ReflFactor) + v_v1Smooth;
    v1Depth = max(v1Depth - (1.0 - smoothstep(0.3, 0.4, v1Depth)) * 0.5, 0.0);

    // lookup refraction texture
    vec3 v3BelowRefraction = coreTexture2D(2, v2ScreenCoord + v2Distortion * v1Depth).rgb;

    // adjust reflection value
    v3AboveReflection = 0.84 * (0.51 * c_v3Blue + 0.59 * v3AboveReflection + vec3(v1ReflFactor));

    // draw final color
    gl_FragColor = vec4(mix(v3BelowRefraction, v3AboveReflection, v1Depth) * (0.9 + 0.12 * v1BumpFactor), 1.0);
}