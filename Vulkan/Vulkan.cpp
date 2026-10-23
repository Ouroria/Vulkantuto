#include <iostream>
#include "app.h"

int main()
{
	App appli;
	appli.Init();
	appli.Run();
	appli.Destroy();


	return 0;
}