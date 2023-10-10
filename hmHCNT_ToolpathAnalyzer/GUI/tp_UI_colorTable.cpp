#include <propertyservice/propertybag/propertytypedbag.h>
#include <propertyservice/typednametdef.h>
#include <designerplugins\qhistograminterface.h>
#include <math/histogram.h>
#include <guitools\automaticguibinding.h>
#include "tpanalysisgui.h"
// keys
#include <Commands\tpanalysiskeys.h>
#include <Commands\tpanalysepropertieskeys.h>
// command context
#include <propertyservice/context/context.h>
#include <command/command.h>
#include <command/commandsignal.h>
#include "../Commands/tpanalysepropertiesimpl.h"
// Document
#include <hcntcore\document.h>

#include <intool\intoolsmanager.h>
#include <intool\selector.h>
#include <selectionservice/filter.h>
#include <selectionservice/filters.h>
#include <selectionservice/entityfilter.h>
#include <selectionservice/interactiveselector.h>
#include <selectionservice/selectionservice.h>
#include <selectionservice/snap.h>
#include <selectionservice/entityfilterutilities.h>
#include <gui\guihandshake.h>
#include <hcntcore/application.h>
#include <selectionservice/utils.h>
#include <commands/info/analysiskeys.h>
#include <commands\common_functions.h>
#include <propertyservice/propertybag/minmaxvalidatort.h>
#include <translate.h>

#include <materials/gradient.h>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTableWidgetItem>
#include <QtWidgets/QHeaderView>

#include "ctkRangeSlider.h"

using namespace omsl::command;
using namespace omsl::commands::info;
using namespace omsl::commands::tp_analyzer;
using omsl::linguistic::QT_TRANSLATE_NOOP;

//void ( *s_pFctIntervalChanged )( int, int ) = nullptr;
extern omsl::WeakPtr<omsl::command::Command>            s_pCommand;

struct qt_meta_stringdata_ctkRangeSlider_t
{
    QByteArrayData data [22];
    char stringdata0 [316];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ctkRangeSlider_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ctkRangeSlider_t qt_meta_stringdata_ctkRangeSlider = {
    {
QT_MOC_LITERAL ( 0, 0, 29 ), // "ctkRangeSlider"
QT_MOC_LITERAL ( 1, 30, 19 ), // "minimumValueChanged"
QT_MOC_LITERAL ( 2, 50, 0 ), // ""
QT_MOC_LITERAL ( 3, 51, 3 ), // "min"
QT_MOC_LITERAL ( 4, 55, 19 ), // "maximumValueChanged"
QT_MOC_LITERAL ( 5, 75, 3 ), // "max"
QT_MOC_LITERAL ( 6, 79, 13 ), // "valuesChanged"
QT_MOC_LITERAL ( 7, 93, 22 ), // "minimumPositionChanged"
QT_MOC_LITERAL ( 8, 116, 22 ), // "maximumPositionChanged"
QT_MOC_LITERAL ( 9, 139, 16 ), // "positionsChanged"
QT_MOC_LITERAL ( 10, 156, 15 ), // "setMinimumValue"
QT_MOC_LITERAL ( 11, 172, 15 ), // "setMaximumValue"
QT_MOC_LITERAL ( 12, 188, 9 ), // "setValues"
QT_MOC_LITERAL ( 13, 198, 14 ), // "onRangeChanged"
QT_MOC_LITERAL ( 14, 213, 7 ), // "minimum"
QT_MOC_LITERAL ( 15, 221, 7 ), // "maximum"
QT_MOC_LITERAL ( 16, 229, 12 ), // "minimumValue"
QT_MOC_LITERAL ( 17, 242, 12 ), // "maximumValue"
QT_MOC_LITERAL ( 18, 255, 15 ), // "minimumPosition"
QT_MOC_LITERAL ( 19, 271, 15 ), // "maximumPosition"
QT_MOC_LITERAL ( 20, 287, 14 ), // "symmetricMoves"
QT_MOC_LITERAL ( 21, 302, 13 ) // "handleToolTip"

    },
    "ctkRangeSlider\0"
    "minimumValueChanged\0\0min\0maximumValueChanged\0"
    "max\0valuesChanged\0minimumPositionChanged\0"
    "maximumPositionChanged\0positionsChanged\0"
    "setMinimumValue\0setMaximumValue\0"
    "setValues\0onRangeChanged\0minimum\0"
    "maximum\0minimumValue\0maximumValue\0"
    "minimumPosition\0maximumPosition\0"
    "symmetricMoves\0handleToolTip"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ctkRangeSlider [] = {

    // content:
          7,       // revision
          0,       // classname
          0,    0, // classinfo
         10,   14, // methods
          6,  102, // properties
          0,    0, // enums/sets
          0,    0, // constructors
          0,       // flags
          6,       // signalCount

    // signals: name, argc, parameters, tag, flags
          1,    1,   64,    2, 0x06 /* Public */,
          4,    1,   67,    2, 0x06 /* Public */,
          6,    2,   70,    2, 0x06 /* Public */,
          7,    1,   75,    2, 0x06 /* Public */,
          8,    1,   78,    2, 0x06 /* Public */,
          9,    2,   81,    2, 0x06 /* Public */,

          // slots: name, argc, parameters, tag, flags
               10,    1,   86,    2, 0x0a /* Public */,
               11,    1,   89,    2, 0x0a /* Public */,
               12,    2,   92,    2, 0x0a /* Public */,
               13,    2,   97,    2, 0x09 /* Protected */,

               // signals: parameters
                  QMetaType::Void, QMetaType::Int,    3,
                  QMetaType::Void, QMetaType::Int,    5,
                  QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    5,
                  QMetaType::Void, QMetaType::Int,    3,
                  QMetaType::Void, QMetaType::Int,    5,
                  QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    5,

                  // slots: parameters
                     QMetaType::Void, QMetaType::Int,    3,
                     QMetaType::Void, QMetaType::Int,    5,
                     QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    5,
                     QMetaType::Void, QMetaType::Int, QMetaType::Int,   14,   15,

                     // properties: name, type, flags
                          16, QMetaType::Int, 0x00095103,
                          17, QMetaType::Int, 0x00095103,
                          18, QMetaType::Int, 0x00095103,
                          19, QMetaType::Int, 0x00095103,
                          20, QMetaType::Bool, 0x00095103,
                          21, QMetaType::QString, 0x00095103,

                           0        // eod
};

            void ctkRangeSlider::maximumPositionChanged ( int /*_t1*/ )
            {
                //void *_a [] = { Q_NULLPTR, const_cast< void* >( reinterpret_cast< const void* >( &_t1 ) ) };
                //QMetaObject::activate ( this, &staticMetaObject, 4, _a );
            }
            void ctkRangeSlider::minimumPositionChanged ( int /*_t1*/ )
            {
                //void *_a [] = { Q_NULLPTR, const_cast< void* >( reinterpret_cast< const void* >( &_t1 ) ) };
                //QMetaObject::activate ( this, &staticMetaObject, 4, _a );
            }
            void ctkRangeSlider::positionsChanged ( int /*_t1*/, int /*t2*/ )
            {
                //void *_a [] = { Q_NULLPTR, const_cast< void* >( reinterpret_cast< const void* >( &_t1 ) ) };
                //QMetaObject::activate ( this, &staticMetaObject, 4, _a );
            }
            void ctkRangeSlider::valuesChanged ( int _t1, int t2 )
            {
                int gap = t2 - _t1;
                int oldMin = s_pCommand->GetContext ()->GetValue<int> ( tpanalysis::UI_sliderValueMin );
                int oldMax = s_pCommand->GetContext ()->GetValue<int> ( tpanalysis::UI_sliderValueMax );
                bool bn =  oldMin != _t1;
                bool bx =  oldMax != t2;

                if ( bn && bx ) // both moved
                {
                    s_pCommand->GetContext ()->SetValue ( tpanalysis::UI_sliderValueMin, _t1 );
                    s_pCommand->GetContext ()->SetValue ( tpanalysis::UI_sliderValueMax, t2 );
                }
                else if ( bn && gap >= MINIMUMSEGGAP )
                {
                    s_pCommand->GetContext ()->SetValue ( tpanalysis::UI_sliderValueMin, _t1 );
                }
                else if ( bx && gap >= MINIMUMSEGGAP )
                {
                    s_pCommand->GetContext ()->SetValue ( tpanalysis::UI_sliderValueMax, t2 );
                }
                
                int val = s_pCommand->GetContext ()->GetValue <int>( tpanalysis::updateSliderDraw );
                s_pCommand->GetContext ()->SetValue ( tpanalysis::updateSliderDraw,val +1 );

                //void *_a [] = { Q_NULLPTR, const_cast< void* >( reinterpret_cast< const void* >( &_t1 ) ) };
                //QMetaObject::activate ( this, &staticMetaObject, 4, _a );
            }
            void ctkRangeSlider::maximumValueChanged ( int /*_t1*/ )
            {
                //void *_a [] = { Q_NULLPTR, const_cast< void* >( reinterpret_cast< const void* >( &_t1 ) ) };
                //QMetaObject::activate ( this, &staticMetaObject, 4, _a );
            }
            void ctkRangeSlider::minimumValueChanged ( int /*_t1*/ )
            {
                //void *_a [] = { Q_NULLPTR, const_cast< void* >( reinterpret_cast< const void* >( &_t1 ) ) };
                //QMetaObject::activate ( this, &staticMetaObject, 4, _a );
            }

            const QMetaObject *ctkRangeSlider::metaObject () const
            {
                return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject () : &staticMetaObject;
            }


            void *ctkRangeSlider::qt_metacast ( const char *_clname )
            {
                if ( !_clname ) return Q_NULLPTR;
//                if ( !strcmp ( _clname, qt_meta_stringdata_ctkRangeSlider.stringdata0 ) )
                    return static_cast< void* >( const_cast< ctkRangeSlider* >( this ) );
                //return QSlider::qt_metacast ( _clname );
            }

            int ctkRangeSlider::qt_metacall ( QMetaObject::Call _c, int _id, void **_a )
            {
                _id = QSlider::qt_metacall ( _c, _id, _a );
                if ( _id < 0 )
                    return _id;
                if ( _c == QMetaObject::InvokeMetaMethod )
                {
                    if ( _id < 10 )
                        qt_static_metacall ( this, _c, _id, _a );
                    _id -= 10;
                }
                else if ( _c == QMetaObject::RegisterMethodArgumentMetaType )
                {
                    if ( _id < 10 )
                        *reinterpret_cast< int* >( _a [0] ) = -1;
                    _id -= 10;
                }
#ifndef QT_NO_PROPERTIES
                else if ( _c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
                          || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType )
                {
                    qt_static_metacall ( this, _c, _id, _a );
                    _id -= 6;
                }
                else if ( _c == QMetaObject::QueryPropertyDesignable )
                {
                    _id -= 6;
                }
                else if ( _c == QMetaObject::QueryPropertyScriptable )
                {
                    _id -= 6;
                }
                else if ( _c == QMetaObject::QueryPropertyStored )
                {
                    _id -= 6;
                }
                else if ( _c == QMetaObject::QueryPropertyEditable )
                {
                    _id -= 6;
                }
                else if ( _c == QMetaObject::QueryPropertyUser )
                {
                    _id -= 6;
                }
#endif // QT_NO_PROPERTIES
                return _id;
            }

            void ctkRangeSlider::qt_static_metacall ( QObject *_o, QMetaObject::Call _c, int _id, void **_a )
            {
                if ( _c == QMetaObject::InvokeMetaMethod )
                {
                    ctkRangeSlider *_t = static_cast< ctkRangeSlider * >( _o );
                    Q_UNUSED ( _t )
                        switch ( _id )
                        {
                            case 0: _t->minimumValueChanged ( ( *reinterpret_cast< int ( *) >( _a [1] ) ) ); break;
                            case 1: _t->maximumValueChanged ( ( *reinterpret_cast< int ( *) >( _a [1] ) ) ); break;
                            case 2: _t->valuesChanged ( ( *reinterpret_cast< int ( *) >( _a [1] ) ), ( *reinterpret_cast< int ( *) >( _a [2] ) ) ); break;
                            case 3: _t->minimumPositionChanged ( ( *reinterpret_cast< int ( *) >( _a [1] ) ) ); break;
                            case 4: _t->maximumPositionChanged ( ( *reinterpret_cast< int ( *) >( _a [1] ) ) ); break;
                            case 5: _t->positionsChanged ( ( *reinterpret_cast< int ( *) >( _a [1] ) ), ( *reinterpret_cast< int ( *) >( _a [2] ) ) ); break;
                            case 6: _t->setMinimumValue ( ( *reinterpret_cast< int ( *) >( _a [1] ) ) ); break;
                            case 7: _t->setMaximumValue ( ( *reinterpret_cast< int ( *) >( _a [1] ) ) ); break;
                            case 8: _t->setValues ( ( *reinterpret_cast< int ( *) >( _a [1] ) ), ( *reinterpret_cast< int ( *) >( _a [2] ) ) ); break;
                            case 9: _t->onRangeChanged ( ( *reinterpret_cast< int ( *) >( _a [1] ) ), ( *reinterpret_cast< int ( *) >( _a [2] ) ) ); break;
                            default:;
                        }
                }
                else if ( _c == QMetaObject::IndexOfMethod )
                {
                    int *result = reinterpret_cast< int * >( _a [0] );
                    void **func = reinterpret_cast< void ** >( _a [1] );
                    {
                        typedef void ( ctkRangeSlider::*_t )( int );
                        if ( *reinterpret_cast< _t * >( func ) == static_cast< _t >( &ctkRangeSlider::minimumValueChanged ) )
                        {
                            *result = 0;
                            return;
                        }
                    }
                    {
                        typedef void ( ctkRangeSlider::*_t )( int );
                        if ( *reinterpret_cast< _t * >( func ) == static_cast< _t >( &ctkRangeSlider::maximumValueChanged ) )
                        {
                            *result = 1;
                            return;
                        }
                    }
                    {
                        typedef void ( ctkRangeSlider::*_t )( int, int );
                        if ( *reinterpret_cast< _t * >( func ) == static_cast< _t >( &ctkRangeSlider::valuesChanged ) )
                        {
                            *result = 2;
                            return;
                        }
                    }
                    {
                        typedef void ( ctkRangeSlider::*_t )( int );
                        if ( *reinterpret_cast< _t * >( func ) == static_cast< _t >( &ctkRangeSlider::minimumPositionChanged ) )
                        {
                            *result = 3;
                            return;
                        }
                    }
                    {
                        typedef void ( ctkRangeSlider::*_t )( int );
                        if ( *reinterpret_cast< _t * >( func ) == static_cast< _t >( &ctkRangeSlider::maximumPositionChanged ) )
                        {
                            *result = 4;
                            return;
                        }
                    }
                    {
                        typedef void ( ctkRangeSlider::*_t )( int, int );
                        if ( *reinterpret_cast< _t * >( func ) == static_cast< _t >( &ctkRangeSlider::positionsChanged ) )
                        {
                            *result = 5;
                            return;
                        }
                    }
                }
#ifndef QT_NO_PROPERTIES
                else if ( _c == QMetaObject::ReadProperty )
                {
                    ctkRangeSlider *_t = static_cast< ctkRangeSlider * >( _o );
                    Q_UNUSED ( _t )
                        void *_v = _a [0];
                    switch ( _id )
                    {
                        case 0: *reinterpret_cast< int* >( _v ) = _t->minimumValue (); break;
                        case 1: *reinterpret_cast< int* >( _v ) = _t->maximumValue (); break;
                        case 2: *reinterpret_cast< int* >( _v ) = _t->minimumPosition (); break;
                        case 3: *reinterpret_cast< int* >( _v ) = _t->maximumPosition (); break;
                        case 4: *reinterpret_cast< bool* >( _v ) = _t->symmetricMoves (); break;
                        case 5: *reinterpret_cast< QString* >( _v ) = _t->handleToolTip (); break;
                        default: break;
                    }
                }
                else if ( _c == QMetaObject::WriteProperty )
                {
                    ctkRangeSlider *_t = static_cast< ctkRangeSlider * >( _o );
                    Q_UNUSED ( _t )
                        void *_v = _a [0];
                    switch ( _id )
                    {
                        case 0: _t->setMinimumValue ( *reinterpret_cast< int* >( _v ) ); break;
                        case 1: _t->setMaximumValue ( *reinterpret_cast< int* >( _v ) ); break;
                        case 2: _t->setMinimumPosition ( *reinterpret_cast< int* >( _v ) ); break;
                        case 3: _t->setMaximumPosition ( *reinterpret_cast< int* >( _v ) ); break;
                        case 4: _t->setSymmetricMoves ( *reinterpret_cast< bool* >( _v ) ); break;
                        case 5: _t->setHandleToolTip ( *reinterpret_cast< QString* >( _v ) ); break;
                        default: break;
                    }
                }
                else if ( _c == QMetaObject::ResetProperty )
                {
                }
#endif // QT_NO_PROPERTIES
            }

            const QMetaObject ctkRangeSlider::staticMetaObject = {
    { &QSlider::staticMetaObject, qt_meta_stringdata_ctkRangeSlider.data,
      qt_meta_data_ctkRangeSlider,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
            };

namespace omsl
{
    namespace commands
    {

        static const double kOutOfProjectionLimit = DBL_MAX / 2.;
        static const wchar_t* QT_LISTITEMCOLORVALUES_NAME = L"ColorSteps";
        static const Bool kDefaultTransparencyActive = True;
        static const double  kDefaultTransparency = 0.5;
        static const int kFromValueColumnIndex = 0;
        static const int kToValueColumnIndex = 1;
        static const int kColorColumnIndex = 2;

        namespace tp_analyzer
        {
            QTableWidget * TPAnalysisGUI::FindTableWidget ( const wchar_t* widgetName )
            {
                QTableWidget * pListData = nullptr;
                HWND wId = m_pBind->GetObjectId ( widgetName );
                if ( wId )
                {
                    QWidget *pWid = QWidget::find ( ( WId ) wId );
                    if ( pWid )
                    {
                        pListData = qobject_cast< QTableWidget * >( pWid );
                        if ( pListData )
                        {
                            pListData->clear ();
                            pListData->setColumnCount ( 3 );
                            pListData->horizontalHeader ()->setSectionResizeMode ( QHeaderView::Stretch );
                            pListData->horizontalHeader ()->resizeSections ( QHeaderView::Stretch );
                            // trick to resize the rows correctly
                            QFontMetrics metrics ( pListData->font () );
                            QRect outRect = metrics.boundingRect ( QRect ( 0, 0, 5000, 200 ), Qt::AlignLeft, "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890." );
                            pListData->verticalHeader ()->setDefaultSectionSize ( outRect.height ()*1.5 );
                            const Int& nLevels = m_pCommand->GetContext ()->GetValue <int> ( tpanalysis::UI_feedrateLevels );
                            const Double& dMin = m_pCommand->GetContext ()->GetValue <double> ( tpanalysis::UI_feedrateValueMin );
                            const Double& dMax = m_pCommand->GetContext ()->GetValue <double> ( tpanalysis::UI_feedrateValueMax );
                            UpdateTableWidget ( pListData, nLevels, dMin, dMax, 0x7000 );
                        }
                    }
                }
                return pListData;
            }

            void TPAnalysisGUI::UpdateSliderWidget ( ctkRangeSlider* /*slider*/, const int & /*dMin*/, const int & /*dMax*/ )
            {
            }

            void TPAnalysisGUI::OnSliderEnable ( const Int& n )
            {
                pRangeSlider->setEnabled ( n != 0 );
            }

            void TPAnalysisGUI::OnSliderUserMinimum ( const Int& n )
            {
                pRangeSlider->setMinimumPosition ( n );
            }

            void TPAnalysisGUI::OnSliderUserMaximum ( const Int& n )
            {
                pRangeSlider->setMaximumPosition ( n );
            }

            void TPAnalysisGUI::OnSliderRangeMinimum ( const Int& n )
            {
                pRangeSlider->setMinimum ( n );
            }

            void TPAnalysisGUI::OnSliderRangeMaximum ( const Int& n )
            {
                pRangeSlider->setMaximum ( n );
            }

            void TPAnalysisGUI::OnSliderSetMinMax ( int dMin, int dMax )
            {
                //int gap = dMax - dMin;
                s_pCommand->GetContext ()->SetValue ( tpanalysis::UI_sliderValueMin, dMin );

                {
                    s_pCommand->GetContext ()->SetValue ( tpanalysis::UI_sliderValueMax, dMax );
                }
            }

            void TPAnalysisGUI::OnChange_CB_MoveLimits ( const int& OnOff )
            {
                //obsolete
                if ( pRangeSlider != nullptr )
                {
                    pRangeSlider->setSymmetricMoves ( OnOff != 0 ? true : false );
                }
            }

            void TPAnalysisGUI::OnColorsDefinitionChanged ( const int& value )
            {
                if ( value != 2 )
                    return;

                const Double &dMin = m_pCommand->GetContext ()->GetValue <double> ( tpanalysis::UI_feedrateValueMin );
                const Double &dMax = m_pCommand->GetContext ()->GetValue <double> ( tpanalysis::UI_feedrateValueMax );
                const Int &nLevels = m_pCommand->GetContext ()->GetValue <int> ( tpanalysis::UI_feedrateLevels );

                for ( auto pList : m_vpListData )
                    UpdateTableWidget ( pList, nLevels, dMin, dMax, 0x4000 );
                // enable update for User changed min/max/cnt inputs
                m_pCommand->GetContext ()->SetValue ( tpanalysis::updateColorsTableAllowed, 1 );
            }

            void TPAnalysisGUI::UpdateTableWidget ( QTableWidget *pListData, const Int& nLevels, const Double & dMin, const Double & dMax, int /*change*/ )
            {
                if ( !pListData )
                    return;
                bool useFixNumberFeedratesPerColor = false; // calc N feedrates per Color
                bool useExactSpeeds = m_pCommand->GetContext ()->GetValue<int>( tpanalysis::UI_AutoUpdateLvl) ? false : true;
                bool usePartCount = false;// GetProperties ()->GetBag ()->GetValue<omsl::Bool> ( properties::key::UseFeedratePartitionByCount );
                bool useAddData = GetProperties ()->GetBag ()->GetValue<omsl::Bool> ( properties::key::ShowLayer25 );

                omsl::commands::tp_analyzer::Properties* tpp = GetProperties ();
                if ( tpp )
                {
                    omsl::commands::tp_analyzer::PropertiesImpl* p = dynamic_cast < omsl::commands::tp_analyzer::PropertiesImpl* > ( tpp );

                    if ( p->IsInactive ( properties::key::ShowLayer25 ) )
                    {
                        useAddData = false;
                    }
                }

                int myLevels = m_pCommand->GetContext ()->GetValue<int> ( tpanalysis::numberOfDifferentFeedrates );

                if ( useAddData )
                {
                    useExactSpeeds = false;
                    usePartCount = false;
                }
                else
                {
                    if ( useExactSpeeds == false )
                        myLevels = nLevels;

                    if ( myLevels > DEFAULTSPEEDLVLMAX )
                    {
                        if ( useExactSpeeds && usePartCount )
                            useFixNumberFeedratesPerColor = true;
                        useExactSpeeds = false;
                        myLevels = DEFAULTSPEEDLVLMAX;
                    }
                }
                
                m_pCommand->GetContext ()->SetValue <int> ( tpanalysis::useExactFeedrates, useExactSpeeds );

                pListData->clear ();

                m_colorTable.clear ();
                m_speedTable.clear ();
                m_ClrIndexVec.clear ();

                // define rounding
                int prec = m_pCommand->GetContext ()->GetValue <int> ( tpanalysis::clrTblRoundingPrec );
                double dlimit = m_pCommand->GetContext ()->GetValue <double> ( tpanalysis::clrTblRoundingLimit );

                //: TPAnalysis
                //% "From"
                static QString sFrom = qtTrId("9DA45047-21ED-411F-8BBD-D3E154F52D0F");
                //: TPAnalysis
                //% "Color"
                static QString sColor = qtTrId("44B0F88C-A6E7-4ED0-8601-6991629402CB");
                //: TPAnalysis
                //% "To"
                static QString sTo = qtTrId("FB07B392-613C-41EB-8795-B5C9C7C71099");
                //static QString sFrom = QString::fromWCharArray ( QT_TRANSLATE_NOOP ( "TPAnalysis", "From" ).str ()->Get () );
                //static QString sColor = QString::fromWCharArray ( QT_TRANSLATE_NOOP ( "TPAnalysis", "Color" ).str ()->Get () );
                //static QString sTo = QString::fromWCharArray ( QT_TRANSLATE_NOOP ( "TPAnalysis", "To" ).str ()->Get () );
                static QStringList theList;
                if ( theList.empty () )
                {
                    QVector<QString> v;
                    v.resize ( 3 );
                    v [kFromValueColumnIndex] = sFrom;
                    v [kColorColumnIndex] = sColor;
                    v [kToValueColumnIndex] = sTo;
                    theList = QStringList::fromVector ( v );
                }
                //static QStringList theList = QStringList( ) << sFrom << sColor << sTo;
                pListData->setHorizontalHeaderLabels ( theList );

                if ( useExactSpeeds == true || useAddData == true )
                {
                    std::vector<double> tempTable = m_pCommand->GetContext ()->GetValue<std::vector<double>> ( tpanalysis::doubleSpeedVec );
                    size_t end = tempTable.size ();

                    // for additional data we have 3 out of range areas
                    if ( useAddData )
                        m_speedTable.push_back ( 10000 );

                    for ( size_t i = 0; i < end; i++ )
                    {
                        if ( tempTable [i] >= dMin && tempTable [i] <= dMax )
                        {
                            double d = tempTable [i];
                            // convert doubles
                            double xd=d;

//***/ ??? how to map rounded value sto original values ????
                            if ( d > dlimit && useAddData == false ) // to avoid error with negative / Positive values
                                m_pConverter->SetDecimals ( 0 );
                            else
                                m_pConverter->SetDecimals ( prec );

                            // fill vector with rounded values
                            m_pConverter->SetInner ( d );
                            const wchar_t* s2 = m_pConverter->Get ();

                            /*int ret = */ (void)swscanf ( s2, L"%lf", &xd );
/****/

                            // filter double values
                            if ( m_speedTable.size () == 0 || m_speedTable[m_speedTable.size() -1] != xd )
                                m_speedTable.push_back ( xd );
                        }
                    }

                    myLevels =( int ) m_speedTable.size ();

                    for ( size_t i = 0; i < myLevels; i++ )
                    {
                        m_ClrIndexVec.push_back ( ( int ) i + GENERAL_COLOR_SIZE ); //+6
                    }
                }
                else if ( useFixNumberFeedratesPerColor == true )
                {
                    if ( dMin == dMax )
                        myLevels = 1;

                    if ( myLevels > 1 )
                    {
                        std::vector<double> tempTable = m_pCommand->GetContext ()->GetValue<std::vector<double>> ( tpanalysis::doubleSpeedVec );
                        size_t end = tempTable.size ();
                        size_t averageNr = end / myLevels; // how many FR per color
                        size_t j = 0;
                        if ( averageNr < 1 )
                            averageNr = 1;

//                        m_speedTable.push_back ( dMax );

                        while ( j < end )
                        {
                            double d = tempTable [j];
                            double xd;

                            if ( d > dMax )
                            {
                                averageNr = ( end - j ) / myLevels;
                                j++;
                                if ( averageNr == 0 )
                                    averageNr = 1;
                                continue;
                            }
                            if ( d < dMin )
                                break;

                            if ( d > dlimit )
                                m_pConverter->SetDecimals ( 0 );
                            else
                                m_pConverter->SetDecimals ( prec );

                            // fill vector with rounded values
                            m_pConverter->SetInner ( d );
                            const wchar_t* s2 = m_pConverter->Get ();

                            /*int ret = */( void ) swscanf ( s2, L"%lf", &xd );
                            /****/
                            j += averageNr;
                                                        // filter double values
                            if ( m_speedTable.size () == 0 || m_speedTable [m_speedTable.size () - 1] != xd )
                            {
                                m_speedTable.push_back ( xd );
                                if ( j >= end || myLevels <= m_speedTable.size () )
                                    break;

                                averageNr = (end-j) / ( myLevels - m_speedTable.size ());
                                if ( averageNr < 1 )
                                    averageNr = 1;
                            }
                        }

                        m_speedTable.push_back ( dMin );

                        // new size because of filter
                        myLevels = ( int ) (m_speedTable.size ()-1);

                        for ( size_t i = 0; i < myLevels; i++ )
                        {
                            m_ClrIndexVec.push_back ( ( int ) i + GENERAL_COLOR_SIZE ); //+6
                        }
                    }
                    else
                    {
                        m_speedTable.push_back ( dMax + 0.0001 );
                        m_speedTable.push_back ( dMin - 0.0001 );
                        m_ClrIndexVec.push_back ( GENERAL_COLOR_SIZE ); // 6
                    }
                }
                else
                {
                    if ( dMin == dMax )
                        myLevels = 1;

                    if ( myLevels > 1 )
                    {
                        m_speedTable.push_back ( dMax );

                        double dt = ( dMax - dMin ) / ( double ) ( myLevels );
                        for ( size_t i = 1; i < myLevels; i++ )
                        {
                            double d = m_speedTable [0] - dt*(double)i;
                            double xd;
                            if ( d > dlimit )
                                m_pConverter->SetDecimals ( 0 );
                            else
                                m_pConverter->SetDecimals ( prec );

                            // fill vector with rounded values
                            m_pConverter->SetInner ( d );
                            const wchar_t* s2 = m_pConverter->Get ();

                            /*int ret = */( void ) swscanf ( s2, L"%lf", &xd );
                            /****/
                                                        // filter double values
                            if ( i == 0 || m_speedTable [m_speedTable.size () - 1] != xd )
                            {
                                m_speedTable.push_back ( xd );
                            }
                        }

                        m_speedTable.push_back ( dMin );

                        // new size because of filter
                        myLevels = (int) (m_speedTable.size ()-1);

                        for ( size_t i = 0; i < myLevels; i++ )
                        {
                            m_ClrIndexVec.push_back ( ( int ) i + GENERAL_COLOR_SIZE ); //+6
                        }
                    }
                    else
                    {
                        m_speedTable.push_back ( dMax + 0.0001 );
                        m_speedTable.push_back ( dMin - 0.0001 );
                        m_ClrIndexVec.push_back ( GENERAL_COLOR_SIZE ); // 6
                    }
                }

                m_ClrIndexVec.push_back ( ( int ) myLevels + GENERAL_COLOR_SIZE ); // +6

                m_ClrIndexVec.push_back ( ( int ) myLevels + 1 + GENERAL_COLOR_SIZE ); // +6

                m_pCommand->GetContext ()->SetValue <int> ( tpanalysis::UI_feedrateLevelsAuto, myLevels );

                if ( m_speedTable.empty () )
                    return;

                std::vector<omsl::Rgb> cols;
                SharedPtr<materials::Gradient> pGradient = materials::Gradient::CreateBlueRedHSL ();

                if ( useAddData )
                    pGradient->SetQuantization ( myLevels > 1 ? myLevels-1 : 1);
                else
                    pGradient->SetQuantization ( myLevels );

                if ( myLevels >= 2 )
                {
                    cols.push_back ( omsl::Rgb ( 1., 0., 0. ) ); // red
                    m_colorTable.push_back ( 1.);
                    m_colorTable.push_back ( 0.);
                    m_colorTable.push_back ( 0.);

                    int clrsize = ( useAddData ) ? myLevels : myLevels;

                    for ( int i = clrsize -2 ; i >=1 /*- 2*/; i-- )
                    {
                        double t = ( double ) i / ( double ) ( clrsize -1 );//(double)(2 * (i + 1) - 1) / (double)(2 * nLevels + 1);
                        omsl::Rgb col = pGradient->GetColour ( t );
                        cols.push_back ( col );

                        m_colorTable.push_back ( col.r );
                        m_colorTable.push_back ( col.g );
                        m_colorTable.push_back ( col.b );

                    }

                    cols.push_back ( omsl::Rgb ( 0., 0., 1. ) ); // blue
                    m_colorTable.push_back ( 0. );
                    m_colorTable.push_back ( 0. );
                    m_colorTable.push_back ( 1. );
                }
                else
                {
                    cols.push_back ( omsl::Rgb ( 0., 1., 0. ) ); // green
                    m_colorTable.push_back ( 0. );
                    m_colorTable.push_back ( 1. );
                    m_colorTable.push_back ( 0. );
                }

                // outOfRangeColor over limit
                m_colorTable.push_back ( 1. );
                m_colorTable.push_back ( 1. );
                m_colorTable.push_back ( 1. );

                // outOfRangeColor under limit
                m_colorTable.push_back ( 0. );
                m_colorTable.push_back ( 0. );
                m_colorTable.push_back ( 0. );

                int iPart = 0;
                int iIndex = 0;
                if ( useAddData )
                {
                    iIndex = 1;
                    iPart = 1;
                    pListData->setRowCount ( ( int ) cols.size () + 2 ); // +2 additional colors
                }
                else
                {
                    iIndex = 1;
                    iPart = 1;
                    pListData->setRowCount ( ( int ) cols.size () + 2 ); // +2 additional colors
                }

                // !!! speedTable starts from highest value
                //if ( m_speedTable [0] > dlimit )
                //    m_pConverter->SetDecimals ( 0 );
                //else
                   m_pConverter->SetDecimals ( prec );

                // start value
                m_pConverter->SetInner ( m_speedTable [0] );


                QString sLastValue = QString::fromStdWString ( m_pConverter->Get () );

                // over limit box
                {
//                    std::wstring sVal (L"G0 movements to...");
                    //: TPAnalysis
                    //% "G0 movements to..."
                    QString sVal1 = qtTrId("D4BFC716-898C-4303-AB10-2A06AAC7E40C");
                    //: TPAnalysis
                    //% "Deviation not calculated"
                    QString sVal2 = qtTrId("68B435B2-D271-4E13-8739-6D375CCA84C8");
                    QString sValue = useAddData == false ? sVal1 : sVal2;

                                    //? QString::fromWCharArray ( QT_TRANSLATE_NOOP ( "TPAnalysis", "G0 movements to..." ).str ()->Get () )
                                    //: QString::fromWCharArray ( QT_TRANSLATE_NOOP ( "TPAnalysis", "Deviation not calculated" ).str ()->Get () );

                    QTableWidgetItem *itm = new QTableWidgetItem ( sValue );
                    itm->setFlags ( itm->flags () &  ~Qt::ItemIsEditable );
                    pListData->setItem ( 0, kFromValueColumnIndex, itm );

                    if ( useAddData == false )
                        itm = new QTableWidgetItem ( sLastValue );
                    else
                        itm = new QTableWidgetItem ();

                    itm->setFlags ( itm->flags () &  ~Qt::ItemIsEditable );
                    pListData->setItem ( 0, kToValueColumnIndex, itm );

                    QPixmap pix ( 128, 128 );
                    pix.fill ( QColor ( 1.0 * 255, 1.0 * 255, 1.0 * 255 ) );

                    itm = new QTableWidgetItem ( "" );
                    itm->setData ( Qt::DecorationRole, pix );
                    itm->setFlags ( itm->flags () &  ~Qt::ItemIsEditable );
                    pListData->setItem ( 0, kColorColumnIndex, itm );
                }

                if ( myLevels == 1 )
                {
                    iPart = 1;
                    // column 1 start value
                    QTableWidgetItem *itm = new QTableWidgetItem ( sLastValue );
                    itm->setFlags ( itm->flags () &  ~Qt::ItemIsEditable );
                    pListData->setItem ( iPart, kFromValueColumnIndex, itm );

                    // column2 target value
                    itm = new QTableWidgetItem ( sLastValue );
                    itm->setFlags ( itm->flags () &  ~Qt::ItemIsEditable );
                    pListData->setItem ( iPart, kToValueColumnIndex, itm );

                    // column 3 color
                    const omsl::Rgb &col = cols [0];
                    QPixmap pix ( 128, 128 );
                    pix.fill ( QColor ( col.r * 255, col.g * 255, col.b * 255 ) );

                    itm = new QTableWidgetItem ( "" );
                    itm->setData ( Qt::DecorationRole, pix );
                    itm->setFlags ( itm->flags () &  ~Qt::ItemIsEditable );
                    pListData->setItem ( iPart, kColorColumnIndex, itm );
                    iPart++;
                }
                else
                {
                    for ( ; iPart < myLevels + 1; ++iPart, ++iIndex )
                    {
                        if ( iPart < myLevels || ( !useExactSpeeds && !useAddData ) )
                        {
                            const double &dVal = m_speedTable [iPart];
                            m_pConverter->SetInner ( dVal );
                        }

                        if (useAddData && iPart == 1)
                        {
                            //: TPAnalysis
                            //% "High positive deviation ..."
                            sLastValue = qtTrId("A79B2E3A-C082-48DA-ACD3-EAED18C14657");
                            //sLastValue = QString::fromWCharArray ( QT_TRANSLATE_NOOP ( "TPAnalysis", "High positive deviation ..." ).str ()->Get () );
                        }
                        // column 1 start value
                        QTableWidgetItem *itm = new QTableWidgetItem ( sLastValue );
                        itm->setFlags ( itm->flags () &  ~Qt::ItemIsEditable );
                        pListData->setItem ( iIndex, kFromValueColumnIndex, itm );

                        std::wstring sVal = m_pConverter->Get ();
                        QString sValue = QString::fromStdWString ( sVal );

                        if (useAddData && iPart == myLevels)
                        {
                            //: TPAnalysis
                            //% "High negative deviation ..."
                            sValue = qtTrId("A4E4055D-8801-43EB-A41D-B89483E58B66");
                            //                        sValue = QString::fromWCharArray ( QT_TRANSLATE_NOOP ( "TPAnalysis", "High negative deviation ..." ).str ()->Get () );
                        }
                        // column2 target value
                        if ( useExactSpeeds )
                            itm = new QTableWidgetItem ( sLastValue );
                        else
                            itm = new QTableWidgetItem ( sValue );

                        itm->setFlags ( itm->flags () &  ~Qt::ItemIsEditable );
                        pListData->setItem ( iIndex, kToValueColumnIndex, itm );
                        sLastValue = sValue;

                        // column 3 color
                        const omsl::Rgb &col = cols [iIndex - 1];
                        QPixmap pix ( 128, 128 );
                        pix.fill ( QColor ( col.r * 255, col.g * 255, col.b * 255 ) );

                        itm = new QTableWidgetItem ( "" );
                        itm->setData ( Qt::DecorationRole, pix );
                        itm->setFlags ( itm->flags () &  ~Qt::ItemIsEditable );
                        pListData->setItem ( iIndex, kColorColumnIndex, itm );

                    }
                }

                if ( dMin > 0.0 || ( useAddData && m_speedTable [m_speedTable.size () - 1] == - 0.1 ) ) // only for view
                {
                    QTableWidgetItem *itm = new QTableWidgetItem ( sLastValue );
                    itm->setFlags ( itm->flags () &  ~Qt::ItemIsEditable );
                    pListData->setItem ( iIndex, kFromValueColumnIndex, itm );

                    itm = new QTableWidgetItem ( "0" );
                    itm->setFlags ( itm->flags () &  ~Qt::ItemIsEditable );
                    pListData->setItem ( iIndex, kToValueColumnIndex, itm );

                    QPixmap pix ( 128, 128 );
                    pix.fill ( QColor ( .0 * 255, .0 * 255, .0 * 255 ) );

                    itm = new QTableWidgetItem ( "" );
                    itm->setData ( Qt::DecorationRole, pix );
                    itm->setFlags ( itm->flags () &  ~Qt::ItemIsEditable );
                    pListData->setItem ( iIndex, kColorColumnIndex, itm );

                }

                m_pCommand->GetContext ()->SetValue ( tpanalysis::doubleClrVec, m_colorTable );
                m_pCommand->GetContext ()->SetValue ( tpanalysis::doubleSpeedVec, m_speedTable );
                m_pCommand->GetContext ()->SetValue ( tpanalysis::intClrIdxVec, m_ClrIndexVec );
                m_pCommand->GetContext ()->SetValue ( tpanalysis::numberOfDifferentFeedrates, myLevels );


                // to update the TPptr
                //int n = m_pCommand->GetContext ()->GetValue<int> ( tpanalysis::updateFeedrateDraw );
                //if ( n < 0 )
                //    n = 0;
                //else
                //    n++;
                //m_pCommand->GetContext ()->SetValue ( tpanalysis::updateFeedrateDraw, n );

            }
        }  // namespace tp_analyzer
    }  // namespace commands
}  // namespace omsl
