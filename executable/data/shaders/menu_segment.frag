///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////

// TODO 1: integrate with GL_ARB_texture_query_lod and GL_EXT_texture_query_lod (ES)


void FragmentMain()
{
    // 
    float v1Intensity = 1.04 - coreLengthSq(v_av2TexCoord[1]) * 0.08;

    // draw only single channel (as color)
#if (CORE_GL_VERSION >= 400)
    float v1Value = textureLod(u_as2Texture2D[0], v_av2TexCoord[0], textureQueryLod(u_as2Texture2D[0], v_av2TexCoord[0]).y - 0.3).r;
#else
    float v1Value = coreTexture2D(0, v_av2TexCoord[0]).r;
#endif
    gl_FragColor  = vec4(u_v4Color.rgb * (v1Value * v1Intensity), u_v4Color.a);
}