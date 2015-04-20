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
    // lookup detail value and map between -1.0 and +1.0
    float v1Detail = coreTexture2D(0, v_av2TexCoord[0]).r;
          v1Detail = v1Detail * 2.0 - 1.0;

    // draw blended color
    float v1Full = v_v1Strength + v1Detail;
    gl_FragColor = vec4(u_v4Color.rgb * v1Full, u_v4Color.a * min(v1Full, 1.0));
}