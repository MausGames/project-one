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
    float fAlpha = 1.0 - coreLengthSq(2.0 * v_av2TexCoord[1] - 1.0);
    
    // only draw visible area
    if(fAlpha > 0.0) 
    {
        // draw as alpha map with decent highlights
        float fValue = coreTexture2D(0, v_av2TexCoord[0]).r;
        gl_FragColor = vec4(vec3(0.4 + 0.6 * fValue), fAlpha * fValue * u_v4Color.a);
    }
    else gl_FragColor = vec4(0.0);
}