///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// shader uniforms
uniform float u_v1TransitionTime;   // current transition time
uniform vec2  u_v2TransitionDir;    // 


void FragmentMain()
{
    // lookup detail value and map between -1.0 and +1.0
    float v1Offset = coreTexture2D(2, v_av2TexCoord[2]).r * 16.0 - 1.0;

    // lookup color textures
    vec4 v4TexColorOld = coreTexture2D(0, v_av2TexCoord[0]);
    vec4 v4TexColorNew = coreTexture2D(1, v_av2TexCoord[0]);

    // 
    vec2  v2AbsDir  = abs(u_v2TransitionDir);
    float v1ProjPos = dot(v_av2TexCoord[3], u_v2TransitionDir) / (v2AbsDir.x + v2AbsDir.y);

#if (_P1_TRANSITION_) == 1

    // calculate wipe effect
    float v1Rev = v1ProjPos * 0.5 + 0.5;
    float v1Mix = smoothstep(-0.1, 0.1, u_v1TransitionTime - v1Rev + v1Offset * 0.05);

#elif (_P1_TRANSITION_) == 2

    // calculate curtain effect
    float v1Rev = abs(v1ProjPos);
    float v1Mix = smoothstep(-0.1, 0.1, u_v1TransitionTime - v1Rev + v1Offset * 0.05);

#elif (_P1_TRANSITION_) == 3

    // calculate circle effect
    float v1Rev = coreLengthSq(v_av2TexCoord[3] / SQRT2);
    float v1Mix = smoothstep(-0.1, 0.1, u_v1TransitionTime - v1Rev + v1Offset * 0.05);

#else

    // just fade over
    float v1Mix = smoothstep(0.0, 1.0, u_v1TransitionTime);

#endif

    // interpolate smooth between both colors
    gl_FragColor = mix(v4TexColorOld, v4TexColorNew, v1Mix);
}