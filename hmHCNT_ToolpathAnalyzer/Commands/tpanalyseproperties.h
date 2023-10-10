#pragma once
#if !defined(__TPANALYSEPROPERTIES__H__)
#define __TPANALYSEPROPERTIES__H__

#include <linkage.h>
#include <basetypes.h>
#include "commandhandshake.h"
#include <propertyservice/context/applicable.h>

namespace omsl {namespace propertyservice {class PropertyBag;}}

namespace omsl
{
    namespace commands
    {
    namespace tp_analyzer
    {

        class Properties
        {
        public:
            OMSL_METHOD ( propertyservice::PropertyBag* ) GetBag () const = 0;
            OMSL_METHOD ( void ) SetStrength ( const TCHAR* keyname, const omsl::propertyservice::Applicable::Strength& strength ) = 0;

            //OMSL_METHOD( const double& ) TpPointSize() const = 0;
            //OMSL_METHOD( const double& ) TpLineWidth() const = 0;
            //OMSL_METHOD( const double& ) TpVectorLength() const = 0;
            //OMSL_METHOD( const double& ) TmpGrTolerance() const = 0;
            //OMSL_METHOD( const double& ) TmpGrMinimumRadius() const = 0;

            //OMSL_METHOD( const omsl::Bool& )   ShowLayer25() const = 0;
            //OMSL_METHOD( const omsl::Bool& )   HideUpperLimitFeedrates() const = 0;
            //OMSL_METHOD( const omsl::Bool& )   HideLowerLimitFeedrates() const = 0;
            //OMSL_METHOD( const omsl::Bool& )   UseFeedratePartitionByCount() const = 0;

        protected:
            inline ~Properties ()
            {}
        };

        TP_ANALYZER_API_DIRECT ( Properties*, GetProperties, ( void ), ( ) );

    } //namespace tpanalysis
} // namespace commands
} //namespace omsl

#endif //__TPANALYSEPROPERTIES__H__