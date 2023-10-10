#pragma warning( disable : 4996 ) // for warnings: due to compare of boost::uuids

#include "ModelEntityConnection.h"
#include "tpanalysismodel.h"
#include <hcntcore/document.h>
#include <hcntcore\model.h>
#include <Commands\tpanalysiskeys.h>

namespace omsl
{
    namespace commands
    {
        namespace tp_analyzer
        {

TPAnalyzerConnection::TPAnalyzerConnection ()
{
    OMSL_INITIALIZE_REFCOUNTER ( TPAnalyzerConnection );
}

TPAnalyzerConnection::~TPAnalyzerConnection ()
{}


int TPAnalyzerConnection::Connect
(
    omsl::boostwrapper::Connection *p_pc
)
{
    // save connection object
    if ( m_cp.Get () )
        m_cp = ConnectionPtr ();
    m_cp = ConnectionPtr ( p_pc );

    // we 'addref' ourselves here because generally the
    // hcNT listener methods seem not to store a user connection object
    int rc = GetRefCounter ()->AddSharedRef ();
    return 0;
}

int TPAnalyzerConnection::Disconnect ()
{
    // release connection object
    if ( m_cp )
    {
        m_cp->Disconnect ();
        m_cp = ConnectionPtr ();
    }

    // we 'relref' ourselves here because generally the
    // hcNT listener methods seem not to store a user connection object
    int rc = GetRefCounter ()->ReleaseSharedRef ();
    return 0;
}

TPAnalyzerModelEntityConnection::TPAnalyzerModelEntityConnection ()
{
    m_pCommandModel = nullptr;
}

TPAnalyzerModelEntityConnection::~TPAnalyzerModelEntityConnection ()
{}

int TPAnalyzerModelEntityConnection::Init
(
    void* pCommandModel
)
{
    m_pCommandModel = pCommandModel;
    return 0;
}

int TPAnalyzerModelEntityConnection::Connect
(
    omsl::hcntcore::Model *p_pmdl
)
{
    // connect to the model
    ConnectionPtr conp ( p_pmdl->Connect ( TPAnalyzerModelEntityConnection::_entity_signal, (void*) this ) );
    if ( !conp )
    {
        Disconnect ();
        return __LINE__;
    }


    // continue connecting...
    return TPAnalyzerConnection::Connect ( conp.Get () );
}
int TPAnalyzerModelEntityConnection::Disconnect ()
{
    // continue disconnecting...
    m_pCommandModel = nullptr;
    return TPAnalyzerConnection::Disconnect ();

}


bool TPAnalyzerModelEntityConnection::OnEntitySignal
(
    omsl::signal::Phase mode,
    omsl::hcntcore::Model::EntitySignalType type,
    omsl::hcntcore::CadEntity* pEnt,
    const omsl::hcntcore::entitysignal::EntitySignalData* pESD
)
{
    if ( !pEnt )
        return false;
    //switch ( mode )
    //{
    //    case omsl::signal::Phase::kCan :
    //    {
    //        return m_enableSignalListening;
    //    }
    //}

    // to interact with show / hide begin
    if ( mode == omsl::signal::Phase::kCan )
    {
        switch ( type )
        {
            case omsl::hcntcore::Model::kUnused1: 
            case omsl::hcntcore::Model::kUnused2: m_pre_unusedType = type;  break;
        }
    }

    if ( mode == omsl::signal::Phase::kPost ) // end of show hide
    {
        switch ( type )
        {
            case omsl::hcntcore::Model::kUnused1: 
            case omsl::hcntcore::Model::kUnused2:
            {
                bool b = this->OnModifiedByOther ( mode, type, pEnt, pESD );
                m_pre_unusedType = -1;
                return b;
                break;
            }
        }
    }
    // to interact before loosing objects
    if ( mode != omsl::signal::Phase::kPre )
        return true;

    switch ( type )
    {
//        case omsl::hcntcore::Model::kCreated:            return this->OnEntityCreated ( mode, type, pEnt, pESD );
//        case omsl::hcntcore::Model::kGraphicBuilt:       return this->OnEntityGraphicBuilt ( mode, type, pEnt, pESD );
//        case omsl::hcntcore::Model::kPersist:            return this->OnEntityPersist ( mode, type, pEnt, pESD );
        case omsl::hcntcore::Model::kModified:           return this->OnEntityModified ( mode, type, pEnt, pESD );
        case omsl::hcntcore::Model::kDeleted:            return this->OnEntityDeleted ( mode, type, pEnt, pESD );
        case omsl::hcntcore::Model::kHideUnhide:         return this->OnEntityHideUnhide ( mode, type, pEnt, pESD );
        case omsl::hcntcore::Model::kBuildGraphicPlease: return this->OnEntityBuildGraphicPlease ( mode, type, pEnt, pESD );
        case omsl::hcntcore::Model::kUpdateGraphicPlease: return this->OnEntityUpdateGraphicPlease ( mode, type, pEnt, pESD );
        case omsl::hcntcore::Model::kUnused1 : // current state of show / hide
        case omsl::hcntcore::Model::kUnused2 : // = hide
        {
            bool b = this->OnModifiedByOther ( mode, type, pEnt, pESD );
            m_pre_unusedType = -1;
            return b;
            break;
        }        
    }
    return true;
}

bool TPAnalyzerModelEntityConnection::OnModifiedByOther
(
    omsl::signal::Phase // mode,
,   omsl::hcntcore::Model::EntitySignalType type
,   omsl::hcntcore::CadEntity* pEnt
,   const omsl::hcntcore::entitysignal::EntitySignalData* // pESD
)
{
    checkEntity ( pEnt, type );
    return true;
}

bool TPAnalyzerModelEntityConnection::OnEntityCreated
(
    omsl::signal::Phase // mode,
,   omsl::hcntcore::Model::EntitySignalType // type
,   omsl::hcntcore::CadEntity* // pEnt,
,   const omsl::hcntcore::entitysignal::EntitySignalData* // pESD
)
{
    return true;
}

bool TPAnalyzerModelEntityConnection::OnEntityGraphicBuilt
(
    omsl::signal::Phase // mode,
,   omsl::hcntcore::Model::EntitySignalType // type
,   omsl::hcntcore::CadEntity* // pEnt,
,   const omsl::hcntcore::entitysignal::EntitySignalData* // pESD
)
{
    return true;
}

bool TPAnalyzerModelEntityConnection::OnEntityPersist
(
  omsl::signal::Phase // mode,
, omsl::hcntcore::Model::EntitySignalType // type
, omsl::hcntcore::CadEntity* // pEnt,
, const omsl::hcntcore::entitysignal::EntitySignalData* // pESD
)
{
    return true;
}

bool TPAnalyzerModelEntityConnection::OnEntityModified
(
  omsl::signal::Phase // mode,
, omsl::hcntcore::Model::EntitySignalType type
, omsl::hcntcore::CadEntity* pEnt
, const omsl::hcntcore::entitysignal::EntitySignalData* // pESD
)
{
    checkEntity ( pEnt, type );
    return true;
}

bool TPAnalyzerModelEntityConnection::OnEntityDeleted
(
  omsl::signal::Phase // mode,
, omsl::hcntcore::Model::EntitySignalType  type
, omsl::hcntcore::CadEntity* pEnt
, const omsl::hcntcore::entitysignal::EntitySignalData* // pESD
)
{
    checkEntity ( pEnt, type );
    return true;
}

bool TPAnalyzerModelEntityConnection::OnEntityHideUnhide
(
  omsl::signal::Phase // mode,
, omsl::hcntcore::Model::EntitySignalType type
, omsl::hcntcore::CadEntity* pEnt
, const omsl::hcntcore::entitysignal::EntitySignalData* // pESD
)
{
    checkEntity ( pEnt, type );
    return true;
}

bool TPAnalyzerModelEntityConnection::OnEntityBuildGraphicPlease
(
  omsl::signal::Phase // mode,
, omsl::hcntcore::Model::EntitySignalType // type
, omsl::hcntcore::CadEntity* // pEnt
, const omsl::hcntcore::entitysignal::EntitySignalData* // pESD
)
{
    return true;
}

bool TPAnalyzerModelEntityConnection::OnEntityUpdateGraphicPlease
(
  omsl::signal::Phase // mode,
, omsl::hcntcore::Model::EntitySignalType type
, omsl::hcntcore::CadEntity* pEnt
, const omsl::hcntcore::entitysignal::EntitySignalData* // pESD
)
{
    checkEntity ( pEnt, type );
    return true;
}

bool TPAnalyzerModelEntityConnection::_entity_signal
(
    void* obj,
    omsl::signal::Phase mode,
    omsl::hcntcore::Model::EntitySignalType type,
    omsl::hcntcore::CadEntity* pEnt,
    const omsl::hcntcore::entitysignal::EntitySignalData* pESD
)
{
    TPAnalyzerModelEntityConnection *pac = ( TPAnalyzerModelEntityConnection* ) obj;
    return pac->OnEntitySignal ( mode, type, pEnt, pESD );
}


bool TPAnalyzerModelEntityConnection::checkEntity 
( 
    omsl::hcntcore::CadEntity* pEnt 
,   omsl::hcntcore::Model::EntitySignalType type
)
{

//    if ( pEnt == nullptr || m_pCommandModel == nullptr )
        return true;

    TPAnalysisModel* pCommandModel = ( TPAnalysisModel* ) m_pCommandModel;

//    return pCommandModel->CheckSignalTypeForCadEntity ( pEnt, type, m_pre_unusedType );
}


int CreateAnalyzerModelEntityConnection
(
    omsl::hcntcore::Model    *p_pmdl,
    TPAnalyzerModelEntityConnection    *p_pmec,
    TPAnalyzerModelEntityConnectionPtr &p_mecp
)
{
    if ( !p_pmec )
        return __LINE__;

    int err = p_pmec->Connect ( p_pmdl );
    if ( err )
        return __LINE__;

    p_mecp = TPAnalyzerModelEntityConnectionPtr ( p_pmec );
    return p_mecp.Get () ? 0 : __LINE__;
}

int CreateAnalyzerModelEntityConnection
(
    omsl::hcntcore::Model    *p_pmdl,
    TPAnalyzerModelEntityConnection    *p_pmec
)
{
    if ( !p_pmec )
        return __LINE__;

    int err = p_pmec->Connect ( p_pmdl );
    if ( err )
        return __LINE__;
    return 0;
}


int CreateAnalyzerModelEntityConnection
(
    omsl::hcntcore::Model     *p_pmdl,
    TPAnalyzerModelEntityConnectionPtr &p_mecp
)
{
    TPAnalyzerModelEntityConnectionPtr mecp ( new TPAnalyzerModelEntityConnection () );
    if ( !mecp )
        return __LINE__;
    return CreateAnalyzerModelEntityConnection ( p_pmdl, mecp.Get (), p_mecp );
}

        }  // namespace tp_analyzer
    }  // namespace commands
}  // namespace omsl
