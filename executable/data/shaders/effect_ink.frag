///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// shader uniforms
uniform vec2 u_v2Alpha;                       // 
#if defined(_P1_LINE_)
    uniform vec4 u_av4Offset[_P1_SAMPLES_];   // 
#else
    uniform vec2 u_av2Offset[_P1_SAMPLES_];   // 
#endif

// shader input
varying vec4 v_v4Diff;                        // 


void FragmentMain()
{
#if defined(_P1_LINE_)

    // 
    if(abs(v_v4Diff.y) > 0.22)
    {
        gl_FragColor = vec4(0.0);
        return;
    }

    // 
    float v1Value = coreTexture2D(0, v_av2TexCoord[0]).r;

    // 
    vec2 v2Sum = vec2(0.0);
    for(int i = 0; i < (_P1_SAMPLES_); ++i)
    {
        vec2 v2Strength = vec2(distance(v_v4Diff.xy, u_av4Offset[i].xy),
                               distance(v_v4Diff.xy, u_av4Offset[i].zw));
        v2Sum += max(v2Strength * -4.8 + 2.0, 0.0);
    }

    // 
    float v1Max = v2Sum.x + v2Sum.y;

    // 
    v1Max *= u_v2Alpha.x;
    v1Max *= v1Value * 0.5 + 0.9;

#else

    // 
    if((coreLengthSq(v_v4Diff.xy) > 0.05) &&
       (coreLengthSq(v_v4Diff.zw) > 0.05))
    {
        gl_FragColor = vec4(0.0);
        return;
    }

    // 
    float v1Value = coreTexture2D(0, v_av2TexCoord[0]).r;

    // 
    vec2 v2Sum = vec2(0.0);
    for(int i = 0; i < (_P1_SAMPLES_); ++i)
    {
        vec2 v2Strength = vec2(distance(v_v4Diff.xy, u_av2Offset[i].xy),
                               distance(v_v4Diff.zw, u_av2Offset[i].yx));
        v2Sum += max(v2Strength * -4.8 + 2.0, 0.0);
    }

    // 
    v2Sum *= u_v2Alpha;
    v2Sum *= v1Value * 0.5 + 0.9;

    // 
    float v1Max = max(v2Sum.x, v2Sum.y);

#endif

    // 
    gl_FragColor = vec4(mix(vec3(0.03), vec3(0.184, 0.569, 0.635) * 1.35, clamp(v1Max - 2.9, 0.0, 1.0)), smoothstep(2.6, 2.9, v1Max));
}