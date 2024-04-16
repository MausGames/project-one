///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// shader input
varying      vec2 v_v2Interior;   // raw object coordinates (x = normal, y = inverse)
flat varying vec2 v_v2Border;     // border distance values (x = inner, y = outer)
flat varying vec2 v_v2Scale;      // 


void FragmentMain()
{
    // compare object coordinates with border values
    if(any(lessThan   (v_v2Interior.xy, v_v2Border.xx)) &&
       all(greaterThan(v_v2Interior.xy, v_v2Border.yy)))
    {
        // draw plain border
        gl_FragColor = vec4(vec3(c_v1White), 1.0) * u_v4Color;
    }
    else
    {
        // prepare texture coordinates
        float v1Lerp = (coreLinearStep(0.0, v_v2Scale.x, v_av2TexCoord[0].y) +
                        coreLinearStep(v_v2Scale.y, 1.0, v_av2TexCoord[0].y)) * 0.5;

        // lookup textures
        float v1Shape  = coreTexture2D(0, vec2(v_av2TexCoord[0].x, v1Lerp)).r;
        float v1Detail = coreTexture2D(1, v_av2TexCoord[1]).r;

        // 
        vec2  v2ScreenCoord = (gl_FragCoord.xy - u_v4Resolution.xy * 0.5) * max(u_v4Resolution.z, u_v4Resolution.w);
        float v1Factor      = 1.1 - coreLengthSq(v2ScreenCoord) * 0.85;

        // draw shape with detail map and subtle white glow
        gl_FragColor = vec4(mix(vec3(1.0), vec3(v1Detail + c_v1Black + 0.1), v1Shape) * v1Factor, v1Shape) * vec4(mix(vec3(1.0), u_v4Color.rgb, 0.5), u_v4Color.a);
    }
}