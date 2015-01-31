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
          v1Detail = 2.0 * v1Detail - 1.0;
    
    // draw blended color
    gl_FragColor  = u_v4Color * (v_v1Strength + v1Detail);
}