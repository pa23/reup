/*
    reup
    Automatization of support YMZ-530 ECU SW repository.

    File: menu.cpp

    Copyright (C) 2013-2015 Artem Petrov <pa2311@gmail.com>

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

#include "md5.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>

#define BOOST_NO_CXX11_SCOPED_ENUMS

#include <boost/filesystem.hpp>
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

    vector<string> fileNames = readDir(trimhexDir, ".hex", READDIR_FILESONLY);

    const boost::filesystem::path realProgPath = boost::filesystem::current_path();
    boost::filesystem::current_path(trimhexDir);

    for ( const string fileName : fileNames ) {
//        if ( boost::filesystem::file_size(boost::filesystem::path(fileName)) < 5000000 ) { // it is not actual now
            system((conf->val_trimhexExec() + " " + fileName + " > tmp").c_str());
//        }
    }

    boost::filesystem::remove("tmp");

    boost::filesystem::current_path(realProgPath);
}

void updHexIdent(const unique_ptr<Configuration> &conf) {

    const boost::filesystem::path trimhexDir(conf->val_trimhexDir());

    if ( !boost::filesystem::exists(trimhexDir) ) {
        cout << ERRORMSGBLANK << "Path \"" << trimhexDir.string() << "\" not exists!\n";
        return;
    }

    vector<string> fileNames = readDir(trimhexDir, ".hex", READDIR_FILESONLY);

    const boost::filesystem::path realProgPath = boost::filesystem::current_path();
    boost::filesystem::current_path(trimhexDir);

    unique_ptr<k2rei_swver> hexswver(new k2rei_swver());

    for ( const string elem : fileNames ) {

        if ( !hexswver->setFileName(elem) ) {
            cout << ERRORMSGBLANK << "Errors during parameters initialization of *_swver*.\n"
                 << ERRORMSGBLANK << "File " << elem << " skipped.\n";
            continue;
        }

        if ( !hexswver->read() ) {
            cout << ERRORMSGBLANK << "Errors during k2rei_swver parameter reading in " << elem << ".\n";
        }
        else {
            cout << MSGBLANK << "  Read  hex ID: " << hexswver->val() << "\n";
        }

        if ( !hexswver->write(elem) ) {
            cout << ERRORMSGBLANK << "Errors during k2rei_swver parameter writing in " << elem << ".\n";
        }
        else {
            system((conf->val_trimhexExec() + " " + elem + " > tmp").c_str());
            cout << MSGBLANK << "  Wrote hex ID: " << elem << "\n";
        }
    }

    boost::filesystem::remove("tmp");

    boost::filesystem::current_path(realProgPath);
}

void archHex(const unique_ptr<Configuration> &conf) {

    const boost::filesystem::path trimhexDir(conf->val_trimhexDir());

    if ( !boost::filesystem::exists(trimhexDir) ) {
        cout << ERRORMSGBLANK << "Path \"" << trimhexDir.string() << "\" not exists!\n";
        return;
    }

    vector<string> fileNames = readDir(trimhexDir, ".hex", READDIR_FILESONLY);

    const boost::filesystem::path realProgPath = boost::filesystem::current_path();
    boost::filesystem::current_path(trimhexDir);

    for ( const string fileName : fileNames ) {

        if ( boost::filesystem::exists(boost::filesystem::path(fileName+".7z")) ||
             boost::filesystem::file_size(boost::filesystem::path(fileName)) < 5000000 ) {
            continue;
        }

        /* agreement about archived hex-files format and extension */
        system((conf->val_archivExec() + " " + conf->val_archivParam() + " " + fileName + ".7z " + fileName + " > tmp").c_str());
    }

    boost::filesystem::remove("tmp");

    boost::filesystem::current_path(realProgPath);
}

void editEngDescr(const unique_ptr<Configuration> &conf) {

    const boost::filesystem::path trimhexDir(conf->val_trimhexDir());

    if ( !boost::filesystem::exists(trimhexDir) ) {
        cout << ERRORMSGBLANK << "Path \"" << trimhexDir.string() << "\" not exists!\n";
        return;
    }

    vector<string> fileNames = readDir(trimhexDir, ".hex", READDIR_FILESONLY);

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

    for ( const string fileName : fileNames ) {

        vector<string> parts;
        boost::split(parts, fileName, boost::is_any_of("_"));

        /* agreement about hex-file name. 5 parts. parts[2] is engine model */

        if ( parts.size() != 5 ) {
            cout << WARNMSGBLANK << "Wrong file name \"" << fileName << "\". Skipped!\n";
            continue;
        }

        for ( size_t i=0; i<data.size(); i++ ) {

            boost::regex regexp;
            regexp = R"(.+()" + parts[2] + R"(_){1}.+(\.hex){1}.+)";

            const boost::filesystem::path currHexFile = trimhexDir / boost::filesystem::path(fileName);

            if ( boost::regex_match(data[i], regexp) ) {
                data[i] = "            <td>" + fileName + "<br>(" + md5(readFile(currHexFile.string())) + ")</td>";
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

    vector<string> newFileNames = readDir(newPath, ".hex", READDIR_FILESONLY);
    vector<string> localRepoFileNames = readDir(localRepoHexPath, ".hex", READDIR_FILESONLY);

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

    newFileNames = readDir(newPath, ".zip", READDIR_FILESONLY);
    localRepoFileNames = readDir(localRepoMpkPath, ".zip", READDIR_FILESONLY);

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
        tmp = readDir(trimhexDir, ext, READDIR_FILESONLY);
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
    const boost::filesystem::path datDirectory(conf->val_datFilesDir());
    const boost::filesystem::path docDirectory(conf->val_docFilesDir());

    if ( !boost::filesystem::exists(localRepoDirectory / hexDirectory) ) {
        cout << ERRORMSGBLANK << "Path \"" << (localRepoDirectory / hexDirectory).string() << "\" not exists!\n";
        return;
    }

    if ( !boost::filesystem::exists(localRepoDirectory / mpkDirectory) ) {
        cout << ERRORMSGBLANK << "Path \"" << (localRepoDirectory / mpkDirectory).string() << "\" not exists!\n";
        return;
    }

    if ( !boost::filesystem::exists(localRepoDirectory / datDirectory) ) {
        cout << ERRORMSGBLANK << "Path \"" << (localRepoDirectory / datDirectory).string() << "\" not exists!\n";
        return;
    }

    if ( !boost::filesystem::exists(localRepoDirectory / docDirectory) ) {
        cout << ERRORMSGBLANK << "Path \"" << (localRepoDirectory / docDirectory).string() << "\" not exists!\n";
        return;
    }

    //

    vector<string> dirNames = readDir(remoteRepoDirectory, "", READDIR_DIRSONLY);

    const boost::filesystem::path realProgPath = boost::filesystem::current_path();
    boost::filesystem::current_path(remoteRepoDirectory);

    cout << MSGBLANK << "  Archiving old repo directories...\n";

    for ( const string dir : dirNames ) {
        archDir(conf->val_archivExec() + " " + conf->val_archivParam(),
                boost::filesystem::path(dir).filename().string(), false);
        boost::filesystem::remove_all(dir);
    }

    boost::filesystem::current_path(realProgPath);

    const boost::filesystem::path newRemoteRepoDir(remoteRepoDirectory /
                                                   (localRepoDirectory.filename().string() + "__" + currDateTime()));

    boost::filesystem::create_directory(newRemoteRepoDir);
    boost::filesystem::create_directory(newRemoteRepoDir / hexDirectory);
    boost::filesystem::create_directory(newRemoteRepoDir / mpkDirectory);
    boost::filesystem::create_directory(newRemoteRepoDir / datDirectory);
    boost::filesystem::create_directory(newRemoteRepoDir / docDirectory);

    ofstream fout((newRemoteRepoDir / (newRemoteRepoDir.filename().string() + ".md5")).string());
    bool md5needed = true;

    if ( !fout ) {
        cout << ERRORMSGBLANK << "Can not create md5 file! This operation will be skipped.\n";
        md5needed = false;
    }

    if ( md5needed ) {
        cout << MSGBLANK << "  Copying new repo and calculating checksums...\n";
    }
    else {
        cout << MSGBLANK << "  Copying new repo...\n";
    }

    vector<string> filesForCopy = readDir(localRepoDirectory / hexDirectory, "", READDIR_FILESONLY);

    for ( const string fileName : filesForCopy ) {

        boost::filesystem::path copiedFile(newRemoteRepoDir / hexDirectory / boost::filesystem::path(fileName));
        boost::filesystem::copy_file(localRepoDirectory / hexDirectory / boost::filesystem::path(fileName), copiedFile);

        if ( md5needed ) {
            fout << md5(readFile(copiedFile.string())) << " *" << (hexDirectory / copiedFile.filename()).string() << "\n";
        }
    }

    filesForCopy = readDir(localRepoDirectory / mpkDirectory, "", READDIR_FILESONLY);

    for ( const string fileName : filesForCopy ) {

        boost::filesystem::path copiedFile(newRemoteRepoDir / mpkDirectory / boost::filesystem::path(fileName));
        boost::filesystem::copy_file(localRepoDirectory / mpkDirectory / boost::filesystem::path(fileName), copiedFile);

        if ( md5needed ) {
            fout << md5(readFile(copiedFile.string())) << " *" << (mpkDirectory / copiedFile.filename()).string() << "\n";
        }
    }

    filesForCopy = readDir(localRepoDirectory / datDirectory, "", READDIR_FILESONLY);

    for ( const string fileName : filesForCopy ) {

        boost::filesystem::path copiedFile(newRemoteRepoDir / datDirectory / boost::filesystem::path(fileName));
        boost::filesystem::copy_file(localRepoDirectory / datDirectory / boost::filesystem::path(fileName), copiedFile);

        if ( md5needed ) {
            fout << md5(readFile(copiedFile.string())) << " *" << (datDirectory / copiedFile.filename()).string() << "\n";
        }
    }

    filesForCopy = readDir(localRepoDirectory / docDirectory, "", READDIR_FILESONLY);

    for ( const string fileName : filesForCopy ) {

        boost::filesystem::path copiedFile(newRemoteRepoDir / docDirectory / boost::filesystem::path(fileName));
        boost::filesystem::copy_file(localRepoDirectory / docDirectory / boost::filesystem::path(fileName), copiedFile);

        if ( md5needed ) {
            fout << md5(readFile(copiedFile.string())) << " *" << (docDirectory / copiedFile.filename()).string() << "\n";
        }
    }

    fout.close();
}

void archLocalRepo(const unique_ptr<Configuration> &conf) {

    archDir(conf->val_archivExec() + " " + conf->val_archivParam(), conf->val_localRepoDir(), true);
}
