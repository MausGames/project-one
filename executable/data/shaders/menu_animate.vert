//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// constant values
const vec2 c_v2DetailResolution = vec2(4.0,0.8) * 0.6;   // inlined detail-texture resolution


void VertexMain()
{
    // transform position
    gl_Position = coreObject2DPosition();

    // calculate basic texture coordinates (with alpha-value as fixed modifier)
    vec2 vNewTexCoord = a_v2LowTexCoord * u_v2TexSize - vec2(u_v4Color.a, 0.0);

    // animate final texture coordinates into 3 different directions
    v_av2TexCoord[0]  = vNewTexCoord                        + vec2(u_v2TexOffset.x, 0.0);
    v_av2TexCoord[1]  = vNewTexCoord                        - vec2(u_v2TexOffset.x, 0.0);
    v_av2TexCoord[2]  = vNewTexCoord * c_v2DetailResolution - vec2(0.0, u_v2TexOffset.y);
}