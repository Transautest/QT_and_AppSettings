#pragma warning( disable : 4996 ) // for warnings: due to compare of boost::uuids
#include "TPAnalysisModel.h"
// keys
#include <Commands\tpanalysiskeys.h>
#include <Commands\tpanalysepropertieskeys.h>
#include <hcntcore/document.h>
#include <hcntcore\model.h>

#include <propertyservice/propertybag/minmaxvalidatort.h>
#include <hcntcore/cadentityiterators.h>
#include <hcntcore/pargeomentity.h>
#include <hcntcore/workplaneentity.h>

#include <geom/geometriciterators.h>
#include <geom/geom3d.h>
#include <bgeo/coord3diterators.h>
#include <hcntcore/cadentitymgr.h>
#include <hcntcore/polylineithpoint.h>

#include <..\..\..\INTERFACES\iHCNT\iTopoAttrib.h>
#include <graphicattributes/toolpathgraphicattributes.h>
#include <graphicattributes/toolpathsegmentgraphicattributes.h>
#include <graphicattributes/pointcloudgraphicattributes.h>
#include <graphicattributes/graphicattributesfactory.h>
#include <materials/colormaterial.h>
#include <materials/colorspalette.h>
#include <materials/colorspalettefactory.h>
#include <materials/material.h>
#include <materials/materials.h>
#include <materials/materialslibrary.h>
#include <materials/materialsstatus.h>
#include <measureunit\converter.h>
#include <units\dimension.h>
#include <geom/revolvedsurface.h>
#include <hcntcore/toolpathmovement.h>
#include <bgeo/axis3dvectorcontainers.h>
#include <hcntcore/referencesystemfree.h>
#include <bgeo/point3d.h>
#include <drafting/arrow.h>
#include <hcntcore/application.h>
#include <gui/gui.h>
#include <gui/guihandshake.h>
//#include "../Commands/tpanalyseproperties.h"
//#include "../Commands/tpanalysepropertiesimpl.h"
#include "../Commands/common_functions.h"
#include "../Commands/tp_listener.h"
#include "../Commands/tp_signalemitter.h"

#include <math.h>
#include <graphics/group.h>
#include <hcNTLib/ConnectionHlpTT.h>
#include <hcNTLib/GraphicsHlp.h>

#include <hcntLib/TopoAttribHlp.h>
#include <hcntLib/ToolPathHlp.h>
#include <hcntLib/PropertyServiceHlp.h>
#include <containers\mapcontainer.h>
#include <containers/defaultiteratorsreadonly.h>
#include <tchar.h>

#include <qstring.h>
#include "translate.h"


using omsl::linguistic::QT_TRANSLATE_NOOP;

typedef omsl::SharedPtr<omsl::graphics::Group> GraphicsGroupPtr;
typedef omsl::SharedPtr<omsl::graphics::Node> GraphicsNodePtr;

using namespace omsl::command;

typedef omsl::SharedPtr<omsl::hcntcore::GeomEntity> GeomEntityPtr;
typedef omsl::SharedPtr<omsl::geom::Geom3d> Geom3dPtr;
typedef omsl::SharedPtr<omsl::hcntcore::ParGeomEntity> ParGeomEntityPtr;
typedef omsl::SharedPtr<omsl::hcntcore::pargeom::Geom> ParGeomGeomPtr;
typedef omsl::SharedPtr<omsl::hcntcore::pargeom::ToolpathMovement> ParGeomTpMovementPtr;
typedef omsl::SharedPtr<omsl::graphicattributes::GraphicAttributes> GraphicAttributesPtr;
typedef omsl::SharedPtr<omsl::graphicattributes::ToolpathGraphicAttributes> ToolpathGraphicAttributesPtr;
typedef omsl::SharedPtr<omsl::graphicattributes::ToolpathSegmentGraphicAttributes> ToolpathSegmentGraphicAttributesPtr;
typedef omsl::SharedPtr<omsl::graphicattributes::PointCloudGraphicAttributes> PointCloudGraphicAttributesPtr;
typedef omsl::SharedPtr<omsl::hcntcore::CadEntity> CadEntityPtr;
typedef omsl::SharedPtr<omsl::iterator::hcntcore::CadEntityForward> CadEntityForwardPtr;

typedef omsl::container::MapContainerWrapper<omsl::ULong, omsl::ULong, std::map<omsl::ULong, omsl::ULong>, omsl::iterator::UlongUlongMapIteratorForwardReadOnly> ULULMap;
typedef omsl::SharedPtr<ULULMap> ULULMapPtr;

enum pBad_Id
{
      tpMInfo_JOBNAME = 0
    , tpMInfo_FEEDRATE
    , tpMInfo_FEEDRATETURNING
    , tpMInfo_SPEED
    , tpMInfo_CUTTINGSPEED
    , tpMInfo_GTYPE
    , tpMInfo_MOVENR
    , tpMInfo_TANGLEA
    , tpMInfo_TANGLEB
    , tpMInfo_TANGLEC
    , tpMInfo_CNTG0
    , tpMInfo_CNTG1
    , tpMInfo_CNTG2
    , tpMInfo_CNTG3
    , tpMInfo_CNT5X
    , tpMInfo_CNTPOINTS
    , tpMInfo_measurepoint0
    , tpMInfo_measurepoint1
    , tpMInfo_measurepoint2
    , tpMInfo_referencepoint
    , tpMInfo_centerpoint
    , tpMInfo_mp0_X
    , tpMInfo_mp0_Y
    , tpMInfo_mp0_Z
    , tpMInfo_mp0_D
    , tpMInfo_mp1_X
    , tpMInfo_mp1_Y
    , tpMInfo_mp1_Z
    , tpMInfo_mp1_D
    , tpMInfo_mp2_X
    , tpMInfo_mp2_Y
    , tpMInfo_mp2_Z
    , tpMInfo_mp2_D
    , tpMInfo_Length
    , tpMInfo_Radius_A
    , tpMInfo_Radius_E
    , tpMInfo_Angle_A
    , tpMInfo_Angle_C
    , tpMInfo_Angle_E
    , tpMInfo_Delta
    , tpMInfo_MD_P1
    , tpMInfo_MD_P2
    , tpMInfo_MD_DD
    , tpMInfo_MD_DX
    , tpMInfo_MD_DY
    , tpMInfo_MD_DZ
    , tpMInfo_MD_MOVENR
    , tpMInfo_MD_Tpar
    , tpMInfo_MD_Upar
    , tpMInfo_MD_Vpar
    // must be the last entry
    , tpMInfo_ALLdefs
};
wchar_t const * const pBagPropNames[2][tpMInfo_ALLdefs] {
{
 L"tpanalysismodel:Move1Info:JOBNAME"
,L"tpanalysismodel:Move1Info:FEEDRATE"
,L"tpanalysismodel:Move1Info:FEEDRATETURNING"
,L"tpanalysismodel:Move1Info:SPEED"
,L"tpanalysismodel:Move1Info:CUTTINGSPEED"
,L"tpanalysismodel:Move1Info:GTYPE"
,L"tpanalysismodel:Move1Info:MOVENR"
,L"tpanalysismodel:Move1Info:TANGLEA"
,L"tpanalysismodel:Move1Info:TANGLEB"
,L"tpanalysismodel:Move1Info:TANGLEC"
,L"tpanalysismodel:Move1Info:CNTG0"
,L"tpanalysismodel:Move1Info:CNTG1"
,L"tpanalysismodel:Move1Info:CNTG2"
,L"tpanalysismodel:Move1Info:CNTG3"
,L"tpanalysismodel:Move1Info:CNT5X"
,L"tpanalysismodel:Move1Info:CNTPOINTS"
,L"tpanalysismodel:Move1Geo:measurepoint0"
,L"tpanalysismodel:Move1Geo:measurepoint1"
,L"tpanalysismodel:Move1Geo:measurepoint2"
,L"tpanalysismodel:Move1Geo:referencepoint"
,L"tpanalysismodel:Move1Geo:centerpoint"
,L"tpanalysismodel:Move1Geo:mp0_X"
,L"tpanalysismodel:Move1Geo:mp0_Y"
,L"tpanalysismodel:Move1Geo:mp0_Z"
,L"tpanalysismodel:Move1Geo:mp0_D"
,L"tpanalysismodel:Move1Geo:mp1_X"
,L"tpanalysismodel:Move1Geo:mp1_Y"
,L"tpanalysismodel:Move1Geo:mp1_Z"
,L"tpanalysismodel:Move1Geo:mp1_D"
,L"tpanalysismodel:Move1Geo:mp2_X"
,L"tpanalysismodel:Move1Geo:mp2_Y"
,L"tpanalysismodel:Move1Geo:mp2_Z"
,L"tpanalysismodel:Move1Geo:mp2_D"
,L"tpanalysismodel:Move1Geo:Length"
,L"tpanalysismodel:Move1Geo:Radius_A"
,L"tpanalysismodel:Move1Geo:Radius_E"
,L"tpanalysismodel:Move1Geo:Angle_A"
,L"tpanalysismodel:Move1Geo:Angle_C"
,L"tpanalysismodel:Move1Geo:Angle_E"
,L"tpanalysismodel:Move1Geo:Delta"
,L"FirstPoint"
,L"SecondPoint"
,L"MinDistance"
,L"Deltax"
,L"Deltay"
,L"Deltaz"
,L"MD_MOVENR1"
,L"Tpar1"
,L"Upar1"
,L"Vpar1"
},
{ 
 L"tpanalysismodel:Move2Info:JOBNAME"
,L"tpanalysismodel:Move2Info:FEEDRATE"
,L"tpanalysismodel:Move2Info:FEEDRATETURNING"
,L"tpanalysismodel:Move2Info:SPEED"
,L"tpanalysismodel:Move2Info:CUTTINGSPEED"
,L"tpanalysismodel:Move2Info:GTYPE"
,L"tpanalysismodel:Move2Info:MOVENR"
,L"tpanalysismodel:Move2Info:TANGLEA"
,L"tpanalysismodel:Move2Info:TANGLEB"
,L"tpanalysismodel:Move2Info:TANGLEC"
,L"tpanalysismodel:Move2Info:CNTG0"
,L"tpanalysismodel:Move2Info:CNTG1"
,L"tpanalysismodel:Move2Info:CNTG2"
,L"tpanalysismodel:Move2Info:CNTG3"
,L"tpanalysismodel:Move2Info:CNT5X"
,L"tpanalysismodel:Move2Info:CNTPOINTS"
,L"tpanalysismodel:Move2Geo:measurepoint0"
,L"tpanalysismodel:Move2Geo:measurepoint1"
,L"tpanalysismodel:Move2Geo:measurepoint2"
,L"tpanalysismodel:Move2Geo:referencepoint"
,L"tpanalysismodel:Move2Geo:centerpoint"
,L"tpanalysismodel:Move2Geo:mp0_X"
,L"tpanalysismodel:Move2Geo:mp0_Y"
,L"tpanalysismodel:Move2Geo:mp0_Z"
,L"tpanalysismodel:Move2Geo:mp0_D"
,L"tpanalysismodel:Move2Geo:mp1_X"
,L"tpanalysismodel:Move2Geo:mp1_Y"
,L"tpanalysismodel:Move2Geo:mp1_Z"
,L"tpanalysismodel:Move2Geo:mp1_D"
,L"tpanalysismodel:Move2Geo:mp2_X"
,L"tpanalysismodel:Move2Geo:mp2_Y"
,L"tpanalysismodel:Move2Geo:mp2_Z"
,L"tpanalysismodel:Move2Geo:mp2_D"
,L"tpanalysismodel:Move2Geo:Length"
,L"tpanalysismodel:Move2Geo:Radius_A"
,L"tpanalysismodel:Move2Geo:Radius_E"
,L"tpanalysismodel:Move2Geo:Angle_A"
,L"tpanalysismodel:Move2Geo:Angle_C"
,L"tpanalysismodel:Move2Geo:Angle_E"
,L"tpanalysismodel:Move2Geo:Delta"
,L"FirstPoint"
,L"SecondPoint"
,L"MinDistance"
,L"Deltax"
,L"Deltay"
,L"Deltaz"
,L"MD_MOVENR2"
,L"Tpar2"
,L"Upar2"
,L"Vpar2"
}
};

namespace omsl
{
    namespace commands
    {
        namespace tp_analyzer
        {
            void S_calcEuler ( SharedPtr<propertyservice::PropertyBag> pBag, int sel, const bgeo::Vector3d& v , bgeo::RigidMotion3d& motion1 )
            {
                double A = 0., B = 0., C = 0.;
                double angles [3];
                const bgeo::RigidMotion3d::CartesianAxis rotationAxes [3] = { bgeo::RigidMotion3d::CartesianAxis::XAxis ,bgeo::RigidMotion3d::CartesianAxis::YAxis ,bgeo::RigidMotion3d::CartesianAxis::ZAxis };
                omsl::bgeo::Status s = motion1.DecomposeRotation ( rotationAxes, angles );

                if ( s != omsl::bgeo::Status::OK )
                {
                    // this function is the same with omHxyzAnglesVz() in Omheuler.c from EDK
                    double V_EPS = 1.0e-8;
                    bgeo::RigidMotion3d motion;
                    bgeo::UnitVector3d x_vector, y_vector, z_vector ( v [0], v [1], v [2] );
                    motion.ApplyToUnitVector ( z_vector );
                    z_vector.BuildXYZVectors ( x_vector, y_vector );

                    motion = motion1;
                    x_vector = motion.XUnitVector ();
                    y_vector = motion.YUnitVector ();
                    z_vector = motion.ZUnitVector ();
                    if ( x_vector [2] < ( -1.0 + V_EPS ) )
                    {
                        A = 0.0;
                        B = 90.0;
                        C = 180.0 / M_PI * atan2 ( z_vector [1], z_vector [0] );
                    }
                    else if ( x_vector [2] > ( 1.0 - V_EPS ) )
                    {
                        A = 0.0;
                        B = -90.0;

                        if ( z_vector [1] >= 0.0 ) /* d.h.  0 < wz < 180 ==> -180 to 0 */
                            C = -180.0 + ( 180 / M_PI * atan2 ( z_vector [1], z_vector [0] ) );
                        else             /* d.h.  -180 < wz < 0 ==> 0 bis 180 */
                            C = 180.0 + ( 180 / M_PI * atan2 ( z_vector [1], z_vector [0] ) );
                    }
                    else  /* -90 < y_Angle < 90 */
                    {
                        double phi1 = 0;
                        double phi2 = 0;
                        double phi3 = 0;
                        phi2 = -asin ( x_vector [2] );

                        if ( ( ( z_vector [2] < 0.0 ) ? -z_vector [2] : z_vector [2] ) > V_EPS ) /*E06*/
                            phi1 = atan2 ( y_vector [2], z_vector [2] );
                        else
                        {
                            double cs = cos ( phi2 );

                            double ratio = y_vector [2] / cs;
                            if ( ratio > 0.999999 )        ratio = 1.0;
                            else if ( ratio < -0.999999 )  ratio = -1.0;

                            phi1 = asin ( ratio );
                        }
                        if ( ( fabs ( x_vector [0] ) > V_EPS ) ||
                            ( fabs ( x_vector [1] ) > V_EPS ) )
                            phi3 = atan2 ( x_vector [1], x_vector [0] );
                        else
                            phi3 = atan2 ( -y_vector [0], y_vector [1] );

                        A = 180. / M_PI * phi1;
                        B = 180. / M_PI * phi2;
                        C = 180. / M_PI * phi3;
                    }

                    A *= ( M_PI / 180.0 );
                    B *= ( M_PI / 180.0 );
                    C *= ( M_PI / 180.0 );
                }
                else
                {
                    A = angles [0];
                    B = angles [1];
                    C = angles [2];
                }

                if ( A != B )
                {
                    pBag->AddTyped<double> ( pBagPropNames [sel] [tpMInfo_TANGLEA], A );
                    pBag->AddTyped<double> ( pBagPropNames [sel] [tpMInfo_TANGLEB], B );
                }
                else
                {
                    pBag->AddTyped<double> ( pBagPropNames [sel] [tpMInfo_TANGLEA], A );
                }

                pBag->AddTyped<double> ( pBagPropNames [sel] [tpMInfo_TANGLEC], C );
            }

            void TPAnalysisModel::SetupPropertyBag ()
            {
                bool newCreated = false;
                SharedPtr<propertyservice::PropertyBag> pBag = CreatePropertyBag ();
                omsl::SharedPtr<omsl::propertyservice::Context> Context ( GetContext () );

                if ( pBag.Get () == nullptr )
                {
                    omsl::SharedPtr<omsl::propertyservice::Context> Context1 ( GetContext () );
                    pBag = Context1.shared_cast< omsl::propertyservice::PropertyBag >( L"PropertyBag" );
                    if ( pBag.Get () && Context1->Exists ( L"UI_Panel_AnalysisResult" ) )
                    {
                        pBag = Context1->GetValue <omsl::propertyservice::PropertyBag*> ( L"UI_Panel_AnalysisResult" );
                    }
                }
                else
                {
                    pBag->SetName ( m_descriptionPath.Get ()->Get () );
                    newCreated = true;
                }
                
                tpanalysis::compTPAMode resMode = checkTPAMode ( U_SEL );

                if ( s_data.m [L_SEL].specialIndex [analyseData::dai_SXP1] == UNKNOWNINDEX && s_data.m [R_SEL].specialIndex [analyseData::dai_SXP1] == UNKNOWNINDEX )
                   SetupTemporaryArcMovements ( 0, 0 );

                int res = CalculateNearestPoint ( resMode );

                switch ( resMode )
                {
                    case tpanalysis::cm_Point:
                    {
                        SetupPropertyBagFor_P ( pBag );
                        break;
                    }
                    case tpanalysis::cm_Movement:
                    {
                        SetupPropertyBagFor_M ( pBag );
                        break;
                    }
                    case tpanalysis::cm_Point_Point :
                    case tpanalysis::cm_Point_Movement : // not implmented yet
                    case tpanalysis::cm_Point_Geometry : // not implmented yet
                    case tpanalysis::cm_Movement_Movement : // not implmented yet
                    case tpanalysis::cm_Movement_Geometry : // not implmented yet
                    {
                        if ( res == 0 )
                            SetupPropertyBagFor_P_P ( pBag );
                        break;
                    }
                    case tpanalysis::cm_TPALast: break;
                    default : // nothing to do
                    {
                        break;
                    }
                }

                if ( newCreated )
                {
                    SetValue ( tpanalysis::UI_AnalysisResult, pBag );
                }

                SetupTemporary_Measure_Graphic ();

            }
#ifdef XXXXX
            void  TPAnalysisModel::SetupPropertyBagForSettings ()
            {

                omsl::commands::tp_analyzer::Properties* tpp = GetProperties ();
                if ( tpp )
                {

                    omsl::commands::tp_analyzer::PropertiesImpl* p = dynamic_cast < omsl::commands::tp_analyzer::PropertiesImpl* > ( tpp );
                    if ( p )
                        p->SetModel ( this );

                    SharedPtr<propertyservice::PropertyBag> pBagPtr ( tpp->GetBag ());
                    if ( !pBagPtr->IsEmpty () )
                    {
                        SetValue ( tpanalysis::UI_Settings, pBagPtr );
                    }

                    {
#ifndef DBGVER
                        tpp->SetStrength ( properties::key::TmpGrTolerance, omsl::propertyservice::Applicable::Strength::kInactive );
                        tpp->SetStrength ( properties::key::TmpGrMinimumRadius, omsl::propertyservice::Applicable::Strength::kInactive );
                        //tpp->SetStrength ( properties::key::HideUpperLimitFeedrates, omsl::propertyservice::Applicable::Strength::kInactive );
                        //tpp->SetStrength ( properties::key::HideLowerLimitFeedrates, omsl::propertyservice::Applicable::Strength::kInactive );
                        //tpp->SetStrength ( properties::key::UseFeedratePartitionByCount, omsl::propertyservice::Applicable::Strength::kInactive );
#endif
                    }

                }
            }

//            void TPAnalysisModel::SetupPropertyBag ( SharedPtr<propertyservice::PropertyBag> pBag, int sel )
//            {
//                    return;
//                if ( s_data.m[sel].specialIndex [analyseData::dai_SXP1] == UNKNOWNINDEX )
//                {
//                    SetupTemporaryArcMovements ( 0, -sel );
//                    return;
//                }
//
//                bool showLayer25 = GetProperties ()->GetBag ()->Exists ( properties::key::ShowLayer25 ) && GetProperties ()->GetBag ()->GetValue<omsl::Bool> ( properties::key::ShowLayer25 );
//
//                //                bool bPointFound = false;
//                bgeo::Coord3d pt0;
//                bgeo::Coord3d pt1;
//                bgeo::Coord3d pt2;
//                bgeo::Coord3d ptref;
//                SharedPtr<measureunit::Converter> pConverter;
//                pConverter = omsl::measureunit::Converter::CreateGeneric ( omsl::units::Dimension::Length (), nullptr );
//
//                if ( s_data.m[L_SEL].specialIndex[analyseData::dai_REFP] != UNKNOWNINDEX )
//                    ptref = s_data.m[L_SEL].points [analyseData::dai_REFP];
//
//                if ( !s_data.m[L_SEL].motion.IsIdentity () )
//                { // maybe is the inverse??
//                    s_data.m[L_SEL].motion.ApplyToPoint ( ptref );
//                }
//
//                tpanalysis::compTPAMode mode = checkTPAMode (sel);
//                switch ( mode )
//                {
//                    case tpanalysis::cm_Point:
//                    {
//                        if ( s_data.m[sel].specialIndex [analyseData::dai_SXP1] == UNKNOWNINDEX )
//                            break;
//
//                        pt0 = s_data.m[sel].points [analyseData::dai_SXP1];
//
//                        if ( !s_data.m[L_SEL].motion.IsIdentity () )
//                        { // maybe is the inverse??
//                            s_data.m[L_SEL].motion.ApplyToPoint ( pt0 );
//                        }
//
////                        pBag->AddTyped<bgeo::Coord3d> ( pBagPropNames[sel][tpMInfo_measurepoint0", pt0 );
//                        omsl::bgeo::Point3d p3d ( pt0 );
//
//                        int offset = 0;
//                        int nAddData1=0, nAddData2=0;
//                        CTADDDATA a, b;
//                        a.data.w [0] = 0;
//                        b.data.w [0] = 0;
//                        if ( s_data.m[sel].m_pAttr )
//                        {
//                            ToolpathData* ptpData = GetToolpathData ( s_data.m[sel].m_pAttr );
//
//                            if ( ptpData )
//                            {
//                                offset = ptpData->G_Count ()[ToolpathData::eSegmentCount];
//                                omPofTechData data ( ptpData->TechData ( s_data.m[sel].specialIndex[analyseData::dai_SXP1] ) );
//
//                                omsl::SharedWStringPtr str ( omsl::WStringImpl::Create ( ptpData->Jobname () ) );
//                                pBag->AddTyped<omsl::SharedWStringPtr> ( pBagPropNames[sel][tpMInfo_JOBNAME], str );
//
//                                if ( showLayer25 )
//                                {
//                                    nAddData1 = ptpData->getSpecialDataSize ( s_data.m[sel].specialIndex [analyseData::dai_SXP1] );
//
//                                    if ( nAddData1 > 0 )
//                                    {
//                                        a.data.d = ptpData->getSpecialData ( s_data.m[sel].specialIndex [analyseData::dai_SXP1], 0 ).data.d;
//                                    }
//
//                                    nAddData2 = ptpData->getSpecialDataSize ( s_data.m[sel].specialIndex [analyseData::dai_SXP1 + 1] );
//                                    if ( nAddData2 > 1 )
//                                    {
//                                        b.data.d = ptpData->getSpecialData ( s_data.m[sel].specialIndex [analyseData::dai_SXP1 + 1], 0 ).data.d;
//                                    }
//                                }
//                                else
//                                {
//                                    if ( data.gmotion > 0 )
//                                    {
//                                        if ( data.feedUnit == 0 ) // milling mm/min
//                                        {
//                                            double xd;
//                                            pConverter->SetDecimals ( 0 );
//                                            // fill vector with rounded values
//                                            pConverter->SetInner ( data.feedrate );
//                                            const wchar_t* s2 = pConverter->Get ();
//
//                                            /*int ret =*/ swscanf ( s2, L"%lf", &xd );
//                                            pBag->AddTyped<int> ( pBagPropNames[sel][tpMInfo_FEEDRATE], ( int ) xd );
//                                        }
//                                        else // turning mm/U
//                                            pBag->AddTyped<double> ( pBagPropNames[sel][tpMInfo_FEEDRATETURNING], ( double ) data.feedrate );
//                                    }
//
//                                    if ( data.speedUnit == 0 ) // milling U/min
//                                    {
//                                        SharedPtr<measureunit::Converter> pConverterUnitLess;
//                                        pConverterUnitLess = omsl::measureunit::Converter::CreateGeneric ( omsl::units::Dimension::Length (), nullptr );
//                                        double xd;
//                                        pConverterUnitLess->SetDecimals ( 0 );
//                                        // fill vector with rounded values
//                                        pConverterUnitLess->SetInner ( data.speed );
//                                        const wchar_t* s2 = pConverterUnitLess->Get ();
//
//                                        /*int ret = */swscanf ( s2, L"%lf", &xd );
//                                        pBag->AddTyped<int> ( pBagPropNames[sel][tpMInfo_SPEED], ( int ) xd );
//                                    }
//                                    else // turning m/min
//                                    {
//                                        pBag->AddTyped<double> ( pBagPropNames[sel][tpMInfo_CUTTINGSPEED], ( double ) data.speed );
//                                    }
//
//                                    omsl::SharedWStringPtr str1 ( omsl::WStringImpl::Create ( data.gmotion == 0 ? L"G0" : data.gmotion == 1 ? L"G1" : data.gmotion == 2 ? L"G2" : data.gmotion == 3 ? L"G3" : L"GX" ) );
//                                    pBag->AddTyped<omsl::SharedWStringPtr> ( pBagPropNames[sel][tpMInfo_GTYPE], str1 );
//                                }
//                            }
//                        }
//
//                        pBag->AddTyped<Double> ( pBagPropNames[sel][tpMInfo_mp0_X], p3d[0] );
//                        pBag->AddTyped<Double> ( pBagPropNames[sel][tpMInfo_mp0_Y], p3d[1] );
//                        pBag->AddTyped<Double> ( pBagPropNames[sel][tpMInfo_mp0_Z], p3d[2] );
//
//                        if ( nAddData1 )
//                        {
//                            pBag->AddTyped<Double> ( pBagPropNames[sel][tpMInfo_mp0_D], a.data.d );
//                        }
//
//
//                        if ( s_data.m[sel].specialIndex [analyseData::dai_Index1stTpSeg] >= 0 && s_data.m[sel].specialIndex [analyseData::dai_Index1stTpSeg] < s_data.m[sel].segmentSizeVec.size () )
//                            pBag->AddTyped<int> ( pBagPropNames[sel][tpMInfo_MOVENR], s_data.m[sel].specialIndex [analyseData::dai_SXP1] + s_data.m[sel].specialIndex [analyseData::dai_Index1stTpSeg] + offset );
//                        else
//                        {
//                            pBag->AddTyped<int> ( pBagPropNames[sel][tpMInfo_MOVENR], s_data.m[sel].specialIndex [analyseData::dai_SXP1] + offset );
//                        }
//
//                        const bgeo::Vector3d* pv = s_data.m[sel].m_pTpSeg->GetDirection ( s_data.m[sel].specialIndex [analyseData::dai_SXP1] );
//                        if ( pv )
//                        {
//                            bgeo::Vector3d v ( *pv );
//                            double A = 0., B = 0., C = 0.;
//
//                            //                        void omCGeMatrix::GetEulerAngles ( double& A, double& B, double& C ) const
//                            {
//                                // this function is the same with omHxyzAnglesVz() in Omheuler.c from EDK
//                                double V_EPS = 1.0e-8;
//
//                                bgeo::UnitVector3d x_vector, y_vector, z_vector ( v [0], v [1], v [2] );
//                                s_data.m[L_SEL].motion.ApplyToUnitVector ( z_vector );
//                                z_vector.BuildXYZVectors ( x_vector, y_vector );
//
//                                if ( x_vector [2] < ( -1.0 + V_EPS ) )
//                                {
//                                    A = 0.0;
//                                    B = 90.0;
//                                    C = 180.0 / M_PI * atan2 ( z_vector [1], z_vector [0] );
//                                }
//                                else if ( x_vector [2] > ( 1.0 - V_EPS ) )
//                                {
//                                    A = 0.0;
//                                    B = -90.0;
//
//                                    if ( z_vector [1] >= 0.0 ) /* d.h.  0 < wz < 180 ==> -180 to 0 */
//                                        C = -180.0 + ( 180 / M_PI * atan2 ( z_vector [1], z_vector [0] ) );
//                                    else             /* d.h.  -180 < wz < 0 ==> 0 bis 180 */
//                                        C = 180.0 + ( 180 / M_PI * atan2 ( z_vector [1], z_vector [0] ) );
//                                }
//                                else  /* -90 < y_Angle < 90 */
//                                {
//                                    double phi1 = 0;
//                                    double phi2 = 0;
//                                    double phi3 = 0;
//                                    phi2 = -asin ( x_vector [2] );
//
//                                    if ( ( ( z_vector [2] < 0.0 ) ? -z_vector [2] : z_vector [2] ) > V_EPS ) /*E06*/
//                                        phi1 = atan2 ( y_vector [2], z_vector [2] );
//                                    else
//                                    {
//                                        double cs = cos ( phi2 );
//
//                                        double ratio = y_vector [2] / cs;
//                                        if ( ratio > 0.999999 )        ratio = 1.0;
//                                        else if ( ratio < -0.999999 )  ratio = -1.0;
//
//                                        phi1 = asin ( ratio );
//                                    }
//                                    if ( ( fabs ( x_vector [0] ) > V_EPS ) ||
//                                        ( fabs ( x_vector [1] ) > V_EPS ) )
//                                        phi3 = atan2 ( x_vector [1], x_vector [0] );
//                                    else
//                                        phi3 = atan2 ( -y_vector [0], y_vector [1] );
//
//                                    A = 180. / M_PI * phi1;
//                                    B = 180. / M_PI * phi2;
//                                    C = 180. / M_PI * phi3;
//                                }
//                            }
//                        }
//
//                        //if ( A != B )
//                        //{
//                        //    pBag->AddTyped<double> ( pBagPropNames[sel][tpMInfo_TANGLEA], A );
//                        //    pBag->AddTyped<double> ( pBagPropNames[sel][tpMInfo_TANGLEB], B );
//                        //}
//                        //else
//                        //{
//                        //    pBag->AddTyped<double> ( pBagPropNames[sel][tpMInfo_TANGLEA], A );
//                        //}
//                        //pBag->AddTyped<double> ( pBagPropNames[sel][tpMInfo_TANGLEC], C );
//
//                        break;
//                    }
//                    case tpanalysis::cm_Point_Point:
//                    {
//                        if ( s_data.m[L_SEL].specialIndex [analyseData::dai_SXP1] == UNKNOWNINDEX )
//                            break;
//
//                        pt0 = s_data.m[L_SEL].points [analyseData::dai_SXP1];
//                        pt1 = s_data.m[R_SEL].points [analyseData::dai_SXP1];
//
//                        if ( !s_data.m[L_SEL].motion.IsIdentity () )
//                        { // maybe is the inverse??
//                            s_data.m[L_SEL].motion.ApplyToPoint ( pt0 );
//                            s_data.m[L_SEL].motion.ApplyToPoint ( pt1 );
//                        }
//
//                        pBag->AddTyped<bgeo::Coord3d> ( pBagPropNames[sel][tpMInfo_measurepoint1], pt0 );
//                        pBag->AddTyped<bgeo::Coord3d> ( pBagPropNames[sel][tpMInfo_measurepoint2], pt1 );
//                        break;                                                                               
//                    }
//
//                    case tpanalysis::cm_Toolpath:
//                    {
//                        if ( s_data.m[sel].pointVec.size () == 0 )
//                            break;
//
//                        pt0 = s_data.m[sel].pointVec [0];
//                        if ( !s_data.m[L_SEL].motion.IsIdentity () )
//                        { // maybe is the inverse??
//                            s_data.m[L_SEL].motion.ApplyToPoint ( pt0 );
//                        }
//
//                        pBag->AddTyped<bgeo::Coord3d> ( pBagPropNames[sel][tpMInfo_measurepoint0], pt0 );
//
//                        if ( s_data.m[sel].m_pAttr )
//                        {
//                            ToolpathData* ptpData = GetToolpathData ( s_data.m[sel].m_pAttr );
//                            if ( ptpData )
//                            {
//                                omPofTechData data ( ptpData->TechData ( 0 ) );
//                                omsl::SharedWStringPtr str ( omsl::WStringImpl::Create ( data.gmotion == 0 ? L"G0" : data.gmotion == 1 ? L"G1" : data.gmotion == 2 ? L"G2" : data.gmotion == 3 ? L"G3" : L"GX" ) );
//
//                                pBag->AddTyped<omsl::SharedWStringPtr> ( pBagPropNames[sel][tpMInfo_GTYPE], str );
//                                pBag->AddTyped<int> ( pBagPropNames[sel][tpMInfo_FEEDRATE], (int)data.feedrate );
//                                pBag->AddTyped<int> ( pBagPropNames[sel][tpMInfo_SPEED], (int)data.speed );
//                                pBag->AddTyped<int> ( pBagPropNames[sel][tpMInfo_CNTG0], ptpData->G_Count ( 0 ) );
//                                pBag->AddTyped<int> ( pBagPropNames[sel][tpMInfo_CNTG1], ptpData->G_Count ( 1 ) );
//                                pBag->AddTyped<int> ( pBagPropNames[sel][tpMInfo_CNTG2], ptpData->G_Count ( 2 ) );
//                                pBag->AddTyped<int> ( pBagPropNames[sel][tpMInfo_CNTG3], ptpData->G_Count ( 3 ) );
//                                pBag->AddTyped<int> ( pBagPropNames[sel][tpMInfo_CNT5X], ptpData->G_Count ( 5 ) );
//                                pBag->AddTyped<int> ( pBagPropNames[sel][tpMInfo_CNTPOINTS], ptpData->G_Count ( 6 ) );
//                            }
//                        }
//                        
//                        pBag->AddTyped<int> ( pBagPropNames[sel][tpMInfo_MOVENR], s_data.m[sel].specialIndex [analyseData::dai_SXP1] );
//                        break;
//                    }
//                    case tpanalysis::cm_Movement:
//                    {
//                        if ( s_data.m[sel].specialIndex [analyseData::dai_SXP1] == UNKNOWNINDEX )
//                            break;
//
//                        pt0 = s_data.m[sel].points [analyseData::dai_SXS];
//                        pt1 = s_data.m[sel].points [analyseData::dai_SXE];
//
//                        if ( !s_data.m[L_SEL].motion.IsIdentity () )
//                        { // maybe is the inverse??
//                            s_data.m[L_SEL].motion.ApplyToPoint ( pt0 );
//                            s_data.m[L_SEL].motion.ApplyToPoint ( pt1 );
//                        }
//
//                        int offset = 0;
//                        int nAddData1=0, nAddData2=0;
//                        CTADDDATA a, b;
//
//                        if ( s_data.m[sel].m_pAttr )
//                        {
//                            ToolpathData* ptpData = GetToolpathData ( s_data.m[sel].m_pAttr );
//
//                            if ( ptpData )
//                            {
//                                offset = ptpData->G_Count () [ToolpathData::eSegmentCount];
//                                omPofTechData data ( ptpData->TechData ( s_data.m[sel].specialIndex [analyseData::dai_SXP1] ) );
//
//                                omsl::SharedWStringPtr str ( omsl::WStringImpl::Create ( ptpData->Jobname () ) );
//                                pBag->AddTyped<omsl::SharedWStringPtr> ( pBagPropNames[sel][tpMInfo_JOBNAME], str );
//
//                                if ( showLayer25 )
//                                {
//                                    nAddData1 = ptpData->getSpecialDataSize ( s_data.m[sel].specialIndex [analyseData::dai_SXP1] );
//
//                                    if ( nAddData1 > 0 )
//                                    {
//                                        a.data.d = ptpData->getSpecialData ( s_data.m[sel].specialIndex [analyseData::dai_SXP1], 0 ).data.d;
//                                    }
//
//                                    nAddData2 = ptpData->getSpecialDataSize ( s_data.m[sel].specialIndex [analyseData::dai_SXP1 + 1] );
//                                    if ( nAddData2 > 1 )
//                                    {
//                                        b.data.d = ptpData->getSpecialData ( s_data.m[sel].specialIndex [analyseData::dai_SXP1 + 1], 0 ).data.d;
//                                    }
//                                }
//                                else
//                                {
//                                    if ( data.gmotion > 0 )
//                                    {
//                                        if ( data.feedUnit == 0 ) // milling mm/min
//                                        {
//                                            double xd;
//                                            pConverter->SetDecimals ( 0 );
//                                            // fill vector with rounded values
//                                            pConverter->SetInner ( data.feedrate );
//                                            const wchar_t* s2 = pConverter->Get ();
//
//                                            /*int ret = */swscanf ( s2, L"%lf", &xd );
//                                            pBag->AddTyped<int> ( pBagPropNames[sel][tpMInfo_FEEDRATE], ( int ) xd );
//                                        }
//                                        else // turning mm/U
//                                            pBag->AddTyped<double> ( pBagPropNames[sel][tpMInfo_FEEDRATETURNING], ( double ) data.feedrate );
//                                    }
//
//                                    if ( data.speedUnit == 0 ) // milling U/min
//                                    {
//                                        SharedPtr<measureunit::Converter> pConverterUnitLess;
//                                        pConverterUnitLess = omsl::measureunit::Converter::CreateGeneric ( omsl::units::Dimension::Length (), nullptr );
//                                        double xd;
//                                        pConverterUnitLess->SetDecimals ( 0 );
//                                        // fill vector with rounded values
//                                        pConverterUnitLess->SetInner ( data.speed );
//                                        const wchar_t* s2 = pConverterUnitLess->Get ();
//
//                                        /*int ret = */swscanf ( s2, L"%lf", &xd );
//                                        pBag->AddTyped<int> ( pBagPropNames[sel][tpMInfo_SPEED], ( int ) xd );
//                                    }
//                                    else // turning m/min
//                                    {
//                                        pBag->AddTyped<double> ( pBagPropNames[sel][tpMInfo_CUTTINGSPEED], ( double ) data.speed );
//                                    }
//
//                                    omsl::SharedWStringPtr str1 ( omsl::WStringImpl::Create ( data.gmotion == 0 ? L"G0" : data.gmotion == 1 ? L"G1" : data.gmotion == 2 ? L"G2" : data.gmotion == 3 ? L"G3" : L"GX" ) );
//                                    pBag->AddTyped<omsl::SharedWStringPtr> ( pBagPropNames[sel][tpMInfo_GTYPE], str1 );
//                                }
//                            }
//                        }
//
//                        omsl::bgeo::Point3d p30d ( pt0 );
//                        omsl::bgeo::Point3d p31d ( pt1);
//
//                        pBag->AddTyped<Double> ( pBagPropNames[sel][tpMInfo_mp1_X], p30d [0] );
//                        pBag->AddTyped<Double> ( pBagPropNames[sel][tpMInfo_mp1_Y], p30d [1] );
//                        pBag->AddTyped<Double> ( pBagPropNames[sel][tpMInfo_mp1_Z], p30d [2] );
//
//                        if ( nAddData1 )
//                        {
//                            pBag->AddTyped<Double> ( pBagPropNames[sel][tpMInfo_mp1_D], a.data.d );
//                        }
//
//                        pBag->AddTyped<Double> ( pBagPropNames[sel][tpMInfo_mp2_X], p31d [0] );
//                        pBag->AddTyped<Double> ( pBagPropNames[sel][tpMInfo_mp2_Y], p31d [1] );
//                        pBag->AddTyped<Double> ( pBagPropNames[sel][tpMInfo_mp2_Z], p31d [2] );
//
//                        if ( nAddData1 )
//                        {
//                            pBag->AddTyped<Double> ( pBagPropNames[sel][tpMInfo_mp2_D], b.data.d );
//                        }
//
//                        if ( s_data.m[sel].specialIndex [analyseData::dai_Index1stTpSeg] >= 0 && s_data.m[sel].specialIndex [analyseData::dai_Index1stTpSeg] < s_data.m[sel].segmentSizeVec.size () )
//                            pBag->AddTyped<int> ( pBagPropNames[sel][tpMInfo_MOVENR], s_data.m[sel].specialIndex [analyseData::dai_SXP1] + s_data.m[sel].specialIndex [analyseData::dai_Index1stTpSeg] + offset );
//                        else
//                        {
//                            pBag->AddTyped<int> ( pBagPropNames[sel][tpMInfo_MOVENR], s_data.m[sel].specialIndex [analyseData::dai_SXP1] + offset );
//                        }
//
//                        const bgeo::Vector3d* pv = s_data.m[sel].m_pTpSeg->GetDirection ( s_data.m[sel].specialIndex [analyseData::dai_SXP1] );
//                        if ( pv )
//                        {
//                            bgeo::Vector3d v ( *pv );
//                            double A = 0., B = 0., C = 0.;
//
//                            //                        void omCGeMatrix::GetEulerAngles ( double& A, double& B, double& C ) const
//                            {
//                                // this function is the same with omHxyzAnglesVz() in Omheuler.c from EDK
//                                double V_EPS = 1.0e-8;
//
//                                bgeo::UnitVector3d x_vector, y_vector, z_vector ( v [0], v [1], v [2] );
//                                s_data.m[L_SEL].motion.ApplyToUnitVector ( z_vector );
//                                z_vector.BuildXYZVectors ( x_vector, y_vector );
//
//                                if ( x_vector [2] < ( -1.0 + V_EPS ) )
//                                {
//                                    A = 0.0;
//                                    B = 90.0;
//                                    C = 180.0 / M_PI * atan2 ( z_vector [1], z_vector [0] );
//                                }
//                                else if ( x_vector [2] > ( 1.0 - V_EPS ) )
//                                {
//                                    A = 0.0;
//                                    B = -90.0;
//
//                                    if ( z_vector [1] >= 0.0 ) /* d.h.  0 < wz < 180 ==> -180 to 0 */
//                                        C = -180.0 + ( 180 / M_PI * atan2 ( z_vector [1], z_vector [0] ) );
//                                    else             /* d.h.  -180 < wz < 0 ==> 0 bis 180 */
//                                        C = 180.0 + ( 180 / M_PI * atan2 ( z_vector [1], z_vector [0] ) );
//                                }
//                                else  /* -90 < y_Angle < 90 */
//                                {
//                                    double phi1 = 0;
//                                    double phi2 = 0;
//                                    double phi3 = 0;
//                                    phi2 = -asin ( x_vector [2] );
//
//                                    if ( ( ( z_vector [2] < 0.0 ) ? -z_vector [2] : z_vector [2] ) > V_EPS ) /*E06*/
//                                        phi1 = atan2 ( y_vector [2], z_vector [2] );
//                                    else
//                                    {
//                                        double cs = cos ( phi2 );
//
//                                        double ratio = y_vector [2] / cs;
//                                        if ( ratio > 0.999999 )        ratio = 1.0;
//                                        else if ( ratio < -0.999999 )  ratio = -1.0;
//
//                                        phi1 = asin ( ratio );
//                                    }
//                                    if ( ( fabs ( x_vector [0] ) > V_EPS ) ||
//                                        ( fabs ( x_vector [1] ) > V_EPS ) )
//                                        phi3 = atan2 ( x_vector [1], x_vector [0] );
//                                    else
//                                        phi3 = atan2 ( -y_vector [0], y_vector [1] );
//
//                                    A = 180. / M_PI * phi1;
//                                    B = 180. / M_PI * phi2;
//                                    C = 180. / M_PI * phi3;
//                                }
//                            }
//                        }
//
//                        //if ( A != B )
//                        //{
//                        //    pBag->AddTyped<double> ( pBagPropNames[sel][tpMInfo_TANGLEA], A );
//                        //    pBag->AddTyped<double> ( pBagPropNames[sel][tpMInfo_TANGLEB], B );
//                        //}
//                        //else
//                        //{
//                        //    pBag->AddTyped<double> ( pBagPropNames[sel][tpMInfo_TANGLEA], A );
//                        //}
//                        //pBag->AddTyped<double> ( pBagPropNames[sel][tpMInfo_TANGLEC], C );
//
//                        break;
//                    }
//                    case tpanalysis::cm_Point_Movement:
//                    case tpanalysis::cm_Point_Geometry:
//                    case tpanalysis::cm_Movement_Movement:
//                    case tpanalysis::cm_Movement_Geometry:
//
//                    {
//                        if ( s_data.m[sel].pointVec.size () == 0 )
//                            break;
//
//                        pt0 = s_data.m[sel].points [analyseData::dai_SXS];
//                        pt1 = s_data.m[sel].points [analyseData::dai_SXE];
//
//                        if ( !s_data.m[L_SEL].motion.IsIdentity () )
//                        { // maybe is the inverse??
//                            s_data.m[L_SEL].motion.ApplyToPoint ( pt0 );
//                            s_data.m[L_SEL].motion.ApplyToPoint ( pt1 );
//                        }
//
//                        pBag->AddTyped<bgeo::Coord3d> ( pBagPropNames[sel][tpMInfo_measurepoint1], pt0 );
//                        pBag->AddTyped<bgeo::Coord3d> ( pBagPropNames[sel][tpMInfo_measurepoint2], pt1 );
//                        break;
//                    }
//                    case tpanalysis::cm_Geometry: break;
//
//                }
//
//                // refrence point not necessary
//                //if ( sel == L_SEL && s_data.m[L_SEL].specialIndex [analyseData::dai_REFP] != UNKNOWNINDEX )
//                //    pBag->AddTyped<bgeo::Coord3d> ( pBagPropNames[sel][tpMInfo_referencepoint], ptref );
//
//                if ( checkTPAMode (sel) != tpanalysis::cm_Toolpath ) // ?????
//                {
//                    if ( s_data.m[sel].pointVec.size () > 0 )
//                    {
//                        int ix = s_data.m[sel].specialIndex [analyseData::dai_SXP1]-1;
//                        int type = 999;
//                        if ( ix >= 0 )
//                            type = s_data.m[sel].typeVec [ix];
//
//                        SetupTemporaryArcMovements ( vm_SHOWLABEL, 1 ); // checkTPAMode () == tpanalysis::cm_Movement ? type : 0 );
//
//                        switch ( type )
//                        {
//                            case analyseData::dai_tUnknown: // 0 = Point
//                            {
//                                if ( checkTPAMode (sel) != tpanalysis::cm_Movement )
//                                    break;
//                            }
//                            case analyseData::dai_tLine: // 1 = line
//                            {
//                                bgeo::Vector3d Xxv = pt1 - pt0;
//                                pBag->AddTyped<double> ( pBagPropNames[sel][tpMInfo_Length], Xxv.Distance ( pt1, pt0 ) );
//                                break;
//                            }
//                            case analyseData::dai_tArc: // 2 = arc // dangerous ?
//                            {
//                                pt2 = s_data.m[sel].points [analyseData::dai_CTR1];
//                                if ( !s_data.m[L_SEL].motion.IsIdentity () )
//                                { // maybe is the inverse??
//                                    s_data.m[L_SEL].motion.ApplyToPoint ( pt2 );
//                                }
//
//                                pBag->AddTyped<bgeo::Coord3d> ( pBagPropNames[sel][tpMInfo_centerpoint], pt2 );
//                                pBag->AddTyped<double> ( pBagPropNames[sel][tpMInfo_Radius_A], s_data.m[sel].vectors [analyseData::dai_VR] [analyseData::dai_SXS] );
//                                pBag->AddTyped<double> ( pBagPropNames[sel][tpMInfo_Radius_E], s_data.m[sel].vectors [analyseData::dai_VR] [analyseData::dai_SXE] );
//                                pBag->AddTyped<double> ( pBagPropNames[sel][tpMInfo_Angle_A], s_data.m[sel].vectors [analyseData::dai_VA] [analyseData::dai_SXS] );
////                                pBag->AddTyped<double> ( pBagPropNames[sel][tpMInfo_Angle_C], s_data.m[sel].vectors [analyseData::dai_VA] [analyseData::dai_SXC] );
//                                pBag->AddTyped<double> ( pBagPropNames[sel][tpMInfo_Angle_E], s_data.m[sel].vectors [analyseData::dai_VA] [analyseData::dai_SXE] );
//
//                                double dist = s_data.m[sel].vectors [analyseData::dai_VR] [analyseData::dai_SXS] * ( s_data.m[sel].vectors [analyseData::dai_VA] [analyseData::dai_SXE] - s_data.m[sel].vectors [analyseData::dai_VA] [analyseData::dai_SXS] );
//                                pBag->AddTyped<double> ( pBagPropNames[sel][tpMInfo_Length], dist );
//                                break;
//                            }
//                            case analyseData::dai_tCurve:
//                            default: // unknown
//                            {
//                                break;
//                            }
//                        }
//                    }
//
//                    if ( checkTPAMode (sel) == tpanalysis::cm_Point )
//                    {
//                        //bgeo::Coord3d Xx ( ptref [0] - pt0 [0], ptref [1] - pt0 [1], ptref [2] - pt0 [2] );
//                        //if ( s_data.m[L_SEL].specialIndex [analyseData::dai_REFP] != UNKNOWNINDEX )
//                        //    pBag->AddTyped<bgeo::Coord3d> ( pBagPropNames[sel][tpMInfo_Delta], Xx );
//                    }
//                    else
//                    {
//                        bgeo::Coord3d Xx ( pt1 [0] - pt0 [0], pt1 [1] - pt0 [1], pt1 [2] - pt0 [2] );
//                        pBag->AddTyped<bgeo::Coord3d> ( pBagPropNames[sel][tpMInfo_Delta], Xx );
//                    }
//                }
//            }
#endif
            // techdata
            void TPAnalysisModel::SetupPropertyBagFor_T ( SharedPtr<propertyservice::PropertyBag> pBag, int sel, bool fullData )
            {
                if ( s_data.m [sel].specialIndex [analyseData::dai_SXP1] == UNKNOWNINDEX )
                {
//                    SetupTemporaryArcMovements ( 0, -sel );
                    return;
                }

                bool showLayer25 = GetProperties ()->GetBag ()->Exists ( properties::key::ShowLayer25 ) && GetProperties ()->GetBag ()->GetValue<omsl::Bool> ( properties::key::ShowLayer25 );

                SharedPtr<measureunit::Converter> pConverter;
                pConverter = omsl::measureunit::Converter::CreateGeneric ( omsl::units::Dimension::Length (), nullptr );
                pConverter->SetDecimals ( 0 );

                int offset = 0;
                int nAddData1 = 0, nAddData2 = 0;
                CTADDDATA a, b;
                a.data.w [0] = 0;
                b.data.w [0] = 0;
                if ( s_data.m [sel].m_pAttr )
                {
                    ToolpathData* ptpData = GetToolpathData ( s_data.m [sel].m_pAttr );

                    if ( ptpData )
                    {
                        offset = ptpData->G_Count () [ToolpathData::eSegmentCount];
                        omPofTechData data ( ptpData->TechData ( s_data.m [sel].specialIndex [analyseData::dai_SXP1] ) );

                        if ( fullData == true )
                        {
                            omsl::SharedWStringPtr str ( omsl::WStringImpl::Create ( ptpData->Jobname () ) );
                            pBag->AddTyped<omsl::SharedWStringPtr> ( pBagPropNames [sel] [tpMInfo_JOBNAME], str );

                            if ( showLayer25 )
                            {
                                nAddData1 = ptpData->getSpecialDataSize ( s_data.m [sel].specialIndex [analyseData::dai_SXP1] );

                                if ( nAddData1 > 0 )
                                {
                                    a.data.d = ptpData->getSpecialData ( s_data.m [sel].specialIndex [analyseData::dai_SXP1], 0 ).data.d;
                                }

                                nAddData2 = ptpData->getSpecialDataSize ( s_data.m [sel].specialIndex [analyseData::dai_SXP1 + 1] );
                                if ( nAddData2 > 1 )
                                {
                                    b.data.d = ptpData->getSpecialData ( s_data.m [sel].specialIndex [analyseData::dai_SXP1 + 1], 0 ).data.d;
                                }
                            }
                            else
                            {
                                if ( data.gmotion > 0 )
                                {
                                    if ( data.feedUnit == 0 ) // milling mm/min
                                    {
                                        double xd;
                                        // fill vector with rounded values
                                        pConverter->SetInner ( data.feedrate );
                                        const wchar_t* s2 = pConverter->Get ();

                                        /*int ret =*/ swscanf ( s2, L"%lf", &xd );
                                        pBag->AddTyped<int> ( pBagPropNames [sel] [tpMInfo_FEEDRATE], ( int ) xd );
                                    }
                                    else // turning mm/U
                                        pBag->AddTyped<double> ( pBagPropNames [sel] [tpMInfo_FEEDRATETURNING], ( double ) data.feedrate );
                                }

                                if ( data.speedUnit == 0 ) // milling U/min
                                {
                                    double xd;
                                    // fill vector with rounded values
                                    pConverter->SetInner ( data.speed );
                                    const wchar_t* s2 = pConverter->Get ();

                                    /*int ret = */swscanf ( s2, L"%lf", &xd );
                                    pBag->AddTyped<int> ( pBagPropNames [sel] [tpMInfo_SPEED], ( int ) xd );
                                }
                                else // turning m/min
                                {
                                    pBag->AddTyped<double> ( pBagPropNames [sel] [tpMInfo_CUTTINGSPEED], ( double ) data.speed );
                                }

                                omsl::SharedWStringPtr str1 ( omsl::WStringImpl::Create ( data.gmotion == 0 ? L"G0" : data.gmotion == 1 ? L"G1" : data.gmotion == 2 ? L"G2" : data.gmotion == 3 ? L"G3" : L"GX" ) );
                                pBag->AddTyped<omsl::SharedWStringPtr> ( pBagPropNames [sel] [tpMInfo_GTYPE], str1 );
                            }

                            pBag->AddTyped<int> ( pBagPropNames [sel] [tpMInfo_MOVENR], ptpData->MovementId ( s_data.m [sel].specialIndex [analyseData::dai_SXP1] ));
                        }
                    }
                }
            }

            void TPAnalysisModel::SetupPropertyBagFor_P ( SharedPtr<propertyservice::PropertyBag> pBag )
            {
                if ( s_data.m [L_SEL].specialIndex [analyseData::dai_SXP1] == UNKNOWNINDEX )
                    return;

                bgeo::Coord3d pt0;
                bgeo::Coord3d pt1;
                bgeo::Coord3d pt2;
                bgeo::Coord3d ptref;
                bool showLayer25 = GetProperties ()->GetBag ()->Exists ( properties::key::ShowLayer25 ) && GetProperties ()->GetBag ()->GetValue<omsl::Bool> ( properties::key::ShowLayer25 );

                pt0 = s_data.m [L_SEL].points [analyseData::dai_SXP1];

                if ( !s_data.m [L_SEL].motion.IsIdentity () )
                { // maybe is the inverse??
                    s_data.m [L_SEL].motion.ApplyToPoint ( pt0 );
                }

                //                        pBag->AddTyped<bgeo::Coord3d> ( pBagPropNames[sel][tpMInfo_measurepoint0", pt0 );
                omsl::bgeo::Point3d p3d ( pt0 );

                int movementId = -1;
                int offset = 0;
                int nAddData1 = 0, nAddData2 = 0;
                CTADDDATA a, b;
                a.data.w [0] = 0;
                b.data.w [0] = 0;
                if ( s_data.m [L_SEL].m_pAttr )
                {
                    ToolpathData* ptpData = GetToolpathData ( s_data.m [L_SEL].m_pAttr );

                    if ( ptpData )
                    {
                        movementId = ptpData->MovementId ( s_data.m [L_SEL].specialIndex [analyseData::dai_SXP1] );
                        
                        offset = ptpData->G_Count () [ToolpathData::eSegmentCount];
                        omPofTechData data ( ptpData->TechData ( s_data.m [L_SEL].specialIndex [analyseData::dai_SXP1] ) );

                        omsl::SharedWStringPtr str ( omsl::WStringImpl::Create ( ptpData->Jobname () ) );
                        pBag->AddTyped<omsl::SharedWStringPtr> ( pBagPropNames [L_SEL] [tpMInfo_JOBNAME], str );

                        if ( showLayer25 )
                        {
                            nAddData1 = ptpData->getSpecialDataSize ( s_data.m [L_SEL].specialIndex [analyseData::dai_SXP1] );

                            if ( nAddData1 > 0 )
                            {
                                a.data.d = ptpData->getSpecialData ( s_data.m [L_SEL].specialIndex [analyseData::dai_SXP1], 0 ).data.d;
                            }

                            nAddData2 = ptpData->getSpecialDataSize ( s_data.m [L_SEL].specialIndex [analyseData::dai_SXP1 + 1] );
                            if ( nAddData2 > 1 )
                            {
                                b.data.d = ptpData->getSpecialData ( s_data.m [L_SEL].specialIndex [analyseData::dai_SXP1 + 1], 0 ).data.d;
                            }
                        }
                        else
                        {
                            SharedPtr<measureunit::Converter> pConverter;
                            pConverter = omsl::measureunit::Converter::CreateGeneric ( omsl::units::Dimension::Length (), nullptr );

                            if ( data.gmotion > 0 )
                            {
                                if ( data.feedUnit == 0 ) // milling mm/min
                                {
                                    double xd;
                                    pConverter->SetDecimals ( 0 );
                                    // fill vector with rounded values
                                    pConverter->SetInner ( data.feedrate );
                                    const wchar_t* s2 = pConverter->Get ();

                                    /*int ret =*/ swscanf ( s2, L"%lf", &xd );
                                    pBag->AddTyped<int> ( pBagPropNames [L_SEL] [tpMInfo_FEEDRATE], ( int ) xd );
                                }
                                else // turning mm/U
                                    pBag->AddTyped<double> ( pBagPropNames [L_SEL] [tpMInfo_FEEDRATETURNING], ( double ) data.feedrate );
                            }

                            if ( data.speedUnit == 0 ) // milling U/min
                            {
                                SharedPtr<measureunit::Converter> pConverterUnitLess;
                                pConverterUnitLess = omsl::measureunit::Converter::CreateGeneric ( omsl::units::Dimension::Length (), nullptr );
                                double xd;
                                pConverterUnitLess->SetDecimals ( 0 );
                                // fill vector with rounded values
                                pConverterUnitLess->SetInner ( data.speed );
                                const wchar_t* s2 = pConverterUnitLess->Get ();

                                /*int ret = */swscanf ( s2, L"%lf", &xd );
                                pBag->AddTyped<int> ( pBagPropNames [L_SEL] [tpMInfo_SPEED], ( int ) xd );
                            }
                            else // turning m/min
                            {
                                pBag->AddTyped<double> ( pBagPropNames [L_SEL] [tpMInfo_CUTTINGSPEED], ( double ) data.speed );
                            }

                            omsl::SharedWStringPtr str1 ( omsl::WStringImpl::Create ( data.gmotion == 0 ? L"G0" : data.gmotion == 1 ? L"G1" : data.gmotion == 2 ? L"G2" : data.gmotion == 3 ? L"G3" : L"GX" ) );
                            pBag->AddTyped<omsl::SharedWStringPtr> ( pBagPropNames [L_SEL] [tpMInfo_GTYPE], str1 );
                        }
                    }
                }

                pBag->AddTyped<Double> ( pBagPropNames [L_SEL] [tpMInfo_mp0_X], p3d [0] );
                pBag->AddTyped<Double> ( pBagPropNames [L_SEL] [tpMInfo_mp0_Y], p3d [1] );
                pBag->AddTyped<Double> ( pBagPropNames [L_SEL] [tpMInfo_mp0_Z], p3d [2] );

                if ( nAddData1 )
                {
                    pBag->AddTyped<Double> ( pBagPropNames [L_SEL] [tpMInfo_mp0_D], a.data.d );
                }

                if ( movementId > 0 )
                    pBag->AddTyped<int> ( pBagPropNames [L_SEL] [tpMInfo_MOVENR], movementId );
                else
                {
                    if ( s_data.m [L_SEL].specialIndex [analyseData::dai_Index1stTpSeg] >= 0 && s_data.m [L_SEL].specialIndex [analyseData::dai_Index1stTpSeg] < s_data.m [L_SEL].segmentSizeVec.size () )
                        pBag->AddTyped<int> ( pBagPropNames [L_SEL] [tpMInfo_MOVENR], s_data.m [L_SEL].specialIndex [analyseData::dai_SXP1] + s_data.m [L_SEL].specialIndex [analyseData::dai_Index1stTpSeg] + offset );
                    else
                    {
                        pBag->AddTyped<int> ( pBagPropNames [L_SEL] [tpMInfo_MOVENR], s_data.m [L_SEL].specialIndex [analyseData::dai_SXP1] + offset );
                    }
                }
                const bgeo::Vector3d* pv = s_data.m [L_SEL].m_pTpSeg->GetDirection ( s_data.m [L_SEL].specialIndex [analyseData::dai_SXP1] );
                if ( pv )
                {
                    bgeo::Vector3d v ( *pv );
                    bgeo::RigidMotion3d motion;
                    motion = s_data.m [L_SEL].pof_ncs.Inverse () * s_data.m [L_SEL].pof_frame  ;
                    S_calcEuler ( pBag, L_SEL, v, motion );
                    if (0)
                    {
                        double A = 0., B = 0., C = 0.;
                        // this function is the same with omHxyzAnglesVz() in Omheuler.c from EDK
                        double V_EPS = 1.0e-8;

                        bgeo::UnitVector3d x_vector, y_vector, z_vector ( v [0], v [1], v [2] );
                        s_data.m [L_SEL].motion.ApplyToUnitVector ( z_vector );
                        z_vector.BuildXYZVectors ( x_vector, y_vector );

                        if ( x_vector [2] < ( -1.0 + V_EPS ) )
                        {
                            A = 0.0;
                            B = 90.0;
                            C = 180.0 / M_PI * atan2 ( z_vector [1], z_vector [0] );
                        }
                        else if ( x_vector [2] > ( 1.0 - V_EPS ) )
                        {
                            A = 0.0;
                            B = -90.0;

                            if ( z_vector [1] >= 0.0 ) /* d.h.  0 < wz < 180 ==> -180 to 0 */
                                C = -180.0 + ( 180 / M_PI * atan2 ( z_vector [1], z_vector [0] ) );
                            else             /* d.h.  -180 < wz < 0 ==> 0 bis 180 */
                                C = 180.0 + ( 180 / M_PI * atan2 ( z_vector [1], z_vector [0] ) );
                        }
                        else  /* -90 < y_Angle < 90 */
                        {
                            double phi1 = 0;
                            double phi2 = 0;
                            double phi3 = 0;
                            phi2 = -asin ( x_vector [2] );

                            if ( ( ( z_vector [2] < 0.0 ) ? -z_vector [2] : z_vector [2] ) > V_EPS ) /*E06*/
                                phi1 = atan2 ( y_vector [2], z_vector [2] );
                            else
                            {
                                double cs = cos ( phi2 );

                                double ratio = y_vector [2] / cs;
                                if ( ratio > 0.999999 )        ratio = 1.0;
                                else if ( ratio < -0.999999 )  ratio = -1.0;

                                phi1 = asin ( ratio );
                            }
                            if ( ( fabs ( x_vector [0] ) > V_EPS ) ||
                                ( fabs ( x_vector [1] ) > V_EPS ) )
                                phi3 = atan2 ( x_vector [1], x_vector [0] );
                            else
                                phi3 = atan2 ( -y_vector [0], y_vector [1] );

                            A = 180. / M_PI * phi1;
                            B = 180. / M_PI * phi2;
                            C = 180. / M_PI * phi3;
                        }
                    }
                }
            }

            void TPAnalysisModel::SetupPropertyBagFor_M ( SharedPtr<propertyservice::PropertyBag> pBag )
            {
                int sel = L_SEL;
                if ( s_data.m [sel].specialIndex [analyseData::dai_SXP1] == UNKNOWNINDEX )
                    return;

                bgeo::Coord3d pt0;
                bgeo::Coord3d pt1;
                bgeo::Coord3d pt2;
                bgeo::Coord3d ptref;
                bool showLayer25 = GetProperties ()->GetBag ()->Exists ( properties::key::ShowLayer25 ) && GetProperties ()->GetBag ()->GetValue<omsl::Bool> ( properties::key::ShowLayer25 );

                pt0 = s_data.m [sel].points [analyseData::dai_SXS];
                pt1 = s_data.m [sel].points [analyseData::dai_SXE];

                if ( !s_data.m [sel].motion.IsIdentity () )
                { // maybe is the inverse??
                    s_data.m [sel].motion.ApplyToPoint ( pt0 );
                    s_data.m [sel].motion.ApplyToPoint ( pt1 );
                }

                int offset = 0;
                int nAddData1 = 0, nAddData2 = 0;
                CTADDDATA a, b;

                if ( s_data.m [sel].m_pAttr )
                {
                    ToolpathData* ptpData = GetToolpathData ( s_data.m [sel].m_pAttr );

                    if ( ptpData )
                    {
                        offset = ptpData->G_Count () [ToolpathData::eSegmentCount];
                        omPofTechData data ( ptpData->TechData ( s_data.m [sel].specialIndex [analyseData::dai_SXP1] ) );

                        omsl::SharedWStringPtr str ( omsl::WStringImpl::Create ( ptpData->Jobname () ) );
                        pBag->AddTyped<omsl::SharedWStringPtr> ( pBagPropNames [sel] [tpMInfo_JOBNAME], str );

                        if ( showLayer25 )
                        {
                            nAddData1 = ptpData->getSpecialDataSize ( s_data.m [sel].specialIndex [analyseData::dai_SXP1] );

                            if ( nAddData1 > 0 )
                            {
                                a.data.d = ptpData->getSpecialData ( s_data.m [sel].specialIndex [analyseData::dai_SXP1], 0 ).data.d;
                            }

                            nAddData2 = ptpData->getSpecialDataSize ( s_data.m [sel].specialIndex [analyseData::dai_SXP1 + 1] );
                            if ( nAddData2 > 1 )
                            {
                                b.data.d = ptpData->getSpecialData ( s_data.m [sel].specialIndex [analyseData::dai_SXP1 + 1], 0 ).data.d;
                            }
                        }
                        else
                        {
                            SharedPtr<measureunit::Converter> pConverter;
                            pConverter = omsl::measureunit::Converter::CreateGeneric ( omsl::units::Dimension::Length (), nullptr );
                            if ( data.gmotion > 0 )
                            {
                                if ( data.feedUnit == 0 ) // milling mm/min
                                {
                                    double xd;
                                    pConverter->SetDecimals ( 0 );
                                    // fill vector with rounded values
                                    pConverter->SetInner ( data.feedrate );
                                    const wchar_t* s2 = pConverter->Get ();

                                    /*int ret = */swscanf ( s2, L"%lf", &xd );
                                    pBag->AddTyped<int> ( pBagPropNames [sel] [tpMInfo_FEEDRATE], ( int ) xd );
                                }
                                else // turning mm/U
                                    pBag->AddTyped<double> ( pBagPropNames [sel] [tpMInfo_FEEDRATETURNING], ( double ) data.feedrate );
                            }

                            if ( data.speedUnit == 0 ) // milling U/min
                            {
                                SharedPtr<measureunit::Converter> pConverterUnitLess;
                                pConverterUnitLess = omsl::measureunit::Converter::CreateGeneric ( omsl::units::Dimension::Length (), nullptr );
                                double xd;
                                pConverterUnitLess->SetDecimals ( 0 );
                                // fill vector with rounded values
                                pConverterUnitLess->SetInner ( data.speed );
                                const wchar_t* s2 = pConverterUnitLess->Get ();

                                /*int ret = */swscanf ( s2, L"%lf", &xd );
                                pBag->AddTyped<int> ( pBagPropNames [sel] [tpMInfo_SPEED], ( int ) xd );
                            }
                            else // turning m/min
                            {
                                pBag->AddTyped<double> ( pBagPropNames [sel] [tpMInfo_CUTTINGSPEED], ( double ) data.speed );
                            }

                            omsl::SharedWStringPtr str1 ( omsl::WStringImpl::Create ( data.gmotion == 0 ? L"G0" : data.gmotion == 1 ? L"G1" : data.gmotion == 2 ? L"G2" : data.gmotion == 3 ? L"G3" : L"GX" ) );
                            pBag->AddTyped<omsl::SharedWStringPtr> ( pBagPropNames [sel] [tpMInfo_GTYPE], str1 );
                        }
                    }
                }

                omsl::bgeo::Point3d p30d ( pt0 );
                omsl::bgeo::Point3d p31d ( pt1 );

                pBag->AddTyped<Double> ( pBagPropNames [sel] [tpMInfo_mp1_X], p30d [0] );
                pBag->AddTyped<Double> ( pBagPropNames [sel] [tpMInfo_mp1_Y], p30d [1] );
                pBag->AddTyped<Double> ( pBagPropNames [sel] [tpMInfo_mp1_Z], p30d [2] );

                if ( nAddData1 )
                {
                    pBag->AddTyped<Double> ( pBagPropNames [sel] [tpMInfo_mp1_D], a.data.d );
                }

                pBag->AddTyped<Double> ( pBagPropNames [sel] [tpMInfo_mp2_X], p31d [0] );
                pBag->AddTyped<Double> ( pBagPropNames [sel] [tpMInfo_mp2_Y], p31d [1] );
                pBag->AddTyped<Double> ( pBagPropNames [sel] [tpMInfo_mp2_Z], p31d [2] );

                if ( nAddData1 )
                {
                    pBag->AddTyped<Double> ( pBagPropNames [sel] [tpMInfo_mp2_D], b.data.d );
                }

                if ( s_data.m [sel].specialIndex [analyseData::dai_Index1stTpSeg] >= 0 && s_data.m [sel].specialIndex [analyseData::dai_Index1stTpSeg] < s_data.m [sel].segmentSizeVec.size () )
                    pBag->AddTyped<int> ( pBagPropNames [sel] [tpMInfo_MOVENR], s_data.m [sel].specialIndex [analyseData::dai_SXP1] + s_data.m [sel].specialIndex [analyseData::dai_Index1stTpSeg] + offset );
                else
                {
                    pBag->AddTyped<int> ( pBagPropNames [sel] [tpMInfo_MOVENR], s_data.m [sel].specialIndex [analyseData::dai_SXP1] + offset );
                }

                const bgeo::Vector3d* pv = s_data.m [sel].m_pTpSeg->GetDirection ( s_data.m [sel].specialIndex [analyseData::dai_SXP1] );
                if ( pv )
                {
                    bgeo::Vector3d v ( *pv );

                    S_calcEuler ( pBag, sel, v, s_data.m [sel].motion );

                    if (0)
                    {
                        double A = 0., B = 0., C = 0.;
                        // this function is the same with omHxyzAnglesVz() in Omheuler.c from EDK
                        double V_EPS = 1.0e-8;

                        bgeo::UnitVector3d x_vector, y_vector, z_vector ( v [0], v [1], v [2] );
                        s_data.m [sel].motion.ApplyToUnitVector ( z_vector );
                        z_vector.BuildXYZVectors ( x_vector, y_vector );

                        if ( x_vector [2] < ( -1.0 + V_EPS ) )
                        {
                            A = 0.0;
                            B = 90.0;
                            C = 180.0 / M_PI * atan2 ( z_vector [1], z_vector [0] );
                        }
                        else if ( x_vector [2] > ( 1.0 - V_EPS ) )
                        {
                            A = 0.0;
                            B = -90.0;

                            if ( z_vector [1] >= 0.0 ) /* d.h.  0 < wz < 180 ==> -180 to 0 */
                                C = -180.0 + ( 180 / M_PI * atan2 ( z_vector [1], z_vector [0] ) );
                            else             /* d.h.  -180 < wz < 0 ==> 0 bis 180 */
                                C = 180.0 + ( 180 / M_PI * atan2 ( z_vector [1], z_vector [0] ) );
                        }
                        else  /* -90 < y_Angle < 90 */
                        {
                            double phi1 = 0;
                            double phi2 = 0;
                            double phi3 = 0;
                            phi2 = -asin ( x_vector [2] );

                            if ( ( ( z_vector [2] < 0.0 ) ? -z_vector [2] : z_vector [2] ) > V_EPS ) /*E06*/
                                phi1 = atan2 ( y_vector [2], z_vector [2] );
                            else
                            {
                                double cs = cos ( phi2 );

                                double ratio = y_vector [2] / cs;
                                if ( ratio > 0.999999 )        ratio = 1.0;
                                else if ( ratio < -0.999999 )  ratio = -1.0;

                                phi1 = asin ( ratio );
                            }
                            if ( ( fabs ( x_vector [0] ) > V_EPS ) ||
                                ( fabs ( x_vector [1] ) > V_EPS ) )
                                phi3 = atan2 ( x_vector [1], x_vector [0] );
                            else
                                phi3 = atan2 ( -y_vector [0], y_vector [1] );

                            A = 180. / M_PI * phi1;
                            B = 180. / M_PI * phi2;
                            C = 180. / M_PI * phi3;
                        }
                    }
                }

                bgeo::Coord3d Xx ( pt1 [0] - pt0 [0], pt1 [1] - pt0 [1], pt1 [2] - pt0 [2] );
                pBag->AddTyped<bgeo::Coord3d> ( pBagPropNames [sel] [tpMInfo_Delta], Xx );


                if ( s_data.m [sel].pointVec.size () > 0 )
                {
                    int ix = s_data.m [sel].specialIndex [analyseData::dai_SXP1] - 1;
                    int type = 999;
                    if ( ix >= 0 )
                        type = s_data.m [sel].typeVec [ix];

                    SetupTemporaryArcMovements ( vm_SHOWLABEL, 1 ); // checkTPAMode () == tpanalysis::cm_Movement ? type : 0 );

                    switch ( type )
                    {
                        case analyseData::dai_tUnknown: // 0 = Point
                        {
                            if ( checkTPAMode ( sel ) != tpanalysis::cm_Movement )
                                break;
                        }
                        case analyseData::dai_tLine: // 1 = line
                        {
                            bgeo::Vector3d Xxv = pt1 - pt0;
                            pBag->AddTyped<double> ( pBagPropNames [sel] [tpMInfo_Length], Xxv.Distance ( pt1, pt0 ) );
                            break;
                        }
                        case analyseData::dai_tArc: // 2 = arc // dangerous ?
                        {
                            pt2 = s_data.m [sel].points [analyseData::dai_CTR1];
                            if ( !s_data.m [sel].motion.IsIdentity () )
                            { // maybe is the inverse??
                                s_data.m [sel].motion.ApplyToPoint ( pt2 );
                            }

                            pBag->AddTyped<bgeo::Coord3d> ( pBagPropNames [sel] [tpMInfo_centerpoint], pt2 );
                            pBag->AddTyped<double> ( pBagPropNames [sel] [tpMInfo_Radius_A], s_data.m [sel].vectors [analyseData::dai_VR] [analyseData::dai_SXS] );
                            pBag->AddTyped<double> ( pBagPropNames [sel] [tpMInfo_Radius_E], s_data.m [sel].vectors [analyseData::dai_VR] [analyseData::dai_SXE] );
                            pBag->AddTyped<double> ( pBagPropNames [sel] [tpMInfo_Angle_A], s_data.m [sel].vectors [analyseData::dai_VA] [analyseData::dai_SXS] );
                            //                                pBag->AddTyped<double> ( pBagPropNames[sel][tpMInfo_Angle_C], s_data.m[sel].vectors [analyseData::dai_VA] [analyseData::dai_SXC] );
                            pBag->AddTyped<double> ( pBagPropNames [sel] [tpMInfo_Angle_E], s_data.m [sel].vectors [analyseData::dai_VA] [analyseData::dai_SXE] );

                            double dist = s_data.m [sel].vectors [analyseData::dai_VR] [analyseData::dai_SXS] * ( s_data.m [sel].vectors [analyseData::dai_VA] [analyseData::dai_SXE] - s_data.m [sel].vectors [analyseData::dai_VA] [analyseData::dai_SXS] );
                            pBag->AddTyped<double> ( pBagPropNames [sel] [tpMInfo_Length], dist );
                            break;
                        }
                        case analyseData::dai_tCurve:
                        default: // unknown
                        {
                            break;
                        }
                    }
                }
                //if ( A != B )
                //{
                //    pBag->AddTyped<double> ( pBagPropNames[sel][tpMInfo_TANGLEA], A );
                //    pBag->AddTyped<double> ( pBagPropNames[sel][tpMInfo_TANGLEB], B );
                //}
                //else
                //{
                //    pBag->AddTyped<double> ( pBagPropNames[sel][tpMInfo_TANGLEA], A );
                //}
                //pBag->AddTyped<double> ( pBagPropNames[sel][tpMInfo_TANGLEC], C );

            }

            void TPAnalysisModel::SetupPropertyBagFor_P_P ( SharedPtr<propertyservice::PropertyBag> pBag )
            {
                bgeo::Point3d pt0;
                bgeo::Point3d pt1;

                pt0 = s_data.m [L_SEL].points [analyseData::dai_SXPC];
                pt1 = s_data.m [R_SEL].points [analyseData::dai_SXPC];

                if ( !s_data.m [L_SEL].motion.IsIdentity () )
                { // maybe is the inverse??
                    s_data.m [L_SEL].motion.ApplyToPoint ( pt0 );
                    s_data.m [L_SEL].motion.ApplyToPoint ( pt1 );
                }

                bgeo::Vector3d v ( pt1 - pt0 );

                pBag->AddTyped<bgeo::Point3d> ( pBagPropNames [L_SEL] [tpMInfo_MD_P1], pt0 );
                pBag->AddTyped<bgeo::Point3d> ( pBagPropNames [R_SEL] [tpMInfo_MD_P2], pt1 );

                pBag->AddTyped<Double> ( pBagPropNames [L_SEL] [tpMInfo_MD_DX], v [0] );
                pBag->AddTyped<Double> ( pBagPropNames [L_SEL] [tpMInfo_MD_DY], v [1] );
                pBag->AddTyped<Double> ( pBagPropNames [L_SEL] [tpMInfo_MD_DZ], v [2] );

                pBag->AddTyped<Double> ( pBagPropNames [L_SEL] [tpMInfo_MD_DD], v.Norm () );
            }

            void TPAnalysisModel::SetupPropertyBagFor_P_O ( SharedPtr<propertyservice::PropertyBag> pBag )
            {
                int offset = 0;
                // missing offset check

                for ( int sel = L_SEL; sel < ALL_SEL; sel++ )
                {
                    if ( s_data.m [sel].specialIndex [analyseData::dai_SXP1] > UNKNOWNINDEX )
                    {
                        int moveNr = -1;
                        if ( s_data.m [sel].m_pAttr )
                        {
                            ToolpathData* ptpData = GetToolpathData ( s_data.m [sel].m_pAttr );
                            moveNr = ptpData->MovementId ( s_data.m [sel].specialIndex [analyseData::dai_SXP1] );
                        }
                        else
                        {
                            if ( s_data.m [sel].specialIndex [analyseData::dai_Index1stTpSeg] >= 0 && s_data.m [sel].specialIndex [analyseData::dai_Index1stTpSeg] < s_data.m [sel].segmentSizeVec.size () )
                                moveNr = s_data.m [sel].specialIndex [analyseData::dai_SXP1] + s_data.m [sel].specialIndex [analyseData::dai_Index1stTpSeg] + offset;
                            else
                            {
                                moveNr = s_data.m [sel].specialIndex [analyseData::dai_SXP1] + offset;
                            }

                        }
                        if ( moveNr > -1 )
                        {
                            pBag->AddTyped<int> ( pBagPropNames [sel] [tpMInfo_MD_MOVENR], moveNr );
                        }
                    }
                }

                // additional info like curve U/V entId or MovementNr
            }

        }  // namespace tp_analyzer
    }  // namespace commands
}  // namespace omsl
