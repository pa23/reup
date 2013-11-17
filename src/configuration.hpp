/*
    reup
    Automatization of support YMZ-530 ECU SW repository.

    File: configuration.hpp

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

#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <string>
#include <vector>

class Configuration {

public:

    Configuration();

    void readConfigFile();

    std::string val_localRepoDir() const {
        return m_localRepoDir;
    }
    std::string val_remoteRepoDir() const {
        return m_remoteRepoDir;
    }
    std::string val_hexFilesDir() const {
        return m_hexFilesDir;
    }
    std::string val_mpkFilesDir() const {
        return m_mpkFilesDir;
    }
    std::string val_docFilesDir() const {
        return m_docFilesDir;
    }
    std::string val_trimhexDir() const {
        return m_trimhexDir;
    }
    std::string val_trimhexExec() const {
        return m_trimhexExec;
    }
    std::string val_archivExec() const {
        return m_archivExec;
    }
    std::string val_archivParam() const {
        return m_archivParam;
    }
    std::vector<std::string> val_fileExtToDel() const {
        return ma_fileExtForDel;
    }

private:

    bool createBlank() const;

    std::string m_localRepoDir  = "YMZ-530_ECU_SW_REPO";
    std::string m_remoteRepoDir = "";
    std::string m_hexFilesDir   = "hex";
    std::string m_mpkFilesDir   = "mpk";
    std::string m_docFilesDir   = "doc";
    std::string m_trimhexDir    = "THex";
    std::string m_trimhexExec   = "trimmhex.bat";
    std::string m_archivExec    = "7z";
    std::string m_archivParam   = "a";
    std::vector<std::string> ma_fileExtForDel =
    {"hex", "7z", "zip", "ini", "txt"};

};

#endif // CONFIGURATION_HPP
