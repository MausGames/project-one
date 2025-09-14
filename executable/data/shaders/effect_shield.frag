///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// shader input
smooth varying float v_v1Strength;   // 
flat   varying float v_v1Blink;      // 


void FragmentMain()
{
    // 
    float v1Value  = coreTexture2D(0, v_av2TexCoord[0]).r * 0.85 + 0.15;
          v1Value *= v_v1Strength;
    // 
    gl_FragColor.rgb = u_v4Color.rgb * (v1Value + v_v1Blink);
    gl_FragColor.a   = u_v4Color.a   * min(v1Value, 1.0);
}