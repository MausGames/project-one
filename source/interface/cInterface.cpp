//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cInterface::cInterface()noexcept
: coreMenu (2, 0)
{

    m_Health.Construct   ("ethnocentric.ttf", 16, 16);
    m_Health.SetCenter   (coreVector2(-0.5f,-0.5f));
    m_Health.SetAlignment(coreVector2(1.0f,1.0f));

    m_Health.SetText("100 / 100");


    this->BindObject(0, &m_Health);
}


// ****************************************************************
// 
void cInterface::Move()
{


    coreMenu::Move();
}