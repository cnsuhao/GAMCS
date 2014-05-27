// -----------------------------------------------------------------------------
//
// GAMCS -- Generalized Agent Model and Computer Simulation
//
// Copyright (C) 2013-2014, Andy Huang  <andyspider@126.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// -----------------------------------------------------------------------------
//
// sgviewer.cpp - view memory of agent stored in varies storages
//
// -----------------------------------------------------------------------------

#include <unistd.h>
#include <string>
#include <iostream>
#include "gamcs/Agent.h"
#include "gamcs/Storage.h"
#include "gamcs/MemoryViewer.h"
#include "gamcs/DotViewer.h"
#include "gamcs/CDotViewer.h"
#include "gamcs/PrintViewer.h"
#ifdef _MYSQL_FOUND_
#include "gamcs/Mysql.h"
#endif
#ifdef _SQLITE_FOUND_
#include "gamcs/Sqlite.h"
#endif

using namespace gamcs;

void display_usage(void)
{
    std::cout
            << "Usage: sgviewer [-S<storage>] [-V<viewer>] [-(W)<state>] <storage related arguments>"
            << std::endl << std::endl;
    std::cout
            << " -Sv        - Specify the storage type as v in which the memory was stored"
            << std::endl;
    std::cout << " -Vv        - Choose the viewer type as v to show the memory"
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

    Storage *storage = NULL;
    MemoryViewer *viewer = NULL;
    Agent::State st = Agent::INVALID_STATE;

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
#ifdef _MYSQL_FOUND_
        int remain_arg = argc - optind;
        if (remain_arg != 4 && remain_arg != 3)    // args: server username passwd database, in which server can be ommit
        {
            std::cout << "Wrong number of arguments for mysql!" << std::endl;
            display_usage();
        }

        Mysql *mysql = NULL;
        if (remain_arg == 4)
        mysql = new Mysql(argv[optind], argv[optind + 1], argv[optind + 2],
                argv[optind + 3]);
        else if (remain_arg == 3)
        mysql = new Mysql("localhost", argv[optind], argv[optind + 1],
                argv[optind + 2]);
        storage = mysql;
#else
        std::cout << "GAMCS is built without support of Mysql!" << std::endl;
        display_usage();
#endif
    }    // other storages come here
    else if (storage_name.compare("sqlite") == 0)    // storage is sqlite
    {
#ifdef _SQLITE_FOUND_
        int remain_arg = argc - optind;
        if (remain_arg != 1)
        {
            std::cout << "Wrong number of arguments for sqlite!" << std::endl;
            display_usage();
        }

        Sqlite *sqlite = NULL;
        sqlite = new Sqlite(argv[optind]);
        storage = sqlite;
#else
        std::cout << "GAMCS is built without support of Sqlite!" << std::endl;
        display_usage();
#endif
    }
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
        DotViewer *cdv = new CDotViewer(storage);
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
    if (st != Agent::INVALID_STATE)
        viewer->dumpState(st);
    else
        viewer->dump();

    delete storage;
    delete viewer;

    return 0;
}

