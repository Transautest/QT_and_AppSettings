#pragma once
#ifndef TPSPEEDANALYSISMODEL_H__
#define TPSPEEDANALYSISMODEL_H__

/*!
* \file TPSpeedAnalysisModel.h
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
#include <bgeo/coord3d.h>
#include <bgeo/Mat4x4.h>
#include <geom/geometriccontainers.h>
#include <containers/stringconcretecontainervectorimpl.h>
#include <geom/toolpathsegment.h>
#include <geom/toolpathsegmentcreator.h>
#include <geom/point3d.h>
#include <hcntcore/toolpathentity.h>
#include <geom/attributes.h>
#include <geom/attributerefcounter.h>
#include <bgeo/typeofbgeotypesiterators.h>
#include <bgeo/coord3dvectorcontainers.h>
#include <hcntcore/cadentitycontainers.h>
#include <map>


//#include "ModelEntityConnection.h"

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
typedef omsl::SharedPtr<omsl::hcntcore::CadEntity> CadEntityPtr;

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
             * \class TPSpeedAnalysisModel
             *
             * \brief implementation of TPSpeedAnalysis model
             *
             * \author Uwe Spieﬂ
             * \date 21/09/2017
             */

            enum viewMode1
            {
                  vm1_allOff = 0
                , vm1_SHOWDIRECTION = 1 << 0
                , vm1_SHOWCOMPPATH = 1 << 1
                , vm1_SHOWORIENTATION = 1 << 2
                , vm1_SHOWVECTORS = 1 << 3
                , vm1_SHOWLABEL = 1 << 4
            };
            enum coordsystemType1
            {
                cs1_NCS = 0
                , cs1_Frame = 1
                , cs1_World = 2
            };

            enum compTPAMode1
            {
                cm1_First = 0
                , cm1_Point = 0
                , cm1_Point_Point = 1
                , cm1_Point_Movement = 2
                , cm1_Point_Geometry = 3
                , cm1_Movement = 4
                , cm1_Movement_Movement = 5
                , cm1_Movement_Geometry = 6
                , cm1_Vector = 7
                , cm1_Vector_Vector = 8
                , cm1_MovementGroup = 9
                , cm1_Toolpath = 10
                , cm1_Last = 10
            };

            class analyseData1
            {
                public:

                    enum dataAccessIndex1
                    {
                        dai_SXS = 0,        // start, also X-value
                        dai_SXC,           // center, also Y-value
                        dai_SXE,           // end, also Z-value
                        dai_SXP1,           // pickpoint1
                        dai_SXP2,           // pickpoint2
                        dai_CTR1,           // centerpoint1
                        dai_CTR2,           // centerpoint2
                        dai_DIR1,           // direction point 1
                        dai_DIR2,           // direction point 2
                        dai_REFP,           // refpoint
                        dai_VR,             // stored radius
                        dai_VA,             // stored angles
                        dai_1stSeg,         // index last point of pointsL, calculated based on dai_2ndSegmentSize and dai_SXP1
                        dai_3rdSeg,         // index firstpoint of pointsR
                        dai_Index1stTpSeg,  // index of toolpath segment related to toolpathentity->cadentities
                        dai_Index2ndTpSeg,  // index of toolpath segment related to toolpathentity->cadentities
                        dai_Index1stTpEnt,  // index of parent toolpath related to dai_Index1stTpSeg
                        dai_Index2ndTpEnt,  // index of parent toolpath related to dai_Index12ndTpSeg
                        dai_SpinBoxValue,   // used to step throw pointVec with UI spinbox
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

                    // 0 = pickpoint ent1
                    // 1 = pickpoint ent2
                    // 2 = pickpoint ent3
                    bgeo::Point3d points[dai_pointArraySize];
                
                    // 0 = vector pX1
                    // 1 = vector pY1
                    // 2 = vector pZ1
                    // 3 = vector pX2
                    // 4 = vector pY2
                    // 5 = vector pZ2
                    // 6 = vector pX3
                    // 7 = vector pY3
                    // 8 = vector pZ3
                    bgeo::Vector3d vectors[dai_vectorArraySize];
                    // contains the toolpath points
                    std::vector<bgeo::Coord3d> pointVec;
                    int specialIndex [dai_indexArraySize];
                    bgeo::RigidMotion3d motion;
                    bgeo::RigidMotion3d pof_frame;
                    bgeo::RigidMotion3d pof_ncs;
                    SharedPtr<omsl::geom::Attribute> m_pAttr;
                    SharedPtr<omsl::geom::ToolpathSegment> m_pTpSeg;
                    SharedPtr<omsl::geom::ToolpathSegment> m_pTpSegCast;
                    SharedPtr<hcntcore::CadEntity> m_pTpSegCadEnt;
                    SharedPtr<hcntcore::CadEntity> m_pParGeomCast;
                    std::vector<unsigned int> typeVec;
                    std::vector<unsigned int> viewVec;
                    std::vector<double> pointSizeVec;
                    std::vector<int> segmentSizeVec;
                    SharedPtr<omsl::container::bgeo::VectCoord3dConcreteForward> pPnt_L1stSegment;
                    SharedPtr<omsl::container::bgeo::VectCoord3dConcreteForward> pPnt_R3rdSegment;

                    SharedPtr<omsl::container::bgeo::VectCoord3dConcreteForward> pPnt_L2ndSegment;
                    SharedPtr<omsl::container::bgeo::VectCoord3dConcreteForward> pPnt_C2ndSegment;
                    SharedPtr<omsl::container::bgeo::VectCoord3dConcreteForward> pPnt_R2ndSegment;

                    SharedPtr<omsl::container::hcntcore::VectorCadEntityPtrConcreteForward> pTpSelectionVec;
            };

            class TPSpeedAnalysisModel :
                public omsl::command::CommandModelImpl<TPSpeedAnalysisModel>
                //, public ModelEntityConnection
            {
                OMSL_FRIEND_COMMANDMODEL_FACTORY ( TPSpeedAnalysis )
                    typedef omsl::command::CommandModelImpl<TPSpeedAnalysisModel> BaseClass;

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
                void OnModeChange ( const int& UI_GBox_RefSpeedType );
                SharedPtr<propertyservice::PropertyBag> Compute ( const int& );
                int OnSetupColorMap ( const int& UI_GBox_RefSpeedType );
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

                int HighlightToolPath ( const SharedPtr<iterator::hcntcore::CadEntityForward>& pTPEntsIt );

                /**
                * @brief react to the changes of the checkboxes in UI
                * @see \ref
                */
                int ChangeViewSettings ( const int& bFlag, const int& label );
                int OnChange_CB_Label ( const int& );
                int OnChange_CB_Orientation ( const int& );
                int OnChange_CB_Direction ( const int& );
                int OnChange_CB_Vector ( const int& );

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

                void ComputeOnPoints ( const SharedPtr<iterator::hcntcore::CadEntityForward>& pEnts1 );

                void ComputeOnEnts ( const SharedPtr<iterator::hcntcore::CadEntityForward>& pEnts1,
                                     const SharedPtr<iterator::hcntcore::CadEntityForward>& pEnts2 );

                void CalcBaseDataForTempGraphic ();


                bool CheckSignalTypeForCadEntityFRA
                (
                    omsl::hcntcore::CadEntity* pEnt
                    , int internal_HM_type
                    , int data
                );

                struct
                {
                    std::wstring s, ls;
                    int index;
                } ttdata;


                bool AddTTipLocString ( const wchar_t* name, int type, const wchar_t* locstring, int data )
                {
                    return true;
                }

                bool CheckToggleSignal ( int );
                /**
                * @brief enable / disable second selection button
                * @param [in] const int&   type of selection function
                */
                propertyservice::Applicable::Strength ActivateToolpath ( const int& );

            protected:
                TPSpeedAnalysisModel ( omsl::hcntcore::Document* pDocument );
                ~TPSpeedAnalysisModel ();


                //////////////////////////////////////////////////////////////////////////
                //  CommandModelImpl implementation
                //////////////////////////////////////////////////////////////////////////
                virtual void InternalApply ( void );
                virtual void InternalApply2 ( int , void*  [] )
                {
                    ;
                }
            private:
                TPSpeedAnalysisModel ( const TPSpeedAnalysisModel& )
                {}
                const TPSpeedAnalysisModel& operator= ( const TPSpeedAnalysisModel& );

                void ComputeAnalyseData ( const SharedPtr<iterator::hcntcore::CadEntityForward>& pEnts1 );

                void SetupTemporaryGraphics ();

                void SetupTemporaryGraphicsPickPoints ( const int viewModeParameter );
                void SetupTemporaryGraphicsDirection ( const int viewModeParameter);
                void SetupTemporaryGraphicsCompensation ( const int viewModeParameter);
                void SetupTemporaryGraphicsOrientation ( const int viewModeParameter);
                void SetupTemporaryGraphicsVector ( const int viewModeParameter);
                void SetupTemporaryGraphicsLabel ( const int viewModeParameter);
                void SetupCurrentToolDirection ( const int viewModeParameter );

                void SetupPropertyBag ();

                void SetupPropertyBag ( SharedPtr<propertyservice::PropertyBag> pBag );

                void fillToolpathPoints ( ToolpathSegmentPtr& pgep );

                omsl::geom::ToolpathSegment* CastToToolpathSegment ( const SharedPtr<hcntcore::CadEntity>& pEnt );

                bgeo::Point3d TranslateWithRefPoint ( const bgeo::Point3d& point );
                bgeo::Point3d TranslateWithRefPoint ( const bgeo::Coord3d& point );
                bgeo::Point3d GetPoint ( const SharedPtr<hcntcore::CadEntity> &ent_0, const unsigned int index );
                void StoreRefPoint ( const SharedPtr<hcntcore::CadEntity> &pEnt, const unsigned int index );
                void StoreEntityPoint ( const tpanalysis::CadEntitiesIt_T& pEnts, const unsigned int index );
                void StoreArcData ( SharedPtr<hcntcore::CadEntity> &pEnt, const unsigned int index );

                int getSpeedColorIndexMap ( std::map < double, int>& map );

                int setupToolpathColors ( const tpanalysis::CadEntitiesIt_T& pTPEntsIt );
                int createPointIdMaps ( const tpanalysis::CadEntitiesIt_T& pTPEntsIt );
                void ResetPreviewData ( const int type );

                WeakPtr<hcntcore::Document>       m_pDocument;

                compTPAMode1 m_compTPAMode;
                analyseData1 m_data;
                SharedPtr<omsl::WString>        m_descriptionPath;

//                ModelEntityConnectionPtr  m_mecp;

                bool m_withUpdate;
public:
                inline bool SignalListeningIsEnabled ()
                {
                    return m_enableSignalListening;
                }
                inline bool SetSignalListening ( const bool b )
                {
                    bool old = m_enableSignalListening;
                    //sl_callcounter++;
                    //sl_counter += ( b == true ? 1 : -1 );
                    m_enableSignalListening = b;
                    return old;
                }

            private:





                bool m_enableSignalListening = true;
                bool b_GVIcon = false;

                public:
                    std::vector<std::wstring>                          m_callsm;
                    int level = 0;
                    void callStackString ( wchar_t* s, int inout )
                    {
#ifdef DBGVER
                        if ( s )
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
                            m_callsm.push_back ( cls );
                        }
#endif
                    }
private:

            };
        }  // namespace tp_analyzer
    }  // namespace commands
}  // namespace omsl

#endif // TPSPEEDANALYSISMODEL_H__
