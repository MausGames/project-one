//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


void VertexMain()
{
    // transform position and texture coordinates
    gl_Position      = coreObject3DPosition();
    v_av2TexCoord[0] = coreObject3DTexCoord();
    
    // dot-3 transform lighting vectors
    coreDot3VertexInit();
    v_av4LightDir[0].xyz = coreDot3VertexTransform(-u_asLight[0].v4Direction.xyz);
    v_v3ViewDir          = coreDot3VertexTransform(-coreObject3DViewDir().xyz);
}