///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// 
#define u_v1TransitionTime (u_v4Color.a)
#define u_v2TransitionDir  (u_v2TexOffset)


void FragmentMain()
{
    // 
    float v1Offset = coreTexture2D(2, v_av2TexCoord[2]).r * 16.0 - 1.0;

    // 
#if defined(_P1_LABEL_)
    vec2 v2Alpha = coreTexture2D(1, v_av2TexCoord[0]).rg;
#else
    vec4 v4Texture = coreTexture2D(0, v_av2TexCoord[0]);
#endif

    // 
    vec2  v2AbsDir  = abs(u_v2TransitionDir);
    float v1ProjPos = dot(v_av2TexCoord[1] * 0.8, u_v2TransitionDir) / (v2AbsDir.x + v2AbsDir.y);

    // 
    float v1Rev = v1ProjPos * 0.5 + 0.5;
    float v1Mix = coreSmoothStep(-0.1, 0.1, u_v1TransitionTime - v1Rev + v1Offset * 0.05);

    // 
#if defined(_P1_LABEL_)
    gl_FragColor = vec4(mix(vec3(0.1), u_v4Color.rgb * mix(1.05, 0.87, v_av2TexCoord[0].y / u_v2TexSize.y), v2Alpha.r), max(v2Alpha.r, v2Alpha.g) * v1Mix);
#else
    gl_FragColor = v4Texture * vec4(u_v4Color.rgb, v1Mix);
#endif
}