// InputRequestView.h
/**
 * @class InputRequestView
 *
 * @author Christian Lörchner
 * @date 2006/10/04
 *
 */
#ifndef _INPUTREQUEST_VIEW_H
#define _INPUTREQUEST_VIEW_H

#include <View.h>

#include <Font.h>
#include <Bitmap.h>
#include <interface/Button.h>
#include <interface/StringView.h>
#include <interface/TextControl.h>

class InputRequestView : public BView
{
  public:
    #define TEXT_MSG	'text'
  	
	#define BTN0_MSG	'btn0'
	#define BTN1_MSG	'btn1'
	#define BTN2_MSG	'btn2'

   
    				InputRequestView(BRect frame, const char* btn0_label, const char* btn1_label, const char* btn2_label);
	virtual	float	Width(); //auxilliary function for use in our window

	inline virtual void			SelectText();

	inline virtual const char*	Label();
	inline virtual void		SetLabel(const char* label);
	inline virtual const char*	Text();
	inline virtual void			SetText(const char* text);
	
  private:
    virtual void	Draw(BRect rect);
    
    BStringView		*fLabel;
    BTextControl	*fText;

  	BButton		*fButton0;
  	BButton		*fButton1;
  	BButton		*fButton2;
  	
  	float	fHeight;
    float	fWidth;
    
    BFont	fFont;
    float 	fBtn0_width;
    float 	fBtn1_width;
    float 	fBtn2_width; 
    
    BBitmap	*fIcon; 	
    
    #define	BTN_TXT_SPACE	20 // right plus left side of the button label space
  	#define BTN_SPACE		10 // space between buttons
  	#define	BTN_MIN_WIDTH	80 // minimum width of a button
  				
};

#endif
