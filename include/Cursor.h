// =============================================================================
//
// libwalter Cursor.h
//
// Set a custom cursor for any BView or window.
//
// Distributed under the terms of the MIT license.
//
// Authors:
//		Gabriele Biffi - www.biffuz.it
//
// =============================================================================

#ifndef _CURSOR_H_
#define _CURSOR_H_

// Haiku classes
class BCursor;
class BMessage;
class BWindow;

void SetViewCursor(BView *view, const BCursor *cursor);
void SetViewCursor(BView *view, const void *data);
void UnsetViewCursor(BView *view);

void SetWindowCursor(BWindow *window, const BCursor *cursor,
	bool impose = false);
void SetWindowCursor(BWindow *window, const void *data, bool impose = false);
void UnsetWindowCursor(BWindow *window);

void SetAppCursor(const BCursor *cursor, bool impose = false);
void SetAppCursor(const void *data, bool impose = false);
void UnsetAppCursor(void);

#endif // _CURSOR_H_
