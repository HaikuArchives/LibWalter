// InputRequest.h
/**
 * @class InputRequest
 * @brief A BAlert like class with a TextInput field
 *
 *
 * @author Christian Lörchner
 * @date 2006/10/04
 *
 * @todo 
 */

#ifndef _INPUTREQUEST_H
#define _INPUTREQUEST_H

#include <Window.h>  
#include "InputRequestView.h"
#include "ScreenInfo.h"

class InputRequest : public BWindow
{
  public:
						InputRequest(const char* title, const char* label, const char* text, const char* btn0_label);
						InputRequest(const char* title, const char* label, const char* text, const char* btn0_label, const char* btn1_label);
						InputRequest(const char* title, const char* label, const char* text, const char* btn0_label, const char* btn1_label, const char* btn2_label);
	virtual	int32		Go(char** input); //be carful! new memory (for input) is alloceted by this funktion. you have to free memory by yourself

  private:
	virtual	void		Init(const char* label, const char* text, const char* btn0_label, const char* btn1_label, const char* btn2_label);
	virtual	void		MessageReceived(BMessage *msg);
	virtual	void		SetLabel(const char* label);
	virtual	const char*	Label();
	virtual	void		SetText(const char* text);
	virtual	const char*	Text();
	
	#define	IR_WINDOW_WIDTH		315 // recommend min. value! the view does not take this in every case (button width)
	#define	IR_WINDOW_HEIGHT	75

	ScreenInfo			fScreen;

	InputRequestView	*fIrView;
	int32				fButton_index;
};

#endif
