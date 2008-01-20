// InputRequestView.cpp
/**
 * @class InputRequestView
 *
 * @author Christian Lörchner
 * @date 2006/10/04
 *
 */

#include "InputRequestView.h"

#include <TranslationUtils.h>
#include <TranslatorFormats.h>
//#include <Roster.h>

InputRequestView::InputRequestView(BRect frame, const char* btn0_label, const char* btn1_label, const char* btn2_label)
	:BView(frame, "InputRequestView", B_FOLLOW_ALL, B_WILL_DRAW)
{
  float 	temp;
  
  //calculate the necessary width of our view
  //first we have to know the width of every button
  temp = fFont.StringWidth(btn0_label);
  fBtn0_width = (temp + BTN_TXT_SPACE < BTN_MIN_WIDTH)? BTN_MIN_WIDTH : temp + BTN_TXT_SPACE;
  if (btn1_label != NULL)
  {
    temp = fFont.StringWidth(btn1_label);
    fBtn1_width = (temp + BTN_TXT_SPACE < BTN_MIN_WIDTH)? BTN_MIN_WIDTH : temp + BTN_TXT_SPACE;
  }
  else
    fBtn1_width = 0;
  if (btn2_label != NULL)
  {
    temp = fFont.StringWidth(btn2_label);
    fBtn2_width = (temp + BTN_TXT_SPACE < BTN_MIN_WIDTH)? BTN_MIN_WIDTH : temp + BTN_TXT_SPACE;
  }
  else
    fBtn2_width = 0;
    
  fHeight = frame.Height();
  temp = 15 + fBtn2_width + BTN_SPACE + fBtn1_width + BTN_SPACE + fBtn0_width + BTN_SPACE + 50;
  fWidth = (temp > frame.Width())? temp : frame.Width();

  ResizeTo(fWidth, fHeight);

  //install our grafical elements
  SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
  
  fLabel = new BStringView(BRect(60, 10, fWidth - 15 - 150 - 10, 30), "label", NULL);
  AddChild(fLabel);
  
  fText = new BTextControl(BRect(fWidth - 15 - 150, 10, fWidth - 15, 30), "text", NULL, NULL, new BMessage(TEXT_MSG));
  fText->SetDivider(0);
  AddChild(fText);
  
  BRect		rect0;
  BRect		rect1;
  BRect		rect2;
  
  // only show needed buttons and stretch them if necessary
  // btn0 always is the leftmost button
  if (btn1_label == NULL)
  {
    rect0 = BRect(fWidth - 15 - fBtn0_width, 40, fWidth - 15, 60);
    fButton0 = new BButton(rect0, "button0", btn0_label, new BMessage(BTN0_MSG));
    AddChild(fButton0);
  }
  else if (btn2_label == NULL)
  {
    rect1 = BRect(fWidth - 15 - fBtn1_width, 40, fWidth - 15, 60);
    rect0 = BRect(fWidth - 15 - fBtn1_width - BTN_SPACE - fBtn0_width, 40, fWidth - 15 - fBtn1_width - BTN_SPACE, 60);
    
    fButton0 = new BButton(rect0, "button0", btn0_label, new BMessage(BTN0_MSG));
    AddChild(fButton0);
    fButton1 = new BButton(rect1, "button1", btn1_label, new BMessage(BTN1_MSG));
    AddChild(fButton1);
  }
  else
  {
    rect2 = BRect(fWidth - 15 - fBtn2_width, 40, fWidth - 15, 60);
    rect1 = BRect(fWidth - 15 - fBtn2_width - BTN_SPACE - fBtn1_width, 40, fWidth - 15 - fBtn2_width - BTN_SPACE, 60);
    rect0 = BRect(fWidth - 15 - fBtn2_width - BTN_SPACE - fBtn1_width - BTN_SPACE - fBtn0_width, 40, fWidth - 15 - fBtn2_width - BTN_SPACE - fBtn1_width - BTN_SPACE, 60);
    
    fButton0 = new BButton(rect0, "button0", btn0_label, new BMessage(BTN0_MSG));
    AddChild(fButton0);
    fButton1 = new BButton(rect1, "button1", btn1_label, new BMessage(BTN1_MSG));
    AddChild(fButton1);
    fButton2 = new BButton(rect2, "button2", btn2_label, new BMessage(BTN2_MSG));
    AddChild(fButton2);
  }
  
  //just prepare our quotation mark (thanks to staphan aßmus for this great tip :))
  SetDrawingMode(B_OP_ALPHA);
  //get the icon from the resource file (thanks to paradoxon for help)
  fIcon = BTranslationUtils::GetBitmap(B_PNG_FORMAT,"check");
  
  fButton0->MakeDefault(true);

}

void InputRequestView::SelectText()
{
  fText->TextView()->MakeFocus(true);
  fText->TextView()->SelectAll();  
}

float InputRequestView::Width()
{
  return fWidth;
}

const char*	InputRequestView::Label()
{
  return fLabel->Text();
};
void InputRequestView::SetLabel(const char* label)
{
  fLabel->SetText(label);
};
const char* InputRequestView::Text()
{
  return fText->Text();
};
void	InputRequestView::SetText(const char* text)
{
  fText->SetText(text);
};

void InputRequestView::Draw(BRect rect)
{
  //draw our nice gray rect on the left side of the window
  SetHighColor(183, 183, 183, 255);
  FillRect(BRect(0, 0, 30, fHeight), B_SOLID_HIGH);
  
  //draw the question mark
  if (fIcon != NULL)
  {
    DrawBitmap(fIcon, BPoint(15, 20));
  }
  //else
  //{
  //  AddChild(new BStringView(BRect(5, 5, 150, 20), "ErrorMSG1", "Quotation mark is missing!"));
  //}
}
