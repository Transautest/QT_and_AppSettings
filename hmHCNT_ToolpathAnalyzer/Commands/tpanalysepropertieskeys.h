#pragma once
#ifndef TPANALYSISPROPERTIESKEYS_H__
#define TPANALYSISPROPERTIESKEYS_H__

/*!
* \file tpanalysispropertieskeys.h
*
* \author Uwe Spiess
* \date 24/04/2019
*
* contains the defines for the interaction with the UI
*/

#include <propertyservice/typednametdef.h>
#include <typeofbasetypes.h>
#include "tpanalyseproperties.h"

namespace omsl
{
    namespace commands
    {
    namespace tp_analyzer
    {
        namespace properties
        {
            namespace key
            {
                enum eBallMill
                {
                     kJobsetting = 0
                    ,kCentered
                    ,kTipped
                    ,kBoth
                };

                //PROPERTYBAG_KEY ( omsl::Bool, HideUpperLimitFeedrates );
                //PROPERTYBAG_KEY ( omsl::Bool, HideLowerLimitFeedrates );

                //PROPERTYBAG_KEY ( omsl::Bool, UseFeedratePartitionByCount );

                PROPERTYBAG_KEY ( omsl::Bool, ShowLayer25 );
                PROPERTYBAG_KEY ( omsl::Bool, ShowNCPath );
                PROPERTYBAG_KEY ( omsl::Int, ShowBallMillSpecPath);
                PROPERTYBAG_KEY ( omsl::Bool, ShowClippPath );

                PROPERTYBAG_KEY ( double, TpPointSize );
                PROPERTYBAG_KEY ( double, TpLineWidth );
                PROPERTYBAG_KEY ( double, TpVectorLength );

                PROPERTYBAG_KEY ( double, TmpGrTolerance );
                PROPERTYBAG_KEY ( double, TmpGrMinimumRadius );


            }  //namespace key
        }  //namespace properties
    }  // namespace tpanalysis
    }  // namespace commands
}  // namespace omsl

#endif // TPANALYSISPROPERTIESKEYS_H__
