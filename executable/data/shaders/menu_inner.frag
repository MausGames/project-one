//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// constant values
const float c_v1White = 0.8;    // white contrast value (to reduce eye strain)
const float c_v1Black = 0.14;   // black contrast value

// shader input
varying vec2 v_v2Interior;   // raw object coordinates (x = normal, y = inverse)
varying vec2 v_v2Border;     // border distance values (x = inner, y = outer)


void FragmentMain()
{
    // compare object coordinates with border values
    if(all(bvec2(any(lessThan   (v_v2Interior.xy, v_v2Border.xx)),
                 all(greaterThan(v_v2Interior.xy, v_v2Border.yy)))))
    {
        // draw plain border
        gl_FragColor = vec4(vec3(c_v1White), 1.0) * u_v4Color;
    }
    else
    {
        // lookup textures
        vec4 v4Shape  = coreTexture2D(0, v_av2TexCoord[0]);
        vec3 v3Detail = coreTexture2D(1, v_av2TexCoord[1]).rgb;

        // draw animated shape with detail map and subtle white glow
        gl_FragColor = vec4(mix(vec3(1.0), v4Shape.rgb + v3Detail + vec3(c_v1Black), v4Shape.a), v4Shape.a) * u_v4Color;
    }
}