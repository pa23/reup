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

using std::unique_ptr;
using std::cout;
using std::cin;

int main() {

    cout << "\n\t" << Identification{}.name() << " v" << Identification{}.version() << "\n"
         << "\t" << Identification{}.description() << "\n\n"
         << "Copyright (C) " << Identification{}.copyrightYears() << " " << Identification{}.authors() << "\n\n"
         << "Source code hosting: " << Identification{}.sourceCodeHosting() << "\n"
         << "Author's blog (RU): " << Identification{}.authorsBlog() << "\n\n"
         << Identification{}.licenseInformation() << "\n";

    unique_ptr<Configuration> conf(new Configuration());
    conf->readConfigFile();

    bool work = true;
    size_t currTask = MENU_EXIT;

    while ( work ) {

        showMenu();
        cin >> currTask;

        if ( currTask == MENU_TRIMHEX ) {
            trimHex(conf);
        }
        else if ( currTask == MENU_ARCHHEX ) {
            archHex(conf);
        }
        else if ( currTask == MENU_ADDNEW ) {
            addNewToRepo(conf);
        }
        else if ( currTask == MENU_CLEANDIR ) {
            cleanDir(conf);
        }
        else if ( currTask == MENU_PUBREPO ) {
            publishRepo(conf);
        }
        else if ( currTask == MENU_ARCHREPO ) {
            archRepo(conf);
        }
        else if ( currTask == MENU_RELOADCONF ) {
            conf->readConfigFile();
        }
        else if ( currTask == MENU_EXIT ) {
            work = false;
        }
        else {
            work = false;
        }
    }

    return 0;
}
