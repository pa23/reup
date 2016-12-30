/*
    reup
    Automatization of support YMZ-530 ECU SW repository.

    File: configuration.cpp

    Copyright (C) 2013-2016 Artem Petrov <pa2311@gmail.com>

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

#include "configuration.hpp"
#include "constants.hpp"
#include "identification.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define BOOST_NO_CXX11_SCOPED_ENUMS

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using std::cout;
using std::string;
using std::vector;
using std::ifstream;
using std::ofstream;

Configuration::Configuration() {
}

void Configuration::readConfigFile() {

    const boost::filesystem::path cfgfile(CONFIGFILE);

    if ( !boost::filesystem::exists(cfgfile) ) {

        cout << ERRORMSGBLANK << "Cofiguration file \"" << CONFIGFILE << "\" not found!\n"
             << ERRORMSGBLANK << PRGNAME << " will create blank of configuration.\n"
             << ERRORMSGBLANK << "Please edit file \"" << CONFIGFILE << "\" and reload program configuration.\n";

        if ( !createBlank() ) {
            cout << ERRORMSGBLANK << "Can not create file \"" << CONFIGFILE << "\"!\n"
                 << ERRORMSGBLANK << "Default values will be used.\n";
        }

        return;
    }

    ifstream fin(CONFIGFILE);

    if ( !fin ) {
        cout << ERRORMSGBLANK << "Can not open file \"" << CONFIGFILE << "\" to read!\n";
        return;
    }

    string s;
    vector<string> elem;

    while ( !fin.eof() ) {

        getline(fin, s);

        if ( !s.empty() ) {

            boost::split(elem, s, boost::is_any_of(PARAMDELIMITER));

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
            else if ( elem[0] == "DAT files directory" ) {
                m_datFilesDir = elem[1];
            }
            else if ( elem[0] == "DOC files directory" ) {
                m_docFilesDir = elem[1];
            }
            else if ( elem[0] == "Engine description file" ) {
                m_engDescription = elem[1];
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

    ofstream fout(CONFIGFILE);

    if ( !fout ) {
        cout << ERRORMSGBLANK << "Can not open file \"" << CONFIGFILE << "\" to write!\n";
        return false;
    }

    fout << "//\n"
         << "// This is " << PRGNAME << " configuration file.\n"
         << "// Parameter-Value delimiter is symbol \"" << PARAMDELIMITER << "\".\n"
         << "// Text after \"//\" is comment.\n"
         << "//\n\n";

    fout << "Local repository directory"   << PARAMDELIMITER << m_localRepoDir   << "\n"
         << "Remote repository directory"  << PARAMDELIMITER << m_remoteRepoDir  << "\n"
         << "HEX files directory"          << PARAMDELIMITER << m_hexFilesDir    << "\n"
         << "MPK files directory"          << PARAMDELIMITER << m_mpkFilesDir    << "\n"
         << "DAT files directory"          << PARAMDELIMITER << m_datFilesDir    << "\n"
         << "DOC files directory"          << PARAMDELIMITER << m_docFilesDir    << "\n"
         << "Engine description file"      << PARAMDELIMITER << m_engDescription << "\n"
         << "trimhex directory"            << PARAMDELIMITER << m_trimhexDir     << "\n"
         << "trimhex executable"           << PARAMDELIMITER << m_trimhexExec    << "\n"
         << "Archivator executable"        << PARAMDELIMITER << m_archivExec     << "\n"
         << "Archivator parameters"        << PARAMDELIMITER << m_archivParam    << "\n"
         << "File extensions for deletion" << PARAMDELIMITER;

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
