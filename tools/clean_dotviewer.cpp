/*
 * clean_dotviewer.cpp
 *
 *  Created on: Feb 8, 2014
 *      Author: andy
 */

#include "DotViewer.h"
#include "Mysql.h"

int main(int argc, char ** argv)
{
    if (argc != 5)
    {
        printf("Usage1: printviewer dbserver user password dbname\n");
        return -1;
    }

    Mysql mysql;
    mysql.SetDBArgs(argv[1], argv[2], argv[3], argv[4]);

    DotViewer dv(&mysql);
    dv.CleanShow();
    return 0;
}

