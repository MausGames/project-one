///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


void FragmentMain()
{
    // 
    float v1Value     = coreTextureBase2D(0, v_av2TexCoord[0]).r;
    float v1Intensity = (1.0 - coreLengthSq(v_av2TexCoord[1])) * 0.3;

    // 
    gl_FragColor  = vec4(vec3(0.0), mix(0.8, 0.0, max(v1Value, v1Intensity)));
}