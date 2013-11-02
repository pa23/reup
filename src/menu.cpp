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

#include "menu.h"
#include "identification.h"
#include "configuration.h"
#include "auxfunctions.h"

#include <iostream>
#include <string>
#include <vector>
#include <ctime>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

using std::cout;
using std::string;
using std::vector;

void showMenu() {

    cout << "\nMenu:\n"
         << "  0. Trim available hex files\n"
         << "  1. Archive trimmed hex files\n"
         << "  2. Add new hex and mpk files to repository\n"
         << "  3. Archive repository\n"
         << "  4. Clean trimhex directory\n"
         << "  5. Reload program configuration\n"
         << "  6. Exit\n\n"
         << "Your select: ";
}

void trimHex(const std::unique_ptr<Configuration> &conf) {

    string path = conf->val_localRepoDir() + "/" + conf->val_trimhexDir();
    string ext = ".hex";
    vector<string> fileNames;

    findFiles(path, ext, fileNames);

    string trimComm = conf->val_trimhexExec();

    string realProgPath = boost::filesystem::current_path().string();
    boost::filesystem::current_path(path);

    for ( auto fileName : fileNames ) {

        if ( boost::filesystem::file_size(boost::filesystem::path(fileName)) < 5000000 ) {
            system((trimComm + " " + fileName).c_str());
        }
    }

    boost::filesystem::current_path(realProgPath);
}

void archHex(const std::unique_ptr<Configuration> &conf) {

    string path = conf->val_localRepoDir() + "/" + conf->val_trimhexDir();
    string ext = ".hex";
    vector<string> fileNames;

    findFiles(path, ext, fileNames);

    string archComm = conf->val_archivExec() + " " + conf->val_archivParam();

    string realProgPath = boost::filesystem::current_path().string();
    boost::filesystem::current_path(path);

    for ( auto fileName : fileNames ) {

        if ( boost::filesystem::exists(boost::filesystem::path(fileName+".7z")) ||
             boost::filesystem::file_size(boost::filesystem::path(fileName)) < 5000000 ) {
            continue;
        }

        system((archComm + " " + fileName + ".7z " + fileName).c_str());
    }

    boost::filesystem::current_path(realProgPath);
}

void addNewToRepo() {

    //
}

void archRepo(const std::unique_ptr<Configuration> &conf) {

    system((conf->val_archivExec()
            + " "
            + conf->val_archivParam()
            + " "
            + conf->val_localRepoDir()
            + "_"
            + boost::lexical_cast<string>(time(NULL))
            + ".7z "
            + conf->val_localRepoDir()).c_str());
}

void cleanDir(const std::unique_ptr<Configuration> &conf) {

    string path = conf->val_localRepoDir() + "/" + conf->val_trimhexDir();
    vector<string> fileNames;

    for ( auto ext : conf->val_fileExtToDel() ) {
        findFiles(path, "."+ext, fileNames);
    }

    string realProgPath = boost::filesystem::current_path().string();
    boost::filesystem::current_path(path);

    for ( auto fileName : fileNames ) {
        boost::filesystem::remove(boost::filesystem::path(fileName));
    }

    boost::filesystem::current_path(realProgPath);
}
