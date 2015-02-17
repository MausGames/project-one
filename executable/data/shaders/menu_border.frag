//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// shader input
varying vec4 v_v4Interior;   // raw object coordinates (xy = normal, zw = reciprocal)
varying vec4 v_v4Border;     // border distance values (xy = inner, zw = outer)


void FragmentMain()
{
    // compare object coordinates with border values
    if(all(bvec2(any(lessThan   (v_v4Interior, v_v4Border.xyxy)),
                 all(greaterThan(v_v4Interior, v_v4Border.zwzw)))))
    {
        // draw plain border
        gl_FragColor = vec4(0.8, 0.8, 0.8, 1.0) * u_v4Color;
    }
    else
    {
        // draw interior
        gl_FragColor = coreTexture2D(0, v_av2TexCoord[0]) * u_v4Color;
    }
}