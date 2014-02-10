/*
 * sgviewer.cpp
 *
 *  Created on: Feb 10, 2014
 *      Author: andy
 */

#include <unistd.h>
#include <string>
#include <iostream>
#include "Agent.h"
#include "Storage.h"
#include "Mysql.h"
#include "MemoryViewer.h"
#include "DotViewer.h"
#include "PrintViewer.h"

void display_usage_mysql(void)
{
    std::cout << "mysql usage: " << std::endl;
    exit(-1);
}

void display_usage(void)
{
    std::cout << "Usage: " << std::endl;
    exit(-1);
}

int main(int argc, char *argv[])
{
    std::string storage_name;
    std::string viewer_type;

    Storage *storage;
    MemoryViewer *viewer;
    Agent::State st = INVALID_STATE;

    static const char *optString = "T:V:S:?";

    int opt = 0;
    opt = getopt(argc, argv, optString);
    while (opt != -1)
    {
        switch (opt)
        {
            case 'T':    // storage name
                storage_name = optarg;
                break;
            case 'V':    // viewer type
                viewer_type = optarg;
                break;
            case 'S':    // specified state value
                st = atol(optarg);
                break;
            case '?':
                display_usage();
                break;
            default:
                display_usage();
                break;
        }
        opt = getopt(argc, argv, optString);
    }

    // check if storage and viewer are set
    if (storage_name.empty() || viewer_type.empty())
        display_usage();

    // check storage names
    if (storage_name.compare("mysql") == 0)    // storage is mysql
    {
        // number of remaining args must be 4: server, username, passwd, database
        int remain_arg = argc - optind;
        if (remain_arg != 4)
        {
            display_usage_mysql();
        }

        Mysql *mysql = new Mysql();
        mysql->SetDBArgs(argv[optind], argv[optind + 1], argv[optind + 2],
                argv[optind + 3]);
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

