///////////////////////////////////////////////////////////
//*-----------------------------------------------------*//
//| Part of the Core Engine (https://www.maus-games.at) |//
//*-----------------------------------------------------*//
//| Copyright (c) 2013 Martin Mauersics                 |//
//| Released under the zlib License                     |//
//*-----------------------------------------------------*//
///////////////////////////////////////////////////////////
#include "engine/data_transform_2d.glsl"


void FragmentMain()
{
#if defined(_P1_SHADING_PIXEL_)

    // 
    mat2 m2Rota = mat2(u_v2TwoRotation.y, -u_v2TwoRotation.x, u_v2TwoRotation.x, u_v2TwoRotation.y);

    // 
    vec2 v2Center   = (v_av2TexCoord[0] - 0.5) * m2Rota;
    vec2 v2Factor   = u_v2TwoSize / round(u_v2TwoSize / 12.0) + 0.01;   // # offset to prevent sampling flickering due to rotation
    vec2 v2TexCoord = (m2Rota * ((floor(v2Center * v2Factor) + 0.5) / v2Factor) + 0.5) * u_v2TexSize + u_v2TexOffset;

#else

    // 
    vec2 v2TexCoord = v_av2TexCoord[0];

#endif

    // 
    gl_FragColor = coreTexture2D(0, v2TexCoord) * u_v4Color;

#if defined(_P1_SHADING_PIXEL_)

    // 
    gl_FragColor.rgb *= mix(1.1, 0.9, coreLengthSq(fract(v2Center * v2Factor)));
    gl_FragColor.a    = coreSmoothBlend(gl_FragColor.a);

#endif

#if defined(_P1_SHADING_SEPIA_)

    // 
    gl_FragColor.rgb = vec3(dot(gl_FragColor.rgb, vec3(0.299, 0.587, 0.114))) + vec3(0.191, -0.054, -0.221);

#endif
}