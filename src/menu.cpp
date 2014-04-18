/*
    reup
    Automatization of support YMZ-530 ECU SW repository.

    File: menu.cpp

    Copyright (C) 2013-2014 Artem Petrov <pa2311@gmail.com>

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
#include "k2rei_swver.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <memory>
#include <fstream>
#include <ctime>
#include <sstream>

#define BOOST_NO_CXX11_SCOPED_ENUMS

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

using std::cout;
using std::string;
using std::vector;
using std::unique_ptr;
using std::ifstream;
using std::ofstream;
using std::stringstream;

void showMenu() {

    cout << "\nMenu:\n"
         << "  " << MENU_TRIMHEX      << ". Trim available hex files\n"
         << "  " << MENU_UPDHEXID     << ". Update hex identification\n"
         << "  " << MENU_ARCHHEX      << ". Archive trimmed hex files\n"
         << "  " << MENU_EDITENGDESCR << ". Update engine description file\n"
         << "  " << MENU_ADDNEW       << ". Add new hex and mpk files to repository\n"
         << "  " << MENU_CLEANDIR     << ". Clean trimhex directory\n"
         << "  " << MENU_PUBREPO      << ". Publish repository\n"
         << "  " << MENU_ARCHREPO     << ". Archive repository\n"
         << "  " << MENU_RELOADCONF   << ". Reload program configuration\n"
         << "  " << MENU_EXIT         << ". Exit\n\n"
         << "Your select: ";
}

void trimHex(const unique_ptr<Configuration> &conf) {

    const boost::filesystem::path trimhexDir(conf->val_trimhexDir());

    if ( !boost::filesystem::exists(trimhexDir) ) {

        cout << ERRORMSGBLANK << "Path \"" << trimhexDir.string() << "\" not exists!\n";
        return;
    }

    vector<string> fileNames = findFiles(trimhexDir, ".hex");

    const boost::filesystem::path realProgPath = boost::filesystem::current_path();
    boost::filesystem::current_path(trimhexDir);

    for ( const string fileName : fileNames ) {

        if ( boost::filesystem::file_size(boost::filesystem::path(fileName)) < 5000000 ) {

            system((conf->val_trimhexExec() + " " + fileName).c_str());
        }
    }

    boost::filesystem::current_path(realProgPath);
}

void updHexIdent(const unique_ptr<Configuration> &conf) {

    const boost::filesystem::path trimhexDir(conf->val_trimhexDir());

    if ( !boost::filesystem::exists(trimhexDir) ) {

        cout << ERRORMSGBLANK << "Path \"" << trimhexDir.string() << "\" not exists!\n";
        return;
    }

    vector<string> fileNames = findFiles(trimhexDir, ".hex");

    const boost::filesystem::path realProgPath = boost::filesystem::current_path();
    boost::filesystem::current_path(trimhexDir);

    unique_ptr<k2rei_swver> hexswver(new k2rei_swver());

    for ( const string elem : fileNames ) {

        hexswver->init(elem, conf->val_k2rei_swver_addr(), conf-> val_k2rei_swver_lenght());

        if ( !hexswver->read() ) {
            cout << ERRORMSGBLANK << "Errors during k2rei_swver parameter reading in " << elem << ".\n";
        }
        else {
            cout << MSGBLANK << "Readed hex ID: " << hexswver->val() << "\n";
        }

        if ( !hexswver->write(elem) ) {
            cout << ERRORMSGBLANK << "Errors during k2rei_swver parameter writing in " << elem << ".\n";
        }
        else {
            system((conf->val_trimhexExec() + " " + elem).c_str());
            cout << MSGBLANK << "Written hex ID: " << elem << "\n";
        }
    }

    boost::filesystem::current_path(realProgPath);
}

void archHex(const unique_ptr<Configuration> &conf) {

    const boost::filesystem::path trimhexDir(conf->val_trimhexDir());

    if ( !boost::filesystem::exists(trimhexDir) ) {

        cout << ERRORMSGBLANK << "Path \"" << trimhexDir.string() << "\" not exists!\n";
        return;
    }

    vector<string> fileNames = findFiles(trimhexDir, ".hex");

    const boost::filesystem::path realProgPath = boost::filesystem::current_path();
    boost::filesystem::current_path(trimhexDir);

    for ( const string fileName : fileNames ) {

        if ( boost::filesystem::exists(boost::filesystem::path(fileName+".7z")) ||
             boost::filesystem::file_size(boost::filesystem::path(fileName)) < 5000000 ) {
            continue;
        }

        /* agreement about archived hex-files format and extension */
        system((conf->val_archivExec() + " " + conf->val_archivParam() + " " + fileName + ".7z " + fileName).c_str());
    }

    boost::filesystem::current_path(realProgPath);
}

void editEngDescr(const unique_ptr<Configuration> &conf) {

    const boost::filesystem::path trimhexDir(conf->val_trimhexDir());

    if ( !boost::filesystem::exists(trimhexDir) ) {

        cout << ERRORMSGBLANK << "Path \"" << trimhexDir.string() << "\" not exists!\n";
        return;
    }

    vector<string> fileNames = findFiles(trimhexDir, ".hex");

    const boost::filesystem::path engDescrFile(
                boost::filesystem::path(conf->val_localRepoDir()) /
                boost::filesystem::path(conf->val_docFilesDir()) /
                boost::filesystem::path(conf->val_engDescription())
                );
    const string descrFileName = engDescrFile.string();

    ifstream fin(descrFileName);

    if ( !fin ) {
        cout << ERRORMSGBLANK << "Can not open file \"" << descrFileName << "\" to read!\n";
        return;
    }

    vector<string> data;
    string s;

    while ( !fin.eof() ) {
        getline(fin, s);
        data.push_back(s);
    }

    fin.close();

    vector<string> parts;
    boost::regex regexp;

    for ( const string fileName : fileNames ) {

        boost::split(parts, fileName, boost::is_any_of("_"));

        /* agreement about hex-file name. 5 parts. parts[2] is engine model */

        if ( parts.size() != 5 ) {
            continue;
        }

        for ( size_t i=0; i<data.size(); i++ ) {

            regexp = R"(.*)" + parts[2] + R"(_+.*(\.hex</td>){1}$)";

            if ( boost::regex_match(data[i], regexp) ) {
                data[i] = "            <td>" + fileName + "</td>";
            }
        }
    }

    ofstream fout(descrFileName);

    if ( !fout ) {
        cout << ERRORMSGBLANK << "Can not open file \"" << descrFileName << "\" to write!\n";
        return;
    }

    for ( const string str : data ) {
        fout << str << "\n";
    }

    fout.close();
}

void addNewToRepo(const unique_ptr<Configuration> &conf) {

    const boost::filesystem::path newPath(conf->val_trimhexDir());

    if ( !boost::filesystem::exists(newPath) ) {

        cout << ERRORMSGBLANK << "Path \"" << newPath.string() << "\" not exists!\n";
        return;
    }

    const boost::filesystem::path localRepoDir(conf->val_localRepoDir());
    const boost::filesystem::path hexFilesDir(conf->val_hexFilesDir());
    const boost::filesystem::path localRepoHexPath = localRepoDir / hexFilesDir;

    if ( !boost::filesystem::exists(localRepoHexPath) ) {

        cout << ERRORMSGBLANK << "Path \"" << localRepoHexPath.string() << "\" not exists!\n";
        return;
    }

    vector<string> newFileNames = findFiles(newPath, ".hex");
    vector<string> localRepoFileNames = findFiles(localRepoHexPath, ".hex");

    vector<string> parts;
    boost::regex regexp;

    for ( const string newFileName : newFileNames ) {

        boost::split(parts, newFileName, boost::is_any_of("_"));

        /* agreement about hex-file name. 5 parts. parts[2] is engine model */

        if ( parts.size() != 5 ) {
            continue;
        }

        for ( const string localRepoFileName : localRepoFileNames ) {

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

    const boost::filesystem::path mpkFilesDir(conf->val_mpkFilesDir());
    const boost::filesystem::path localRepoMpkPath = localRepoDir / mpkFilesDir;

    if ( !boost::filesystem::exists(localRepoMpkPath) ) {

        cout << ERRORMSGBLANK << "Path \"" << localRepoMpkPath.string() << "\" not exists!\n";
        return;
    }

    newFileNames = findFiles(newPath, ".zip");
    localRepoFileNames = findFiles(localRepoMpkPath, ".zip");

    for ( const string newFileName : newFileNames ) {

        boost::split(parts, newFileName, boost::is_any_of("_"));

        /* agreement about hex-file name. 5 parts. parts[2] is engine model */

        if ( parts.size() != 5 ) {
            continue;
        }

        for ( const string localRepoFileName : localRepoFileNames ) {

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

void cleanDir(const unique_ptr<Configuration> &conf) {

    const boost::filesystem::path trimhexDir(conf->val_trimhexDir());

    if ( !boost::filesystem::exists(trimhexDir) ) {

        cout << ERRORMSGBLANK << "Path \"" << trimhexDir.string() << "\" not exists!\n";
        return;
    }

    vector<string> fileNames;
    vector<string> tmp;

    for ( const string ext : conf->val_fileExtToDel() ) {

        tmp = findFiles(trimhexDir, ext);
        fileNames.insert(fileNames.end(), tmp.begin(), tmp.end());
    }

    const boost::filesystem::path realProgPath = boost::filesystem::current_path();
    boost::filesystem::current_path(trimhexDir);

    for ( const string fileName : fileNames ) {
        boost::filesystem::remove(boost::filesystem::path(fileName));
    }

    boost::filesystem::current_path(realProgPath);
}

void publishRepo(const unique_ptr<Configuration> &conf) {

    if ( conf->val_remoteRepoDir().empty() ) {

        cout << ERRORMSGBLANK << "Remote repository directory not defined! Edit configuration file.\n";
        return;
    }

    const boost::filesystem::path remoteRepoDirectory(conf->val_remoteRepoDir());

    if ( !boost::filesystem::exists(remoteRepoDirectory) ) {

        cout << ERRORMSGBLANK << "Path \"" << remoteRepoDirectory.string() << "\" not exists!\n";
        return;
    }

    const boost::filesystem::path localRepoDirectory(conf->val_localRepoDir());
    const boost::filesystem::path hexDirectory(conf->val_hexFilesDir());
    const boost::filesystem::path mpkDirectory(conf->val_mpkFilesDir());
    const boost::filesystem::path docDirectory(conf->val_docFilesDir());

    if ( !boost::filesystem::exists(localRepoDirectory / hexDirectory) ) {

        cout << ERRORMSGBLANK << "Path \"" << (localRepoDirectory / hexDirectory).string() << "\" not exists!\n";
        return;
    }

    if ( !boost::filesystem::exists(localRepoDirectory / mpkDirectory) ) {

        cout << ERRORMSGBLANK << "Path \"" << (localRepoDirectory / mpkDirectory).string() << "\" not exists!\n";
        return;
    }

    if ( !boost::filesystem::exists(localRepoDirectory / docDirectory) ) {

        cout << ERRORMSGBLANK << "Path \"" << (localRepoDirectory / docDirectory).string() << "\" not exists!\n";
        return;
    }

    boost::filesystem::remove_all(remoteRepoDirectory);
    boost::filesystem::create_directory(remoteRepoDirectory);
    boost::filesystem::create_directory(remoteRepoDirectory / hexDirectory);
    boost::filesystem::create_directory(remoteRepoDirectory / mpkDirectory);
    boost::filesystem::create_directory(remoteRepoDirectory / docDirectory);

    vector<string> filesForCopy = findFiles(localRepoDirectory / hexDirectory, "");

    for ( const string fileName : filesForCopy ) {
        boost::filesystem::copy_file(localRepoDirectory / hexDirectory / boost::filesystem::path(fileName),
                                     remoteRepoDirectory / hexDirectory / boost::filesystem::path(fileName));
    }

    filesForCopy = findFiles(localRepoDirectory / mpkDirectory, "");

    for ( const string fileName : filesForCopy ) {
        boost::filesystem::copy_file(localRepoDirectory / mpkDirectory / boost::filesystem::path(fileName),
                                     remoteRepoDirectory / mpkDirectory / boost::filesystem::path(fileName));
    }

    filesForCopy = findFiles(localRepoDirectory / docDirectory, "");

    for ( const string fileName : filesForCopy ) {
        boost::filesystem::copy_file(localRepoDirectory / docDirectory / boost::filesystem::path(fileName),
                                     remoteRepoDirectory / docDirectory / boost::filesystem::path(fileName));
    }
}

void archRepo(const unique_ptr<Configuration> &conf) {

    const string localRepoDirectory = conf->val_localRepoDir();

    if ( !boost::filesystem::exists(localRepoDirectory) ) {

        cout << ERRORMSGBLANK << "Path \"" << localRepoDirectory << "\" not exists!\n";
        return;
    }

    time_t t = time(NULL);
    struct tm *dtnow = localtime(&t);

    string year = boost::lexical_cast<string>(dtnow->tm_year + 1900);
    string mon  = boost::lexical_cast<string>(dtnow->tm_mon + 1);
    string day  = boost::lexical_cast<string>(dtnow->tm_mday);
    string hour = boost::lexical_cast<string>(dtnow->tm_hour);
    string min  = boost::lexical_cast<string>(dtnow->tm_min);

    system((conf->val_archivExec()
            + " "
            + conf->val_archivParam()
            + " "
            + localRepoDirectory
            + "__"
            + year + "-" + trimDate(mon) + "-" + trimDate(day) + "_" + trimDate(hour) + "-" + trimDate(min)
            + ".7z "
            + localRepoDirectory).c_str());
}
