//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// shadow uniforms
//uniform mat4 u_mShadow;

// shader output
varying float v_fMix;
//varying vec4  v_v4ShadowPos;

        
void main()
{   
    vec3 v3ViewDir   = vec3(u_m4ModelView     * vec4(a_v3Position, 1.0));
    gl_Position      =      u_m4ModelViewProj * vec4(a_v3Position, 1.0);
    //v_v4ShadowPos  =      u_mShadow         * vec4(a_v3Position, 1.0);
    v_av2TexCoord[0] = a_v2Texture;
    
    coreDot3Init();
    coreDot3Transform(u_asLight[0].v4Direction.xyz, v_av4LightDir[0].xyz);
    coreDot3Transform(v3ViewDir,                    v_v3ViewDir);
    
    v_fMix = (a_v3Position.z + 20.0)*0.2;
}