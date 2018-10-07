//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// constant values
const float c_v1White = 0.8;       // white contrast value (to reduce eye strain)
const float c_v1Black = 0.04;      // black contrast value

// shader input
varying      vec4  v_v4Interior;   // raw object coordinates (xy = normal, zw = inverse)
flat varying vec4  v_v4Border;     // border distance values (xy = inner, zw = outer)
varying      float v_v1Factor;     // 


void FragmentMain()
{
    // compare object coordinates with border values
    if(any(lessThan   (v_v4Interior, v_v4Border.xyxy)) &&
       all(greaterThan(v_v4Interior, v_v4Border.zwzw)))
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
        vec3 v3Detail = coreTexture2D(0, v_av2TexCoord[0]).rgb;

        // draw interior with detail map
        gl_FragColor = vec4((v3Detail + vec3(c_v1Black)) * fFactor, 1.0) * u_v4Color;
    }
}