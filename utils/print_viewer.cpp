/*
 * print_viewer.cpp
 *
 *  Created on: Feb 7, 2014
 *      Author: andy
 */

#include "PrintViewer.h"
#include "Mysql.h"

int main(int argc, char ** argv)
{
    if (argc != 5)
    {
        printf("Usage: printviewer dbserver user password dbname\n");
        return -1;
    }

    Mysql mysql;
    mysql.SetDBArgs(argv[1], argv[2], argv[3], argv[4]);

    PrintViewer pv(&mysql);
    pv.Show();
    return 0;
}


