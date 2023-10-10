#pragma once
#ifndef TPANALYSISKEYS_H__
#define TPANALYSISKEYS_H__

/*!
* \file tpanalysiskeys.h
*
* \author Uwe Spiess
* \date 24/04/2017
*
* contains the defines for the interaction with the UI
*/

#include <command\commandparameterimplt.h>
#include <hcntcore/typeofcadentity.h>
#include <hcntcore/referencesystem.h>
#include <hcntcore/referencesystemfree.h>
#include <geom/typeofgeomiterators.h>
#include <hcntcore/typeofreferencesystem.h>
#include <bgeo/typeofbgeotypes.h>
#include <propertyservice/typeofproperty.h>
#include <bgeo/coord3dvectorcontainers.h>
#include <bgeo/coord3diterators.h>
#include <bgeo/axis3diterators.h>

#include <boost/lexical_cast.hpp>
#include <measureunit\converter.h>
#include <measureunit\utility.h>
#include <wstringimpl.h>
#include <geom/typeofanalysisfunction.h>
#include <commands/info/algorithmparameters.h>
//#include <commands/info/analysiskeys.h>
#ifndef CadEntitiesIt_T
typedef omsl::SharedPtr<omsl::iterator::hcntcore::CadEntityForward> CadEntitiesIt_T;
#endif

#define GENERAL_COLOR_SIZE 9 // for clipPath
#define ADDITIONAL_FEEDRATE_COLORLIMIT 105
#define DEFAULTSPEEDMIN 0.0
#define DEFAULTSPEEDMAX 50000.0
#define DEFAULTSPEEDLVL 10
#define DEFAULTSPEEDLVLMAX (ADDITIONAL_FEEDRATE_COLORLIMIT-5)
#define DEFAULTSEGMAX 99999999
#define MINIMUMSEGGAP 5


namespace omsl
{
    typedef std::map<double, int> SpeedClrIndexMap;
    OMSL_TYPEOF_DEF ( SpeedClrIndexMap );

}

namespace omsl {
namespace commands {
namespace tp_analyzer {
namespace tpanalysis 
{
typedef SharedPtr<iterator::geom::TopoGeomPtrForward> TopoGeomIt_T;
typedef SharedPtr<iterator::bgeo::Coord3dForward> Coord3dIt_T;
typedef SharedPtr<iterator::WCharTForward> WCharIt_T;
typedef SharedPtr<iterator::bgeo::Axis3dForward> AxisIt_T;
typedef SharedPtr<hcntcore::workplane::ReferenceSystem> RefSys_T;
typedef SharedPtr<hcntcore::workplane::ReferenceSystemFree> FreeRefSys_T;
typedef SharedPtr<iterator::ULongForward> ULongIt_T;
typedef SharedPtr<iterator::IntForward> IntIt_T;
typedef SharedPtr<iterator::IntRandom> IntRIt_T;
typedef bgeo::RigidMotion3d Motion_T;

enum compTPAMode
{
    cm_TPAFirst = 0                /*!< marker for the first entry , internal */
    , cm_Point = 0                /*!< single toolpath point measurements */
    , cm_Movement                 /*!< movement data*/
    , cm_Geometry                 /*!< geometry data*/
    , cm_Point_Point              /*!< point to point measurements */
    , cm_Point_Movement           /*!< point to movement measurements */
    , cm_Point_Geometry           /*!< point to geometry measurements */
    , cm_Movement_Movement        /*!< movement to movement measurements */
    , cm_Movement_Geometry        /*!< movement to geometry measurements */
    , cm_Toolpath                 /*!< toolpath data */
    , cm_TPALast = cm_Toolpath       /*!< marker for the last entry , internal*/
};

enum compFRAMode
{
    cm_FRAFirst = 0
    , cm_Feedrate = 0
    , cm_Spindelspeed
    , cm_G_movement
    , cm_FRALast = cm_G_movement
};


// UI general display 
OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, UI_Direction   , int, L"UI_CB_Direction", L"type of selected functionality" );
OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, UI_Label       , int, L"UI_CB_Label", L"type of selected functionality" );
OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, UI_Vector      , int, L"UI_CB_Vector", L"type of selected functionality" );
OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, UI_Point       , int, L"UI_CB_Point", L"type of selected functionality" );
//OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, UI_Tool        , int, L"UI_CB_Tool", L"type of selected functionality" );
OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, UI_FeedRate    , int, L"UI_CB_FeedRate", L"type of selected functionality" );
OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, UI_ClearanceLevel, int, L"UI_CB_ClearanceAsMax", L"type of selected functionality" );
OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, UI_AutoUpdateMin, int, L"UI_CB_AutoUpdateMin", L"type of selected functionality" );
OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, UI_AutoUpdateMax, int, L"UI_CB_AutoUpdateMax", L"type of selected functionality" );
OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, UI_AutoUpdateLvl, int, L"UI_CB_AutoUpdateLevels", L"type of selected functionality" );

//OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, UI_Orientation , int, L"UI_CB_Orientation", L"type of selected functionality" );
//OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, UI_Compensation, int, L"UI_CB_Compensation", L"type of selected functionality" );

// common tempgraphic data
OMSL_HCD_COMMAND_KEY(tp_analyzer.TPAnalysis, TGr_ReferenceSystem, Motion_T,  L"TGr_ReferenceSystem", L"This are my computed entities: use for preview!");
OMSL_HCD_COMMAND_KEY ( tp_analyzer.TPAnalysis, WorkplaneEnabled, bool, L"WorkplaneEnabled", L"This are my computed entities: use for preview!" );
//OMSL_HCD_COMMAND_KEY(tp_analyzer.TPAnalysis, TGr_ReferenceArrowS, AxisIt_T, L"TGr_ReferenceArrowS", L"This are my computed entities: use for preview!");
//OMSL_HCD_COMMAND_KEY(tp_analyzer.TPAnalysis, TGr_ReferenceArrowE, AxisIt_T, L"TGr_ReferenceArrowE", L"This are my computed entities: use for preview!");
OMSL_HCD_COMMAND_KEY(tp_analyzer.TPAnalysis, TGr_ReferenceArrowC, AxisIt_T, L"TGr_ReferenceArrowC", L"This are my computed entities: use for preview!" );

//OMSL_HCD_COMMAND_KEY(tp_analyzer.TPAnalysis,    TGr_CompensationVectors,TopoGeomIt_T,  L"TGr_CompensationVectors", L"This are my computed entities: use for preview!");
OMSL_HCD_COMMAND_KEY(tp_analyzer.TPAnalysis,    TGr_OrientationVectorsL, TopoGeomIt_T,  L"TGr_OrientationVectorsL", L"This are my computed entities : use for preview!");
OMSL_HCD_COMMAND_KEY(tp_analyzer.TPAnalysis,    TGr_OrientationVectorsR, TopoGeomIt_T, L"TGr_OrientationVectorsR", L"This are my computed entities : use for preview!" );
OMSL_HCD_COMMAND_KEY(tp_analyzer.TPAnalysis,    TGr_LabelVectors,        TopoGeomIt_T,  L"TGr_LabelVectors", L"This are my computed entities: use for preview!");
OMSL_HCD_COMMAND_KEY(tp_analyzer.TPAnalysis,    TGr_TextPoints,          Coord3dIt_T,    L"TGr_TextPoints", L"Text Points" );
OMSL_HCD_COMMAND_KEY(tp_analyzer.TPAnalysis,    TGr_Texts,               WCharIt_T,      L"TGr_Texts", L"Texts" );
OMSL_HCD_COMMAND_KEY(tp_analyzer.TPAnalysis,    TGr_Tool,                TopoGeomIt_T, L"TGr_Tool", L"This are my computed entities : use for preview!" );
OMSL_HCD_COMMAND_KEY(tp_analyzer.TPAnalysis,    TGr_MovementArcMarkers   ,TopoGeomIt_T, L"TGr_MovementArcMarkers", L"This are my computed entities : use for preview!" );

// graphic for measurement
OMSL_HCD_COMMAND_KEY ( tp_analyzer.TPAnalysis, Msr_DistPointsTexts, WCharIt_T, L"Msr_DistPointsTexts", L"Texts" );
OMSL_HCD_COMMAND_KEY ( tp_analyzer.TPAnalysis, Msr_TextPoints, Coord3dIt_T, L"Msr_TextPoints", L"Text Points" );
OMSL_HCD_COMMAND_KEY ( tp_analyzer.TPAnalysis, Msr_DistanceTexts, WCharIt_T, L"Msr_DistanceTexts", L"Texts" );
OMSL_HCD_COMMAND_KEY ( tp_analyzer.TPAnalysis, Msr_DistanceTextPoints, Coord3dIt_T, L"Msr_DistanceTextPoints", L"Text Points" );
OMSL_HCD_COMMAND_KEY ( tp_analyzer.TPAnalysis, Msr_DistanceLine, TopoGeomIt_T, L"Msr_DistanceLine", L"This are my computed entities: use for preview!" );
OMSL_HCD_COMMAND_KEY ( tp_analyzer.TPAnalysis, Msr_DistPoints, Coord3dIt_T, L"Msr_DistPoints", L"Text Points" );

// general tab

OMSL_COMMAND_KEY(tp_analyzer.TPAnalysis,    UI_Tab_General, int, L"UI_Tab_General", L"This are my computed entities : use for preview!" );

// TPA selections // not necessary yet
//OMSL_HCD_COMMAND_KEY ( tp_analyzer.TPAnalysis, TGr_PreviewPoints_Ent1, Coord3dIt_T, L"TGr_PreviewPoints_Ent1", L"toolpathpoints_L" );
// obsolete OMSL_HCD_COMMAND_KEY(tp_analyzer.TPAnalysis,    TGr_PreviewPoints_C,    Coord3dIt_T, L"TGr_PreviewPoints_C", L"toolpathpoints_C" );
//OMSL_HCD_COMMAND_KEY ( tp_analyzer.TPAnalysis, TGr_PreviewPoints_Ent2, Coord3dIt_T, L"TGr_PreviewPoints_Ent2", L"toolpathpoints_R" );
//OMSL_HCD_COMMAND_KEY(tp_analyzer.TPAnalysis,    TGr_PreviewPoints_R3rd, Coord3dIt_T, L"TGr_PreviewPoints_R3rd", L"toolpathpoints_R3rd" );


// UI data TPA
OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, UI_FirstEntities, CadEntitiesIt_T, L"UI_Btn_FirstEntities", L" 1st selected" );
OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, UI_SecondEntities, CadEntitiesIt_T, L"UI_Btn_SecondEntities", L" 2nd selection" );
OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, UI_RefSystemType, int, L"UI_GBox_RefSystemType", L"ID of selected UCS" );
//OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, UI_RefPoint, SharedPtr<hcntcore::CadEntity>, L"UI_Btn_RefPoint", L"additional Reference Point" );
OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, UI_AnalysisResult, SharedPtr<propertyservice::PropertyBag>, L"UI_Panel_AnalysisResult", L"Output of calculation" );
OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, UI_Settings, SharedPtr<propertyservice::PropertyBag>, L"UI_Panel_Settings", L"Output of calculation" );
OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, DummyParameter, int, L"DummyParameter", L"internal value" );
OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, TabIndex, int, L"TabIndex", L"internal value" );
//OMSL_COMMAND_KEY(tp_analyzer.TPAnalysis,        ToolpathCounter,        int, L"ToolpathCounter", L"internal value" );
//OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, UI_Sel_TPA_Mode, int, L"UI_Sel_TPA_Mode", L"type of selected functionality" );
OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, UI_SpinBox_MoveNr, int, L"UI_SpinBox_MoveNr", L"type of selected functionality" );
// UI data Setting

OMSL_PRIVATE_COMMAND_KEY ( tp_analyzer.TPAnalysis, UI_Env_ShowAdditionalPathes , Int, L"UI_Env_ShowAdditionalPathes", L"addditonal toolpath components" );
// UI color table

OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, UI_feedrateLevels, Int, L"UI_TargetLevels", L"The number of target values" );
OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, UI_feedrateLevelsAuto, Int, L"UI_TargetLevelsAuto", L"The number of target values" );
OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, UI_feedrateValueMin, Double, L"UI_TargetValueMin", L"The min value of target" );
OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, UI_feedrateValueMax, Double, L"UI_TargetValueMax", L"The max value of target" );
OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, UI_colorMapMin_2, Double, L"UI_ColorMapMin_2", L"The min value of target" );
OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, UI_colorMapMax_2, Double, L"UI_ColorMapMax_2", L"The max value of target" );
OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, UI_colorMapCMax_3, Double, L"UI_ColorMapCMax_3", L"The max value of target" );

//OMSL_PRIVATE_COMMAND_KEY ( info.Analysis, intClrNr, Int, L"intClrNr", L"id what was changed" );
OMSL_PRIVATE_COMMAND_KEY ( tp_analyzer.TPAnalysis, intClrIdxVec, omsl::Std_Int_Vector, L"SpeedIndexVec", L"Face Inversions map" );
OMSL_PRIVATE_COMMAND_KEY ( tp_analyzer.TPAnalysis, doubleClrVec, omsl::Std_Double_Vector, L"RGBColorTable", L"Mesh Normals map" );
OMSL_PRIVATE_COMMAND_KEY ( tp_analyzer.TPAnalysis, doubleSpeedVec, omsl::Std_Double_Vector, L"RGBIndexVec", L"Mesh Normals map" );
OMSL_PRIVATE_COMMAND_KEY ( tp_analyzer.TPAnalysis, numberOfDifferentFeedrates, int, L"numberOfDifferentFeedrates", L"internal value, used for updating the clrtable" );
OMSL_PRIVATE_COMMAND_KEY ( tp_analyzer.TPAnalysis, clrTblRoundingPrec, int, L"clrTblRoundingPrec", L"internal value, depends on turning / milling jobs" );
OMSL_PRIVATE_COMMAND_KEY ( tp_analyzer.TPAnalysis, clrTblRoundingLimit, double, L"clrTblRoundingLimit", L"internal value, depends on turning / milling jobs" );
OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, updateColorsTableAllowed, int, L"updateColorsTableAllowed", L"internal value" );
OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, updateFeedrateDraw, int, L"updateFeedrateDraw", L"internal value" );
OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, updateSliderDraw, int, L"updateSliderDraw", L"internal value" );
OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, useExactFeedrates, int, L"useExactFeedrates", L"type of selected functionality" );
// UI double slider
// TP limit settings

OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, UI_DblSldr_Limits, int, L"UI_DblSldr_Limits", L"type of selected functionality" );

OMSL_PRIVATE_COMMAND_KEY ( tp_analyzer.TPAnalysis, resetPickPointSelection, int, L"resetPickPointSelection", L"internal value" );
OMSL_PRIVATE_COMMAND_KEY ( tp_analyzer.TPAnalysis, showLayer25, int, L"showLayer25", L"internal value" );
OMSL_PRIVATE_COMMAND_KEY ( tp_analyzer.TPAnalysis, sl25_cb_min, int, L"sl25_cb_min", L"internal value" );
OMSL_PRIVATE_COMMAND_KEY ( tp_analyzer.TPAnalysis, sl25_cb_max, int, L"sl25_cb_max", L"internal value" );
OMSL_PRIVATE_COMMAND_KEY ( tp_analyzer.TPAnalysis, sl25_cb_lvl, int, L"sl25_cb_lvl", L"internal value" );
OMSL_PRIVATE_COMMAND_KEY ( tp_analyzer.TPAnalysis, sl25_cb_clr, int, L"sl25_cb_clr", L"internal value" );
OMSL_PRIVATE_COMMAND_KEY ( tp_analyzer.TPAnalysis, sl25_in_min, double, L"sl25_in_min", L"internal value" );
OMSL_PRIVATE_COMMAND_KEY ( tp_analyzer.TPAnalysis, sl25_in_max, double, L"sl25_in_max", L"internal value" );
OMSL_PRIVATE_COMMAND_KEY ( tp_analyzer.TPAnalysis, sl25_in_clr, double, L"sl25_in_clr", L"internal value" );
OMSL_PRIVATE_COMMAND_KEY ( tp_analyzer.TPAnalysis, sl25_in_lvl, int, L"sl25_in_lvl", L"internal value" );

//
OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, UI_sliderValueMin, Int, L"UI_sliderValueMin", L"The min value of target" );
OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, UI_sliderValueMax, Int, L"UI_sliderValueMax", L"The max value of target" );
OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, UI_sliderMin_2, Int, L"UI_sliderMin_2", L"The min value of target" );
OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, UI_sliderMax_2, Int, L"UI_sliderMax_2", L"The max value of target" );
OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, UI_sliderEnable, Int, L"UI_sliderEnable", L"The max value of target" );

OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, UI_LeftSliderPick, SharedPtr<hcntcore::CadEntity>, L"UI_Btn_LeftPick", L"left selected" );
OMSL_COMMAND_KEY ( tp_analyzer.TPAnalysis, UI_RightSliderPick, SharedPtr<hcntcore::CadEntity>, L"UI_Btn_RightPick", L"right selected" );

}  // namespace tpanalysis 
}  // namespace tp_analyzer
}  // namespace commands
}  // namespace omsl
#endif // TPANALYSISKEYS_H__
