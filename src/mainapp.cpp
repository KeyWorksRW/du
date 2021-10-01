/////////////////////////////////////////////////////////////////////////////
// Purpose:   Gets disk usage information about one or more directories
// Author:    Randalph
// Copyright: Copyright (c) 2018-2021 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../LICENSE
/////////////////////////////////////////////////////////////////////////////

#if defined(_WIN32)

    #pragma comment(lib, "kernel32.lib")
    #pragma comment(lib, "user32.lib")
    #pragma comment(lib, "gdi32.lib")
    #pragma comment(lib, "comctl32.lib")
    #pragma comment(lib, "comdlg32.lib")
    #pragma comment(lib, "shell32.lib")

    #pragma comment(lib, "rpcrt4.lib")
    #pragma comment(lib, "advapi32.lib")

#endif

#include <iostream>

#include <ttconsole.h>  // concolor -- Sets/restores console foreground color
#include <ttcvector.h>  // cstrVector -- Vector of ttlib::cstr strings
#include <ttparser.h>   // cmd -- Command line parser
#include <ttwinff.h>    // Wrapper around Windows FindFile

#include "strings.h"  // Contains translatable string IDs

struct FILE_SIZES
{
    std::string name;

    size_t files;
    size_t folders;
    int64_t true_size;
};

int main(int /* argc */, char** /* argv */)
{
    ttlib::cmd cmd;

    cmd.addHelpOption("h|help", _tt(strIdUsage));

    cmd.addOption("a|all", _tt(strIdAllHelp));
    cmd.addOption("s|size", _tt(strIdSizeHelp));

    cmd.parse();

    if (cmd.isHelpRequested())
    {
        std::cout << txtVersion << '\n' << txtCopyRight << "\n\n";
        std::cout << _tt(strIdCmdLineHelp) << '\n';

        for (auto& iter: cmd.getUsage())
        {
            std::cout << iter << '\n';
        }
        return 0;
    }

    ttlib::cstrVector directories = cmd.getExtras();

    // If the only thing specified on the command line is options, then default to list all top level directories
    bool parseAllDirs = directories.empty();
    if (cmd.isOption("all"))
        parseAllDirs = true;

    if (parseAllDirs)
    {
        // directories needs to be the final list of directories to check. If we're asked to parse all directories, then that
        // will include any directories passed to us on the command line -- so we clear it here and add to it as we parse all
        // the command-line directories.

        directories.clear();

        auto sub_folders = cmd.getExtras();
        if (sub_folders.empty())
            sub_folders.emplace_back("./");

        for (auto root: sub_folders)
        {
            root.addtrailingslash();
            root.append_filename("*.*");

            ttlib::winff all_dirs(root);
            while (all_dirs.isvalid())
            {
                if (all_dirs.isdir())
                {
                    directories.emplace_back(all_dirs.getcstr());
                }
                all_dirs.next();
            }
        }
    }

    if (directories.empty())
        directories.emplace_back("./");

    std::vector<FILE_SIZES> results;

    size_t total_files { 0 };
    size_t total_folders { 0 };
    int64_t total_true_size { 0 };

    for (auto& folder: directories)
    {
        size_t cFiles = 0;
        size_t cFolders = 0;
        int64_t cTrueSize = 0;

        ttlib::cstrVector subdirs;
        subdirs.emplace_back(folder);

        for (size_t subpos = 0; subpos < subdirs.size(); ++subpos)
        {
            subdirs[subpos].append_filename("*.*");
            ttlib::winff ff(subdirs[subpos].c_str());
            while (ff.isvalid())
            {
                if (ff.isdir())
                {
                    auto& newdir = subdirs.emplace_back();
                    newdir.assign(subdirs[subpos]);
                    newdir.replace_filename(ff.c_str());
                    cFolders++;
                }
                else
                {
                    cFiles++;
                    cTrueSize += ff.GetFullFileSize();
                }
                if (!ff.next())
                    break;
            }
        }

        auto& folder_info = results.emplace_back();
        folder_info.name = folder;
        folder_info.files = cFiles;
        folder_info.folders = cFolders;
        folder_info.true_size = cTrueSize;

        total_files += cFiles;
        total_folders += cFolders;
        total_true_size += cTrueSize;
    }

    if (cmd.isOption("size"))
    {
        std::sort(results.begin(), results.end(),
                  [](const FILE_SIZES& a, const FILE_SIZES& b) { return (a.true_size < b.true_size); });
    }
    else
    {
        std::sort(results.begin(), results.end(),
                  [](const FILE_SIZES& a, const FILE_SIZES& b) { return (a.name < b.name); });
    }

    for (auto& iter: results)
    {
        ttlib::concolor clr(ttlib::concolor::YELLOW);
        std::cout << '[' << iter.name << "] ";
        clr.ResetColor();

        std::cout << ttlib::cstr().Format("%ku files, %ku folders, %kllu bytes\n", iter.files, iter.folders, iter.true_size);
    }

    if (results.size() > 1)
    {
        std::cout << ttlib::cstr().Format("\nTotal: %ku files, %ku folders, %kllu bytes", total_files, total_folders,
                                          total_true_size);
    }

    return 0;
}
