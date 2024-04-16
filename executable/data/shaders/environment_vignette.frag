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
    // 
    vec2 v2ScreenCoord = gl_FragCoord.xy * u_v4Resolution.zw;
         v2ScreenCoord = abs(v2ScreenCoord - 0.5);

    // 
    float v1Alpha = coreSaturate(max(v2ScreenCoord.x, v2ScreenCoord.y) * 12.0 - 3.7);

    //if(v1Alpha == 1.0)
    //{
    //    gl_FragColor = vec4(0.0, 1.0, 0.0, 0.5);
    //    return;
    //}
    //if(v1Alpha != 0.0)
    //{
    //    gl_FragColor = vec4(1.0, 0.0, 0.0, 0.5);
    //    return;
    //}

#if defined(_P1_GREY_)

    // draw only single channel (as color)
    float v1Value = coreTexture2D(0, v_av2TexCoord[0]).r;
    gl_FragColor  = vec4(vec3(v1Value), v1Alpha);

#else

    // 
    vec3 v3Color = coreTexture2D(0, v_av2TexCoord[0]).rgb;
    gl_FragColor = vec4(v3Color, v1Alpha);

#endif
}