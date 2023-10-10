#pragma once
#ifndef TPANALYSISMODEL_H__
#define TPANALYSISMODEL_H__

/*!
* \file tpanalysismodel.h
*
* \author Uwe Spiess
* \date 24/04/2017
*
* contains the functionality to act with the model
*/

//Handshaking
#include <command/commandmodelhandshake.h>
// base class implementation
#include <command/commandmodelimpl.h>
#include <bgeo\unitvector3d.h>
#include <bgeo\vector3d.h>
#include <bgeo\point3d.h>
#include <geom/arc3d.h>
#include <bgeo/coord3d.h>
#include <bgeo/Mat4x4.h>
//#include <geom/geometriccontainers.h>
//#include <containers/stringconcretecontainervectorimpl.h>
#include <geom/toolpathsegment.h>
#include <geom/toolpathsegmentcreator.h>
#include <geom/point3d.h>
#include <hcntcore/toolpathentity.h>
#include <geom/attributes.h>
#include <geom/attributerefcounter.h>
#include <bgeo/typeofbgeotypesiterators.h>
#include <bgeo/coord3dvectorcontainers.h>
#include <hcntcore/cadentitycontainers.h>
#include <hcntcore/cadentitycontainersinterfaces.h>

#include <map>
#include <materials/materials.h>
#include <materials/materialslibrary.h>

#include "hcntLib/containernew.h"

#ifndef CadEntitiesIt_T
typedef omsl::SharedPtr<omsl::iterator::hcntcore::CadEntityForward> CadEntitiesIt_T;
#endif

#define UNKNOWNINDEX (int)(-1)
#define UNSPECIFICINDEX (int)(-2)
#define MASTER_TP 0
#define L_SEL 0
#define R_SEL 1
#define ALL_SEL 2
#define U_SEL -1

namespace omsl
{
    namespace bgeo
    {
        class RigidMotion3d;
    }
}
namespace omsl
{
    namespace geom
    {
        class TopoGeom;
    }
}
namespace omsl
{
    namespace hcntcore
    {
        class CadEntity;
    }
}
namespace omsl
{
    namespace hcntcore
    {
        class Document;
    }
}

namespace omsl
{
    namespace iterator
    {
        namespace geom
        {
            class TopoGeomPtrForward;
        }
    }
}
namespace omsl
{
    namespace iterator
    {
        namespace hcntcore
        {
            class CadEntityForward;
        }
    }
}

typedef omsl::SharedPtr<omsl::geom::TopoGeom> TopoGeomPtr;
typedef omsl::SharedPtr<omsl::geom::ToolpathSegment> ToolpathSegmentPtr;
typedef omsl::SharedPtr<omsl::hcntcore::ToolPathEntity> ToolpathPtr;
typedef omsl::SharedPtr<omsl::materials::Material> MaterialPtr;
typedef omsl::SharedPtr<omsl::materials::MaterialsLibrary> MaterialsLibraryPtr;
typedef omsl::SharedPtr<omsl::materials::MaterialsLibraryIterator> MaterialsLibraryIteratorPtr;

class graphDispVal
{
public:
    int dirPos1;
    int dirPos2;
    int cnt;
    int tptypeMain;
    int tptype1=-1;
    int tptype2=-1;

};

typedef std::map< ToolpathPtr, graphDispVal> ToolPathPtrgraphDispValMap;

namespace omsl
{
    namespace commands
    {
        namespace tp_analyzer
        {
            namespace tpanalysis
            {
                enum compFRAMode;
                enum compTPAMode;
            }
        }
    }
}

namespace omsl
{
    namespace commands
    {
        namespace tp_analyzer
        {
            namespace tpanalysis
            {
#ifndef CadEntitiesIt_T
                typedef SharedPtr<iterator::hcntcore::CadEntityForward> CadEntitiesIt_T;
#endif 
            }
        }
    }
}


namespace omsl
{
    namespace commands
    {
        namespace tp_analyzer
        {
            /*!
             * \class TPAnalysisModel
             *
             * \brief implementation of TPAnalysis model
             *
             * \author Leonardo Cecchinato
             * \date 21/09/2017
             */

            enum viewMode
            {
                vm_allOff = 0
                , vm_SHOWDIRECTION = 1 << 0
                , vm_SHOWCOMPPATH = 1 << 1
                , vm_SHOWORIENTATION = 1 << 2
                , vm_SHOWVECTORS = 1 << 3
                , vm_SHOWLABEL = 1 << 4
                , vm_SHOWTOOL = 1 << 5
                , vm_SHOWPOINTS = 1 << 6
                , vm_SHOWCLEARANCE = 1 << 7
                , vm_SHOWFEEDRATEALWAYS = 1 << 8
                , vm_AUTOUPDATEMIN = 1 << 9
                , vm_AUTOUPDATEMAX = 1 << 10
                , vm_AUTOUPDATELVL = 1 << 11
            };
            enum coordsystemType
            {
                cs_NCS = 0
                , cs_Frame = 1
                , cs_World = 2
            };

            class extremaData
            {
            public:
                bool withCMax;
                double smin, smax, fmin, fmax, cmax;
                int segSize;
                int speedUnit,feedUnit;
                int tpType;
            };

            typedef std::map<std::wstring, extremaData > extremaMap;

            class analyseData
            {
                public:

                    enum dataAccessIndex
                    {
                        dai_SXS = 0,        // start, also X-value
                        dai_SXC,           // center, also Y-value
                        dai_SXE,           // end, also Z-value
                        dai_SXP1,           // pickpoint1
                        dai_SXP2,           // pickpoint2
                        dai_CTR1,           // centerpoint1
                        dai_CTR2,           // centerpoint2
                        dai_CTR3,           // centerpoint3
                        dai_CTR4,           // centerpoint4
                        dai_DIR1,           // direction point 1
                        dai_DIR2,           // direction point 2
                        dai_SXPC,           // calculated point
                        dai_REFP,           // refpoint
                        dai_VR,             // stored radius
                        dai_VA,             // stored angles
//                        dai_1stSeg,         // index last point of pointsL, calculated based on dai_2ndSegmentSize and dai_SXP1
//                        dai_3rdSeg,         // index firstpoint of pointsR
                        dai_Index1stTpSeg,  // index of toolpath segment related to toolpathentity->cadentities
                        dai_Index2ndTpSeg,  // index of toolpath segment related to toolpathentity->cadentities
                        dai_Index1stTpEnt,  // index of parent toolpath related to dai_Index1stTpSeg
                        dai_Index2ndTpEnt,  // index of parent toolpath related to dai_Index12ndTpSeg
                        dai_SpinBoxValue,   // used to step throw pointVec with UI spinbox
                        dai_leftPick,
                        dai_rightPick,
                        // non indizes values
                        dai_last,           // array size
                        dai_ArcOffset = 2,  // used for storage arcs
                        dai_tUnknown = 0,   // datatype
                        dai_tLine = 1,      // datatype
                        dai_tArc = 2,       // datatype
                        dai_tCurve = 3,     // datatype
                        dai_viewOrigNormals = 0x0001, // viewmode segment
                        dai_viewOrigPoints  = 0x0002, // viewmode segment
                        dai_viewOrigDirection = 0x0004, // viewmode segment
                        dai_pointArraySize = dai_REFP + 1,
                        dai_vectorArraySize = dai_VA + 1,
                        dai_indexArraySize = dai_last + 1,
                        dai_2ndSegmentSize = 100 // used to reduce the redraw of marking points ,  rule : 0...dai_1stSeg,dai_2ndSegment ( dai_SXP1 ), dai_3rdSeg ... pointVec.size ()
                    };

                    struct
                    {
                        // 0 = pickpoint ent1
                        // 1 = pickpoint ent2
                        // 2 = pickpoint ent3
                        bgeo::Point3d points [dai_pointArraySize];

                        // 0 = vector pX1
                        // 1 = vector pY1
                        // 2 = vector pZ1
                        // 3 = vector pX2
                        // 4 = vector pY2
                        // 5 = vector pZ2
                        // 6 = vector pX3
                        // 7 = vector pY3
                        // 8 = vector pZ3
                        bgeo::Vector3d vectors [dai_vectorArraySize];
                        // contains the toolpath points
                        std::vector<bgeo::Coord3d> pointVec;
                        int specialIndex [dai_indexArraySize];
                        tpanalysis::compTPAMode m_compTPAMode;
                        bgeo::RigidMotion3d motion;
                        bgeo::RigidMotion3d pof_frame;
                        bgeo::RigidMotion3d pof_ncs;
                        SharedPtr<omsl::geom::Attribute> m_pAttr;
                        SharedPtr<omsl::geom::ToolpathSegment> m_pTpSeg;
                        SharedPtr<omsl::geom::ToolpathSegment> m_pTpSegCast;
                        SharedPtr<hcntcore::CadEntity> m_pTpSegCadEnt;
                        SharedPtr<hcntcore::CadEntity> m_pParGeomCast;
                        SharedPtr<hcntcore::CadEntity> m_origCadEnt;
                        std::vector<unsigned int> typeVec;
                        std::vector<unsigned int> viewVec;
                        std::vector<double> pointSizeVec;
                        std::vector<int> segmentSizeVec;
                    } m[ALL_SEL];

                    // overall data
                    SharedPtr<omsl::container::bgeo::VectCoord3dConcreteForward> pPnt_L1stSegment;
                    SharedPtr<omsl::container::bgeo::VectCoord3dConcreteForward> pPnt_R3rdSegment;

                    SharedPtr<omsl::container::bgeo::VectCoord3dConcreteForward> pPnt_L2ndSegment;
                    SharedPtr<omsl::container::bgeo::VectCoord3dConcreteForward> pPnt_C2ndSegment;
                    SharedPtr<omsl::container::bgeo::VectCoord3dConcreteForward> pPnt_R2ndSegment;

                    ToolpathPtr pSelectedTP[2];
                    ToolPathPtrgraphDispValMap pToolpathMap;
                    extremaMap m_extrema;

                    void reset ();
                    void reset ( int sel );
            };

            class TPAnalysisModel :
                public omsl::command::CommandModelImpl<TPAnalysisModel>
                //, public ModelEntityConnection
            {
                OMSL_FRIEND_COMMANDMODEL_FACTORY ( TPAnalysis )
                    typedef omsl::command::CommandModelImpl<TPAnalysisModel> BaseClass;

            public:

                //////////////////////////////////////////////////////////////////////////
                //  CommandModelImpl implementation
                //////////////////////////////////////////////////////////////////////////
                /**
                * @brief used to create rules, to connect values to rules ...
                * @see \ref
                */

                void InternalDefineContext ();

                //////////////////////////////////////////////////////////////////////////
                //  Model methods
                //////////////////////////////////////////////////////////////////////////
                /**
                * @brief reset entities selections
                * @see \ref
                */
//                void OnModeTPAChange ( const int& );

                //   //Rules
                //   SharedPtr<iterator::geom::TopoGeomPtrForward> Compute(const int&, const double&, const omsl::Bool&, const SharedPtr<iterator::hcntcore::CadEntityForward>&);
                /**
                * @brief computes coordinates from WCS to the selected UCS
                * @param [in] const SharedPtr<iterator::hcntcore::CadEntityForward>& pEnts1  iterator to first selection
                * @param [in] const SharedPtr<iterator::hcntcore::CadEntityForward>& pEnts2  iterator to second selection
                * @param [in] const SharedPtr<hcntcore::CadEntity> &pPoint                   additional refernce point
                * @param [in] const int& RefSystemType                                       type of reference system ( WCS, Frame, NCS )
                * @see \ref
                */

                SharedPtr<propertyservice::PropertyBag> Compute ( const SharedPtr<iterator::hcntcore::CadEntityForward>& pEnts1,
                                                                  const SharedPtr<iterator::hcntcore::CadEntityForward>& pEnts2);

                int OnChangeReferencePoint ( const SharedPtr<hcntcore::CadEntity> &pPoint );
                int OnSetupGeneralTab ( const int& UI_Tab_General );
                int OnSetupCoordinateSystem ( const int& UI_BtnGroup_RefSystemType );
                int HighlightToolPath ( const SharedPtr<iterator::hcntcore::CadEntityForward>& pTPEntsIt1, const SharedPtr<iterator::hcntcore::CadEntityForward>& pTPEntsIt2 );
                int HighlightToolPath2 ( const SharedPtr < hcntcore::ToolPathEntity>& pTPEnt, const int index, unsigned int sel );
                int CheckSelection ( const SharedPtr<iterator::hcntcore::CadEntityForward>& pEntsIt1, const SharedPtr<iterator::hcntcore::CadEntityForward>& pEntsIt2 );
                int CheckLeftPick ( const SharedPtr<hcntcore::CadEntity>& pPoint );
                int CheckRightPick ( const SharedPtr<hcntcore::CadEntity>& pPoint );
                int SetSpecialIndex (  const SharedPtr<iterator::hcntcore::CadEntityForward>& pEntsIt, const int segindex , const int tpindex, const int ptindex, int sel );
                int OnChangeRangeSlider ( const int& );
                
                // new 3rd tab

                OMSL_METHOD ( void ) OnChangeEnvClrMapByCounts ( const omsl::Bool& );
                OMSL_METHOD ( void ) OnChangeEnvUIClrMapByAnalyseData ( const omsl::Bool& );
                OMSL_METHOD ( void ) OnLayerVisisbilityChanged ( const omsl::Bool& b, const int tpTpType );
                OMSL_METHOD ( void ) OnShowLayer25Changed ( const omsl::Bool& );
                OMSL_METHOD ( void ) OnShowNCPathChanged ( const omsl::Bool& );
                OMSL_METHOD ( void ) OnShowBallMillSpecPathChanged ( const int& );
                OMSL_METHOD ( void ) OnShowClippPathChanged ( const omsl::Bool& );

                //OMSL_METHOD ( void ) OnHideUpperLimitFeedratesChanged ( const omsl::Bool& );
                //OMSL_METHOD ( void ) OnHideLowerLimitFeedratesChanged ( const omsl::Bool& );

                OMSL_METHOD ( void ) OnChangePointSize ( const double& );
                OMSL_METHOD ( void ) OnChangeLineWidth ( const double& );
                OMSL_METHOD ( void ) OnChangeVectorLength ( const double& );

                OMSL_METHOD ( void ) OnChangeEnvMinTolRadius ( const double&  );
                OMSL_METHOD ( void ) OnChangeEnvTolerance ( const double&  );

                int OnUpdateFeedrateDraw ( const int& );
                /**
                * @brief react to the moveNr selection
                * @see \ref
                */
                SharedPtr<propertyservice::PropertyBag>  OnChangeMoveNr ( const int& );

                void OnChangeMoveNr2 ( const int& );

                /**
                * @brief react to the changes of the checkboxes in UI
                * @see \ref
                */
                int ChangeViewSettings ( const int& bFlag, const int& label );

                int OnChange_CB_Label ( const int& );
                int OnChange_CB_Direction ( const int& );
                int OnChange_CB_Vector ( const int& );
                int OnChange_CB_Point ( const int& );
				int OnChange_CB_Feedrate ( const int& );

                // 3rd tab new
                //OMSL_METHOD (void ) OnChange_CB_Direction1 ( const omsl::Bool & );
                //OMSL_METHOD (void ) OnChange_CB_Label1 ( const omsl::Bool & );
                //OMSL_METHOD (void ) OnChange_CB_Feedrate1 ( const omsl::Bool & );

                // unused
                //int OnChange_CB_Compensation ( const int& );
                int OnChange_CB_Orientation ( const int& );
                //int OnChange_CB_Tool ( const int& );

                // 2nd tab
                int OnChange_CB_Clearance ( const int& );
                int OnChange_CB_AutoUpdateMin ( const int& );
                int OnChange_CB_AutoUpdateMax ( const int& );
                int OnChange_CB_AutoUpdateLvl ( const int& );

                int OnColorStepCount ( const Int& nLevels );
                int OnColorTargetMin ( const Double & dMin );
                int OnColorTargetMax ( const Double & dMax );

                /**
                * @brief used to setup temporary graphics, like temporary direction vectors, highligted objects ...
                * @param [in] const SharedPtr<iterator::hcntcore::CadEntityForward>& pEnts1
                * @param [in] const SharedPtr<hcntcore::CadEntity> &pPoint
                * @param [in] const int& RefSystemType    currently obsolete, maybe used for different color selections
                * @param [in] const bgeo::RigidMotion3d &motion...referenced UCS
                * @param [out] propertyservice::PropertyBag *pBag   contains the result for the UI, like distanecs,angles, coordinates
                * @param [out] std::vector<SharedPtr<geom::TopoGeom>> &vDirections        temporary directionVectors
                * @param [out] std::vector<SharedPtr<geom::TopoGeom>> &vCompensations     temporary directionVectors
                * @param [out] std::vector<SharedPtr<geom::TopoGeom>> &vOrientations      temporary directionVectors
                * @param [out] std::vector<SharedPtr<geom::TopoGeom>> &vVectors           temporary directionVectors
                * @param [out] std::vector<SharedPtr<geom::TopoGeom>> &vLabels            temporary strings
                * @see \ref
                */

                void ComputeOnPoints ( const SharedPtr<iterator::hcntcore::CadEntityForward>& pEnts1, int sel );

                void ComputeOnEnts ( const SharedPtr<iterator::hcntcore::CadEntityForward>& pEnts1,
                                     const SharedPtr<iterator::hcntcore::CadEntityForward>& pEnts2 );

                void CalcBaseDataForTempGraphic ( int sel );

                /**
                * @brief enable / disable second selection button
                * @param [in] const int&   type of selection function
                */
                propertyservice::Applicable::Strength ActivateEnts1 ( const int& );
                propertyservice::Applicable::Strength ActivateEnts2 ( const int& );
                //propertyservice::Applicable::Strength ActivateRefPnts  ( const int& );
                //propertyservice::Applicable::Strength ActivateToolpath ( const int& );
                propertyservice::Applicable::Strength ActivateFeedRateMin ( const int& iMode );
                propertyservice::Applicable::Strength ActivateFeedRateMax ( const int& iMode );
                propertyservice::Applicable::Strength ActivateColorLevel ( const int& iMode );

                propertyservice::Applicable::Strength Activate_CB_Min_ColorSettings ( const int& iMode );
                propertyservice::Applicable::Strength Activate_CB_Max_ColorSettings ( const int& iMode );
                propertyservice::Applicable::Strength Activate_CB_Lvl_ColorSettings ( const int& iMode );
                propertyservice::Applicable::Strength Activate_CB_Clr_ColorSettings ( const int& iMode );

                void ActivateSlider ( const bool );

            protected:
                TPAnalysisModel ( omsl::hcntcore::Document* pDocument );
                ~TPAnalysisModel ();


                SharedPtr<propertyservice::PropertyBag> CreatePropertyBag ();
                //////////////////////////////////////////////////////////////////////////
                //  CommandModelImpl implementation
                //////////////////////////////////////////////////////////////////////////
                virtual void InternalApply ( void );
                virtual void InternalApply2 ( int , void*  [] )
                {
                    s_data.reset ();
                }
            private:
                TPAnalysisModel ( const TPAnalysisModel& )
                {}
                const TPAnalysisModel& operator= ( const TPAnalysisModel& );

                void ComputeAnalyseData ( const SharedPtr<iterator::hcntcore::CadEntityForward>& pEnts1 , int sel );
                bool ComputeSelection ( const CadEntitiesIt_T& pEnts, int sel );

                void SetupTemporaryGraphics ();

//                void SetupTemporaryGraphicsDirection ( const int viewModeParameter);
                void SetupTemporaryGraphicsCompensation ( const int viewModeParameter);
                void SetupTemporaryGraphicsOrientation ( const int viewModeParameter);
                void SetupTemporaryArcMovements ( const int viewModeParameter, int type );
                void SetupTemporary_Measure_Graphic ();
                //void SetupTemporary_P_P_Graphic ( const int viewModeParameter );
                //void SetupTemporary_M_M_Graphic ( const int viewModeParameter );
                //void SetupTemporary_P_M_Graphic ( const int viewModeParameter );
                void SetupTemporaryTPGraphics ( const int type, const int viewModeParameter);
                void SetupTemporaryGraphicsLabel ( const int viewModeParameter);
                void SetupCurrentToolDirection ( const int viewModeParameter );
//                void SetupMeasurementGraphic ( const int viewModeParameter );
//                void SetupTemporaryTool ( const int viewModeParameter );
                void SetupTemporaryGraphicsLabel ( const int viewModeParameter, omsl::hcntcore::CadEntity* entity );
//                void SetupTemporaryGraphicsDirection ( const int viewModeParameter, omsl::hcntcore::CadEntity* entity );
                void SetupFeedRateColors ( const int viewModeParameter, omsl::hcntcore::CadEntity* entity );
                void SetupMotionColors ( const int viewModeParameter, omsl::hcntcore::CadEntity* entity );
                void SetupFeedRateColors ( const int viewModeParameter );
                void SetupMotionColors ( const int viewModeParameter );

                void SetupAdditionalCheckboxes ( int content );

                void SetupPropertyBag ();

                // void SetupPropertyBag ( SharedPtr<propertyservice::PropertyBag> pBag, int sel );
                // void  SetupPropertyBagForSettings ();

                int CalculateNearestPoint ( int resMode );

                void SetupPropertyBagFor_T ( SharedPtr<propertyservice::PropertyBag> pBag, int sel, bool fullData );
                void SetupPropertyBagFor_P ( SharedPtr<propertyservice::PropertyBag> pBag );
                void SetupPropertyBagFor_M ( SharedPtr<propertyservice::PropertyBag> pBag );
                void SetupPropertyBagFor_P_P ( SharedPtr<propertyservice::PropertyBag> pBag );
                void SetupPropertyBagFor_P_O ( SharedPtr<propertyservice::PropertyBag> pBag );

                void createLabel ( ToolpathPtr pToolpath
                                   , SharedPtr<container::VectWCharTConcreteForward>& pTxts
                                   , SharedPtr<container::bgeo::VectCoord3dConcreteForward>& pTxtPts
                                   , SharedPtr<container::geom::VectTopoGeomPtrConcreteForward>& vGraphics
                );

                int calcArcVector ( const omsl::geom::Arc3d* pArc, bgeo::Point3d& center, bgeo::UnitVector3d& v0, bgeo::UnitVector3d& v2 );

                int removeLabel ( hcntcore::CadEntity* cent );
//                int removeDirection ( hcntcore::CadEntity* cent );

                void AddExtrema ( omsl::hcntcore::CadEntity* cent );
                int RemoveExtrema ( hcntcore::CadEntity* cent );
                void ReplaceExtrema ( hcntcore::CadEntity* cent );

                void SetupExtremaToUI ( const int data );
                bool GetExtremaFromMap ( std::vector<double>& vecOfValues, double& smin, double& smax, double& fmin, double& fmax, double& cmax, int& segMax );

                void fillToolpathPoints ( ToolpathSegmentPtr& pgep, int sel );
public:
//                void ResetGraphicsSize ();
private:
//                void ResetGraphicsSize ( hcntcore::CadEntity* cent );
//                void SetupGraphicsSize ( hcntcore::CadEntity* cent );

                omsl::geom::ToolpathSegment* CastToToolpathSegment ( const SharedPtr<hcntcore::CadEntity>& pEnt, int sel );

                //bgeo::Point3d TranslateWithRefPoint ( const bgeo::Point3d& point );
                //bgeo::Point3d TranslateWithRefPoint ( const bgeo::Coord3d& point );

                bgeo::Point3d GetPoint ( const SharedPtr<hcntcore::CadEntity> &ent_0, const unsigned int index, int sel );
                void StoreRefPoint ( const SharedPtr<hcntcore::CadEntity> &pEnt, const unsigned int index, int sel );
                void StoreEntityPoint ( const tpanalysis::CadEntitiesIt_T& pEnts, const unsigned int index, int sel );
                void StoreArcData ( SharedPtr<hcntcore::CadEntity> &pEnt, const unsigned int index, int sel );

                void ResetPreviewData ( const int type, int sel );

                bool AddTP2Map ( ToolpathPtr& );
                bool RemoveTPfromMap ( ToolpathPtr& );

                int GetViewModeValue ();

                //int MaterialFindByDiffuseColour ( const int [3] );// not necessary yet
                WeakPtr<hcntcore::Document>       m_pDocument;

public:
                tpanalysis::compTPAMode checkTPAMode ( int sel );
                bool CheckSignalTypeForCadEntityTPA ( omsl::hcntcore::CadEntity*, int, int );
                void SetInternalSelection ( SharedPtr<hcntcore::CadEntity>& pEntsIn, int sel );
                //bool CheckSignalTypeForCadEntityFRA
                //(
                //    omsl::hcntcore::CadEntity* pEnt
                //    , int internal_HM_type
                //    , int data
                //);

                class ttdata
                {
                public:
                    std::wstring s, ls;
                    int index;
                } ;

                typedef std::map<int, ttdata> ttorderedMap;
                ttorderedMap smap;
                bool AddTTipLocString ( const wchar_t* name, int /*type*/, const wchar_t* locstring, int data )
                {
                    if ( data == -999999 )
                        smap.clear ();
                    std::wstring s (name);
                    std::wstring ls ( locstring );
                    ttdata t;
                    t.ls.assign( ls);
                    t.s.assign ( s );
                    t.index = data;
                    smap [data] = t;
                    return true;
                }


private:

                analyseData s_data;

                SharedPtr<omsl::WString>        m_descriptionPath;
                //MaterialPtr   m_pMaterial; // not necessary yet
                //int m_rgb [3] { -1,-1,-1 };

                bool m_withUpdate;
                bool m_waitForCadShell = false;
public:
                inline bool SignalListeningIsEnabled ()
                {
                    return m_enableSignalListening;
                }
                inline bool SetSignalListening ( const bool b )
                {
                    bool old = m_enableSignalListening;
                    sl_callcounter++;
                    sl_counter += ( b == true ? 1 : -1 ) ;
                    m_enableSignalListening = b;
                    return old;
                }

                void copyInternalSettings ( const TPAnalysisModel* );

            private:
                //bool b_GVIcon = false;
                //bool b_PTIcon = false;

                int s_ftype = 0;
                int b_GnIcon = -1;
                bool m_enableSignalListening = true;
                int sl_counter = 0;
                int sl_callcounter = 0;
                public:
                    std::vector<std::wstring>                          m_callsm;
                    int level = 0;
                    void callStackString ( const wchar_t* s, int inout )
                    {
#ifdef DBGVER
                        static bool b = true;
                        if ( b && s )
                        {
                            wchar_t ls [100];
                            wchar_t ll [100] = L"                                                                           ";
                            if ( inout > 0 )
                                level += inout;
                            ll [level * 3] = L'\0';
                            swprintf ( ls, L"%5.5d : ", level );
                            if ( inout < 0 )
                                level += inout;


                            std::wstring cls;
                            cls = ll;
                            cls += ls;
                            cls += s;
//                            size_t ss = m_callsm.size ();
                            m_callsm.push_back ( cls );
                        }
#endif
                    }
private:

            };
        }  // namespace tp_analyzer
    }  // namespace commands
}  // namespace omsl

#endif // TPANALYSISMODEL_H__
