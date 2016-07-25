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
cLeaderboard::cLeaderboard()noexcept
{
    for(coreUintW i = 0u; i < LEADERBOARD_ENTRIES; ++i)
    {
        // 
        m_aLine[i].DefineTexture(0u, "menu_detail_03.png");
        m_aLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aLine[i].DefineProgram("menu_inner_program");
        m_aLine[i].SetTexOffset (coreVector2(I_TO_F(i)*0.1f, 0.0f));

        // 
        m_aRank[i].Construct(MENU_FONT_SMALL, MENU_OUTLINE_SMALL, 3u);
        m_aRank[i].SetColor3(COLOR_MENU_WHITE);
        m_aRank[i].SetText  (PRINT("%d", i + 1u));

        // 
        m_aName[i].Construct(MENU_FONT_SMALL, MENU_OUTLINE_SMALL, LEADERBOARD_NAME_LENGTH + 1u);
        m_aName[i].SetColor3(COLOR_MENU_WHITE);
        m_aName[i].SetText  ("Horst");

        // 
        m_aScore[i].Construct(MENU_FONT_SMALL, MENU_OUTLINE_SMALL, 8u);
        m_aScore[i].SetColor3(COLOR_MENU_WHITE);
        m_aScore[i].SetText  ("0000000");
    }
}


// ****************************************************************
// 
void cLeaderboard::Render()
{
    // 
    #define __RENDER_ARRAY(a)                               \
    {                                                       \
        for(coreUintW i = 0u; i < LEADERBOARD_ENTRIES; ++i) \
        {                                                   \
            (a)[i].SetAlpha(this->GetAlpha());              \
            (a)[i].Render();                                \
        }                                                   \
    }
    {
        __RENDER_ARRAY(m_aLine)
        __RENDER_ARRAY(m_aRank)
        __RENDER_ARRAY(m_aName)
        __RENDER_ARRAY(m_aScore)
    }
    #undef __RENDER_ARRAY
}


// ****************************************************************
// 
void cLeaderboard::Move()
{
    // 
    if(CONTAINS_FLAG(m_iUpdate, CORE_OBJECT_UPDATE_TRANSFORM))
    {
        const coreVector2& vPos  = this->GetPosition();
        const coreVector2& vSize = this->GetSize();

        for(coreUintW i = 0u; i < LEADERBOARD_ENTRIES; ++i)
        {
            const coreFloat fHeight = I_TO_F(i) * -0.05f;

            // 
            m_aLine[i].SetPosition(vPos + coreVector2(0.0f, fHeight));
            m_aLine[i].SetSize    (coreVector2(vSize.x, 0.05f));
            m_aLine[i].Move();

            // 
            m_aRank[i].SetPosition (vPos + coreVector2(vSize.x * -0.44f, fHeight));
            m_aRank[i].SetAlignment(coreVector2(0.0f,0.0f));
            m_aRank[i].Move();

            // 
            m_aName[i].SetPosition (m_aRank[i].GetPosition() + coreVector2(0.05f,0.0f));
            m_aName[i].SetAlignment(coreVector2(1.0f,0.0f));
            m_aName[i].Move();

            // 
            m_aScore[i].SetPosition (vPos + coreVector2(vSize.x * 0.45f, fHeight));
            m_aScore[i].SetAlignment(coreVector2(-1.0f,0.0f));
            m_aScore[i].Move();
        }
    }

    // 
    coreObject2D::Move();
}