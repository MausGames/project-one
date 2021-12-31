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
// constructor
cVolcanoBackground::cVolcanoBackground()noexcept
{
    // 
    this->__InitOwn();

    // create outdoor-surface object
    m_pOutdoor = new cOutdoor("rock", "lava", 7u, 4.0f);
    m_pOutdoor->LoadProgram(true);
}


// ****************************************************************
// destructor
cVolcanoBackground::~cVolcanoBackground()
{
    // 
    this->__ExitOwn();
}


// ****************************************************************
// 
void cVolcanoBackground::__InitOwn()
{
    // 
    m_pLavaSound = Core::Manager::Resource->Get<coreSound>("environment_lava.wav");
    m_pLavaSound.OnUsableOnce([this, pResource = m_pLavaSound]()
    {
        pResource->PlayRelative(this, 0.0f, 1.0f, true, SOUND_AMBIENT);
    });
}


// ****************************************************************
// 
void cVolcanoBackground::__ExitOwn()
{
    // 
    m_pLavaSound.OnUsableOnce([this, pResource = m_pLavaSound]()
    {
        if(pResource->EnableRef(this))
            pResource->Stop();
    });
}


// ****************************************************************
// 
void cVolcanoBackground::__RenderOwnBefore()
{
    // 
    m_Lava.Render();
}


// ****************************************************************
// 
void cVolcanoBackground::__MoveOwn()
{
    // 
    m_Lava.SetFlyOffset(g_pEnvironment->GetFlyOffset());
    m_Lava.Move();

    // 
    if(m_pLavaSound->EnableRef(this))
        m_pLavaSound->SetVolume(g_pEnvironment->RetrieveTransitionBlend(this));
}