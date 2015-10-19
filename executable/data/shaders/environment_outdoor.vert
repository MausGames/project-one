//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// constant values
const int   c_i1Size   = 31;     // number of vertices per axis
const float c_v1Detail = 6.0;    // size of a block

// shader uniforms
uniform mat4 u_m4ShadowMatrix;   // own shadow view-projection matrix (with coordinate adjustment)

// vertex attributes
attribute float a_v1Height;      // vertex height (full position is deduced from the vertex-ID)

// shader output
varying float v_v1Mix;           // mix value between both outdoor textures
varying vec4  v_v4ShadowCoord;   // pixel coordinates viewed from the light source


void VertexMain()
{
    // calculate full vertex position
    ivec2 i2Index       = ivec2(gl_VertexID % c_i1Size, gl_VertexID / c_i1Size);
    vec3  v3NewPosition = vec3(vec2(i2Index - ivec2(c_i1Size/2)) * c_v1Detail, a_v1Height);
    
    // transform position and shadow projection
    gl_Position     = u_m4ViewProj     * vec4(v3NewPosition, 1.0);
    v_v4ShadowCoord = u_m4ShadowMatrix * vec4(v3NewPosition, 1.0);
    
    // forward texture coordinates
    v_av2TexCoord[0] = v3NewPosition.xy * 0.04167;

    // transform lighting properties
    coreLightingTransform(v3NewPosition);

    // calculate mix value relative to vertex height
    v_v1Mix = (v3NewPosition.z + 20.0)*0.21;
}