//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// shader input
varying float v_v1Strength;   // 


void FragmentMain()
{
    // 
    float v1Value = coreTexture2D(0, v_av2TexCoord[0]).g;
    gl_FragColor  = u_v4Color * (v_v1Strength * v1Value);
}