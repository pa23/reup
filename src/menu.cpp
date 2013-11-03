/*
    reup
    Automatization of support YMZ-530 ECU SW repository.

    File: menu.cpp

    Copyright (C) 2013 Artem Petrov <pa2311@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "menu.hpp"
#include "identification.hpp"
#include "configuration.hpp"
#include "auxfunctions.hpp"
#include "constants.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <ctime>

#define BOOST_NO_CXX11_SCOPED_ENUMS

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

using std::cout;
using std::string;
using std::vector;

void showMenu() {

    cout << "\nMenu:\n"
         << "  1. Trim available hex files\n"
         << "  2. Archive trimmed hex files\n"
         << "  3. Add new hex and mpk files to repository\n"
         << "  4. Archive repository\n"
         << "  5. Clean trimhex directory\n"
         << "  6. Reload program configuration\n"
         << "  0. Exit\n\n"
         << "Your select: ";
}

void trimHex(const std::unique_ptr<Configuration> &conf) {

    const string path = conf->val_localRepoDir() + "/" + conf->val_trimhexDir();

    if ( !boost::filesystem::exists(path) ) {

        cout << Constants{}.errorMsgBlank() << "Path \"" << path << "\" not exists!\n";
        return;
    }

    vector<string> fileNames;

    findFiles(path, ".hex", fileNames);

    const string trimComm = conf->val_trimhexExec();

    const string realProgPath = boost::filesystem::current_path().string();
    boost::filesystem::current_path(path);

    for ( const auto fileName : fileNames ) {

        if ( boost::filesystem::file_size(boost::filesystem::path(fileName)) < 5000000 ) {

            system((trimComm + " " + fileName).c_str());
        }
    }

    boost::filesystem::current_path(realProgPath);
}

void archHex(const std::unique_ptr<Configuration> &conf) {

    const string path = conf->val_localRepoDir() + "/" + conf->val_trimhexDir();

    if ( !boost::filesystem::exists(path) ) {

        cout << Constants{}.errorMsgBlank() << "Path \"" << path << "\" not exists!\n";
        return;
    }

    vector<string> fileNames;

    findFiles(path, ".hex", fileNames);

    const string archComm = conf->val_archivExec() + " " + conf->val_archivParam();

    const string realProgPath = boost::filesystem::current_path().string();
    boost::filesystem::current_path(path);

    for ( const auto fileName : fileNames ) {

        if ( boost::filesystem::exists(boost::filesystem::path(fileName+".7z")) ||
             boost::filesystem::file_size(boost::filesystem::path(fileName)) < 5000000 ) {
            continue;
        }

        /* agreement about archived hex-files format and extension */
        system((archComm + " " + fileName + ".7z " + fileName).c_str());
    }

    boost::filesystem::current_path(realProgPath);
}

void addNewToRepo(const std::unique_ptr<Configuration> &conf) {

    const string newPath = conf->val_localRepoDir() + "/" + conf->val_trimhexDir();

    if ( !boost::filesystem::exists(newPath) ) {

        cout << Constants{}.errorMsgBlank() << "Path \"" << newPath << "\" not exists!\n";
        return;
    }

    const string localRepoHexPath = conf->val_localRepoDir() + "/" + conf->val_hexFilesDir();

    if ( !boost::filesystem::exists(localRepoHexPath) ) {

        cout << Constants{}.errorMsgBlank() << "Path \"" << localRepoHexPath << "\" not exists!\n";
        return;
    }

    vector<string> newFileNames;
    findFiles(newPath, ".hex", newFileNames);

    vector<string> localRepoFileNames;
    findFiles(localRepoHexPath, ".hex", localRepoFileNames);

    vector<string> parts;
    boost::regex regexp;

    for ( const auto newFileName : newFileNames ) {

        boost::split(parts, newFileName, boost::is_any_of("_"));

        /* agreement about hex-file name. 5 parts. parts[2] is engine model */

        if ( parts.size() != 5 ) {
            continue;
        }

        for ( const auto localRepoFileName : localRepoFileNames ) {

            regexp = R"(.*)" + parts[2] + R"(.*(\.hex){1}$)";

            if ( boost::regex_match(localRepoFileName, regexp) ) {
                boost::filesystem::remove(
                            boost::filesystem::path(localRepoHexPath + "/" + localRepoFileName)
                            );
            }
        }

        boost::filesystem::copy_file(
                    boost::filesystem::path(newPath + "/" + newFileName),
                    boost::filesystem::path(localRepoHexPath + "/" + newFileName),
                    boost::filesystem::copy_option::overwrite_if_exists
                    );
    }

    newFileNames.clear();
    localRepoFileNames.clear();

    const string localRepoMpkPath = conf->val_localRepoDir() + "/" + conf->val_mpkFilesDir();

    if ( !boost::filesystem::exists(localRepoMpkPath) ) {

        cout << Constants{}.errorMsgBlank() << "Path \"" << localRepoMpkPath << "\" not exists!\n";
        return;
    }

    findFiles(newPath, ".zip", newFileNames);
    findFiles(localRepoMpkPath, ".zip", localRepoFileNames);

    for ( const auto newFileName : newFileNames ) {

        boost::split(parts, newFileName, boost::is_any_of("_"));

        /* agreement about hex-file name. 5 parts. parts[2] is engine model */

        if ( parts.size() != 5 ) {
            continue;
        }

        for ( const auto localRepoFileName : localRepoFileNames ) {

            regexp = R"(.*)" + parts[2] + R"(.*(\.zip){1}$)";

            if ( boost::regex_match(localRepoFileName, regexp) ) {
                boost::filesystem::remove(
                            boost::filesystem::path(localRepoMpkPath + "/" + localRepoFileName)
                            );
            }
        }

        boost::filesystem::copy_file(
                    boost::filesystem::path(newPath + "/" + newFileName),
                    boost::filesystem::path(localRepoMpkPath + "/" + newFileName),
                    boost::filesystem::copy_option::overwrite_if_exists
                    );
    }
}

void archRepo(const std::unique_ptr<Configuration> &conf) {

    string localRepoDirectory = conf->val_localRepoDir();

    if ( !boost::filesystem::exists(localRepoDirectory) ) {

        cout << Constants{}.errorMsgBlank() << "Path \"" << localRepoDirectory << "\" not exists!\n";
        return;
    }

    system((conf->val_archivExec()
            + " "
            + conf->val_archivParam()
            + " "
            + localRepoDirectory
            + "_"
            + boost::lexical_cast<string>(time(NULL))
            + ".7z "
            + localRepoDirectory).c_str());
}

void cleanDir(const std::unique_ptr<Configuration> &conf) {

    const string path = conf->val_localRepoDir() + "/" + conf->val_trimhexDir();

    if ( !boost::filesystem::exists(path) ) {

        cout << Constants{}.errorMsgBlank() << "Path \"" << path << "\" not exists!\n";
        return;
    }

    vector<string> fileNames;

    for ( const auto ext : conf->val_fileExtToDel() ) {
        findFiles(path, "."+ext, fileNames);
    }

    const string realProgPath = boost::filesystem::current_path().string();
    boost::filesystem::current_path(path);

    for ( const auto fileName : fileNames ) {
        boost::filesystem::remove(boost::filesystem::path(fileName));
    }

    boost::filesystem::current_path(realProgPath);
}
