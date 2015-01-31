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
varying float v_v1Mix;           // mix value between both outdoor textures
varying vec4  v_v4ShadowCoord;   // pixel coordinates viewed from the light source

        
void VertexMain()
{   
    // transform position and texture coordinates
    gl_Position      = u_m4ViewProj     * vec4(a_v3RawPosition, 1.0);
    v_v4ShadowCoord  = u_m4ShadowMatrix * vec4(a_v3RawPosition, 1.0);
    v_av2TexCoord[0] = a_v3RawPosition.xy * 0.04167;
    
    // dot-3 transform lighting vectors
    coreDot3VertexInit();
    v_av4LightDir[0].xyz = coreDot3VertexTransform(-u_aLight[0].v4Direction.xyz);
    v_v3ViewDir          = coreDot3VertexTransform((u_m4Camera * vec4(-a_v3RawPosition, 1.0)).xyz);
    
    // calculate mix value relative to vertex height
    v_v1Mix = (a_v3RawPosition.z + 20.0)*0.2;
}