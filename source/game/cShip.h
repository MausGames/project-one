//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_SHIP_H_
#define _P1_GUARD_SHIP_H_


// ****************************************************************
// ship interface
class INTERFACE cShip : public coreObject3D
{
protected:
    coreModelPtr m_pModelLow;   // low-polygon model object (used for depth-only, shadow, outline-effect)

    coreVector2 m_vNewPos;      // new position for smooth movement and animation


public:
    cShip()noexcept;
    virtual ~cShip() {}

    DISABLE_COPY(cShip)

    // define the visual appearance
    inline const coreModelPtr& DefineModelLow(const coreModelPtr& pModel) {m_pModelLow = pModel;                                          return m_pModelLow;}
    inline const coreModelPtr& DefineModelLow(const char*         pcName) {m_pModelLow = Core::Manager::Resource->Get<coreModel>(pcName); return m_pModelLow;}

    // render the ship (low-polygon)
    void Render(const coreProgramPtr& pProgram)override;
};


#endif // _P1_GUARD_SHIP_H_