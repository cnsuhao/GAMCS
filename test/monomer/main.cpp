#include "monomer.h"
#include "gamcs/CSOSAgent.h"
#include "gamcs/Mysql.h"
#include "gamcs/CDotViewer.h"
#include "gamcs/PrintViewer.h"
#include "gamcs/Sqlite.h"

int main(void)
{
	CSOSAgent ma(1, 0.9, 1);
//	Mysql db("localhost", "root", "huangk", "Monomer");
	Sqlite db("./test.db");
	ma.loadMemoryFromStorage(&db);

	Monomer mono;
	mono.connectAgent(&ma);
	mono.loop();

	//ma.deleteState(9);
	PrintViewer dotv;
	dotv.attachStorage(&ma);
	dotv.view();

	ma.dumpMemoryToStorage(&db);
//#ifdef _MYSQL_FOUND_
//	ma.dumpMemoryToStorage(&mysql);
//#endif
}
