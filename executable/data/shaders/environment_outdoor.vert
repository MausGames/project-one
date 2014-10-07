//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// shadow uniforms
uniform mat4 u_m4ShadowMatrix;   // own shadow view-projection matrix (with coordinate adjustment)

// shader output
varying float v_fMix;            // mix value between both outdoor textures
varying vec4  v_v4ShadowPixel;   // pixel coordinates viewed from the light source

        
void VertexMain()
{   
    // transform position and texture coordinates
    gl_Position      = u_m4ViewProj     * vec4(a_v3Position, 1.0);
    v_v4ShadowPixel  = u_m4ShadowMatrix * vec4(a_v3Position, 1.0);
    v_av2TexCoord[0] = a_v2Texture;
    
    // dot-3 transform lighting vectors
    coreDot3VertexInit(mat3(1.0), a_v3Normal, a_v4Tangent);
    v_av4LightDir[0].xyz = coreDot3VertexTransform(-u_asLight[0].v4Direction.xyz);
    v_v3ViewDir          = coreDot3VertexTransform((u_m4Camera * vec4(-a_v3Position, 1.0)).xyz);
    
    // calculate mix value relative to vertex height
    v_fMix = (a_v3Position.z + 20.0)*0.2;
}