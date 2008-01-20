// InputRequest.cpp
/**
 * @class InputRequestView
 *
 * @author Christian Lörchner
 * @date 2006/10/04
 *
 */

#include "InputRequest.h"

#include <stdlib.h>

InputRequest::InputRequest(const char* title, const char* label, const char* text, const char* btn0_label)
	:BWindow(BRect(0, 0, IR_WINDOW_WIDTH, IR_WINDOW_HEIGHT), title,
	 B_MODAL_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL, B_NOT_RESIZABLE | B_NOT_ZOOMABLE)
{
  Init(label, text, btn0_label, NULL, NULL);
}

InputRequest::InputRequest(const char* title, const char* label, const char* text, const char* btn0_label, const char* btn1_label)
	:BWindow(BRect(0, 0, IR_WINDOW_WIDTH, IR_WINDOW_HEIGHT), title,
	 B_MODAL_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL, B_NOT_RESIZABLE | B_NOT_ZOOMABLE)
{
  Init(label, text, btn0_label, btn1_label, NULL);
}

InputRequest::InputRequest(const char* title, const char* label, const char* text, const char* btn0_label, const char* btn1_label, const char* btn2_label)
	:BWindow(BRect(0, 0, IR_WINDOW_WIDTH, IR_WINDOW_HEIGHT), title,
	 B_MODAL_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL, B_NOT_RESIZABLE | B_NOT_ZOOMABLE)
{
  Init(label, text, btn0_label, btn1_label, btn2_label);
}

void InputRequest::Init(const char* label, const char* text, const char* btn0_label, const char* btn1_label, const char* btn2_label)
{
  BRect frame = Bounds();
  fIrView = new InputRequestView(frame, btn0_label, btn1_label, btn2_label);
  
  // now move and resize the window with the calculated width of our view
  MoveTo(BPoint(fScreen.GetX()/2-fIrView->Width()/2, fScreen.GetY()/2-IR_WINDOW_HEIGHT/2-125)); 
  ResizeTo(fIrView->Width(), IR_WINDOW_HEIGHT);
  
  AddChild(fIrView);

  SetLabel(label);
  SetText(text);
  
  fIrView->SelectText();
  
  //init
  fButton_index = -1;
}

void InputRequest::MessageReceived(BMessage *msg)
{
  switch(msg->what)
  {
	case BTN0_MSG:
	{
	  fButton_index = 0;
	  break;
	}
	case BTN1_MSG:
	{
	  fButton_index = 1;
	  break;
	}
	case BTN2_MSG:
	{
	  fButton_index = 2;
	  break;
	}
	default:
	  BWindow::MessageReceived(msg);
  }
}

void InputRequest::SetLabel(const char* label)
{
  fIrView->SetLabel(label);  
}

const char* InputRequest::Label()
{
  return fIrView->Label();
}

void InputRequest::SetText(const char* text)
{
  fIrView->SetText(text);
}

const char* InputRequest::Text()
{
  return fIrView->Text();
}

int32 InputRequest::Go(char** input)
{
  fButton_index = -1;
  Show();
  while (fButton_index == -1) //wait for a click. i think this should be made in a better way with BMessages ;)
  {
    snooze(100000);
  }
  
  char* input_value = (char*) calloc(strlen(fIrView->Text()) + 1, 1);
  strcpy(input_value, fIrView->Text());
  
  *input = input_value;
  Hide();
  return fButton_index;
}

