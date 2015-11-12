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
uniform float u_v1Offset;   // current y-position offset

// shader output
varying float v_v1Smooth;   // height offset for smooth shores


void VertexMain()
{
    // transform position
    vec4 v4NewPosition = vec4(coreObject3DTransformLow(), 1.0);
    gl_Position        = u_m4ViewProj * v4NewPosition;

    // transform lighting properties (resolved)
    v_v3TangentPos = v4NewPosition.xyz * vec3(1.0,-1.0,1.0);
    v_v3TangentCam = u_v3CamPosition   * vec3(1.0,-1.0,1.0);

    // transform texture coordinates 
    v_av2TexCoord[0] = vec2(a_v2LowTexCoord.x * c_v1MapResolution,
                            a_v2LowTexCoord.y * c_v1MapResolution + u_v1Offset) * 8.0;

    // calculate height offset
    v_v1Smooth = 0.042 * (u_v3Position.z - c_v1WaterHeight);
}