#pragma once

// om
#include "utils/omTString.h"
#include "utils/omTStrings.h"

namespace om {
    namespace ast {

        class LanguageSettings {
        public:
            LanguageSettings();

            bool Init(const om::tstring &version);
            bool Save(const om::tstring &version);

            const om::tstring &GetUserLanguage() const { return m_userLanguage; }
            void SetUserLanguage(const om::tstring &lang) { m_userLanguage = lang; }

            const om::tstring &GetMachineLanguage() const { return m_machineLanguage; }
            void SetMachineLanguage(const om::tstring &lang) { m_machineLanguage = lang; }

            const om::tstrings &GetAvailableLanguages() const { return m_availableLanguages; }

        private:
            om::tstring m_userLanguage;
            om::tstring m_machineLanguage;

            om::tstrings m_availableLanguages;
        };

    } // namespace ast
} // namespace om
