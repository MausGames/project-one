///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "engine/data_transform_2d.glsl"


void FragmentMain()
{
#if defined(_P1_VIGNETTE_)

    // 
    vec2  v2ScreenCoord = (gl_FragCoord.xy - u_v4Resolution.xy * 0.5) * max(u_v4Resolution.z, u_v4Resolution.w);
    float v1Factor      = 1.1 - coreLengthSq(v2ScreenCoord) * 1.0;

#else

    // 
    const float v1Factor = 1.0;

#endif

    // draw only single channel (as color)
    float v1Value = coreTexture2D(0, v_av2TexCoord[0]).r;
    gl_FragColor  = vec4(u_v4Color.rgb * (v1Value * v1Factor), u_v4Color.a);
}