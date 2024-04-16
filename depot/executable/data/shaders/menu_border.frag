///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// shader input
varying      vec4  v_v4Interior;   // raw object coordinates (xy = normal, zw = inverse)
flat varying vec4  v_v4Border;     // border distance values (xy = inner, zw = outer)
varying      float v_v1Factor;     // 


void FragmentMain()
{
    // 
#if defined(_P1_DIRECT_)
    bool bBorder = (v_v1Factor > 0.5);
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
        float fFactor = max(v_v1Factor, 0.0);
    #else
        const float fFactor = 1.0;
    #endif

        // lookup texture
        float v1Detail = coreTexture2D(0, v_av2TexCoord[0]).r;

        // draw interior with detail map
        gl_FragColor = vec4(vec3((v1Detail + c_v1Black) * fFactor), u_v4Color.a);
    }
}