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
    float v1Value = coreTextureBase2D(0, v_av2TexCoord[0]).r;

    // 
    gl_FragColor  = vec4(vec3(0.0), (1.0 - v1Value) * u_v4Color.a);
}