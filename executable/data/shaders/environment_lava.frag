///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// constant values
const vec3 c_v3LightDir = vec3(-0.583953857, -0.642349243, 0.496360779);   // overridden light direction


void FragmentMain()
{
    // lookup normal map (multiple times)
    vec2 v2Distortion1 = coreTexture2D(2, v_av2TexCoord[0]).xy;
    vec2 v2Distortion2 = coreTexture2D(2, v_av2TexCoord[1]).xy;
    vec2 v2Distortion  = (v2Distortion1 + v2Distortion2) * 2.0 - 2.0;

    // 
    vec2 v2FinalCoord = v_av2TexCoord[2] + v2Distortion * 0.05;
    vec3 v3TexNormal  = coreTexture2D(1, v2FinalCoord).xyz;
    vec3 v3TexColor   = coreTexture2D(0, v2FinalCoord).rgb;

    // calculate dot-3 bump factor
    vec3  v3MathLightDir = c_v3LightDir;
    vec3  v3BumpNormal   = normalize(v3TexNormal * 2.0 - 1.0);
    float v1BumpFactor   = max(dot(v3MathLightDir, v3BumpNormal), 0.0);

    // calculate diffuse value
    vec3 v3Diffuse = v3TexColor * (v1BumpFactor * 1.1 + 0.7);

    // draw final color
    gl_FragColor = vec4(v3Diffuse, 1.0);
}