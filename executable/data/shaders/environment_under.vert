//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// constant values
const float c_v1MapResolution = 0.42;   // normal map resolution

// shader uniforms
uniform float u_v1Time;     // current animation value
uniform float u_v1Offset;   // current y-position offset


void VertexMain()
{
    // transform position (with reduced depth)
    gl_Position    = coreObject3DPositionLow();
    gl_Position.z -= 0.4 * gl_Position.w;
    
    // calculate current mapping base
    vec2 vMapCoord = vec2(a_v2LowTexCoord.x * c_v1MapResolution,
                          a_v2LowTexCoord.y * c_v1MapResolution + u_v1Offset);

    // transform texture coordinates with different animation properties
    v_av2TexCoord[0] = (vMapCoord + (vec2( u_v1Time*0.5, -u_v1Time*0.5))) * 1.0;
}