/*
 * dot_viewer.cpp
 *
 *  Created on: Feb 7, 2014
 *      Author: andy
 */

#include "DotViewer.h"
#include "Mysql.h"

int main(int argc, char ** argv)
{
    if (argc != 5 && argc != 6)
    {
        printf("Usage1: printviewer dbserver user password dbname\n");
        printf("Usage2: printviewer dbserver user password dbname state\n");
        return -1;
    }

    Mysql mysql;
    mysql.SetDBArgs(argv[1], argv[2], argv[3], argv[4]);

    DotViewer dv(&mysql);
    if (argc == 5)
        dv.Show();
    else if (argc == 6) dv.ShowState(atol(argv[5]));
    return 0;
}

