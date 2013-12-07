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

#include "configuration.hpp"
#include "identification.hpp"
#include "menu.hpp"
#include "constants.hpp"

using std::unique_ptr;
using std::cout;
using std::cin;

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

    size_t currTask = MENU_EXIT;

    showMenu();

    while ( cin >> currTask ) {

        if ( currTask > 0 ) {
            execTask(conf, currTask);
        }
        else {
            cout << Constants{}.msgBlank() << "Bye!\n";
            break;
        }
    }

    return 0;
}
