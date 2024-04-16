///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// 
void cOutlineStyle::Construct(const coreHashString& sProgramSingleName, const coreHashString& sProgramInstancedName)
{
    // load shader-programs for outlined objects
    s_pProgramSingle    = Core::Manager::Resource->Get<coreProgram>(sProgramSingleName);
    s_pProgramInstanced = Core::Manager::Resource->Get<coreProgram>(sProgramInstancedName);
}


// ****************************************************************
// apply deferred outline-style
void cOutlineStyle::Apply()
{
    // render single objects
    FOR_EACH(it, this->GetObjectSet())
        (*it)->Render(s_pProgramSingle, false);

    // render lists with objects
    FOR_EACH(it, this->GetListSet())
        (*it)->Render(s_pProgramInstanced, s_pProgramSingle, false);
}


// ****************************************************************
// apply immediate outline-style to single object
void cOutlineStyle::ApplyObject(coreObject3D* pObject)const
{
    pObject->Render(s_pProgramSingle, false);
}


// ****************************************************************
// apply immediate outline-style to list with objects
void cOutlineStyle::ApplyList(coreBatchList* pList)const
{
    pList->Render(s_pProgramInstanced, s_pProgramSingle, false);
}


// ****************************************************************
// constructor
cOutline::cOutline()noexcept
{
    // 
    m_aOutlineStyle[OUTLINE_STYLE_FULL]              .Construct("effect_outline_program",                    "effect_outline_inst_program");
    m_aOutlineStyle[OUTLINE_STYLE_DIRECT]            .Construct("effect_outline_direct_program",             "effect_outline_direct_inst_program");
    m_aOutlineStyle[OUTLINE_STYLE_FLAT_FULL]         .Construct("effect_outline_flat_program",               "effect_outline_flat_inst_program");
    m_aOutlineStyle[OUTLINE_STYLE_FLAT_DIRECT]       .Construct("effect_outline_flat_direct_program",        "effect_outline_flat_direct_inst_program");
    m_aOutlineStyle[OUTLINE_STYLE_FLAT_THICK]        .Construct("effect_outline_flat_thick_program",         "effect_outline_flat_thick_inst_program");
    m_aOutlineStyle[OUTLINE_STYLE_BULLET_FULL]       .Construct("effect_outline_bullet_program",             "effect_outline_bullet_inst_program");
    m_aOutlineStyle[OUTLINE_STYLE_BULLET_DIRECT]     .Construct("effect_outline_bullet_direct_program",      "effect_outline_bullet_direct_inst_program");
    m_aOutlineStyle[OUTLINE_STYLE_LIGHT]             .Construct("effect_outline_light_program",              "effect_outline_light_inst_program");
    m_aOutlineStyle[OUTLINE_STYLE_LIGHT_THICK]       .Construct("effect_outline_light_thick_program",        "effect_outline_light_thick_inst_program");
    m_aOutlineStyle[OUTLINE_STYLE_LIGHT_BULLET_THICK].Construct("effect_outline_light_bullet_thick_program", "effect_outline_light_bullet_thick_inst_program");
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