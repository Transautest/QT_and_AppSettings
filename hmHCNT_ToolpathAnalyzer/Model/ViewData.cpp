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
#include <geom/line3d.h>
#include <geom/arc3d.h>
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

namespace omsl
{
    namespace commands
    {
        namespace tp_analyzer
        {
            hcNTLib::HCNTToolTipConnection m_tt_cnct;

            int TPAnalysisModel::RemoveExtrema ( hcntcore::CadEntity* entity )
            {
                CadEntityPtr cent ( entity );

                ToolpathPtr pToolpath = cent.shared_cast < omsl::hcntcore::ToolPathEntity > ( L"ToolPathEntity" );
                
                bool ccp_exist = false;

                if ( pToolpath.Get () ) // selected an Toolpath
                {
                    CadEntityForwardPtr it ( pToolpath->GetCadEntities () );
                    ToolpathData* ptpData = nullptr;
                    OMSL_FOREACH ( it ) // all segments of a TP
                    {
                        CadEntityPtr cep ( it->Get () );
                        ToolpathSegmentPtr pSegment ( CastToToolpathSegment ( cep, U_SEL ) );
                        if ( !pSegment.Get () )
                            continue;

                        ptpData = GetToolpathData ( pSegment );

                        // error
                        if ( !ptpData )
                            continue;

                        int t1 = ptpData->GetInternalTpSubType(eCt_tp);
                        int t2 = ptpData->GetInternalTpSubType(eCt_ccp);
                        if (t1 != -1 && t2 != -2) // different
                        {
                            ccp_exist = true;
                        }

                        extremaMap::iterator it1;
                        it1 = s_data.m_extrema.find ( ptpData->JobUUID () );
                        if ( it1 != s_data.m_extrema.end () )
                        {
                            s_data.m_extrema.erase ( ptpData->JobUUID () );
                        }
                    }

                    size_t s = 0;
                    extremaMap::const_iterator cit = s_data.m_extrema.begin ();

                    while ( cit != s_data.m_extrema.end () )
                    {
                        if ( ( *cit ).second.tpType == eCt_tp || ( *cit ).second.tpType == eCt_ccp )
                        {
                            s++;
                        }

                        cit++;
                    }

                    if ( s < 3 ) // 2 = on/off, 1 = on, 0 = off
                    {
                        bool show_OffsetPath = GetProperties()->GetBag()->Exists(properties::key::ShowBallMillSpecPath);

                        if ( ccp_exist && show_OffsetPath )
                        {
                            ActivateSlider(s == 1||s == 2);
                        }
                        else
                            ActivateSlider(s == 1);
                    }
                    else
                        ActivateSlider(s == 1);
                }
                return 0;
            }
            
            bool TPAnalysisModel::GetExtremaFromMap ( std::vector<double>& vecOfValues, double& smin, double& smax, double& fmin, double& fmax, double& cmax, int& segMax )
            {
                bool withCMax = false;
                smin = 100000.0;
                smax = -100000.0;
                fmin = 100000.0;
                fmax = -100000.0;
                cmax = -100000.0;
                double limit = 0.0;
                bool showLayer25 = GetProperties ()->GetBag ()->Exists (properties::key::ShowLayer25) && GetProperties ()->GetBag ()->GetValue<omsl::Bool> ( properties::key::ShowLayer25 );
                //int usedTypes=0;                
                
                std::map<double,int> speedMap;
                // additional check for auto level necessary for getting the original speedmaps

                if ( s_data.m_extrema.size () )
                {
                    segMax = 0;
                    extremaMap::const_iterator cit;

                    int autoLvl = showLayer25 ? 0 : GetValue ( tpanalysis::UI_AutoUpdateLvl );
                    if ( !autoLvl )
                    {
                        //if ( ( *cit ).second.smax > 1000 )
                        //    usedTypes |= 0x0001;
                        //else
                        //    usedTypes |= 0x0002;

                        // loopTPMap
                        ToolPathPtrgraphDispValMap::const_iterator cit1;
                        for ( cit1 = s_data.pToolpathMap.cbegin (); cit1 != s_data.pToolpathMap.cend (); cit1++ )
                        {
                            CadEntityForwardPtr it ( (*cit1).first->GetCadEntities () );

                            OMSL_FOREACH ( it ) // all segments of a TP
                            {
                                CadEntityPtr cep ( it->Get () );
                                ToolpathSegmentPtr pSegment ( CastToToolpathSegment ( cep, U_SEL ) );
                                if ( !pSegment.Get () )
                                    continue;

                                ToolpathData* ptpData = GetToolpathData ( pSegment );

                                // error
                                if ( !ptpData )
                                    continue;
                                double ccmax;
                                if ( ptpData->FillValueToColourMap( s_ftype, speedMap, ccmax, GetValue ( tpanalysis::UI_ClearanceLevel ) != 0) )
//                                if ( ptpData->FillFeedrateMap ( speedMap, GetValue ( tpanalysis::UI_ClearanceLevel ) != 0 ) )
                                {
                                    if ( GetValue ( tpanalysis::UI_ClearanceLevel ) != 0 )
                                    {
                                        speedMap [ccmax] = speedMap.begin ()->second;
                                    }
                                    withCMax = true;
                                }
                            }
                        }

                        smin = speedMap.begin ()->first;
                        fmin = speedMap.begin ()->first;
                        smax = speedMap.rbegin ()->first;

                        for ( cit = s_data.m_extrema.begin (); cit != s_data.m_extrema.end (); cit++ )
                        {
                            if ( ( *cit ).second.segSize > segMax ) segMax = ( *cit ).second.segSize;
                            if ( ( *cit ).second.fmax > fmax ) fmax = ( *cit ).second.fmax;
                            if ( ( *cit ).second.withCMax )
                            {
                                withCMax = true;
                                if ( ( *cit ).second.cmax > cmax ) cmax = ( *cit ).second.cmax;
                            }
                        }
                    }
                    else
                    {
                        std::map<double, int>::const_reverse_iterator rit;
                        for ( rit = speedMap.rbegin (); rit != speedMap.rend (); rit++ )
                        {
                            if ( ( *rit ).second == 1 )
                            {
                                limit = ( *rit ).first;
                                break;
                            }
                        }

                        SetValue ( tpanalysis::numberOfDifferentFeedrates, GetValue ( tpanalysis::UI_feedrateLevels ) );
                        for ( cit = s_data.m_extrema.begin (); cit != s_data.m_extrema.end (); cit++ )
                        {
                            if ( ( *cit ).second.segSize > segMax ) segMax = ( *cit ).second.segSize;
                            if ( ( *cit ).second.smin < smin ) smin = ( *cit ).second.smin;
                            if ( ( *cit ).second.fmin < fmin ) fmin = ( *cit ).second.fmin;
                            if ( ( *cit ).second.smax > smax ) smax = ( *cit ).second.smax;
                            if ( ( *cit ).second.fmax > fmax ) fmax = ( *cit ).second.fmax;
                            if ( ( *cit ).second.withCMax )
                            {
                                withCMax = true;
                                if ( ( *cit ).second.cmax > cmax ) cmax = ( *cit ).second.cmax;
                            }
                            if ( ( *cit ).second.feedUnit == 1 )
                            {
                                limit = ( *cit ).second.fmax;
                            }
                        }
                    }

                    if ( segMax == 0 )
                        segMax = DEFAULTSEGMAX;

                    if ( autoLvl == 0 ) // use different 
                    {
                        // limit used for differ between turning / milling
                        std::map<double, int>::const_reverse_iterator rit;
                        bool found = false;
                        for ( rit = speedMap.rbegin (); rit != speedMap.rend (); rit++ )
                        {
                            if ( !found && ( *rit ).second == 1 ) // last speed used by turning job
                            {
                                limit = ( *rit ).first;
                                found = true;
                            }
                            {
                                vecOfValues.push_back ( ( *rit ).first );
                            }
                        }
                    }

                    if ( s_ftype )
                        SetValue ( tpanalysis::clrTblRoundingPrec, 9 );
                    else
                        SetValue ( tpanalysis::clrTblRoundingPrec, 3 );

                    SetValue ( tpanalysis::clrTblRoundingLimit, limit );

                }
                else
                {
                    smin = DEFAULTSPEEDMIN;
                    smax = DEFAULTSPEEDMAX;
                    fmin = DEFAULTSPEEDMIN;
                    fmax = DEFAULTSPEEDMAX;
                    cmax = DEFAULTSPEEDMAX;
                    segMax = DEFAULTSEGMAX;
                    SetValue ( tpanalysis::numberOfDifferentFeedrates, GetValue (tpanalysis::UI_feedrateLevels));
                }

                if ( showLayer25 )
                {
                    SetValue ( tpanalysis::UI_feedrateValueMin, fmin );
                    SetValue ( tpanalysis::UI_feedrateValueMax, fmax );
                }

                if ( withCMax == false )
                {
                    cmax = fmax;
                }

                return withCMax;
            }

            void TPAnalysisModel::SetupExtremaToUI ( const int data )
            {
                double smin = 100000.0, smax = -100000.0, fmin = 100000.0, fmax = -100000.0, cmax = -100000.0;
                bool withCMax = false;
                int segMax = 0;
                std::vector<double> vecOfValues;

                int changeCnt = 0;

                double oldlimit = GetValue ( tpanalysis::clrTblRoundingLimit );
                double limit;

                withCMax = GetExtremaFromMap ( vecOfValues, smin, smax, fmin, fmax, cmax, segMax );

                int old_vecsize = GetValue ( tpanalysis::numberOfDifferentFeedrates );
                if ( old_vecsize != vecOfValues.size () )
                    changeCnt++;

                limit = GetValue ( tpanalysis::clrTblRoundingLimit );

                if ( oldlimit != limit )
                    changeCnt++;

                int userPos = GetValue ( tpanalysis::UI_sliderValueMax );
                int oldMax = GetValue ( tpanalysis::UI_sliderMax_2 );

                SetValue ( tpanalysis::UI_sliderMax_2, segMax );

                if ( userPos == oldMax )
                    SetValue ( tpanalysis::UI_sliderValueMax, segMax );

                SetValue ( tpanalysis::updateColorsTableAllowed, 0 );


                // autoUpdateLvl changed
                // necessary because of switch between exact and range settings
                if ( (data & 0x1000) )
                    changeCnt = 1;

                int autoMin = GetValue ( tpanalysis::UI_AutoUpdateMin );
                int autoMax = GetValue ( tpanalysis::UI_AutoUpdateMax );
                //int autoLvl = GetValue ( tpanalysis::UI_AutoUpdateLvl );

                double oldval = GetValue ( tpanalysis::UI_colorMapMin_2 );
                if ( oldval != fmin )
                {
                    SetValue ( tpanalysis::UI_colorMapMin_2, fmin );
                    changeCnt++;
                }

                oldval = GetValue ( tpanalysis::UI_colorMapMax_2 );
                if ( oldval != fmax )
                {
                    SetValue ( tpanalysis::UI_colorMapMax_2, fmax );
                    changeCnt++;
                }


                oldval = GetValue ( tpanalysis::UI_colorMapCMax_3 );
                if ( withCMax )
                {
                    if ( oldval != cmax )
                    {
                        SetValue ( tpanalysis::UI_colorMapCMax_3, cmax );
                        changeCnt++;
                    }
                }
                else
                {
                    if ( oldval != 0.0 )
                    {
                        SetValue ( tpanalysis::UI_colorMapCMax_3, 0.0 );
                        changeCnt++;
                    }
                }

                if ( !autoMin )
                {
                    oldval = GetValue ( tpanalysis::UI_feedrateValueMin );
                    if ( oldval != fmin )
                    {
                        SetValue ( tpanalysis::UI_feedrateValueMin, fmin );
                        changeCnt++;
                    }
                }

                if ( !autoMax )
                {
                    oldval = GetValue ( tpanalysis::UI_feedrateValueMax );

                    if ( withCMax && GetValue ( tpanalysis::UI_ClearanceLevel) != 0 )
                    {
                        if ( oldval != cmax )
                        {
                            SetValue ( tpanalysis::UI_feedrateValueMax, cmax );
                            changeCnt++;
                        }
                    }
                    else
                    {
                        if ( oldval != fmax )
                        {
                            SetValue ( tpanalysis::UI_feedrateValueMax, fmax );
                            changeCnt++;
                        }
                    }
                }
                // check autolevel missing
                // enable update for calculated update
                if ( changeCnt > 0 )
                {
                    SetValue ( tpanalysis::numberOfDifferentFeedrates, ( int ) vecOfValues.size () );
                    SetValue ( tpanalysis::doubleSpeedVec, vecOfValues );

                    SetValue ( tpanalysis::updateColorsTableAllowed, 2 );
                    OnUpdateFeedrateDraw ( -55 );
                }
                else
                    SetValue ( tpanalysis::updateColorsTableAllowed, 1 );
            }

            void TPAnalysisModel::AddExtrema ( omsl::hcntcore::CadEntity* entity )
            {
                CadEntityPtr cent ( entity );

                ToolpathPtr pToolpath = cent.shared_cast < omsl::hcntcore::ToolPathEntity > ( L"ToolPathEntity" );

                if ( pToolpath.Get () ) // selected an Toolpath
                {
                    double smin = 100000.0, smax = -100000.0, fmin = 100000.0, fmax = -100000.0, cmax = -100000.0;

                    int nSpeed, nFeed, nOffset; // , speedUnit, feedUnit;
                    CadEntityForwardPtr it ( pToolpath->GetCadEntities () );
                    extremaData extrem;
                    extrem.smin = 100000.0;
                    extrem.smax = -100000.0;
                    extrem.fmin = 100000.0;
                    extrem.fmax = -100000.0;
                    extrem.cmax = -100000.0;
                    extrem.withCMax = false;
                    extrem.segSize = 0;
                    extrem.speedUnit = 0;
                    extrem.feedUnit = 0;
                    extrem.tpType = 0;
                    ToolpathData* ptpData = nullptr;

                    bool ccp_exist = false;

                    OMSL_FOREACH ( it ) // all segments of a TP
                    {
                        CadEntityPtr cep ( it->Get () );
                        ToolpathSegmentPtr pSegment ( CastToToolpathSegment ( cep, U_SEL ) );

                        if ( !pSegment.Get () )
                            continue;

                        extrem.segSize += pSegment->Size ();

                        ptpData = GetToolpathData ( pSegment );

                        // error
                        if ( !ptpData )
                            continue;

                        bool withCMax = ptpData->GetExtrema (s_ftype, smin, smax, fmin, fmax, cmax, nSpeed, nFeed, extrem.speedUnit, extrem.feedUnit, nOffset );
                        extrem.segSize += nOffset;
                        extrem.tpType = ( eCtTpType ) ptpData->GetInternalTpType ();


                        int t1 = ptpData->GetInternalTpSubType(eCt_tp);
                        int t2 = ptpData->GetInternalTpSubType(eCt_ccp);

                        if (t1 != -1 && t2 != -2) // different
                        {
                            ccp_exist = true;
                        }

                        if ( smin < extrem.smin ) extrem.smin = smin;
                        if ( fmin < extrem.fmin ) extrem.fmin = fmin;
                        if ( smax > extrem.smax ) extrem.smax = smax;
                        if ( fmax > extrem.fmax ) extrem.fmax = fmax;
                        if ( withCMax )
                        {
                            extrem.withCMax = true;
                            if ( cmax > extrem.cmax ) extrem.cmax = cmax;
                        }
                    }
                    // !!!! not safe
                    if ( ptpData )
                    {
                        std::wstring a ( ptpData->JobUUID () );
                        s_data.m_extrema [a] = extrem;
                        ToolPathPtrgraphDispValMap::iterator it1 = s_data.pToolpathMap.find ( pToolpath );
                        if (it1 != s_data.pToolpathMap.end())
                        {
                            (*it1).second.tptypeMain = (int)extrem.tpType;
                        }
                        size_t s = 0;
                        extremaMap::const_iterator cit = s_data.m_extrema.begin ();

                        while ( cit != s_data.m_extrema.end () )
                        {
                            if ( ( *cit ).second.tpType == eCt_tp || ( *cit ).second.tpType == eCt_ccp )
                            {
                                s++;
                            }
                            cit++;
                        }

                        if ( s < 3 ) // 2 = off, 1 = on, 0 = off
                        {
                            bool show_OffsetPath = GetProperties()->GetBag()->Exists(properties::key::ShowBallMillSpecPath);

                            if (ccp_exist && show_OffsetPath)
                            {
                                ActivateSlider(s == 1 || s == 2);
                            }
                            else
                                ActivateSlider(s == 1);
                        }
                        else
                            ActivateSlider(s == 1);
                    }
                }
            }

            void TPAnalysisModel::ReplaceExtrema ( omsl::hcntcore::CadEntity* entity )
            {
                CadEntityPtr cent ( entity );

                ToolpathPtr pToolpath = cent.shared_cast < omsl::hcntcore::ToolPathEntity > ( L"ToolPathEntity" );

                if ( pToolpath.Get () ) // selected an Toolpath
                {
                    double smin = 100000.0, smax = -100000.0, fmin = 100000.0, fmax = -100000.0, cmax = -100000.0;

                    int nSpeed=0, nFeed=0, nOffset=0; // , speedUnit, feedUnit;
                    CadEntityForwardPtr it ( pToolpath->GetCadEntities () );
                    extremaData extrem;
                    extrem.smin = 100000.0;
                    extrem.smax = -100000.0;
                    extrem.fmin = 100000.0;
                    extrem.fmax = -100000.0;
                    extrem.cmax = -100000.0;
                    extrem.withCMax = false;
                    extrem.segSize = 0;
                    extrem.speedUnit = 0;
                    extrem.feedUnit = 0;
                    extrem.tpType = 0;
                    ToolpathData* ptpData = nullptr;

                    bool ccp_exist = false;

                    OMSL_FOREACH ( it ) // all segments of a TP
                    {
                        CadEntityPtr cep ( it->Get () );
                        ToolpathSegmentPtr pSegment ( CastToToolpathSegment ( cep, U_SEL ) );
                        if ( !pSegment.Get () )
                            continue;

                        extrem.segSize += pSegment->Size ();


                        ptpData = GetToolpathData ( pSegment );
                        // error
                        if ( !ptpData )
                            continue;

                        bool withCMax = ptpData->GetExtrema ( s_ftype, smin, smax, fmin, fmax, cmax, nSpeed, nFeed, extrem.speedUnit, extrem.feedUnit, nOffset );
                        extrem.segSize += nOffset;
                        extrem.tpType = ( eCtTpType ) ptpData->GetInternalTpType ();

                        if ( smin < extrem.smin ) extrem.smin = smin;
                        if ( fmin < extrem.fmin ) extrem.fmin = fmin;
                        if ( smax > extrem.smax ) extrem.smax = smax;
                        if ( fmax > extrem.fmax ) extrem.fmax = fmax;
                        if ( withCMax )
                        {
                            extrem.withCMax = true;
                            if ( cmax > extrem.cmax ) extrem.cmax = cmax;
                        }
                    }

                    if ( ptpData )
                    {
                        ToolPathPtrgraphDispValMap::iterator it3 = s_data.pToolpathMap.find ( pToolpath );
                        if ( it3 != s_data.pToolpathMap.end () )
                        {
                             ( *it3 ).second.tptypeMain = ptpData->GetInternalTpType ();
                             (*it3).second.tptype1 = ptpData->GetInternalTpSubType (eCt_tp);
                             (*it3).second.tptype2 = ptpData->GetInternalTpSubType (eCt_ccp);
                        }

                        std::wstring a ( ptpData->JobUUID () );

                        extremaMap::iterator it2;
                        it2 = s_data.m_extrema.find ( ptpData->JobUUID () );
                        if ( it2 == s_data.m_extrema.end () )
                        {
                            s_data.m_extrema [a] = extrem;
                            it2 = s_data.m_extrema.find ( ptpData->JobUUID () );
                        }
                        else
                        {
                            ( *it2 ).second.smin = extrem.smin;
                            ( *it2 ).second.smax = extrem.smax;
                            ( *it2 ).second.fmin = extrem.fmin;
                            ( *it2 ).second.fmax = extrem.fmax;
                            ( *it2 ).second.cmax = extrem.cmax;
                            ( *it2 ).second.withCMax = extrem.withCMax;
                            if ( ( *it2 ).second.segSize  < extrem.segSize )
                                ( *it2 ).second.segSize  = extrem.segSize;
                            ( *it2 ).second.speedUnit = extrem.speedUnit;
                            ( *it2 ).second.feedUnit  = extrem.feedUnit;
                            ( *it2 ).second.tpType = extrem.tpType;
                        }

                        ToolPathPtrgraphDispValMap::iterator it1 = s_data.pToolpathMap.find ( pToolpath );
                        if (it1 != s_data.pToolpathMap.end())
                        {
                            (*it1).second.tptypeMain = (int)extrem.tpType;
                        }


                        int t1 = ptpData->GetInternalTpSubType(eCt_tp);
                        int t2 = ptpData->GetInternalTpSubType(eCt_ccp);
                        if (t1 != -1 && t2 != -2) // different
                        {
                            ccp_exist = true;
                        }

                        size_t s = 0;
                        extremaMap::const_iterator cit = s_data.m_extrema.begin ();

                        while ( cit != s_data.m_extrema.end () )
                        {
                            if ( ( *cit ).second.tpType == eCt_tp || ( *cit ).second.tpType == eCt_ccp )
                            {
                                s++;
                            }
                            cit++;
                        }

                        if ( s < 3 ) // 2 = off, 1 = on, 0 = off
                        {
                            bool show_OffsetPath = GetProperties()->GetBag()->Exists(properties::key::ShowBallMillSpecPath);

                            if (ccp_exist && show_OffsetPath)
                            {
                                ActivateSlider(s == 1 || s == 2);
                            }
                            else
                                ActivateSlider(s == 1);
                        }
                        else
                            ActivateSlider(s == 1);
                    }
                }
            }

            void TPAnalysisModel::SetupFeedRateColors ( const int viewModeParameter )
            {
                bool old = SetSignalListening ( false );
                // loopTPMap
                ToolPathPtrgraphDispValMap::const_iterator cit;
                for ( cit = s_data.pToolpathMap.cbegin (); cit != s_data.pToolpathMap.cend (); cit++ )
                {
                    SetupFeedRateColors ( viewModeParameter, ( *cit ).first.Get () );
                }

                SetSignalListening ( old );
            }

            void TPAnalysisModel::SetupFeedRateColors ( const int /*viewModeParameter*/, omsl::hcntcore::CadEntity* entity )
            {
                //bool useAutoUpdateLevels = GetValue ( tpanalysis::UI_AutoUpdateLvl ) ? false : true;

                //int myLevels = GetValue ( tpanalysis::numberOfDifferentFeedrates );

                bool useExactSpeeds = GetValue ( tpanalysis::useExactFeedrates ) ? true : false;

                CadEntityPtr cent ( entity );

                ToolpathPtr pToolpath = cent.shared_cast < omsl::hcntcore::ToolPathEntity > ( L"ToolPathEntity" );

                if ( pToolpath.Get () ) // selected an Toolpath
                {

                    std::map<double, int> speedClrIndexMap;
                    std::vector<double> colorTable, speedTable;
                    std::vector<int> colorIdxVec;
                    std::map<omsl::ULong, omsl::ULong> newPt2IdMap;
                    //omsl::materials::ColorsPalette*pPalette = nullptr;


                    colorTable = GetValue ( tpanalysis::doubleClrVec );
                    colorIdxVec = GetValue ( tpanalysis::intClrIdxVec );
                    speedTable = GetValue ( tpanalysis::doubleSpeedVec );


                    CadEntityForwardPtr it ( pToolpath->GetCadEntities () );

                    OMSL_FOREACH ( it )
                    {
                        newPt2IdMap.clear ();
                        CadEntityPtr cep ( it->Get () );
                        ToolpathSegmentPtr pSegment ( CastToToolpathSegment ( cep, U_SEL ) );

                        if ( !pSegment.Get () )
                            continue;

                        ToolpathData* ptpData = GetToolpathData ( pSegment );
                        // error
                        if ( !ptpData )
                            continue;

                        size_t s11;
                        size_t sOrig = GENERAL_COLOR_SIZE;

                        std::vector<int> colorIdxVecMod;
                        std::vector<double>colorTableMod;

                        for ( s11 = 0; s11 < sOrig; s11++ )
                        {
                            colorIdxVecMod.push_back ( (int) s11 );
                        }

                        int s3 = (int) colorIdxVec.size ();
                        for ( s11 = 0; s11 < s3 ; s11++ )
                        {
                            colorIdxVecMod.push_back ( colorIdxVec [s11] );// + sOrig
                        }

                        speedClrIndexMap.clear ();
                        omsl::Std_Double_Vector::const_iterator crit = speedTable.cbegin ();
                        int maxsize = (int)colorIdxVecMod.size ();
                        for ( int i = 0; crit != speedTable.cend (); i++ )
                        {
                            if ( i + sOrig < maxsize )
                                speedClrIndexMap [( *crit )] = colorIdxVecMod [i + sOrig];
                            else
                                speedClrIndexMap [( *crit )] = colorIdxVecMod [0];

                            crit++;
                        }

                        // add white as default out of limits color
                        int outOfUpperRangeColorIndex = colorIdxVecMod [colorIdxVecMod.size () - 2]; //
                        int outOfLowerRangeColorIndex = colorIdxVecMod [colorIdxVecMod.size () - 1];

                        GraphicAttributesPtr gap ( cep->GetGraphicAttributes () );
                        if ( gap.Get () )
                        {
                            ToolpathSegmentGraphicAttributesPtr tpsgaporig = gap.shared_cast< omsl::graphicattributes::ToolpathSegmentGraphicAttributes >( L"ToolpathSegmentGraphicAttributes" );

                            GraphicAttributesPtr tpsgnew = omsl::graphicattributes::OverriddenGraphicAttributesFactory ( gap.get () );
                            ToolpathSegmentGraphicAttributesPtr tpsgap = tpsgnew.shared_cast< omsl::graphicattributes::ToolpathSegmentGraphicAttributes >( L"ToolpathSegmentGraphicAttributes" );

                            if ( tpsgap.Get () )
                            {
                                hcNTLib::ColorsPalettePtr pColorPalette1 ( tpsgaporig->GetColorsPalette () );
                                s11 = pColorPalette1->Size ();

                                float r, g, b;
                                size_t i = 0;
                                for ( i = 0; i < sOrig; i++ )
                                {
                                    pColorPalette1->GetColor ( ( unsigned long ) i )->GetAll ( r, g, b );

                                    colorTableMod.push_back ( r );
                                    colorTableMod.push_back ( g );
                                    colorTableMod.push_back ( b );
                                }

                                colorTableMod.insert ( colorTableMod.end (), colorTable.begin (), colorTable.end () );
                                if ( s_ftype )
                                    useExactSpeeds = true;

                                ptpData->createPtIndexBySpeed ( s_ftype, speedClrIndexMap, newPt2IdMap, 0, outOfLowerRangeColorIndex, outOfUpperRangeColorIndex, useExactSpeeds );

                                ULULMapPtr ulptr ( new ULULMap ( newPt2IdMap ) );

                                {
//                                    size_t s1 = ulptr->Size ();
                                    size_t s2 = pSegment->Size ();
                                    size_t ixf, ixs;
                                    ulptr->MoveToBegin ();
                                    while ( !ulptr->IsAtEnd () )
                                    {
                                        ulptr->Next ();
                                        ixf = ulptr->GetFirst () ;
                                        ixs = ulptr->GetSecond ();
                                        if ( ixf >= s2 )
                                        {
                                            break;
                                        }
                                    }
                                }

                                size_t s1 = ulptr->Size ();
                                if ( s1 > 0 ) // don´t modify the pointmap if its empty
                                {

                                    hcNTLib::ColorsPalettePtr pColorPalette;
                                    hcNTLib::ColorsPaletteCreateAsMap ( pColorPalette, ( int ) colorIdxVecMod.size (), &colorTableMod.front (), &colorIdxVecMod.front () );

                                    tpsgap->SetColorsPalette ( pColorPalette.Get () );

                                    pSegment->ResetPointToIdentifierMap ();
                                    pSegment->MapPointsToIdentifiers ( ulptr.Get () );

                                    hcntcore::Model* model = ( hcntcore::Model* )m_pDocument->Cast ( L"Model" );
                                    ( void ) model->CadEntityMgr ()->RebuildCadEntityGraphic ( omsl::hcntcore::CadEntityMgr::kVisible );

                                    cep->UpdateGraphicAttributes ( tpsgnew.Get () );
                                }
                                //                                    

                                //pToolpath->UpdateGraphicAttributes ( tpsgap.Get () );
                                //hcNTLib::ToolPathEntityUpdateGraphicAttributes ( pToolpath );


                                //int stat = model->CadEntityMgr ()->RebuildCadEntityGraphic ( omsl::hcntcore::CadEntityMgr::kVisible );
                            }
                        }
                    }

                    //                        hcNTLib::ToolPathEntityUpdateGraphicAttributes ( pToolpath );

                    static unsigned long s_MYDUMPFILTER = DUMP_NCDATA_EXTREMA | DUMP_SEGMENT_GRAPHICATTRIBUTE;
                    const TCHAR *env1 = _tgetenv ( _TEXT ( "OM_TOOLPATH" ) );
                    const TCHAR *env2 = _tgetenv ( _TEXT ( "OM_TOOLPATHDUMP" ) );
                    const TCHAR *env3 = _tgetenv ( _TEXT ( "OM_TOOLPATHDUMPOUT" ) );
                    std::vector<std::wstring> out1;
                    std::vector<std::wstring> out2;
                    std::vector<std::wstring> out3;
                    if ( env2 )
                    {
                        swscanf ( env2, L"%x", &s_MYDUMPFILTER );
                        if ( env2 )
                        {
                            hcNTLib::dumpToolpath ( pToolpath, out1, s_MYDUMPFILTER, true );
                        }
                    }

                    if ( env3 )
                    {
                        hcNTLib::printDump ( env3, out1, true );
                    }
                    else
                    {
                        hcNTLib::printDump ( L"D:/temp/dumpTP.txt", out1, true );
                    }

                }
            }

            void TPAnalysisModel::SetupMotionColors ( const int viewModeParameter )
            {
                // loopTPMap
                ToolPathPtrgraphDispValMap::const_iterator cit;
                for ( cit = s_data.pToolpathMap.cbegin (); cit != s_data.pToolpathMap.cend (); cit++ )
                {
                    SetupMotionColors ( viewModeParameter, ( *cit ).first.Get () );
                }
            }

            void TPAnalysisModel::SetupMotionColors ( const int /*viewModeParameter*/, omsl::hcntcore::CadEntity* entity )
            {
                CadEntityPtr cent ( entity );

                ToolpathPtr pToolpath = cent.shared_cast < omsl::hcntcore::ToolPathEntity > ( L"ToolPathEntity" );

                if ( pToolpath.Get () ) // selected an Toolpath
                {
                    static unsigned long s_MYDUMPFILTER = DUMP_NCDATA_EXTREMA | DUMP_SEGMENT_GRAPHICATTRIBUTE;
                    const TCHAR *env1 = _tgetenv ( _TEXT ( "OM_TOOLPATH" ) );
                    const TCHAR *env2 = _tgetenv ( _TEXT ( "OM_TOOLPATHDUMP" ) );
                    const TCHAR *env3 = _tgetenv ( _TEXT ( "OM_TOOLPATHDUMPOUT" ) );
                    std::vector<std::wstring> out1;
                    std::vector<std::wstring> out2;
                    std::vector<std::wstring> out3;

                    std::map<int, int> motionClrMap;
                    //omsl::materials::ColorsPalette*pPalette = nullptr;

                    CadEntityForwardPtr it ( pToolpath->GetCadEntities () );

                    OMSL_FOREACH ( it )
                    {
                        std::map<omsl::ULong, omsl::ULong> newPt2IdMap;
                        CadEntityPtr cep ( it->Get () );
                        ToolpathSegmentPtr pSegment ( CastToToolpathSegment ( cep, U_SEL ) );

                        if ( !pSegment.Get () )
                            continue;


                        ToolpathData* ptpData = GetToolpathData ( pSegment );
                        // error
                        if ( !ptpData )
                            continue;

                        double smin, smax, fmin, fmax, cmax;
                        int nSpeed, nFeed, speedUnit, feedUnit, ptOffset;
                        /*bool withCMax = */ (void) ptpData->GetExtrema (s_ftype, smin, smax, fmin, fmax, cmax, nSpeed, nFeed, speedUnit, feedUnit, ptOffset );

                        switch ( ptpData->GetInternalTpType () )
                        {
                            case eCtTpType::eCt_clp : // ect_clp
                            {
                                motionClrMap [-1] = 0; // unknown
                                motionClrMap [0] = 2; // G0
                                motionClrMap [1] = 2; // G1

                                {
                                    motionClrMap [2] = 2; // G2
                                    motionClrMap [3] = 2; // G3
                                }
                                break;
                            }
                            case eCtTpType::eCt_ccp: // using the NC path palette
                            {
                                motionClrMap [-1] = 0; // unknown
                                motionClrMap [0] = 0; // G0
                                motionClrMap [1] = 1; // G1

                                {
                                    motionClrMap [2] = 1; // G2
                                    motionClrMap [3] = 1; // G3
                                }
                                break;
                            }
                            default:
                            {
                                motionClrMap [-1] = 0; // unknown
                                motionClrMap [0] = 0; // G0
                                motionClrMap [1] = 1; // G1

                                {
                                    motionClrMap [2] = 1; // G2
                                    motionClrMap [3] = 1; // G3
                                }
                                break;
                            }
                        }

                        ptpData->createPtIndexByMotion ( motionClrMap, newPt2IdMap );
                        ULULMapPtr ulptr ( new ULULMap ( newPt2IdMap ) );
                        pSegment->ResetPointToIdentifierMap ();
                        pSegment->MapPointsToIdentifiers ( ulptr.Get () );
                    }

                    hcNTLib::ToolPathEntityUpdateGraphicAttributes ( pToolpath );

                    if ( env2 )
                    {
                        swscanf ( env2, L"%x", &s_MYDUMPFILTER );
                        if ( env2 )
                        {
                            hcNTLib::dumpToolpath ( pToolpath, out1, s_MYDUMPFILTER, true );
                        }
                    }

                    if ( env3 )
                    {
                        hcNTLib::printDump ( env3, out1, true );
                    }
                    else
                    {
                        hcNTLib::printDump ( L"D:/temp/dumpTP.txt", out1, true );
                    }
                }
            }

            int TPAnalysisModel::CheckSelection ( const CadEntitiesIt_T& pEntsIt1, const CadEntitiesIt_T& pEntsIt2 )
            {
                if ( m_withUpdate == false )
                {
                    return 0;
                }

                //int counts = 0;
                //SharedPtr<omsl::container::bgeo::VectCoord3dConcreteForward> pPntVec;
                //SharedPtr<omsl::container::bgeo::VectCoord3dConcreteForward> pPntVec1;
                //SharedPtr<omsl::container::bgeo::VectCoord3dConcreteForward> pPntVec2;

                tpanalysis::Coord3dIt_T pPoint;
                std::map<int,int> rsMap;

                //int viewModeParameter = GetValue ( tpanalysis::viewModeParameter );

                //SetValue ( tpanalysis::TGr_PreviewPoints_Ent1, pPoint );
                //SetValue ( tpanalysis::TGr_PreviewPoints_Ent2, pPoint );

                bool old = SetSignalListening ( false );

                for ( int sel = MASTER_TP ; sel < ALL_SEL; sel++ )
                {  // correct indizes in case of changing the tp selection
                    int u = s_data.m[sel].specialIndex [analyseData::dai_Index1stTpEnt];
                    if ( sel == 0 )
                    {
                        SetSpecialIndex ( pEntsIt1, analyseData::dai_Index1stTpSeg, analyseData::dai_Index1stTpEnt, analyseData::dai_SXP1, sel );
                        SetSpecialIndex ( pEntsIt1, analyseData::dai_Index2ndTpSeg, analyseData::dai_Index2ndTpEnt, analyseData::dai_SXP2, sel );
                    }
                    else
                    {
                        SetSpecialIndex ( pEntsIt2, analyseData::dai_Index1stTpSeg, analyseData::dai_Index1stTpEnt, analyseData::dai_SXP1, sel );
                        SetSpecialIndex ( pEntsIt2, analyseData::dai_Index2ndTpSeg, analyseData::dai_Index2ndTpEnt, analyseData::dai_SXP2, sel );
                    }

                    if ( u !=  s_data.m[sel].specialIndex [analyseData::dai_Index1stTpEnt] && s_data.pSelectedTP[s_data.m[sel].specialIndex [analyseData::dai_Index1stTpEnt]].Get () )
                    {

                        ToolpathPtr pToolpath;

                        if ( s_data.m[sel].specialIndex [analyseData::dai_Index1stTpEnt] > UNKNOWNINDEX )
                        {
                            pToolpath = s_data.pSelectedTP [s_data.m[sel].specialIndex [analyseData::dai_Index1stTpEnt]].shared_cast< omsl::hcntcore::ToolPathEntity >( L"ToolPathEntity" );
                        }

                        if ( pToolpath.Get () )
                        {
                            CadEntitiesIt_T ents ( pToolpath->GetCadEntities () );
                            ToolpathSegmentPtr pToolpathSegment;

                            if ( !ents.Get () )
                            {
                                SetSignalListening ( old );
                                return 0;
                            }
                            s_data.m[sel].segmentSizeVec.clear ();
                            size_t s = 0;

                            for ( ents->MoveToBegin (); !ents->IsAtEnd (); ents->Next () )
                            {
                                s_data.m[sel].segmentSizeVec.push_back ( (int)s );

                                omsl::SharedPtr<omsl::hcntcore::CadEntity> cep ( ents->Get () );
                                pToolpathSegment = CastToToolpathSegment ( cep, sel );
                                if ( !pToolpathSegment.Get () )
                                    continue;

                                s += pToolpathSegment->Size ();
                            }

                            s_data.m[sel].segmentSizeVec.push_back ( (int) s );
                        }
                    }
                }

                SetSignalListening ( old );

                return 0;
            }

            int TPAnalysisModel::SetSpecialIndex ( const CadEntitiesIt_T& pEntsIt, const int segindex, const int tpindex, const int /*ptindex*/, int sel )
            {
                int idxtp = 0;
                int idxseg = 0;
                int arraypos = tpindex - analyseData::dai_Index1stTpEnt; 

                if ( arraypos > 1 || arraypos < 0 )
                    return -9999;

                s_data.m[sel].specialIndex [segindex] = UNKNOWNINDEX;
                s_data.m[sel].specialIndex [arraypos] = UNKNOWNINDEX;

                if ( !pEntsIt || !pEntsIt.Get () || pEntsIt->IsAtEnd () )
                {
                    s_data.m[sel].specialIndex [analyseData::dai_SXP1]= UNKNOWNINDEX;
                    s_data.m[sel].specialIndex [arraypos] = UNKNOWNINDEX;
                    return -1;
                }

                pEntsIt->MoveToBegin ();

                if ( pEntsIt->IsAtEnd () )
                {
                    return -3;
                }

                idxtp = 0;

                // find index of toolpathentity
                CadEntityPtr  cent0 ( pEntsIt->Get () );
                
                s_data.m [sel].m_origCadEnt = cent0;

                cent0 = GetReferencedToolpath ( cent0 );


                if ( cent0.Get () )
                {
                     ToolpathPtr pTPEnt = cent0.shared_cast< omsl::hcntcore::ToolPathEntity >( L"ToolPathEntity" );

                     s_data.pSelectedTP [arraypos] = pTPEnt;
                     s_data.m[sel].specialIndex [tpindex] = arraypos;
                     const omsl::hcntcore::CompositeEntity* pCompEnt = ( omsl::hcntcore::CompositeEntity* ) pTPEnt->Cast( L"CompositeEntity" );
                     if ( pCompEnt )
                     {
                         ToolpathSegmentPtr p_segment;
                         SharedPtr<hcntcore::CadEntity> pE ( pEntsIt->Get () ); // get first pargeom of pointselection
                         p_segment = CastToToolpathSegment ( pE, sel );

                         // try to find the segment inside the list 
                         idxseg = 0;
                         CadEntitiesIt_T depEnts_It ( pCompEnt->GetCadEntities () ); // get chields from current TP
                         for ( OMSL_EACH ( depEnts_It ) )                            // search for the TPSeg to get the segment index
                         {
                             SharedPtr<hcntcore::CadEntity> pE2 ( depEnts_It->Get () );
                             ToolpathSegmentPtr p_segment2 ( CastToToolpathSegment ( pE2, sel ) );

                             // store the segmentindex
                             if ( p_segment2.Get () == p_segment.Get () )
                             {
                                 s_data.m[sel].specialIndex [segindex] = idxseg;

                                 return idxseg;
                             }

                             idxseg++;
                         }

                         return idxseg;
                     }
                }

                s_data.m[sel].specialIndex [tpindex] = idxtp;

                return -4;
            }

            omsl::geom::ToolpathSegment* TPAnalysisModel::CastToToolpathSegment ( const SharedPtr<hcntcore::CadEntity>& pEntsIn, int sel )
            {
                if ( sel != U_SEL )
                {
                    if ( s_data.m [sel].m_pTpSegCast.Get () != nullptr && s_data.m [sel].m_pTpSegCadEnt.Get () == pEntsIn.Get () )
                    {
                        return s_data.m [sel].m_pTpSegCast.Get ();
                    }

                    s_data.m [sel].m_pTpSegCadEnt = pEntsIn.Get ();
                    s_data.m [sel].m_pTpSegCast = nullptr;

                    SharedPtr<hcntcore::CadEntity> pEnts ( pEntsIn.Get () );
                    GeomEntityPtr gentp = pEnts.shared_cast< omsl::hcntcore::GeomEntity >( L"GeomEntity" );
                    if ( gentp.Get () )
                    {
                        Geom3dPtr g3dp ( gentp->Geom () );
                        if ( g3dp.Get () )
                            s_data.m [sel].m_pTpSegCast = g3dp.shared_cast< omsl::geom::ToolpathSegment >( L"ToolpathSegment" );
                    }

                    if ( s_data.m [sel].m_pTpSegCast.Get () == nullptr )
                    {
                        SharedPtr<omsl::hcntcore::ParGeomEntity> pParGeom = pEnts.shared_cast< omsl::hcntcore::ParGeomEntity >( L"ParGeomEntity" );
                        if ( pParGeom.Get () )
                        {
                            omsl::SharedPtr<omsl::hcntcore::pargeom::Geom> pGeom ( pParGeom->GetParGeom () );

                            omsl::SharedPtr<omsl::hcntcore::pargeom::PolylineIthPoint> pgplipp ( pGeom.shared_cast< omsl::hcntcore::pargeom::PolylineIthPoint >( L"PolylineIthPoint" ) );
                            if ( pgplipp.Get () )
                            {
                                s_data.m [sel].m_pParGeomCast = pgplipp->GetRefEntity ();
                                if ( s_data.m [sel].m_pParGeomCast.Get () )
                                {
                                    GeomEntityPtr gentp2 = s_data.m [sel].m_pParGeomCast.shared_cast< omsl::hcntcore::GeomEntity >( L"GeomEntity" );
                                    if ( gentp2.Get () )
                                    {
                                        Geom3dPtr g3dp2 ( gentp2->Geom () );
                                        if ( g3dp2.Get () )
                                        {
                                            s_data.m [sel].m_pTpSegCast = g3dp2.shared_cast< omsl::geom::ToolpathSegment >( L"ToolpathSegment" );
                                        }
                                    }
                                }
                            }
                            else
                            {
                                ParGeomTpMovementPtr pTpMove = pGeom.shared_cast< omsl::hcntcore::pargeom::ToolpathMovement >( L"ToolpathMovement" );
                                if ( pTpMove.Get () )
                                    s_data.m [sel].m_pTpSegCadEnt = pTpMove->GetRefEntity ();
                                pEnts = s_data.m [sel].m_pTpSegCadEnt;
                                pEnts = GetReferencedToolpathSegmentEntity ( s_data.m [sel].m_pTpSegCadEnt );
                                s_data.m [sel].m_pTpSegCast = GetReferencedToolpathSegment ( pEnts );
                            }
                        }
                    }

                    return s_data.m [sel].m_pTpSegCast.Get ();
                }
                else // only cast
                {
                    hcNTLib::ToolpathSegmentPtr pTpSegCast;
                    hcNTLib::CadEntityPtr pTpSegCadEnt;
                    hcNTLib::CadEntityPtr pParGeomCast;

                    pTpSegCadEnt = pEntsIn.Get ();
                    pTpSegCast = nullptr;

                    SharedPtr<hcntcore::CadEntity> pEnts ( pEntsIn.Get () );
                    GeomEntityPtr gentp = pEnts.shared_cast< omsl::hcntcore::GeomEntity >( L"GeomEntity" );
                    if ( gentp.Get () )
                    {
                        Geom3dPtr g3dp ( gentp->Geom () );
                        if ( g3dp.Get () )
                            pTpSegCast = g3dp.shared_cast< omsl::geom::ToolpathSegment >( L"ToolpathSegment" );
                    }

                    if ( pTpSegCast.Get () == nullptr )
                    {
                        SharedPtr<omsl::hcntcore::ParGeomEntity> pParGeom = pEnts.shared_cast< omsl::hcntcore::ParGeomEntity >( L"ParGeomEntity" );
                        if ( pParGeom.Get () )
                        {
                            omsl::SharedPtr<omsl::hcntcore::pargeom::Geom> pGeom ( pParGeom->GetParGeom () );

                            omsl::SharedPtr<omsl::hcntcore::pargeom::PolylineIthPoint> pgplipp ( pGeom.shared_cast< omsl::hcntcore::pargeom::PolylineIthPoint >( L"PolylineIthPoint" ) );
                            if ( pgplipp.Get () )
                            {
                                pParGeomCast = pgplipp->GetRefEntity ();
                                if ( pParGeomCast.Get () )
                                {
                                    GeomEntityPtr gentp2 = pParGeomCast.shared_cast< omsl::hcntcore::GeomEntity >( L"GeomEntity" );
                                    if ( gentp2.Get () )
                                    {
                                        Geom3dPtr g3dp2 ( gentp2->Geom () );
                                        if ( g3dp2.Get () )
                                        {
                                            pTpSegCast = g3dp2.shared_cast< omsl::geom::ToolpathSegment >( L"ToolpathSegment" );
                                        }
                                    }
                                }
                            }
                            else
                            {
                                ParGeomTpMovementPtr pTpMove = pGeom.shared_cast< omsl::hcntcore::pargeom::ToolpathMovement >( L"ToolpathMovement" );
                                if ( pTpMove.Get () )
                                    pTpSegCadEnt = pTpMove->GetRefEntity ();
                                pEnts = pTpSegCadEnt;
                                pEnts = GetReferencedToolpathSegmentEntity ( pTpSegCadEnt );
                                pTpSegCast = GetReferencedToolpathSegment ( pEnts );
                            }
                        }
                    }

                    return pTpSegCast.Get ();

                }
            }

            bool TPAnalysisModel::CheckSignalTypeForCadEntityTPA
            (
                omsl::hcntcore::CadEntity* pEnt
                , int internal_HM_type
                , int data
            )
            {
                bool res = true;

                bool showLayer25 = GetProperties ()->GetBag ()->Exists ( properties::key::ShowLayer25 ) && GetProperties ()->GetBag ()->GetValue<omsl::Bool> ( properties::key::ShowLayer25 );

                switch ( internal_HM_type )
                {
                    case omsl::hmbroadcast::bcs_TPCMShowGA : b_GnIcon = -1;  return true;
                    case omsl::hmbroadcast::bcs_TPCMShowG0 : b_GnIcon =  0;  return true;
                    case omsl::hmbroadcast::bcs_TPCMShowG1 : b_GnIcon =  1;  return true;
                    case omsl::hmbroadcast::bcs_TPCMUpdate :
                    {
                        if ( pEnt == nullptr && data != -1 )
                        {
                            //m_additionalContent = data;
                            SetupAdditionalCheckboxes ( data );
                            return true;
                        }

                        break;
                    }
                    case omsl::hmbroadcast::bcs_TPCM_RunInternal :
                    {
                        if ( data == -1 )
                        {
                            m_waitForCadShell = true;
                        }
                        else
                        {
                            m_waitForCadShell = false;
                            switch ( data ) // post process
                            {
                                case omsl::hmbroadcast::bcs_TPCMPingAllTP :
                                {
                                    int vm = GetValue ( tpanalysis::UI_FeedRate ) ? vm_SHOWFEEDRATEALWAYS : 0;
                                    SetupExtremaToUI ( -999 );
                                    if ( vm&vm_SHOWFEEDRATEALWAYS )
                                    {
                                        SetupFeedRateColors ( vm );
                                    }
                                    break;
                                }
                                case omsl::hmbroadcast::bcs_TPCMShowGA : 
                                case omsl::hmbroadcast::bcs_TPCMShowG0 : 
                                case omsl::hmbroadcast::bcs_TPCMShowG1 : 
                                {
                                    int vm = GetViewModeValue ();

                                    SetupTemporaryGraphicsLabel ( vm );
//                                    SetupTemporaryGraphicsDirection ( vm );
                                    break;
                                }
                                case omsl::hmbroadcast::bcs_TPCMShow :
                                {
                                    int vm = GetValue ( tpanalysis::UI_ClearanceLevel ) ? vm_SHOWCLEARANCE : 0;

                                    SetupExtremaToUI ( vm&vm_SHOWCLEARANCE ? 0x0120 : 0x0020 );

                                    //int start = GetValue ( tpanalysis::UI_sliderValueMin );
                                    //int end = GetValue ( tpanalysis::UI_sliderValueMax );

                                    //switch ( data )
                                    //{
                                    //    case 0:
                                    //    case 1:
                                    //    {
                                    //        int err = hcNTLib::ToolPathEntitySetSegmentVisibilityById ( pToolpath, data, true, false, start, end );
                                    //        err = hcNTLib::ToolPathEntityUpdateGraphicAttributes ( pToolpath );
                                    //        break;
                                    //    }
                                    //    case -2:
                                    //    {
                                    //        if ( b_GnIcon > -1 )
                                    //        {
                                    //            int err = hcNTLib::ToolPathEntitySetSegmentVisibilityById ( pToolpath, data, true, false, start, end );
                                    //            err = hcNTLib::ToolPathEntityUpdateGraphicAttributes ( pToolpath );
                                    //            break;
                                    //        }
                                    //    }
                                    //    case -1:
                                    //    {
                                    //        int err = hcNTLib::ToolPathEntityShowPointsPartial ( pToolpath, start, end );
                                    //        err = hcNTLib::ToolPathEntityUpdateGraphicAttributes ( pToolpath );
                                    //        break;
                                    //    }
                                    //}

                                    SetupExtremaToUI ( -999 );
                                    if ( vm&vm_SHOWFEEDRATEALWAYS )
                                    {
                                        SetupFeedRateColors ( vm );
                                    }

                                    SetupTemporaryGraphicsLabel ( vm );
//                                    SetupTemporaryGraphicsDirection ( vm );
                                }
                            }
                        }
                        return true;
                    }
                }

                if ( pEnt == nullptr )
                    return true;

                // strategy 
                // first check if toolpath or toolpathegment

                // check if the entity is part of the selection
                // check if visible : true 
                // check for visible points and visible vectors

                // part of selection and invisible ( current state ) clear selection
                // part of selection : update checkboxes 

                // if not part of selection do nothing

                omsl::SharedPtr < omsl::hcntcore::CadEntity > pEntTP ( pEnt );

                if ( pEntTP.Get () == nullptr )
                    return true;

                pEntTP = GetReferencedToolpath ( pEntTP );

                if ( pEntTP.Get () == nullptr )
                    return true;

                ToolpathPtr pToolpath = pEntTP.shared_cast< omsl::hcntcore::ToolPathEntity >( L"ToolPathEntity" );
                if ( pToolpath.Get () == nullptr )
                    return true;
                
                // delete reaction missing

                switch ( internal_HM_type )
                {
                    case omsl::hmbroadcast::bcs_TPCMHide: // only in hide case the entity is known by the command
                    {
                        if ( data > -3 ) // hide master tp
                        {
                            int vm = GetViewModeValue ();

                            removeLabel ( pEnt );
//                            removeDirection ( pEnt );

                            RemoveExtrema ( pEnt );

                            SetupMotionColors ( vm&vm_SHOWFEEDRATEALWAYS, pEnt );

                            RemoveTPfromMap ( pToolpath );

                            // to avoid endless updates
                            if ( m_waitForCadShell == true )
                                break;

                            SetupExtremaToUI ( vm&vm_SHOWCLEARANCE ? 0x0110 : 0x0010 );

                            // reset selection if necessary

                            int i = 0;
                            for ( i = 0; i < 2; i++ )
                            {
                                if ( !s_data.pSelectedTP [i].Get () )
                                    continue;

                                if ( s_data.pSelectedTP [i].Get () == pToolpath.Get () )
                                {
                                    break;
                                }
                            }

                            if ( i == 2 )
                            {
                                break;
                            }

                            // clear content
                            CadEntitiesIt_T pEmptyEnts;
                            if ( i == 0 )
                                SetValue ( tpanalysis::UI_FirstEntities, pEmptyEnts );
                            else if ( i == 1 )
                                SetValue ( tpanalysis::UI_SecondEntities, pEmptyEnts );
                        }
                        else
                        {
                            removeLabel ( pEnt );
                            //removeDirection ( pEnt );

                            RemoveExtrema ( pEnt );
                        }
                        // clear selection
                        break;
                    }
                    case omsl::hmbroadcast::bcs_TPCMUpdate :
                    {
                        switch ( data )
                        {
                            case -1:
                            {
                                removeLabel ( pEnt );
                                //removeDirection ( pEnt );
                                RemoveExtrema ( pEnt );
                                break;
                            }
                        }

                        break;
                    }
                    case omsl::hmbroadcast::bcs_TPCMShow:
                    {
//                        if ( data > -3 )
                        {
                            // check if the TP contains points
                            CadEntitiesIt_T ents ( pToolpath->GetCadEntities () );
                            ToolpathSegmentPtr pSegment;
                            if ( !ents.Get () )
                                break;

                            for ( ents->MoveToBegin (); !ents->IsAtEnd (); ents->Next () )
                            {
                                omsl::SharedPtr<omsl::hcntcore::CadEntity> cep ( ents->Get () );
                                pSegment = CastToToolpathSegment ( cep, U_SEL );
                                if ( !pSegment.Get () )
                                    continue;
                                if ( pSegment->Size () > 1 )
                                    break;
                            }

                            if ( !pSegment.Get () || pSegment->Size () < 3 )
                                break;

                            if ( AddTP2Map ( pToolpath ) == false ) // existing in map
                            {
                                removeLabel ( pEnt );
                                //removeDirection ( pEnt );
                                RemoveExtrema ( pEnt );
                            }

                            ReplaceExtrema ( pEnt );

                            // to avoid endless updates
                            if ( m_waitForCadShell == true )
                                break;

                            int vm = GetViewModeValue ();

                            SetupExtremaToUI ( vm&vm_SHOWCLEARANCE ? 0x0120 : 0x0020 );

                            int start = GetValue ( tpanalysis::UI_sliderValueMin );
                            int end = GetValue ( tpanalysis::UI_sliderValueMax );

                            switch ( data )
                            {
                                case 0:
                                case 1:
                                {
                                    int err = hcNTLib::ToolPathEntitySetSegmentVisibilityById ( pToolpath, data, true, false, start, end );
                                    err = hcNTLib::ToolPathEntityUpdateGraphicAttributes ( pToolpath );
                                    break;
                                }
                                case -2:
                                case -4:
                                {
                                    if ( b_GnIcon > -1 )
                                    {
                                        // use internal value to setup G0/G1
                                        int err = hcNTLib::ToolPathEntitySetSegmentVisibilityById ( pToolpath, b_GnIcon, true, false, start, end );
                                        err = hcNTLib::ToolPathEntityUpdateGraphicAttributes ( pToolpath );
                                        break;
                                    }
                                }
                                case -3:
                                case -1:
                                {
                                    /*int err = */hcNTLib::ToolPathEntityShowPointsPartial ( pToolpath, start, end );
                                    break;
                                }
                            }

                            if ( vm&vm_SHOWFEEDRATEALWAYS )
                            {
                                SetupFeedRateColors ( vm, pEnt );
                            }
                            if ( vm&vm_SHOWLABEL )
                            {
                                removeLabel ( pEnt );
                                SetupTemporaryGraphicsLabel ( vm, pEnt );
                            }
                            if ( vm&vm_SHOWDIRECTION )
                            {
                                //removeDirection ( pEnt );
//                                SetupTemporaryGraphicsDirection ( vm, pEnt );
                            }
                        }
                        //else
                        //{
                        //    AddExtrema ( pEnt );
                        //}
                        break;
                    }
                    case omsl::hmbroadcast::bcs_TPCMPingAllTP: // simple on mode or state depending on phase
                    {
                        // check if the TP contains points
                        CadEntitiesIt_T ents ( pToolpath->GetCadEntities () );
                        ToolpathSegmentPtr pSegment;
                        if ( !ents.Get () )
                            return false;
                        for ( ents->MoveToBegin (); !ents->IsAtEnd (); ents->Next () )
                        {
                            omsl::SharedPtr<omsl::hcntcore::CadEntity> cep ( ents->Get () );
                            pSegment = CastToToolpathSegment ( cep, U_SEL );
                            if ( !pSegment.Get () )
                                continue;
                            if ( pSegment->Size () > 1 )
                                break;
                        }

                        if ( !pSegment.Get () || pSegment->Size () < 3 )
                            break;

                        AddTP2Map ( pToolpath ); // ???????
                        break;
                    }
                    case omsl::hmbroadcast::bcs_TPCM_ShowIndexToolTip :
                    {
                        if ( data < 0 ) // unknown index
                            return res;

                        if ( !m_tt_cnct.IsConnected () )
                        {
                            m_tt_cnct.Connect ();
                            m_tt_cnct.setStrategy ( hcNTLib::HCNTToolTipConnection::tts_check_Toolpath );
                        }

//                        m_tt_cnct.clear_tooltips ();
                        wchar_t s [256];
                        CadEntityPtr pEntOrig ( pEnt ); // should be something tpsegment or pargeom
                        CadEntityPtr pCadSeg ( omsl::commands::tp_analyzer::GetReferencedToolpathSegmentEntity ( pEntOrig, false ) );
                        ToolpathSegmentPtr p_segment ( GetReferencedToolpathSegment ( pCadSeg ) );
                        if ( !p_segment.Get () )
                            return res;

                        int mv = (int) p_segment->GetMovementType ( data );
                        ToolpathData* ptpData = GetToolpathData ( p_segment );
                        if ( !ptpData )
                            return res;
                        int moveNr = ptpData->MovementId ( data );

                        int offset = ptpData->G_Count () [ToolpathData::eSegmentCount];

                        if ( moveNr < 0 ) // for other layer != 31
                            moveNr = data + offset;

                        omPofTechData ncdata ( ptpData->TechData ( data ) );

                        int nAddData = ptpData->getSpecialDataSize ( data );
                        CTADDDATA a, b;
                        if ( nAddData > 0 )
                        {
                            a.data.d =  ptpData->getSpecialData ( data, 0 ).data.d;
                        }
                        if ( nAddData > 1 )
                        {
                            b.data.d =  ptpData->getSpecialData ( data, 1 ).data.d;
                        }

                        ttorderedMap::const_iterator cit;
                        int cnt = 0;
                        SharedPtr<measureunit::Converter> pConverter;
                        pConverter = omsl::measureunit::Converter::CreateGeneric ( omsl::units::Dimension::Length (), nullptr );
                        pConverter->SetDecimals ( 0 );

                        for ( cit = smap.begin (); cit != smap.end (); cit++ )
                        {
                            bool found = false;
                            if ( ( *cit ).second.s == L"NC_motion" )
                            {
                                if ( !showLayer25 )
                                {
                                    //: TPAnalysis
                                    //% "Movement no."
                                    QString sValue = qtTrId("B7558B5D-AAAC-42A1-9EFD-999A5BED06D6");
//                                    QString sValue = QString::fromWCharArray ( omsl::linguistic::QT_TRANSLATE_NOOP ( "TPAnalysis", "Movement no." ).str ()->Get () );
                                    found = true;
#ifdef DBGVER
                                    swprintf ( s, L" %s  : %s = %d [ ptIndex= %d, MovementByPtIndex =%s]", ncdata.gmotion == 0 ? L"G0" : ncdata.gmotion == 1 ? L"G1" : ncdata.gmotion == 2 ? L"G2" : ncdata.gmotion == 3 ? L"G3" : L"GX", sValue.toStdWString ().c_str (), moveNr, data, mv == 0 ? L"G0" : mv == 1 ? L"G1" : mv == 2 ? L"G2" : mv == 3 ? L"G3" : L"GX" );
#else
                                    swprintf ( s, L" %s  : %s = %d ", ncdata.gmotion == 0 ? L"G0" : ncdata.gmotion == 1 ? L"G1" : ncdata.gmotion == 2 ? L"G2" : ncdata.gmotion == 3 ? L"G3" : L"GX", sValue.toStdWString ().c_str (), moveNr );
#endif
                                }
                                else
                                {
                                    //: TPAnalysis
                                    //% "Movement no."
                                    QString sValue = qtTrId("1DB3C87D-FA36-4B3F-84D4-4A5172C4F176");
//                                    QString sValue = QString::fromWCharArray ( omsl::linguistic::QT_TRANSLATE_NOOP ( "TPAnalysis", "Movement no." ).str ()->Get () );
                                    found = true;
#ifdef DBGVER
                                    swprintf ( s, L" %s = %d [ ptIndex= %d, MovementByPtIndex =%s]", sValue.toStdWString ().c_str (), moveNr, data, mv == 0 ? L"G0" : mv == 1 ? L"G1" : mv == 2 ? L"G2" : mv == 3 ? L"G3" : L"GX" );
#else
                                    swprintf ( s, L" %s = %d ", sValue.toStdWString ().c_str (), moveNr );
#endif
                                }
                            }
                            else if ( ( *cit ).second.s == L"NC_feedrate" && !showLayer25 )
                            {
                                if ( ncdata.gmotion > 0 )
                                {
                                    found = true;
                                    if ( ncdata.feedUnit == 0 ) // milling mm/min
                                    {
                                        // fill vector with rounded values
                                        pConverter->SetInner ( ncdata.feedrate );
                                        const wchar_t* s2 = pConverter->Get ();
                                        swprintf ( s, L" %s", s2 );
                                    }
                                    else // turning mm/U
                                        swprintf ( s, L" %6.3lf", ( double ) ncdata.feedrate );
                                }
                            }
                            else if ( ( *cit ).second.s == L"NC_deviation" && showLayer25 )
                            {
                                if ( ncdata.gmotion > 0 )
                                {
                                    if ( checkTPAMode (U_SEL) == tpanalysis::compTPAMode::cm_Movement )
                                    {
                                        found = true;
                                        swprintf ( s, L" %1.6lf , %1.6lf", ( double ) a.data.d , ( double ) b.data.d );
                                    }
                                    if ( checkTPAMode (U_SEL) == tpanalysis::compTPAMode::cm_Movement_Movement )
                                    {
                                        found = true;
                                        swprintf ( s, L" %1.6lf , %1.6lf", ( double ) a.data.d , ( double ) b.data.d );
                                    }
                                    else if ( checkTPAMode (U_SEL) == tpanalysis::compTPAMode::cm_Point )
                                    {
                                        found = true;
                                        swprintf ( s, L" %1.6lf", ( double ) a.data.d );
                                    }
                                    else if ( checkTPAMode (U_SEL) == tpanalysis::compTPAMode::cm_Point_Point )
                                    {
                                        found = true;
                                        swprintf ( s, L" %1.6lf", ( double ) a.data.d );
                                    }
                                    else if ( checkTPAMode (U_SEL) == tpanalysis::compTPAMode::cm_Point_Movement )
                                    {
                                        found = true;
                                        swprintf ( s, L" %1.6lf", ( double ) a.data.d );
                                    }
                                }
                            }
                            else if ( ( *cit ).second.s == L"NC_speed" && !showLayer25 )
                            {
                                if ( ncdata.speedUnit == 0 ) // milling U/min
                                {
                                    found = true;

                                    // fill vector with rounded values
                                    pConverter->SetInner ( ncdata.speed );
                                    const wchar_t* s2 = pConverter->Get ();
                                    swprintf ( s, L" %s", s2 );
                                }
                            }
                            else if ( ( *cit ).second.s == L"NC_cuttingspeed" && !showLayer25 )
                            {
                                if ( ncdata.speedUnit != 0 )
                                {
                                    found = true;
                                    swprintf ( s, L" %6.0lf", ( double ) ncdata.speed );
                                }
                            }

                            if ( found )
                            {
                                if ( !cnt )
                                m_tt_cnct.set_tooltip (
                                    unsigned long ( data ), // pEntOrig.Get (),
                                    L"HyperMill_tooltip",
                                    L"HyperMill_tooltip.descriptions",
                                    ( *cit ).second.ls.c_str (), //L"HM_Job",
                                    ( *cit ).second.ls.c_str (),
                                    s
                                );
                            else
                                m_tt_cnct.add_tooltip (
                                    unsigned long ( data ), // pEntOrig.Get (),
                                    L"HyperMill_tooltip",
                                    L"HyperMill_tooltip.descriptions",
                                    ( *cit ).second.ls.c_str (), //L"HM_Job",
                                    ( *cit ).second.ls.c_str (),
                                    s
                                );
                                cnt++;
                            }
                        }

                        typedef std::vector<std::wstring> ttLocIdStringVec;
                        {
                            //ttLocIdStringVec* ttlocidvec, *ttlocvec;
                            // ptpData->getAddDataAsString ( data , ttlocidvec, ttlocvec );
                            // loop
                        }
                        ttLocIdStringVec* ttlocidvec, *ttlocvec;

                        ptpData->GetSortedTooltipData ( ttlocidvec, ttlocvec );
                        for ( int i = 0; i < ttlocvec->size (); i++ )
                        {
                            if ( !cnt )
                                m_tt_cnct.set_tooltip (
                                    unsigned long ( data ), // pEntOrig.Get (),
                                    L"HyperMill_tooltip",
                                    L"HyperMill_tooltip.descriptions",
                                    (*ttlocidvec) [i].c_str (), //L"HM_Job",
                                    (*ttlocidvec) [i].c_str (),
                                    (*ttlocvec) [i].c_str ()
                                );
                            else
                                m_tt_cnct.add_tooltip (
                                    unsigned long ( data ), // pEntOrig.Get (),
                                    L"HyperMill_tooltip",
                                    L"HyperMill_tooltip.descriptions",
                                    ( *ttlocidvec ) [i].c_str (), //L"HM_Job",
                                    ( *ttlocidvec ) [i].c_str (),
                                    ( *ttlocvec ) [i].c_str ()
                                );
                            cnt++;
                        }


                        return res;
                        break;
                    }
                }

                {
//                    SetupTemporaryGraphics ();
                    //int vm = GetValue ( tpanalysis::viewModeParameter );
                }
                return res;
            }
        }  // namespace tp_analyzer
    }  // namespace commands
}  // namespace omsl
