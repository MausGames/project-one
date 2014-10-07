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

// water uniforms
uniform float u_fSmooth;   // dynamic smoothing value for better shores


void FragmentMain()
{
    vec2 v2ScreenCoord = gl_FragCoord.xy * u_v4Resolution.zw;
    
    // lookup normal map (multiple times) and depth map
    vec3 v3BumpNormal1 = normalize(coreTexture2D(0, v_av2TexCoord[0].st).xyz * 2.0 - 1.0);
    vec3 v3BumpNormal2 = normalize(coreTexture2D(0, v_av2TexCoord[1].st).xyz * 2.0 - 1.0);
    vec3 v3BumpNormal3 = normalize(coreTexture2D(0, v_av2TexCoord[2].st).xyz * 2.0 - 1.0);
    float fDepth       = coreTexture2D(3, v2ScreenCoord).r;

    // calculate dot-3 bump factor
    vec3 v3MathLightDir = normalize(v_av4LightDir[0].xyz);
    vec3 v3BumpNormal   = normalize(v3BumpNormal1 + v3BumpNormal2 + v3BumpNormal3);
    float fBumpFactor   = dot(v3MathLightDir, v3BumpNormal);
    
    // set distortion vector and lookup reflection texture
    vec2 v2Distortion      = v3BumpNormal.xy * 0.018;
    vec3 v3AboveReflection = coreTexture2D(1, v2ScreenCoord + v2Distortion).rgb;

    // calculate dot-3 reflection factor
    vec3 v3MathViewDir = normalize(v_v3ViewDir);
    vec3 v3ReflNormal  = normalize((2.0 * fBumpFactor * v3BumpNormal) - v3MathLightDir); 
    float fReflFactor  = max(0.0, dot(v3MathViewDir, v3ReflNormal));
          fReflFactor  = 0.8 * pow(fReflFactor, 55.0);
          
    // adjust depth value
    fDepth  = smoothstep(0.64, 0.735, fDepth) * 0.8 * (1.0 + fReflFactor);
    fDepth -= smoothstep(0.4,  0.3,   fDepth) * u_fSmooth;
   
    // lookup refraction texture
    vec3 v3BelowRefraction = coreTexture2D(2, v2ScreenCoord + v2Distortion * fDepth).rgb;
    
    // adjust reflection value
    v3AboveReflection = 0.85 * (0.5 * c_v3Blue + 0.6 * v3AboveReflection + vec3(fReflFactor));

    // draw final color
    gl_FragColor = vec4(mix(v3BelowRefraction, v3AboveReflection, fDepth), 1.0);
}