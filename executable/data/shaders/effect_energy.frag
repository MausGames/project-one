//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// shader input
varying float v_v1Strength;   // light and color intensity


void FragmentMain()
{
#if (__VERSION__) >= 400 && (_CORE_QUALITY_) >= 1

    // evaluate shader per sample
    gl_SampleID;

#endif

    // lookup detail value and map between -1.0 and +1.0
    float v1Detail = coreTexture2D(0, v_av2TexCoord[0]).r;
          v1Detail = v1Detail * 2.0 - 1.0;

    // draw blended color
    float v1Full = max(v_v1Strength + v1Detail, 0.0);
    gl_FragColor = vec4(u_v4Color.rgb * v1Full, u_v4Color.a * min(v1Full, 1.0));
}