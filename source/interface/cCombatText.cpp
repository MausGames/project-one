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
cCombatText::sData::sData(const coreChar* pcText, const coreVector2& vPosition, const coreVector3& vColor)noexcept
: sText     (pcText)
, vPosition (vPosition)
, iColor    (coreVector4(vColor, 1.0f).PackUnorm4x8())
{
}

cCombatText::sData::sData(sData&& m)noexcept
: sText     (std::move(m.sText))
, vPosition (m.vPosition)
, iColor    (m.iColor)
{
}


// ****************************************************************
// constructor
cCombatText::cCombatText()noexcept
: m_iCurLabel (0u)
{
    // create label objects
    for(coreUintW i = 0u; i < COMBAT_LABELS; ++i)
        m_aLabel[i].Construct(MENU_FONT_MEDIUM_2, 16u);

    // reset animation timers
    std::memset(m_afTimer, 0, sizeof(m_afTimer));
}


// ****************************************************************
// render the combat text
void cCombatText::Render()
{
    // render active label objects
    for(coreUintW i = 0u; i < COMBAT_LABELS; ++i)
        if(m_afTimer[i]) m_aLabel[i].Render();
}


// ****************************************************************
// move the combat text
void cCombatText::Move()
{
    // check for available text data
    if(!m_aData.empty())
    {
        if(++m_iCurLabel >= COMBAT_LABELS) m_iCurLabel = 0u;

        // get next text data
        const sData& oData = m_aData.front();
        {
            // init label object
            coreLabel& oLabel = m_aLabel[m_iCurLabel];
            oLabel.SetText  (oData.sText.c_str());
            oLabel.SetCenter(oData.vPosition * g_vMenuCenter);
            oLabel.SetColor4(coreVector4::UnpackUnorm4x8(oData.iColor));
        }
        m_aData.pop_front();

        // start animation timer
        m_afTimer[m_iCurLabel] = 1.0f;
    }

    // 
    for(coreUintW i = 0u; i < COMBAT_LABELS; ++i)
    {
        coreFloat& fCurTimer = m_afTimer[i];
        if(fCurTimer)
        {
            // update animation timer
            fCurTimer = MAX(fCurTimer - 0.8f*Core::System->GetTime(), 0.0f);

            // move overlaying label objects away from each other
            for(coreUintW j = i+1u; j < COMBAT_LABELS; ++j)
            {
                if(m_afTimer[j])
                {
                    coreLabel& A = m_aLabel[i];
                    coreLabel& B = m_aLabel[j];

                    // 
                    const coreVector2 vRange = (A.GetSize()     + B.GetSize()) * 0.5f;
                    const coreVector2 vDiff  = (A.GetCenter()   - B.GetCenter()) +
                                               (A.GetPosition() - B.GetPosition());

                    // 
                    if((vRange.x > ABS(vDiff.x)) &&
                       (vRange.y > ABS(vDiff.y)))
                    {
                        // 
                        const coreFloat& fOffset = (vRange.x - ABS(vDiff.x)) * SIGN(vDiff.x) * 0.5f;

                        // 
                        A.SetCenter(A.GetCenter() + coreVector2(fOffset, 0.0f));
                        B.SetCenter(B.GetCenter() - coreVector2(fOffset, 0.0f));
                    }
                }
            }

            // update label objects
            coreLabel& oLabel = m_aLabel[i];
            oLabel.SetPosition(coreVector2(0.0f, 0.03f * (1.0f - fCurTimer)));
            oLabel.SetAlpha   (MIN(fCurTimer * 5.0f, 1.0f));
            oLabel.Move();
        }
    }
}


// ****************************************************************
// add new combat text
void cCombatText::AddText(const coreChar* pcText, const coreVector3& vPosition, const coreVector3& vColor)
{
    ASSERT(pcText)

    // insert new text data
    m_aData.push_back(sData(pcText, g_pForeground->Project(vPosition), vColor));
}


// ****************************************************************
// reset all active label objects
void cCombatText::Reset()
{
    // stop all animation timers
    std::memset(m_afTimer, 0, sizeof(m_afTimer));

    // clear remaining text data
    m_aData.clear();
}