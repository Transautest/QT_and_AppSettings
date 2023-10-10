#pragma once
#ifndef TPANALYSISGUI_H__
#define TPANALYSISGUI_H__

/*!
* \file tpanalysisgui.h
*
* \author Uwe Spiess
* \date 24/04/2017
*
* contains the functionality to act with the GUI
*/

#include <smartpointers.h>
#include <command/commandguihandshake.h>
// Base classes
#include <command/commandgui.h>
#include <refcounterimpl.h>
#include <command\commandsmartpointer.h>
#include <basetypes.h>
#include <composer\compositionexportdefine.h>
#include <gui/handshaker.h>
#include <boostwrapper/connection.h>
#include <vector>
#include <geom/typeofgeomiterators.h>
#include <bgeo/coord3diterators.h>
#include <hcntcore/typeofcadentity.h>
#include <guitools\automaticguibinding.h>
#include "userfilter.h"

#ifndef CadEntitiesIt_T
typedef omsl::SharedPtr<omsl::iterator::hcntcore::CadEntityForward> CadEntitiesIt_T;
#endif

namespace omsl { namespace intool { class IntoolsManager; } }
namespace omsl { namespace hcntcore { class Document; } }
namespace omsl { namespace propertyservice { class Modality; } }
namespace omsl { namespace selectionservice { class InteractiveSelector; } }

class QTableWidget;
class ctkRangeSlider;

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

namespace omsl {
namespace commands {
namespace tp_analyzer {
/*!
 * \class TPAnalysisgui
 *
 * \brief implementation of TPAnalysis gui
 *
 * \author Leonardo Cecchinato
 * \date 22/09/2017
 */

    class TPAnalysisGuiHelper
    {
    public:

        virtual double ConvertDataUnits ( const double& dVal ) const
        {
            return dVal;
        }
        virtual const wchar_t* SelectorKey ();
        virtual bool UseCustomSelectingFilter () const
        {
            return false;
        }
        virtual hcntcore::CadEntity* ApplyFilterWhileSelecting ( hcntcore::CadEntity* pEntity ) const
        {
            return pEntity;
        }

        virtual bool ResetHistogram ( double * dMin, double * dMax );

        virtual bool AllowContinuousTooltipUpdate ()
        {
            return true;
        }
        virtual void TooltipHasBeenHidden () const
        {}
    };



class TPAnalysisGUI:
    public omsl::command::CommandGUI
  , public omsl::commands::tp_analyzer::TPAnalysisGuiHelper
{
  OMSL_FRIEND_COMMANDGUI_FACTORY( TPAnalysis )
  OMSL_ALLOW_PROTECTED_DTOR( TPAnalysisGUI );

public:
  OMSL_IMPLEMENT_WEAKABLE_REFCOUNTER
    OMSL_IMPLEMENT_CAST() {
      OMSL_ADD_CAST( CommandGUI );
      OMSL_ADD_CAST( TPAnalysisGUI );
      OMSL_DEFAULT_CAST();
  }
  //////////////////////////////////////////////////////////////////////////
  //  CommandGUI Interface implementation
  //////////////////////////////////////////////////////////////////////////
  OMSL_METHOD( omsl::Bool ) Initialize();
  //////////////////////////////////////////////////////////////////////////
  //  Object methods
  //////////////////////////////////////////////////////////////////////////
  /**
  * @brief used to setup the filters
  * @param [in] const int& mode 
*/

  // TPAnalysisGUI::rangeSliderIntervalChange ( int, int );

  omsl::commands::tp_analyzer::TPAnalysisGuiHelper * Helper () const
  {
      return m_pHelper;
  }
  void SetHelper ( omsl::commands::tp_analyzer::TPAnalysisGuiHelper * val )
  {
      m_pHelper = val;
  }
  //const wchar_t* UpdateMeasureUnitsKey () const;


public:

  void OnApply ( signal::Phase phase, command::Command* pCommand );
  void OnExit ( signal::Phase phase, command::Command* pCommand );
  void OnSelected ( const SharedPtr<iterator::hcntcore::CadEntityForward> & );

  OMSL_METHOD ( bool ) CheckSliderReset ( omsl::selectionservice::InteractiveSelector* pIntSel );

protected:
  TPAnalysisGUI(void* pParent, omsl::command::Command* pCommand, omsl::hcntcore::Document* pDocument );
  ~TPAnalysisGUI ();

  omsl::Long __stdcall OnModalityStarted ( propertyservice::Modality* );

  int Load( );

  void OnColorsDefinitionChanged ( const int& value );
  void OnResetPickPointChanged ( const int& value );

  void UpdateTableWidget ( QTableWidget *pListData, const Int& nLevels, const Double & dMin, const Double & dMax, int change );
  QTableWidget * FindTableWidget ( const wchar_t* widgetName );
  std::vector<QTableWidget*>                          m_vpListData;

  // user interaction
  void OnSliderRangeMinimum ( const Int& n );
  void OnSliderRangeMaximum ( const Int& n );
  void OnSliderUserMinimum ( const Int& n );
  void OnSliderUserMaximum ( const Int& n );
  void OnSliderEnable ( const Int& n );
  void OnSliderSetMinMax ( int dMin, int dMax );
  void UpdateSliderWidget ( ctkRangeSlider *slider, const int & dMin, const int & dMax );
  void OnChange_CB_MoveLimits ( const int&  );

private:
  OMSL_DECLARE_WEAKABLE_REFCOUNTER;
  TPAnalysisGUI(const TPAnalysisGUI&)  {}
  const TPAnalysisGUI& operator= (const TPAnalysisGUI&);

  /**
  * @brief receives the id of the selected functionality and setup filters and selection modi
  * @param[in] compTPAMode
  */
  void SetupSliderPickSelector ();
  void Setup1stSelector   ( );
  void Setup2ndSelector   ( );

//  void SetupTPathSelector ( const compTPAMode& );

public:
private:
  WeakPtr<omsl::command::Command>                   m_pCommand;
    
  WeakPtr<hcntcore::Document>                       m_pDocument;
  SharedPtr< omsl::gui::HandShaker>                 m_pParent; 
  std::vector<SharedPtr<boostwrapper::Connection>>  m_vConnections;
  SharedPtr<intool::IntoolsManager>                 m_pIntoolMgr;
  ToolpathUsrFilterPtr                              m_tpUsrFilter1;
  ToolpathUsrFilterPtr                              m_tpUsrFilter2;
  ToolpathUsrFilterPtr                              m_tpUsrLeft;
  ToolpathUsrFilterPtr                              m_tpUsrRight;
  WeakPtr<guitools::AutomaticGuiBinding> m_pBind;
  TPAnalysisGuiHelper                 *m_pHelper;
  SharedPtr<measureunit::Converter> m_pConverter;
  std::vector<int> m_ClrIndexVec;
  std::vector<double> m_colorTable;
  std::vector<double> m_speedTable;
  ctkRangeSlider *pRangeSlider;
  omsl::Bool m_postfix;
};

}  // namespace tp_analyzer
}  // namespace commands
}  // namespace omsl

#endif // TPANALYSISGUI_H__
