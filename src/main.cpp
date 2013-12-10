/*
    reup
    Automatization of support YMZ-530 ECU SW repository.

    File: main.cpp

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

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "configuration.hpp"
#include "identification.hpp"
#include "menu.hpp"
#include "constants.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using std::unique_ptr;
using std::cout;
using std::cin;
using std::string;
using std::vector;

string taskFilter(const string &str) {

    string ret;

    for ( size_t i=0; i<str.size(); i++ ) {

        char ch = str[i];

        if ( (ch == 32) || (ch >= 48 && ch <= 57) ) {
            ret.push_back(ch);
        }
    }

    return ret;
}

void execTask(const unique_ptr<Configuration> &conf, const size_t currTask) {

    if ( currTask == MENU_TRIMHEX ) {
        cout << Constants{}.msgBlank() << "Trimming available hex files...\n";
        trimHex(conf);
    }
    else if ( currTask == MENU_UPDHEXID ) {
        cout << Constants{}.msgBlank() << "Updating hex files identification...\n";
        updHexIdent(conf);
    }
    else if ( currTask == MENU_ARCHHEX ) {
        cout << Constants{}.msgBlank() << "Archiving hex files...\n";
        archHex(conf);
    }
    else if ( currTask == MENU_EDITENGDESCR ) {
        cout << Constants{}.msgBlank() << "Updating engine description file...\n";
        editEngDescr(conf);
    }
    else if ( currTask == MENU_ADDNEW ) {
        cout << Constants{}.msgBlank() << "Adding new files to repository...\n";
        addNewToRepo(conf);
    }
    else if ( currTask == MENU_CLEANDIR ) {
        cout << Constants{}.msgBlank() << "Cleaning trimhex directory...\n";
        cleanDir(conf);
    }
    else if ( currTask == MENU_PUBREPO ) {
        cout << Constants{}.msgBlank() << "Publishing repository...\n";
        publishRepo(conf);
    }
    else if ( currTask == MENU_ARCHREPO ) {
        cout << Constants{}.msgBlank() << "Archiving repository...\n";
        archRepo(conf);
    }
    else if ( currTask == MENU_RELOADCONF ) {
        cout << Constants{}.msgBlank() << "Reloading program configuration...\n";
        conf->readConfigFile();
    }
    else {
        cout << Constants{}.errorMsgBlank() << "Unknown task.\n";
        return;
    }

    cout << Constants{}.msgBlank() << "Done.\n";
}

int main() {

    cout << "\n\t" << Identification{}.name() << " v" << Identification{}.version() << "\n"
         << "\t" << Identification{}.description() << "\n\n"
         << "Copyright (C) " << Identification{}.copyrightYears() << " " << Identification{}.authors() << "\n\n"
         << "Source code hosting: " << Identification{}.sourceCodeHosting() << "\n"
         << "Author's blog (RU): " << Identification{}.authorsBlog() << "\n\n"
         << Identification{}.licenseInformation() << "\n\n";

    unique_ptr<Configuration> conf(new Configuration());
    conf->readConfigFile();

    bool work = true;

    while ( work ) {

        showMenu();

        string taskstr;
        getline(cin, taskstr);
        taskstr = taskFilter(taskstr);

        vector<string> tasks;
        boost::split(tasks, taskstr, boost::is_any_of(" "));

        size_t currTask = MENU_EXIT;

        for ( const string task : tasks ) {

            if ( !task.empty() ) {
                currTask = boost::lexical_cast<size_t>(task);
            }
            else {
                continue;
            }

            if ( currTask == MENU_EXIT ) {
                work = false;
                cout << Constants{}.msgBlank() << "Bye!\n";
                break;
            }
            else {
                execTask(conf, currTask);
            }
        }
    }

    return 0;
}
