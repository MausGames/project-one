///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// 
uniform vec3 u_av3OverlayTransform[_P1_OVERLAYS_];   // 

// 
varying vec2 v_av2OverlayCoord[_P1_OVERLAYS_];       // 


void VertexMain()
{
    // transform position
    gl_Position = coreObject2DPosition();

    // 
    vec2 vBaseCoord = a_v2LowTexCoord * u_v2TexSize;

    // 
    for(int i = 0; i < (_P1_OVERLAYS_); ++i)
    {
        v_av2OverlayCoord[i] = vBaseCoord * u_av3OverlayTransform[i].z + u_av3OverlayTransform[i].xy;
    }
}