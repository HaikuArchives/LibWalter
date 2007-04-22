/*
	AutoTextControl.cpp: A BTextControl which notifies on each keypress
	Written by DarkWyrm <darkwyrm@earthlink.net>, Copyright 2007
	Released under the MIT license.
*/
#include "AutoTextControl.h"
#include <Window.h>
#include <String.h>
#include <stdio.h>
#include <ctype.h>

AutoTextControlFilter::AutoTextControlFilter(AutoTextControl *box)
 : BMessageFilter(B_PROGRAMMED_DELIVERY, B_ANY_SOURCE,B_KEY_DOWN),
 	fBox(box),
 	fCurrentMessage(NULL)
{
}


AutoTextControlFilter::~AutoTextControlFilter(void)
{
}


filter_result
AutoTextControlFilter::Filter(BMessage *msg, BHandler **target)
{
	int32 rawchar,mod;
	msg->FindInt32("raw_char",&rawchar);
	msg->FindInt32("modifiers",&mod);
	
	BView *view = dynamic_cast<BView*>(*target);
	if (!view || strcmp("_input_",view->Name()) != 0)
		return B_DISPATCH_MESSAGE;
	
	AutoTextControl *text = dynamic_cast<AutoTextControl*>(view->Parent());
	if (!text || text != fBox)
		return B_DISPATCH_MESSAGE;
	
	fCurrentMessage = msg;
	filter_result result = KeyFilter(rawchar,mod);
	fCurrentMessage = NULL;
	
	if (fBox->fCharLimit && result == B_DISPATCH_MESSAGE) {
		// See to it that we still allow shortcut keys
		if (mod & B_COMMAND_KEY)
			return B_DISPATCH_MESSAGE;
		
		// We don't use strlen() because it is not UTF-8 aware, which can affect
		// how many characters can be typed.
		if (isprint(rawchar) && 
				(uint32)BString(text->Text()).CountChars() == text->fCharLimit)
			return B_SKIP_MESSAGE;
	}
	
	return result;
}


filter_result
AutoTextControlFilter::KeyFilter(const int32 &rawchar, const int32 &mod)
{
	if (fBox)
		fBox->Invoke();
	
	return B_DISPATCH_MESSAGE;
}


AutoTextControl::AutoTextControl(const BRect &frame, const char *name, const char *label,
		const char *text, BMessage *msg, uint32 resize,	uint32 flags)
 : BTextControl(frame,name,label,text,msg,resize,flags),
 	fFilter(NULL),
 	fCharLimit(0)
{
	SetFilter(new AutoTextControlFilter(this));
}


AutoTextControl::~AutoTextControl(void)
{
	if (Window())
		Window()->RemoveCommonFilter(fFilter);

	delete fFilter;
}


void
AutoTextControl::AttachedToWindow(void)
{
	BTextControl::AttachedToWindow();
	if (fFilter)
		Window()->AddCommonFilter(fFilter);
}


void
AutoTextControl::DetachedFromWindow(void)
{
	if (fFilter)
		Window()->RemoveCommonFilter(fFilter);
	BTextControl::DetachedFromWindow();
}


void
AutoTextControl::SetCharacterLimit(const uint32 &limit)
{
	fCharLimit = limit;
}


uint32
AutoTextControl::GetCharacterLimit(const uint32 &limit)
{
	return fCharLimit;
}


void
AutoTextControl::SetFilter(AutoTextControlFilter *filter)
{
	if (fFilter) {
		if (Window())
			Window()->RemoveCommonFilter(fFilter);
		delete fFilter;
	}
	
	fFilter = filter;
	if (Window())
		Window()->AddCommonFilter(fFilter);
}
