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
#if defined(_P1_LIGHT_)

    // 
    vec2  v2ScreenCoord = gl_FragCoord.xy * u_v4Resolution.zw;
    float v1Light       = coreTextureBase2D(3, v2ScreenCoord).r * 1.75;

#else

    // 
    const float v1Light = 1.0;

#endif

#if defined(_P1_SPHERIC_)

    // calculate quartic distance from the center as alpha value
    float v1Alpha = coreLengthSq(v_av2TexCoord[1]);

    // only draw visible area
    if(v1Alpha < 1.0)
    {
        v1Alpha *= v1Alpha;
        v1Alpha *= v1Alpha;
        v1Alpha *= v1Alpha;
        v1Alpha  = 1.0 - v1Alpha;

        // draw color with alpha
        vec4 v4Texture = coreTexture2D(0, v_av2TexCoord[0]);
        gl_FragColor   = vec4(v4Texture.rgb * v1Light, v4Texture.a * v1Alpha) * u_v4Color;
    }
    else gl_FragColor = vec4(0.0);

#elif defined(_P1_SINGLE_)

    // draw only single channel (as alpha)
    float v1Value = coreTexture2D(0, v_av2TexCoord[0]).r;
    gl_FragColor  = vec4(u_v4Color.rgb * v1Light, u_v4Color.a * v1Value);

#else

    // draw color with alpha
    vec4 v4Texture = coreTexture2D(0, v_av2TexCoord[0]);
    gl_FragColor   = vec4(v4Texture.rgb * v1Light, v4Texture.a) * u_v4Color;

#endif
}