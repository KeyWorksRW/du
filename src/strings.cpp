/////////////////////////////////////////////////////////////////////////////
// Purpose:   Localized strings
// Author:    Randalph
// Copyright: Copyright (c) 2018-2020
// License:   Apache License -- see ../LICENSE
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <map>

#include "strings.h"

// clang-format off
const std::map<int, const char*> englishStrings = {

    { strIdUsage, "Show this help message" },
    { strIdCmdLineHelp, "du [options] [folder(s)]: Display size and number of files and folders" },
    { strIdSizeHelp, "Sort folder display based on size" },
    { strIdAllHelp, "Display size of all folders in current or specified directory" },

};
// clang-format on

const std::map<int, const char*>* _tt_english = &englishStrings;
const std::map<int, const char*>* _tt_CurLanguage = &englishStrings;
