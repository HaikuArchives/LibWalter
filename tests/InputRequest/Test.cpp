// test.cpp

#include <stdio.h>

#include <Alert.h>
#include <Application.h>
#include <Bitmap.h>
#include <File.h>
#include <Resources.h>
#include <Window.h>

#include "InputRequest.h"

class MyApp : public BApplication { // ------------------------------------------------------------
public:

MyApp(void) : BApplication("application/libwalter-InputRequest")
{

	char reply[256];
	
	InputRequest* ir = new InputRequest("This is the title", "This the label", 
	                                    "Here comes the text", "button label");
	                                    
	ir->Go((char**)&reply);
	
	be_app->PostMessage(B_QUIT_REQUESTED);

}


}; // MyApp ---------------------------------------------------------------------------------------

int main(int, char**)
{
	(new MyApp())->Run();
	return 0;
}

