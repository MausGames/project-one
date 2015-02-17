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
    // calculate quadratic distance from the center as alpha value
    float v1Alpha = 1.0 - coreLengthSq(v_av2TexCoord[1]);

    // only draw visible area
    if(v1Alpha > 0.0)
    {
        // draw as alpha map with decent highlights
        float v1Value = coreTexture2D(0, v_av2TexCoord[0]).r;
        gl_FragColor  = vec4(vec3(0.4 + 0.6 * v1Value), v1Value * v1Alpha * u_v4Color.a);
    }
    else gl_FragColor = vec4(0.0);
}