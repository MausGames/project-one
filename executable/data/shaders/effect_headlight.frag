//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


void FragmentMain()
{
    // 
    float v1Value     = coreTexture2D(0, v_av2TexCoord[0]).r;
    float v1Intensity = (1.0 - coreLengthSq(v_av2TexCoord[1])) * 0.1;

    // 
    gl_FragColor  = vec4(vec3(0.0), mix(0.8, 0.0, max(v1Value, v1Intensity)));
}