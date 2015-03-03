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
    // choose animation-direction
    vec2 v2SideCoord = (v_av2TexCoord[0].y > 0.5) ? v_av2TexCoord[0] : v_av2TexCoord[1];
    
    // lookup textures
    vec4 v4Shape  = coreTexture2D(0, v2SideCoord);
    vec3 v3Detail = coreTexture2D(1, v_av2TexCoord[2]).rgb;
    
    // draw animated shape with detail map and subtle white glow
    gl_FragColor = vec4(mix(vec3(1.0), v4Shape.rgb + v3Detail, v4Shape.a), v4Shape.a * 0.9);
}