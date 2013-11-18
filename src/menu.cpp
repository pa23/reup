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
         << "  " << MENU_TRIMHEX    << ". Trim available hex files\n"
         << "  " << MENU_ARCHHEX    << ". Archive trimmed hex files\n"
         << "  " << MENU_ADDNEW     << ". Add new hex and mpk files to repository\n"
         << "  " << MENU_CLEANDIR   << ". Clean trimhex directory\n"
         << "  " << MENU_PUBREPO    << ". Publish repository\n"
         << "  " << MENU_ARCHREPO   << ". Archive repository\n"
         << "  " << MENU_RELOADCONF << ". Reload program configuration\n"
         << "  " << MENU_EXIT       << ". Exit\n\n"
         << "Your select: ";
}

void trimHex(const std::unique_ptr<Configuration> &conf) {

    const boost::filesystem::path trimhexDir(conf->val_trimhexDir());

    if ( !boost::filesystem::exists(trimhexDir) ) {

        cout << Constants{}.errorMsgBlank() << "Path \"" << trimhexDir.string() << "\" not exists!\n";
        return;
    }

    vector<string> fileNames;

    findFiles(trimhexDir, ".hex", fileNames);

    const boost::filesystem::path realProgPath = boost::filesystem::current_path();
    boost::filesystem::current_path(trimhexDir);

    for ( const auto fileName : fileNames ) {

        if ( boost::filesystem::file_size(boost::filesystem::path(fileName)) < 5000000 ) {

            system((conf->val_trimhexExec() + " " + fileName).c_str());
        }
    }

    boost::filesystem::current_path(realProgPath);
}

void archHex(const std::unique_ptr<Configuration> &conf) {

    const boost::filesystem::path trimhexDir(conf->val_trimhexDir());

    if ( !boost::filesystem::exists(trimhexDir) ) {

        cout << Constants{}.errorMsgBlank() << "Path \"" << trimhexDir.string() << "\" not exists!\n";
        return;
    }

    vector<string> fileNames;

    findFiles(trimhexDir, ".hex", fileNames);

    const boost::filesystem::path realProgPath = boost::filesystem::current_path();
    boost::filesystem::current_path(trimhexDir);

    for ( const auto fileName : fileNames ) {

        if ( boost::filesystem::exists(boost::filesystem::path(fileName+".7z")) ||
             boost::filesystem::file_size(boost::filesystem::path(fileName)) < 5000000 ) {
            continue;
        }

        /* agreement about archived hex-files format and extension */
        system((conf->val_archivExec() + " " + conf->val_archivParam() + " " + fileName + ".7z " + fileName).c_str());
    }

    boost::filesystem::current_path(realProgPath);
}

void addNewToRepo(const std::unique_ptr<Configuration> &conf) {

    const boost::filesystem::path newPath(conf->val_trimhexDir());

    if ( !boost::filesystem::exists(newPath) ) {

        cout << Constants{}.errorMsgBlank() << "Path \"" << newPath.string() << "\" not exists!\n";
        return;
    }

    const boost::filesystem::path localRepoDir(conf->val_localRepoDir());
    const boost::filesystem::path hexFilesDir(conf->val_hexFilesDir());
    const boost::filesystem::path localRepoHexPath = localRepoDir / hexFilesDir;

    if ( !boost::filesystem::exists(localRepoHexPath) ) {

        cout << Constants{}.errorMsgBlank() << "Path \"" << localRepoHexPath.string() << "\" not exists!\n";
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

            regexp = R"(.*)" + parts[2] + R"(_+.*(\.hex){1}$)";

            if ( boost::regex_match(localRepoFileName, regexp) ) {
                boost::filesystem::remove(
                            localRepoHexPath / boost::filesystem::path(localRepoFileName)
                            );
            }
        }

        boost::filesystem::copy_file(
                    newPath / boost::filesystem::path(newFileName),
                    localRepoHexPath / boost::filesystem::path(newFileName),
                    boost::filesystem::copy_option::overwrite_if_exists
                    );
    }

    newFileNames.clear();
    localRepoFileNames.clear();

    const boost::filesystem::path mpkFilesDir(conf->val_mpkFilesDir());
    const boost::filesystem::path localRepoMpkPath = localRepoDir / mpkFilesDir;

    if ( !boost::filesystem::exists(localRepoMpkPath) ) {

        cout << Constants{}.errorMsgBlank() << "Path \"" << localRepoMpkPath.string() << "\" not exists!\n";
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

            regexp = R"(.*)" + parts[2] + R"(_+.*(\.zip){1}$)";

            if ( boost::regex_match(localRepoFileName, regexp) ) {
                boost::filesystem::remove(
                            localRepoMpkPath / boost::filesystem::path(localRepoFileName)
                            );
            }
        }

        boost::filesystem::copy_file(
                    newPath / boost::filesystem::path(newFileName),
                    localRepoMpkPath / boost::filesystem::path(newFileName),
                    boost::filesystem::copy_option::overwrite_if_exists
                    );
    }
}

void cleanDir(const std::unique_ptr<Configuration> &conf) {

    const boost::filesystem::path trimhexDir(conf->val_trimhexDir());

    if ( !boost::filesystem::exists(trimhexDir) ) {

        cout << Constants{}.errorMsgBlank() << "Path \"" << trimhexDir.string() << "\" not exists!\n";
        return;
    }

    vector<string> fileNames;

    for ( const auto ext : conf->val_fileExtToDel() ) {
        findFiles(trimhexDir, "."+ext, fileNames);
    }

    const boost::filesystem::path realProgPath = boost::filesystem::current_path();
    boost::filesystem::current_path(trimhexDir);

    for ( const auto fileName : fileNames ) {
        boost::filesystem::remove(boost::filesystem::path(fileName));
    }

    boost::filesystem::current_path(realProgPath);
}

void publishRepo(const std::unique_ptr<Configuration> &conf) {

    if ( conf->val_remoteRepoDir().empty() ) {

        cout << Constants{}.errorMsgBlank() << "Remote repository directory not defined! Edit configuration file.\n";
        return;
    }

    const boost::filesystem::path remoteRepoDirectory(conf->val_remoteRepoDir());

    if ( !boost::filesystem::exists(remoteRepoDirectory) ) {

        cout << Constants{}.errorMsgBlank() << "Path \"" << remoteRepoDirectory.string() << "\" not exists!\n";
        return;
    }

    const boost::filesystem::path localRepoDirectory(conf->val_localRepoDir());
    const boost::filesystem::path hexDirectory(conf->val_hexFilesDir());
    const boost::filesystem::path mpkDirectory(conf->val_mpkFilesDir());
    const boost::filesystem::path docDirectory(conf->val_docFilesDir());

    if ( !boost::filesystem::exists(localRepoDirectory / hexDirectory) ) {

        cout << Constants{}.errorMsgBlank() << "Path \"" << (localRepoDirectory / hexDirectory).string() << "\" not exists!\n";
        return;
    }

    if ( !boost::filesystem::exists(localRepoDirectory / mpkDirectory) ) {

        cout << Constants{}.errorMsgBlank() << "Path \"" << (localRepoDirectory / mpkDirectory).string() << "\" not exists!\n";
        return;
    }

    if ( !boost::filesystem::exists(localRepoDirectory / docDirectory) ) {

        cout << Constants{}.errorMsgBlank() << "Path \"" << (localRepoDirectory / docDirectory).string() << "\" not exists!\n";
        return;
    }

    boost::filesystem::remove_all(remoteRepoDirectory);
    boost::filesystem::create_directory(remoteRepoDirectory);
    boost::filesystem::create_directory(remoteRepoDirectory / hexDirectory);
    boost::filesystem::create_directory(remoteRepoDirectory / mpkDirectory);
    boost::filesystem::create_directory(remoteRepoDirectory / docDirectory);

    vector<string> filesForCopy;

    findFiles(localRepoDirectory / hexDirectory, "", filesForCopy);

    for ( const auto fileName : filesForCopy ) {
        boost::filesystem::copy_file(localRepoDirectory / hexDirectory / boost::filesystem::path(fileName),
                                     remoteRepoDirectory / hexDirectory / boost::filesystem::path(fileName));
    }

    filesForCopy.clear();

    findFiles(localRepoDirectory / mpkDirectory, "", filesForCopy);

    for ( const auto fileName : filesForCopy ) {
        boost::filesystem::copy_file(localRepoDirectory / mpkDirectory / boost::filesystem::path(fileName),
                                     remoteRepoDirectory / mpkDirectory / boost::filesystem::path(fileName));
    }

    filesForCopy.clear();

    findFiles(localRepoDirectory / docDirectory, "", filesForCopy);

    for ( const auto fileName : filesForCopy ) {
        boost::filesystem::copy_file(localRepoDirectory / docDirectory / boost::filesystem::path(fileName),
                                     remoteRepoDirectory / docDirectory / boost::filesystem::path(fileName));
    }
}

void archRepo(const std::unique_ptr<Configuration> &conf) {

    const string localRepoDirectory = conf->val_localRepoDir();

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
