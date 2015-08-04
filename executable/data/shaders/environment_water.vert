//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// constant values
const float c_v1MapResolution = 0.42;    // normal map resolution
const float c_v1WaterHeight   = -20.0;   // default water z-position

// shader uniforms
uniform float u_v1Time;     // current animation value
uniform float u_v1Offset;   // current y-position offset

// shader output
varying float v_v1Smooth;   // height offset for smooth shores


void VertexMain()
{
    // transform view direction and position
    vec4 v4NewPosition = vec4(coreObject3DTransformLow(), 1.0);
    vec3 v3ViewDir     = (u_m4Camera   * v4NewPosition).xyz;
    gl_Position        =  u_m4ViewProj * v4NewPosition;

    // dot-3 transform lighting vectors (resolved)
    v_v3ViewDir = normalize(vec3(-v3ViewDir.x, v3ViewDir.y, -v3ViewDir.z));

    // calculate current mapping base
    vec2 vMapCoord = vec2(a_v2LowTexCoord.x * c_v1MapResolution,
                          a_v2LowTexCoord.y * c_v1MapResolution + u_v1Offset);

    // transform texture coordinates with different animation properties
    v_av2TexCoord[0] = (vMapCoord + (vec2( u_v1Time*0.5, -u_v1Time*0.5))) * 2.0;
    v_av2TexCoord[1] = (vMapCoord + (vec2(-u_v1Time*0.1,  u_v1Time*0.1))) * 4.0;

    // calculate height offset
    v_v1Smooth = 0.042 * (u_v3Position.z - c_v1WaterHeight);
}