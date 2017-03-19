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
    // load object resources
    this->DefineProgram("default_2d_program");
    this->DefineTexture(0u, "default_white.png");

    // set object properties
    this->SetPosition (TOOLTIP_MOUSE_OFFSET);
    this->SetAlignment(coreVector2(TOOLTIP_MOUSE_OFFSET.x ? SIGN(TOOLTIP_MOUSE_OFFSET.x) : 0.0f,
                                   TOOLTIP_MOUSE_OFFSET.y ? SIGN(TOOLTIP_MOUSE_OFFSET.y) : 0.0f));
    this->SetColor3   (COLOR_MENU_BLACK);

    // create text lines
    for(coreUintW i = 0u; i < TOOLTIP_LINES; ++i)
    {
        m_aLine[i].Construct   (MENU_FONT_DYNAMIC_1, TOOLTIP_OUTLINE_SIZE, 0u);
        m_aLine[i].SetAlignment(coreVector2(1.0f,-1.0f));
        m_aLine[i].SetColor3   (COLOR_MENU_WHITE);
    }
}


// ****************************************************************
// render the tooltip
void cTooltip::Render()
{
    if(!m_bDisplay) return;
    m_bDisplay = false;   // # display once

    // render the background
    coreObject2D::Render();

    // render all active text lines
    for(coreUintW i = 0u; i < m_iNumLines; ++i)
        m_aLine[i].Render();
}


// ****************************************************************
// move the tooltip
void cTooltip::Move()
{
    if(!m_bDisplay) return;

    // move the background
    coreObject2D::Move();

    // move all active text lines
    for(coreUintW i = 0u; i < m_iNumLines; ++i)
    {
        m_aLine[i].SetCenter(this->GetCenter());
        m_aLine[i].Move();
    }
}


// ****************************************************************
// 
void cTooltip::__ShowText(const coreFloat fWidth, const coreChar* pcText)
{
    const coreFontPtr& pFont   = m_aLine[0].GetFont();
    const coreUint8    iHeight = CORE_LABEL_HEIGHT_RELATIVE(m_aLine[0].GetHeight());
    const coreFloat    fFactor = CORE_LABEL_SIZE_FACTOR;

    // 
    if(!pFont.IsUsable())
    {
        this->Invalidate();
        return;
    }

    // prepare cursor and line pointers
    coreChar* pcCursor    = c_cast<coreChar*>(pcText);
    coreChar* pcLineBegin = pcCursor;
    coreChar* pcLineEnd   = pcCursor;

    // prepare width properties
    coreFloat fLineWidth = 0.0f;   // between line-begin and line-end (last whitespace character)
    coreFloat fCurWidth  = 0.0f;   // between line-begin and cursor
    coreFloat fMaxWidth  = 0.0f;
    coreFloat fSkip      = 0.0f;

    // reset number of active text lines
    m_iNumLines = 0u;

    // save current line and continue processing
    auto nWriteLineFunc = [&]()
    {
        // terminate the string
        (*pcLineEnd) = '\0';

        // save text and width
        m_aLine[m_iNumLines++].SetText(pcLineBegin);
        fMaxWidth = MAX(fMaxWidth, fLineWidth);

        // 
        pcLineBegin = pcLineEnd = pcLineEnd + 1u;
        fCurWidth  -= fSkip;
    };

    // traverse the whole string only once (no rewind)
    for(; (*pcCursor); ++pcCursor)
    {
        // check for newline characters
        if((*pcCursor) == '\n')
        {
            pcLineEnd  = pcCursor;
            fLineWidth = fCurWidth;
            fSkip      = fCurWidth;

            nWriteLineFunc();
        }
        else
        {
            // get glyph width
            coreInt32 iAdvance;
            const coreUint8 iBytes   = pFont->RetrieveGlyphMetrics(pcCursor, iHeight, TOOLTIP_OUTLINE_SIZE, NULL, NULL, NULL, NULL, &iAdvance);
            const coreFloat fAdvance = I_TO_F(iAdvance) * fFactor;

            // check for whitespace character (valid line-end)
            if((*pcCursor) == ' ')
            {
                pcLineEnd  = pcCursor;
                fLineWidth = fCurWidth;
                fSkip      = fCurWidth + fAdvance;   // with whitespace
            }

            // skip additional characters (UTF-8)
            pcCursor += iBytes - 1u;

            // add glyph width and compare with target width
            fCurWidth += fAdvance;
            if(fCurWidth >= fWidth)
            {
                // save current line
                if(pcLineBegin != pcLineEnd)
                {
                    nWriteLineFunc();
                }
            }
        }

        // prevent overflow
        WARN_IF(m_iNumLines >= TOOLTIP_LINES-1u) break;
    }

    // save last text and width
    m_aLine[m_iNumLines++].SetText(pcLineBegin);
    fMaxWidth = MAX(fMaxWidth, fCurWidth) + I_TO_F(2u * TOOLTIP_OUTLINE_SIZE) * fFactor;

    // set background size
    this->SetSize(coreVector2(fMaxWidth, TOOLTIP_LINE_HEIGHT * I_TO_F(m_iNumLines)) + TOOLTIP_BORDER_SIZE);

    // set text line position
    const coreVector2 vBase = (this->GetSize() * this->GetAlignment() + (TOOLTIP_BORDER_SIZE - this->GetSize()) * m_aLine[0].GetAlignment()) * 0.5f + TOOLTIP_MOUSE_OFFSET;
    for(coreUintW i = 0u; i < m_iNumLines; ++i) m_aLine[i].SetPosition(coreVector2(vBase.x, vBase.y - TOOLTIP_LINE_HEIGHT * I_TO_F(i)));
}