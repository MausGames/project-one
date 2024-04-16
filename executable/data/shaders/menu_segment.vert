///////////////////////////////////////////////////////////
//*-----------------------------------------------------*//
//| Part of the Core Engine (https://www.maus-games.at) |//
//*-----------------------------------------------------*//
//| Copyright (c) 2013 Martin Mauersics                 |//
//| Released under the zlib License                     |//
//*-----------------------------------------------------*//
///////////////////////////////////////////////////////////


void VertexMain()
{
    // transform position
    gl_Position = coreObject2DPosition();

    // transform texture coordinates
    v_av2TexCoord[0] = coreObject2DTexCoord();
    v_av2TexCoord[1] = a_v2LowTexCoord * 2.0 - 1.0;
}