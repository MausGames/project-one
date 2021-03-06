///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_LODOBJECT_H_
#define _P1_GUARD_LODOBJECT_H_

// TODO 5: change high-resolution models with pre-baked normal-maps or tessellation ?
// TODO 3: use low-resolution model for reflection rendering (disable RenderHighObject with a global flag ? or identify per engine property ?)


// ****************************************************************
// LOD object class
class cLodObject : public coreObject3D
{
private:
    coreModelPtr m_pModelHigh;   // high-polygon model object (used for regular shading)
    coreModelPtr m_pModelLow;    // low-polygon model object (used for shadow, outline, glow)


public:
    cLodObject() = default;
    virtual ~cLodObject()override = default;

    ENABLE_COPY(cLodObject)

    // define the visual appearance
    inline void DefineModelHigh(std::nullptr_t)               {m_pModelHigh = NULL;}
    inline void DefineModelHigh(const coreModelPtr&   pModel) {m_pModelHigh = pModel;}
    inline void DefineModelHigh(const coreHashString& sName)  {m_pModelHigh = Core::Manager::Resource->Get<coreModel>(sName);}
    inline void DefineModelLow (std::nullptr_t)               {m_pModelLow  = NULL;                                            this->coreObject3D::DefineModel(m_pModelLow);}
    inline void DefineModelLow (const coreModelPtr&   pModel) {m_pModelLow  = pModel;                                          this->coreObject3D::DefineModel(m_pModelLow);}
    inline void DefineModelLow (const coreHashString& sName)  {m_pModelLow  = Core::Manager::Resource->Get<coreModel>(sName);  this->coreObject3D::DefineModel(m_pModelLow);}

    // 
    inline void ActivateModelHigh() {this->coreObject3D::DefineModel(m_pModelHigh);}
    inline void ActivateModelLow () {this->coreObject3D::DefineModel(m_pModelLow);}

    // get object properties
    inline const coreModelPtr& GetModelHigh()const {return m_pModelHigh;}
    inline const coreModelPtr& GetModelLow ()const {return m_pModelLow;}

    // 
    static inline void RenderHighObject(cLodObject*    pObject) {pObject->ActivateModelHigh(); pObject->coreObject3D::Render(); pObject->ActivateModelLow();}
    static inline void RenderHighList  (coreBatchList* pList)   {FOR_EACH(it, *pList->List()) d_cast<cLodObject*>(*it)->ActivateModelHigh(); pList->Render(); FOR_EACH(it, *pList->List()) d_cast<cLodObject*>(*it)->ActivateModelLow();}


private:
    // 
    inline void DefineModel(std::nullptr_t)               {ASSERT(false)}
    inline void DefineModel(const coreModelPtr&   pModel) {ASSERT(false)}
    inline void DefineModel(const coreHashString& sName)  {ASSERT(false)}
};


#endif // _P1_GUARD_LODOBJECT_H_