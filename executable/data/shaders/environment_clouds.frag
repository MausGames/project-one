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
    float v1Alpha = coreLengthSq(v_av2TexCoord[1]);

    // only draw visible area
    if(v1Alpha < 1.0)
    {
        v1Alpha *= v1Alpha;
        v1Alpha  = 1.0 - v1Alpha;
    
        // draw as alpha map with decent highlights
        float v1Value = coreTexture2D(0, v_av2TexCoord[0]).r;
        gl_FragColor  = vec4(vec3((0.35 + 0.65 * v1Value) * u_v4Color.r) , v1Value * v1Alpha * u_v4Color.a);
    }
    else discard; // gl_FragColor = vec4(0.0);
}