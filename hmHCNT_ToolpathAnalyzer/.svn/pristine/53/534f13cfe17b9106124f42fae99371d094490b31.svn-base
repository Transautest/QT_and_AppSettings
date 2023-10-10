#include <portability.h>
#include <propertyservice/eventemitters.h>
#include "tpanalysepropertieskeys.h"
#include <propertyservice/propertybag/propertytypedbag.h>
#include "tpanalysepropertiesimpl.h"

#include <boostwrapper/connection.h>
#include <propertyservice/propertybag/valueslotimpl.h>
#include <propertyservice/context/contextutil.h>
#include <propertyservice/propertybag/bagchangeconnection.h>
#include <propertyservice/propertybag/minmaxvalidatort.h>
#include <opservice/propertiescontainer.h>
#include <opservice/properties.h>

#include <units/dimension.h>

#include <singletonimpl.h>
#include <shutdownprocesscleaner.h>
#include <Model\tpanalysismodel.h>

#define HC_MODULE_NAME OmslGetDllName()

namespace omsl {
namespace commands { 
namespace tp_analyzer
{

DECLARE_EVENT_1( tp_analyzer, Enable, omsl::Bool );
class EnableEmitter : public connection::EventEmitterT<Enable, omsl::Bool> {};

// implement the standard singleton cleaner function
ADD_STANDARD_SINGLETON_CLEANER_FUNCTION_IMPL(PropertiesImpl,Properties)

  COMPOSITION_CONTAINER_FUNCTION_INITIALIZER_AND_IMPL( Properties*, GetProperties, () )
{
  if( !SingletonAccessor<PropertiesImpl,Properties>::CheckSingleton() ){
    ADD_STANDARD_SINGLETON_CLEANER_FUNCTION(PropertiesImpl);
  }
  return SingletonAccessor<PropertiesImpl,Properties>::GetSingleton();
}
 static void CleanPropertiesInstance( )
 {
   if( SingletonAccessor<PropertiesImpl, Properties>::CheckSingleton( ) ) {
     SingletonAccessor<PropertiesImpl, Properties>::CleanSingleton( );
   }
 }

PropertiesImpl::PropertiesImpl ()
{
    m_pModel = nullptr;
    ADD_SHUTDOWN_CLEANER_FUNCTION( PropertiesImpl, CleanPropertiesInstance );
    m_context = propertyservice::Context::Create ();
    m_context->SetName ( L"tp.analysis.description" );
}


OMSL_IMPL (void) PropertiesImpl::SetModel ( TPAnalysisModel* pModel )
{
    if ( m_pModel.Get () == pModel )
        return;

    m_pModel = pModel;

    InitConnections ();
}

void PropertiesImpl::SetStrength ( const TCHAR* keyname, const omsl::propertyservice::Applicable::Strength& strength )
{
    if ( !m_pModel.Get () )
        return;

    m_context->SetStrength ( keyname, strength );
}

OMSL_IMPL (bool) PropertiesImpl::IsInactive ( const TCHAR* keyname )
{
    return m_context->GetStrength ( keyname ) == omsl::propertyservice::Applicable::Strength::kInactive;
}

void PropertiesImpl::InitConnections ()
{
    m_Connections.clear ();

    if ( m_pModel.Get () == nullptr )
        return;

  propertyservice::Value *pValue;

  // value selector 3rd tab
  pValue = AddValue( properties::key::TpPointSize, 1.0 );
  if( pValue )
  {
    pValue->SetDimension ( units::Dimension::Dimensionless () );
    pValue->SetValidator ( new propertyservice::MinMaxValidatorT<Double> ( 0.25, 20. ) );
    pValue = nullptr;
    m_Connections.push_back( m_context->ConnectValueChange( properties::key::TpPointSize, &TPAnalysisModel::OnChangePointSize, m_pModel.Get () ) );
  }

  pValue = AddValue( properties::key::TpLineWidth, 1.0 );
  if( pValue )
  {
    pValue->SetDimension ( units::Dimension::Dimensionless () );
    pValue->SetValidator ( new propertyservice::MinMaxValidatorT<Double> ( 0.25, 20. ) );
    pValue = nullptr;
    m_Connections.push_back( m_context->ConnectValueChange( properties::key::TpLineWidth, &TPAnalysisModel::OnChangeLineWidth, m_pModel.Get () ) );
  }

  pValue = AddValue( properties::key::TpVectorLength, 1.0 );
  if( pValue )
  {
    pValue->SetDimension ( units::Dimension::Dimensionless () );
    pValue->SetValidator ( new propertyservice::MinMaxValidatorT<Double> ( 0.5, 20. ) );

    pValue = nullptr;
    m_Connections.push_back( m_context->ConnectValueChange( properties::key::TpVectorLength, &TPAnalysisModel::OnChangeVectorLength, m_pModel.Get () ) );
  }

  pValue = AddValue( properties::key::TmpGrTolerance, 0.0001 );
  if( pValue )
  {
    pValue->SetDimension ( units::Dimension::Length () );
    pValue->SetValidator ( new propertyservice::MinMaxValidatorT<Double> ( 1.e-20, 1. ) );

    pValue = nullptr;
    m_Connections.push_back( m_context->ConnectValueChange( properties::key::TmpGrTolerance, &TPAnalysisModel::OnChangeEnvTolerance, m_pModel.Get () ) );
  }

  pValue = AddValue( properties::key::TmpGrMinimumRadius, 0.0001 );
  if( pValue )
  {
    pValue->SetDimension ( units::Dimension::Length () );
    pValue->SetValidator ( new propertyservice::MinMaxValidatorT<Double> ( 1.e-20, 1. ) );

    pValue = nullptr;
    m_Connections.push_back( m_context->ConnectValueChange( properties::key::TmpGrMinimumRadius, &TPAnalysisModel::OnChangeEnvMinTolRadius, m_pModel.Get () ) );
  }

  //pValue = AddValue ( properties::key::HideUpperLimitFeedrates , False );
  //if( pValue )
  //{
  //  pValue = nullptr;
  //  m_Connections.push_back( m_context->ConnectValueChange( properties::key::HideUpperLimitFeedrates, &TPAnalysisModel::OnHideUpperLimitFeedratesChanged, m_pModel.Get () ) );
  //}

  //pValue = AddValue ( properties::key::HideLowerLimitFeedrates , False );
  //if( pValue )
  //{
  //  pValue = nullptr;
  //  m_Connections.push_back( m_context->ConnectValueChange( properties::key::HideLowerLimitFeedrates, &TPAnalysisModel::OnHideLowerLimitFeedratesChanged, m_pModel.Get () ) );
  //}

  //pValue = AddValue ( properties::key::UseFeedratePartitionByCount , False );
  //if( pValue )
  //{
  //  pValue = nullptr;
  //  m_Connections.push_back( m_context->ConnectValueChange( properties::key::UseFeedratePartitionByCount, &TPAnalysisModel::OnChangeEnvClrMapByCounts, m_pModel.Get () ) );
  //}

  pValue = AddValue ( properties::key::ShowLayer25, False ); // for normal TP
  if ( pValue )
  {
      pValue = nullptr;
      m_Connections.push_back ( m_context->ConnectValueChange ( properties::key::ShowLayer25, &TPAnalysisModel::OnShowLayer25Changed, m_pModel.Get () ) );
      m_context->SetStrength ( properties::key::ShowLayer25, omsl::propertyservice::Applicable::Strength::kInactive );
  }

  pValue = AddValue ( properties::key::ShowNCPath, False );
  if ( pValue )
  {
      pValue = nullptr;
      m_Connections.push_back ( m_context->ConnectValueChange ( properties::key::ShowNCPath, &TPAnalysisModel::OnShowNCPathChanged, m_pModel.Get () ) );
      m_context->SetStrength ( properties::key::ShowNCPath, omsl::propertyservice::Applicable::Strength::kInactive );
  }

  pValue = AddValue ( properties::key::ShowClippPath, False );
  if ( pValue )
  {
      pValue = nullptr;
      m_Connections.push_back ( m_context->ConnectValueChange ( properties::key::ShowClippPath, &TPAnalysisModel::OnShowClippPathChanged, m_pModel.Get () ) );
      m_context->SetStrength ( properties::key::ShowClippPath, omsl::propertyservice::Applicable::Strength::kInactive );
  }

  pValue = AddValue(properties::key::ShowBallMillSpecPath, (int)properties::key::eBallMill::kJobsetting);
  if (pValue)
  {
      pValue = nullptr;
      m_Connections.push_back(m_context->ConnectValueChange(properties::key::ShowBallMillSpecPath, &TPAnalysisModel::OnShowBallMillSpecPathChanged, m_pModel.Get()));
      m_context->SetStrength(properties::key::ShowBallMillSpecPath, omsl::propertyservice::Applicable::Strength::kInactive);
  }

  m_bagConnection.reset( new propertyservice::BagChangeConnection( m_context.get(), propertyservice::PropertySignal_T::Slot(&PropertiesImpl::OnPropertyChange), this ) );

  try {
    SharedPtr< opservice::Properties > properties( opservice::CreateStandardProperties(m_context.get(), opservice::Properties::kApplication, NULL, NULL, false) );
    if( !properties->Load( m_context->GetName() ) )
      properties->Save();
    opservice::GetPropertiesContainer()->Add( properties.Get() );  
  } 
  catch( HandshakeException& ){}
}

template <class VALUETYPE> propertyservice::Value* PropertiesImpl::AddValue( const omsl::propertyservice::TypedNameT<VALUETYPE> &par, const VALUETYPE& value )
{
  auto retVal(m_context->AddTyped(par, value));
  if ( retVal )
    m_context->SetStrength( par, propertyservice::Applicable::kOptional );
  return retVal;
}

template <class VALUETYPE> const VALUETYPE& PropertiesImpl::GetValue( const omsl::propertyservice::TypedNameT<VALUETYPE> &par ) const
{
  return m_context->GetValue<VALUETYPE>(par);
}

void PropertiesImpl::OnPropertyChange( propertyservice::PropertyBag* bag, const wchar_t* name ) 
{
  propertyservice::PropertyChangedEmitter().Emit( bag, name );
}

} //namespace tpanalysis
} //namespace commands
} //namespace omsl
