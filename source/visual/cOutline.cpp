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
// 
void cOutlineStyle::Construct(const coreChar* pcProgramSingleName, const coreChar* pcProgramInstancedName)
{
    // load shader-programs for outlined objects
    s_pProgramSingle    = Core::Manager::Resource->Get<coreProgram>(pcProgramSingleName);
    s_pProgramInstanced = Core::Manager::Resource->Get<coreProgram>(pcProgramInstancedName);
}


// ****************************************************************
// apply deferred outline-style
void cOutlineStyle::Apply()
{
    // draw single objects
    FOR_EACH(it, this->GetObjectSet())
        (*it)->Render(s_pProgramSingle);

    // draw lists with objects
    FOR_EACH(it, this->GetListSet())
        (*it)->Render(s_pProgramInstanced, s_pProgramSingle);
}


// ****************************************************************
// apply immediate outline-style to single object
void cOutlineStyle::ApplyObject(coreObject3D* pObject)const
{
    pObject->Render(s_pProgramSingle);
}


// ****************************************************************
// apply immediate outline-style to list with objects
void cOutlineStyle::ApplyList(coreBatchList* pList)const
{
    pList->Render(s_pProgramInstanced, s_pProgramSingle);
}


// ****************************************************************
// constructor
cOutline::cOutline()noexcept
{
    // 
    m_aOutlineStyle[OUTLINE_STYLE_FULL]  .Construct("effect_outline_program",        "effect_outline_inst_program");
    m_aOutlineStyle[OUTLINE_STYLE_DIRECT].Construct("effect_outline_direct_program", "effect_outline_direct_inst_program");
}


// ****************************************************************
// apply deferred outline-layer
void cOutline::Apply()
{
    // 
    if(std::any_of(m_aOutlineStyle, m_aOutlineStyle + OUTLINE_STYLES, [](const cOutlineStyle& oOutlineStyle) {return !oOutlineStyle.IsEmpty();}))
    {
        glDepthMask(false);
        {
            // 
            for(coreUintW i = 0u; i < OUTLINE_STYLES; ++i)
                m_aOutlineStyle[i].Apply();
        }
        glDepthMask(true);
    }
}


// ****************************************************************
// 
void cOutline::ClearObjects()
{
    // 
    for(coreUintW i = 0u; i < OUTLINE_STYLES; ++i)
        m_aOutlineStyle[i].ClearObjects();
}


// ****************************************************************
// 
void cOutline::ClearLists()
{
    // 
    for(coreUintW i = 0u; i < OUTLINE_STYLES; ++i)
        m_aOutlineStyle[i].ClearLists();
}