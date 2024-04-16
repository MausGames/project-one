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
    // lookup textures
    float v1Shape  = coreTexture2D(0, v_av2TexCoord[0]).a;   // alpha
    float v1Detail = coreTexture2D(1, v_av2TexCoord[1]).r;

    // draw shape with detail map and subtle white glow
    gl_FragColor = vec4(mix(vec3(1.0), vec3(v1Detail + c_v1Black + 0.1), v1Shape), v1Shape) * u_v4Color;
}