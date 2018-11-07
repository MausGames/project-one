///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


void FragmentMain()
{
    // calculate quadratic distance from the center as alpha value
    float v1Alpha = coreLengthSq(v_av2TexCoord[1]);

    // only draw visible area
    if(v1Alpha < 1.0)
    {
        v1Alpha *= v1Alpha;
        v1Alpha *= v1Alpha;

        // draw as alpha map with decent highlights
        float v1Value = coreTexture2D(0, v_av2TexCoord[0]).r;
              v1Value = max(v1Value - v1Alpha, 0.0);
        gl_FragColor  = vec4(vec3(0.3 + 0.7 * v1Value) * u_v4Color.rgb, v1Value * u_v4Color.a);
    }
    else gl_FragColor = vec4(0.0);
}