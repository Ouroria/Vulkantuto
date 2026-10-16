#include <iostream>
#include "app.h"

int main()
{
	app appli;
	appli.Init();
	appli.Run();
	appli.Destroy();


	return 0;
}