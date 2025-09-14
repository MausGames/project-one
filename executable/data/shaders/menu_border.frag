///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "engine/data_transform_2d.glsl"
#include "shared.glsl"


// shader input
smooth varying vec4 v_v4Interior;   // raw object coordinates (xy = normal, zw = inverse)
flat   varying vec4 v_v4Border;     // border distance values (xy = inner, zw = outer)
smooth varying vec2 v_v2Factor;     // 


void FragmentMain()
{
    // lookup texture
    float v1Detail = coreTexture2D(0, v_av2TexCoord[0]).r;   // # AMD hotfix: fetch outside of branch to prevent border artifacts

    // 
#if defined(_P1_DIRECT_)
    bool bBorder = (v_v2Factor.x > 0.5);
#else
    const bool bBorder = true;
#endif

    // compare object coordinates with border values
    if(any(lessThan   (v_v4Interior, v_v4Border.xyxy)) &&
       all(greaterThan(v_v4Interior, v_v4Border.zwzw)) && bBorder)
    {
        // draw plain border
        gl_FragColor = vec4(vec3(c_v1White), 1.0) * u_v4Color;
    }
    else
    {
        // 
    #if defined(_P1_DIRECT_)
        float v1Factor = max(v_v2Factor.x, 0.0) * (1.0 - abs(v_v2Factor.y));
    #else
        vec2  v2ScreenCoord = (gl_FragCoord.xy - u_v4Resolution.xy * 0.5) * max(u_v4Resolution.z, u_v4Resolution.w);
        float v1Factor      = 1.1 - coreLengthSq(v2ScreenCoord) * 0.85;
    #endif

        // draw interior with detail map
        gl_FragColor = vec4(vec3((v1Detail + c_v1Black) * (v1Factor + (coreDither() / 255.0))), u_v4Color.a);
    }
}