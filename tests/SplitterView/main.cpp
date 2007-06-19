
#include <Application.h>
#include "LayeringTestApp.h"

int
main()
{
	LayeringTestApp *be_app = new LayeringTestApp();
	be_app->Run();
	delete be_app;
	return 0;
};
