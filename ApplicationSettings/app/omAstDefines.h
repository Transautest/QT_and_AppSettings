#pragma once

namespace om {
    namespace ast {

        enum ConfigurationType {
            kUnknownConfigurationType = 0,

            kCommonConfiguration = 1,
            kCurrentVersionConfiguration = 2
        };

        enum SettingsType {
            kUnknownSettingsType = 0,

            kUserSettings = 1,
            kMachineSettings = 2,
        };

    } // namespace ast
} // namespace om
