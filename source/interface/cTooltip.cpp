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
cTooltip::cTooltip()noexcept
: m_iNumLines (0u)
, m_bDisplay  (false)
, m_pLastRef  (NULL)
{
    // 
    this->DefineProgram("default_2d_program");
    this->DefineTexture(0u, "default_white.png");

    // 
    this->SetPosition (TOOLTIP_MOUSE_OFFSET);
    this->SetAlignment(coreVector2(TOOLTIP_MOUSE_OFFSET.x ? SIGN(TOOLTIP_MOUSE_OFFSET.x) : 0.0f,
                                   TOOLTIP_MOUSE_OFFSET.y ? SIGN(TOOLTIP_MOUSE_OFFSET.y) : 0.0f));
    this->SetColor3   (COLOR_MENU_BLACK);

    // 
    for(coreUintW i = 0u; i < TOOLTIP_LINES; ++i)
    {
        m_aLine[i].Construct   (MENU_FONT_SMALL, TOOLTIP_OUTLINE_SIZE, 0u);
        m_aLine[i].SetAlignment(coreVector2(1.0f,-1.0f));
        m_aLine[i].SetColor3   (COLOR_MENU_WHITE);
    }
}


// ****************************************************************
// render the tooltip
void cTooltip::Render()
{
    if(!m_bDisplay) return;
    m_bDisplay = false;

    // 
    coreObject2D::Render();

    // 
    for(coreUintW i = 0u; i < m_iNumLines; ++i)
        m_aLine[i].Render();
}


// ****************************************************************
// move the tooltip
void cTooltip::Move()
{
    if(!m_bDisplay) return;

    // 
    coreObject2D::Move();

    // 
    for(coreUintW i = 0u; i < m_iNumLines; ++i)
    {
        m_aLine[i].SetCenter(this->GetCenter());
        m_aLine[i].Move();
    }
}


// ****************************************************************
// 
void cTooltip::ShowText(const coreFloat& fWidth, const coreChar* pcText)
{
    // 
    m_bDisplay = true;
    this->SetCenter(Core::Input->GetMousePosition());

    // 
    if(m_pLastRef == pcText) return;
    m_pLastRef = pcText;

    // 
    this->__ShowText(fWidth, PRINT("%s", pcText));
}


// ****************************************************************
// 
void cTooltip::__ShowText(const coreFloat& fWidth, const coreChar* pcText)
{
    const coreFontPtr& pFont   = m_aLine[0].GetFont();
    const coreUint8    iHeight = CORE_LABEL_HEIGHT_RELATIVE(m_aLine[0].GetHeight());
    const coreFloat    fFactor = CORE_LABEL_SIZE_FACTOR;

    // 
    coreChar* pcCursor    = c_cast<coreChar*>(pcText);
    coreChar* pcLineBegin = pcCursor;
    coreChar* pcLineEnd   = pcCursor;

    // 
    coreFloat fLineWidth = 0.0f;
    coreFloat fCurWidth  = 0.0f;
    coreFloat fMaxWidth  = 0.0f;
    coreFloat fSkip      = 0.0f;

    // 
    m_iNumLines = 0u;

    // 
    auto nWriteLineFunc = [&]()
    {
        // 
        *pcLineEnd = '\0';

        // 
        m_aLine[m_iNumLines++].SetText(pcLineBegin);
        fMaxWidth = MAX(fMaxWidth, fLineWidth);

        // 
        pcLineBegin = pcLineEnd = pcLineEnd + 1u;
        fCurWidth -= fSkip;

        ASSERT(m_iNumLines < TOOLTIP_LINES)
    };

    // 
    for(; *pcCursor; ++pcCursor)
    {
        // 
        if(*pcCursor == '\n')
        {
            pcLineEnd  = pcCursor;
            fLineWidth = fCurWidth;
            fSkip      = fCurWidth;

            nWriteLineFunc();
        }
        else
        {
            // 
            coreInt32 iAdvance;
            const coreUint8 iBytes   = pFont->RetrieveGlyphMetrics(pcCursor, iHeight, TOOLTIP_OUTLINE_SIZE, NULL, NULL, NULL, NULL, &iAdvance);
            const coreFloat fAdvance = I_TO_F(iAdvance) * fFactor;

            // 
            if(*pcCursor == ' ')
            {
                pcLineEnd  = pcCursor;
                fLineWidth = fCurWidth;
                fSkip      = fCurWidth + fAdvance;   // with whitespace
            }

            //
            pcCursor += iBytes - 1u;

            // 
            fCurWidth += fAdvance;
            if(fCurWidth >= fWidth)
            {
                // 
                if(pcLineBegin != pcLineEnd)
                {
                    nWriteLineFunc();
                }
            }
        }
    }

    // 
    m_aLine[m_iNumLines++].SetText(pcLineBegin);
    fMaxWidth = MAX(fMaxWidth, fCurWidth) + I_TO_F(2u * TOOLTIP_OUTLINE_SIZE) * fFactor;

    // 
    this->SetSize(coreVector2(fMaxWidth, TOOLTIP_LINE_HEIGHT * I_TO_F(m_iNumLines)) + TOOLTIP_BORDER_SIZE);

    // 
    const coreVector2 vBase = (this->GetSize() * this->GetAlignment() + (TOOLTIP_BORDER_SIZE - this->GetSize()) * m_aLine[0].GetAlignment()) * 0.5f + TOOLTIP_MOUSE_OFFSET;
    for(coreUintW i = 0u; i < m_iNumLines; ++i) m_aLine[i].SetPosition(coreVector2(vBase.x, vBase.y - TOOLTIP_LINE_HEIGHT * I_TO_F(i)));
}