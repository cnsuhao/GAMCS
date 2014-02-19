// -----------------------------------------------------------------------------
//
// GIMCS -- Generalized Intelligence Model and Computer Simulation
//
// Copyright (C) 2013-2014, Andy Huang  <andyspider@126.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
// -----------------------------------------------------------------------------
//
// sgviewer.cpp - view memory of agent stored in varies storages
//
// -----------------------------------------------------------------------------


#include <unistd.h>
#include <string>
#include <iostream>
#include "Agent.h"
#include "Storage.h"
#include "Mysql.h"
#include "MemoryViewer.h"
#include "DotViewer.h"
#include "PrintViewer.h"

using namespace gimcs;

void display_usage(void)
{
    std::cout
            << "Usage: sgviewer [-S<storage>] [-T<viewer>] [-(W)<state>] <storage related arguments>"
            << std::endl << std::endl;
    std::cout
            << " -Sv        - Specify the storage type as v in which the memory was stored"
            << std::endl;
    std::cout << " -Tv        - Choose the viewer type as v to show the memory"
            << std::endl;
    std::cout
            << " -Wv        - Only view a single state v in memory, if this option is omitted, the whole memory will be shown"
            << std::endl;
    std::cout
            << "(additional arguments for mysql)    (server) <username> <password> <database>"
            << std::endl;

    exit(-1);
}

int main(int argc, char *argv[])
{
    std::string storage_name;
    std::string viewer_type;

    Storage *storage;
    MemoryViewer *viewer;
    Agent::State st = INVALID_STATE;

    static const char *optString = "S:V:W:?";

    int opt = 0;
    opt = getopt(argc, argv, optString);
    while (opt != -1)
    {
        switch (opt)
        {
            case 'S':    // storage name
                storage_name = optarg;
                break;
            case 'V':    // viewer type
                viewer_type = optarg;
                break;
            case 'W':    // which state
                st = atol(optarg);
                break;
            case '?':
                display_usage();
                break;
            default:
                std::cout << "Unknown option!" << std::endl;
                display_usage();
                break;
        }
        opt = getopt(argc, argv, optString);
    }

    // check if storage and viewer are set
    if (storage_name.empty() || viewer_type.empty()) display_usage();

    // check storage names
    if (storage_name.compare("mysql") == 0)    // storage is mysql
    {
        int remain_arg = argc - optind;
        if (remain_arg != 4 && remain_arg != 3)    // args: server username passwd database, in which server can be ommit
        {
            std::cout << "Wrong number of arguments for mysql!" << std::endl;
            display_usage();
        }

        Mysql *mysql = new Mysql();
        if (remain_arg == 4)
            mysql->SetDBArgs(argv[optind], argv[optind + 1], argv[optind + 2],
                    argv[optind + 3]);
        else if (remain_arg == 3)
            mysql->SetDBArgs("localhost", argv[optind], argv[optind + 1],
                    argv[optind + 2]);
        storage = mysql;
    }    // other storages come here
    else
    {
        std::cout << "Unkown storage type: " << storage_name << "!\n"
                << std::endl;
        display_usage();
    }

    // check viewer types
    if (viewer_type.compare("dot") == 0)
    {
        // use DotViewer
        DotViewer *dv = new DotViewer(storage);
        viewer = dv;
    }
    else if (viewer_type.compare("cdot") == 0)
    {
        // use CleanShow of DotViewer
        DotViewer *cdv = new DotViewer(storage);
        viewer = cdv;
    }
    else if (viewer_type.compare("prt") == 0)
    {
        // use PrintViewer
        PrintViewer *pv = new PrintViewer(storage);
        viewer = pv;
    }
    else
    {
        std::cout << "Unkown viewer type: " << viewer_type << "!\n"
                << std::endl;
        delete storage;
        display_usage();
    }

    // show
    if (st != INVALID_STATE)
        viewer->ShowState(st);
    else
        viewer->Show();

    delete storage;
    delete viewer;

    return 0;
}

