/*
    reup
    Automatization of support YMZ-530 ECU SW repository.

    File: configuration.cpp

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

#include "configuration.h"
#include "constants.h"
#include "identification.h"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using std::cout;
using std::string;
using std::vector;
using std::ifstream;
using std::ofstream;

Configuration::Configuration() {
}

void Configuration::readConfigFile() {

    const string configFileName = Constants{}.configFile();
    const string errorBlank = Constants{}.errorMsgBlank();
    const string delimiter = Constants{}.parametersDelimiter();

    boost::filesystem::path cfgfile(configFileName);

    if ( !boost::filesystem::exists(cfgfile) ) {

        cout << errorBlank << "Cofiguration file " << configFileName << " not found!\n"
             << errorBlank << Identification{}.name() << " will create blank of configuration.\n"
             << errorBlank << "Please edit file " << configFileName << " and reload program configuration.\n";

        if ( !createBlank() ) {

            cout << errorBlank << "Can not create file " << configFileName << "!\n"
                 << errorBlank << "Default values will be used.\n";
        }

        return;
    }

    ifstream fin(configFileName);

    if ( !fin ) {

        cout << errorBlank << "Can not open file " << configFileName << " to read!\n";
        return;
    }

    string s;
    vector<string> elem;

    while ( !fin.eof() ) {

        getline(fin, s);

        if ( !s.empty() ) {

            boost::split(elem, s, boost::is_any_of(delimiter));

            if ( elem.size() != 2 ) {

                s.clear();
                elem.clear();

                continue;
            }

            if ( elem[0] == "Local repository directory" ) {
                m_localRepoDir = elem[1];
            }
            else if ( elem[0] == "Remote repository directory" ) {
                m_remoteRepoDir = elem[1];
            }
            else if ( elem[0] == "HEX files directory" ) {
                m_hexFilesDir = elem[1];
            }
            else if ( elem[0] == "MPK files directory" ) {
                m_mpkFilesDir = elem[1];
            }
            else if ( elem[0] == "trimhex directory" ) {
                m_trimhexDir = elem[1];
            }
            else if ( elem[0] == "trimhex executable" ) {
                m_trimhexExec = elem[1];
            }
            else if ( elem[0] == "Archivator executable" ) {
                m_archivExec = elem[1];
            }
            else if ( elem[0] == "Archivator parameters" ) {
                m_archivParam = elem[1];
            }
            else if ( elem[0] == "File extensions for deletion" ) {

                ma_fileExtForDel.clear();
                boost::split(ma_fileExtForDel, elem[1], boost::is_any_of(","));
            }
        }

        s.clear();
        elem.clear();
    }

    fin.close();
}

bool Configuration::createBlank() const {

    const string configFileName = Constants{}.configFile();
    const string errorBlank = Constants{}.errorMsgBlank();
    const string delimiter = Constants{}.parametersDelimiter();

    ofstream fout(configFileName);

    if ( !fout ) {

        cout << errorBlank << "Can not open file " << configFileName << " to write!\n";
        return false;
    }

    fout << "//\n"
         << "// This is " << Identification{}.name() << " configuration file.\n"
         << "// Parameter-Value delimiter is symbol \"" << delimiter << "\".\n"
         << "// Text after \"//\" is comment.\n"
         << "//\n\n";

    fout << "Local repository directory"   << delimiter << m_localRepoDir  << "\n"
         << "Remote repository directory"  << delimiter << m_remoteRepoDir << "\n"
         << "HEX files directory"          << delimiter << m_hexFilesDir   << "\n"
         << "MPK files directory"          << delimiter << m_mpkFilesDir   << "\n"
         << "trimhex directory"            << delimiter << m_trimhexDir    << "\n"
         << "trimhex executable"           << delimiter << m_trimhexExec   << "\n"
         << "Archivator executable"        << delimiter << m_archivExec    << "\n"
         << "Archivator parameters"        << delimiter << m_archivParam   << "\n"
         << "File extensions for deletion" << delimiter;

    for ( size_t i=0; i<ma_fileExtForDel.size(); i++ ) {

        fout << ma_fileExtForDel[i];

        if ( i == ma_fileExtForDel.size()-1 ) {
            fout << "\n";
        }
        else {
            fout << ",";
        }
    }

    fout.close();

    return true;
}
