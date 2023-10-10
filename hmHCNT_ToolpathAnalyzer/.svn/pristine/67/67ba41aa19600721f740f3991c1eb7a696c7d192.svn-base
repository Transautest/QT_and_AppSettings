#pragma once
#ifndef TPSPEEDANALYSISKEYS_H__
#define TPSPEEDANALYSISKEYS_H__

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

#ifndef CadEntitiesIt_T
typedef omsl::SharedPtr<omsl::iterator::hcntcore::CadEntityForward> CadEntitiesIt_T;
#endif

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
// UI data
OMSL_COMMAND_KEY(tp_analyzer.TPSpeedAnalysis,  UI_RefSpeedType,  int, L"UI_GBox_RefSpeedType",   L"type of selected speed");
OMSL_COMMAND_KEY(tp_analyzer.TPSpeedAnalysis,  UI_RefOutputType, int, L"UI_GBox_RefOutputType", L"type of selected function" );
OMSL_COMMAND_KEY(tp_analyzer.TPSpeedAnalysis,  UI_SpeedAnalyseResult, SharedPtr<propertyservice::PropertyBag>, L"UI_Panel_SpeedAnalyseResult",  L"Output of calculation");
OMSL_COMMAND_KEY(tp_analyzer.TPSpeedAnalysis,  UI_ColorSettingsPanel, SharedPtr<propertyservice::PropertyBag>, L"UI_ColorSettingsPanel", L"Input settings" );
OMSL_COMMAND_KEY(tp_analyzer.TPSpeedAnalysis,  DummyParameter,         int,              L"DummyParameter",  L"internal value");
OMSL_COMMAND_KEY(tp_analyzer.TPSpeedAnalysis,  ToolpathCounter,        int, L"ToolpathCounter", L"internal value" );
OMSL_COMMAND_KEY(tp_analyzer.TPSpeedAnalysis,  UI_Toolpath    , CadEntitiesIt_T, L"UI_Btn_Toolpath", L"Toolpathselect" );

OMSL_COMMAND_KEY ( tp_analyzer.TPSpeedAnalysis, UI_Direction, int, L"UI_CB_Direction", L"type of selected functionality" );
OMSL_COMMAND_KEY ( tp_analyzer.TPSpeedAnalysis, UI_Label, int, L"UI_CB_Label", L"type of selected functionality" );
OMSL_COMMAND_KEY ( tp_analyzer.TPSpeedAnalysis, UI_Vector, int, L"UI_CB_Vector", L"type of selected functionality" );

OMSL_HCD_COMMAND_KEY ( tp_analyzer.TPSpeedAnalysis, TGr_LabelVectors, TopoGeomIt_T, L"TGr_LabelVectors", L"This are my computed entities: use for preview!" );

OMSL_HCD_COMMAND_KEY ( tp_analyzer.TPSpeedAnalysis, TGr_TextPoints, Coord3dIt_T, L"TGr_TextPoints", L"Text Points" );
OMSL_HCD_COMMAND_KEY ( tp_analyzer.TPAnalysis, TGr_Texts, WCharIt_T, L"TGr_Texts", L"Texts" );

}  // namespace tpanalysis 
}  // namespace tp_analyzer
}  // namespace commands
}  // namespace omsl
#endif // TPSPEEDANALYSISKEYS_H__
