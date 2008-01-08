// =============================================================================
//
// libwalter Cursor.cpp
//
// Set a custom cursor for any BView or window.
//
// Distributed under the terms of the MIT license.
//
// Authors:
//		Gabriele Biffi - www.biffuz.it
//
// =============================================================================

// Haiku headers
#include <Application.h>
#include <Autolock.h>
#include <app/Cursor.h>
#include <List.h>
#include <Message.h>
#include <MessageFilter.h>
#include <MessageRunner.h>
#include <View.h>
#include <Window.h>

// libwalter headers
#include "Cursor.h"

// +--------------------------------------------------------------------------+
// | Internal declarations, definitions, and functions                        |
// +--------------------------------------------------------------------------+

// Messages for the filter
#define _MSG_CHECK_WINDOW_FILTERS	'lwWF'
#define _MSG_SET_APP_CURSOR			'lwAC'
#define _MSG_SET_WINDOW_CURSOR		'lwWC'
#define _MSG_SET_VIEW_CURSOR		'lwVC'
#define _MSG_UPDATE_CURSOR			'lwUC'

// Struct to hold the cursor data
typedef struct __CursorStruct {
	BView *			view;
	BWindow *		window;
	bool			impose;
	const BCursor *	system_cursor;
	BCursor *		custom_cursor;
} _CursorStruct;

// Message filter to be installed into the application
class _ApplicationFilter : public BMessageFilter {
private:
			void			SetAppCursor(const BCursor *cursor,
								const void *data, bool impose);
			void			SetViewCursor(BView *view, const BCursor *cursor,
								const void *data, bool impose);
			void			SetWindowCursor(BWindow *window,
								const BCursor *cursor, const void *data,
								bool impose);
			void			UpdateCursor(BWindow *window, BView *view);
			const BCursor * fAppCursorSystem;
			BCursor *		fAppCursorCustom;
			bool			fImposeAppCursor;
			const BCursor *	fLastCursor;
			BList			fCursors;
public:	
							_ApplicationFilter(void);
	virtual 				~_ApplicationFilter();
	// BMessageFilter hooks
	virtual	filter_result	Filter(BMessage *message, BHandler **target);
};

// Message filter to be installed into every window
class _WindowFilter : public BMessageFilter {
public:	
							_WindowFilter(void);
	virtual 				~_WindowFilter();
	// BMessageFilter hooks
	virtual	filter_result	Filter(BMessage *message, BHandler **target);
};

// Pointer to the app filter
_ApplicationFilter * _appFilter = NULL;
static int32 _appFilterCreated = -1;

// Check validity of raw cursor data
bool _check_cursor_data(const void *data)
{
	// A cursor is a 16x16 1bpp bitmap (32 bytes total), plus a transparency
	// mask. Data includes the hotspot, from 0,0 (upper left corner) to 15,15
	// (lower right corner).
	// Cursor data is 67 bytes long:
	// byte 0 is the length of the cursor's bitmap side in pixels (must be 16)
	// byte 1 is the color depth of the bitmap (must be 1)
	// byte 2 is the X of the hotspot (must be 0 <= x < 16)
	// byte 3 is the Y of the hotspot (must be 0 <= y < 16)
	// bytes 4-35 are the bitmap data
	// bytes 36-66 are the transparency mask
	if (data == NULL) return false;
	const unsigned char *tmp = static_cast<const unsigned char*>(data);
	if (tmp[0] != 16 || tmp[1] != 1 || tmp[2] >= 16 || tmp[3] >= 16)
		return false;
	return true;
}

// Return a pointer to our window filter for the specified window, or install
// it
_WindowFilter * _get_window_filter(BWindow *window,
	bool install_if_needed = true)
{
	BAutolock locker(window);

	_WindowFilter *filter = NULL;
	BList *list;
	int32 i;

	// Get filters list
	list = window->CommonFilterList();
	if (list != NULL) {
		// Check if the window already has the filter
		for (i = 0; i < list->CountItems(); i++) {
			filter = dynamic_cast<_WindowFilter*>(
	  			static_cast<BMessageFilter*>(list->ItemAt(i)));
	  		if (filter != NULL)
				break;
		}
	}

	// Install the filter
	if (install_if_needed && filter == NULL) {
		filter = new _WindowFilter();
		window->AddCommonFilter(filter);
	}

	return filter;
}

// Install our app filter
void _install_app_filter(void)
{
	// Besides installing the application filter, we create a message runner
	// that sends a message every half second to the application. When
	// received, the message will cause the filter to scan the threads for new
	// windows, then it will install the window filter to them.
	// This function is multithread safe.
	if (atomic_add(&_appFilterCreated, 1) == 0) {
		_appFilter = new _ApplicationFilter();
		be_app->Lock();
		be_app->AddCommonFilter(_appFilter);
		be_app->Unlock();
		new BMessageRunner(be_app_messenger,
			new BMessage(_MSG_CHECK_WINDOW_FILTERS), 500000);
		be_app->PostMessage(_MSG_CHECK_WINDOW_FILTERS); // force first check
	}
}

// +--------------------------------------------------------------------------+
// | ApplicationFilter                                                        |
// +--------------------------------------------------------------------------+
// | Its job is to take all mouse moved messages (and others) and apply the   |
// | cursor. Also it takes messages to set custom cursors for views, windows, |
// | and the app.                                                             |
// +--------------------------------------------------------------------------+

// Constructors and destructor

_ApplicationFilter::_ApplicationFilter(void) :
	BMessageFilter(B_PROGRAMMED_DELIVERY, B_ANY_SOURCE)
{
	fAppCursorSystem = B_CURSOR_SYSTEM_DEFAULT;
	fAppCursorCustom = NULL;
	fImposeAppCursor = false;
	fLastCursor = B_CURSOR_SYSTEM_DEFAULT;
}

_ApplicationFilter::~_ApplicationFilter()
{
	_CursorStruct *cursor;
	while (fCursors.CountItems() > 0) {
		cursor = static_cast<_CursorStruct*>(fCursors.ItemAt(0));
		if (cursor->custom_cursor != NULL)
			delete cursor->custom_cursor;
		fCursors.RemoveItem(cursor);
		delete cursor;
	}
	if (fAppCursorCustom != NULL)
		delete fAppCursorCustom;
}

// Private

void _ApplicationFilter::SetAppCursor(const BCursor *cursor,
	const void *data, bool impose)
{
	const void *_data = NULL;

	// Check data
	if (data != NULL) {
		if (_check_cursor_data(data))
			_data = data;
	}

	// Remove old cursor
	fAppCursorSystem = B_CURSOR_SYSTEM_DEFAULT;
	if (fAppCursorCustom != NULL) {
		delete fAppCursorCustom;
		fAppCursorCustom = NULL;
	}
	fImposeAppCursor = false;

	// System cursor
	if (cursor != NULL) {
		fAppCursorSystem = cursor;
		fImposeAppCursor = impose;
	}

	// Custom cursor
	else if (_data != NULL) {
		fAppCursorCustom = new BCursor(_data);
		fImposeAppCursor = impose;
	}

	// Update the cursor on screen
	UpdateCursor(NULL, NULL);
}

void _ApplicationFilter::SetViewCursor(BView *view, const BCursor *cursor,
	const void *data, bool impose)
{
	if (view == NULL) return;

	const void *_data = NULL;
	_CursorStruct *curs;
	int32 index;

	// Check data
	if (data != NULL) {
		if (_check_cursor_data(data))
			_data = data;
	}

	// Check if the view already has a cursor
	for (index = 0; index < fCursors.CountItems(); index++) {
		curs = static_cast<_CursorStruct*>(fCursors.ItemAt(index));
		if (curs->view == view)
			break;
	}

	// Update old entry
	if (index < fCursors.CountItems()) {

		// Delete old cursor
		curs->system_cursor = NULL;
		if (curs->custom_cursor != NULL) {
			delete curs->custom_cursor;
			curs->custom_cursor = NULL;
		}
		curs->impose = false;

		// System cursor
		if (cursor != NULL) {
			curs->system_cursor = cursor;
			curs->impose = impose;
		}

		// Custom cursor
		else if (_data != NULL) {
			curs->custom_cursor = new BCursor(_data);
			curs->impose = impose;
		}

		// Delete entry
		else {
			fCursors.RemoveItem(curs);
			delete curs;
		}
	}

	// Add a new entry
	else if (cursor != NULL || _data != NULL) {
		curs = new _CursorStruct;
		curs->view = view;
		curs->window = NULL;
		curs->impose = impose;
		if (cursor != NULL) {
			curs->system_cursor = cursor;
			curs->custom_cursor = NULL;
		}
		else {
			curs->system_cursor = NULL;
			curs->custom_cursor = new BCursor(_data);
		}
		fCursors.AddItem(curs);
	}

	// Update the cursor on screen
	UpdateCursor(NULL, NULL);
}

void _ApplicationFilter::SetWindowCursor(BWindow *window,
	const BCursor *cursor, const void *data, bool impose)
{
	if (window == NULL) return;

	const void *_data = NULL;
	_CursorStruct *curs;
	int32 index;

	// Check data
	if (data != NULL) {
		if (_check_cursor_data(data))
			_data = data;
	}

	// Check if the window already has a cursor
	for (index = 0; index < fCursors.CountItems(); index++) {
		curs = static_cast<_CursorStruct*>(fCursors.ItemAt(index));
		if (curs->window == window)
			break;
	}

	// Update old entry
	if (index < fCursors.CountItems()) {

		// Delete old cursor
		curs->system_cursor = NULL;
		if (curs->custom_cursor != NULL) {
			delete curs->custom_cursor;
			curs->custom_cursor = NULL;
		}
		curs->impose = false;

		// System cursor
		if (cursor != NULL) {
			curs->system_cursor = cursor;
			curs->impose = impose;
		}

		// Custom cursor
		else if (_data != NULL) {
			curs->custom_cursor = new BCursor(_data);
			curs->impose = impose;
		}

		// Delete entry
		else {
			fCursors.RemoveItem(curs);
			delete curs;
		}
	}

	// Add a new entry
	else if (cursor != NULL || _data != NULL) {
		curs = new _CursorStruct;
		curs->view = NULL;
		curs->window = window;
		curs->impose = impose;
		if (cursor != NULL) {
			curs->system_cursor = cursor;
			curs->custom_cursor = NULL;
		}
		else {
			curs->system_cursor = NULL;
			curs->custom_cursor = new BCursor(_data);
		}
		fCursors.AddItem(curs);
	}

	// Update the cursor on screen
	UpdateCursor(NULL, NULL);
}

#define GET_CURSOR(x) (x->custom_cursor != NULL ? \
	x->custom_cursor : x->system_cursor)

void _ApplicationFilter::UpdateCursor(BWindow *window, BView *view)
{
	_CursorStruct *cursor, *windowCursor = NULL;
	const BCursor *newCursor = NULL;
	uint32 buttons;
	BView *_view;
	BPoint point;
	int32 i;

	// TODO check which window is under the mouse pointer
	if (window == NULL) return;

	// It is safer to lock the window
	BAutolock locker(window);

	// App cursor is imposed? Ok, job is over
	if (fImposeAppCursor)
		newCursor = (fAppCursorCustom == NULL ?
			fAppCursorSystem : fAppCursorCustom);

	// Get window cursor
	if (newCursor == NULL) {
		for (i = 0; i < fCursors.CountItems(); i++) {
			cursor = static_cast<_CursorStruct*>(fCursors.ItemAt(i));
			if (cursor->window == window) {
				windowCursor = cursor;
				break;
			}
		}
	}

	// If the window cursor is imposed, don't even check for view's cursor
	if (windowCursor != NULL) {
		if (windowCursor->impose)
			newCursor = GET_CURSOR(windowCursor);
	}

	// If the window cursor isn't imposed, get view cursor
	if (newCursor == NULL) {

		// If no view was given, check wich view is below the mouse pointer
		if (view == NULL) {
			_view = window->ChildAt(0);
			if (_view != NULL) {
				_view->GetMouse(&point, &buttons);
				_view->ConvertToParent(&point);
				_view = window->FindView(point);
			}
		}
		else
			_view = view;

		// No view found
		if (_view != NULL) {
			for (i = 0; i < fCursors.CountItems(); i++) {
				cursor = static_cast<_CursorStruct*>(fCursors.ItemAt(i));
				if (cursor->view == _view) {
					newCursor = GET_CURSOR(cursor);
					break;
				}
			}
		}

		// If the view doesn't have a cursor, or window is viewless, use window
		// cursor
		if (newCursor == NULL && windowCursor != NULL)
			newCursor = GET_CURSOR(windowCursor);
	}

	// Not found a candidate cursor yet? Use app cursor
	if (newCursor == NULL)
		newCursor = (fAppCursorCustom == NULL ?
			fAppCursorSystem : fAppCursorCustom);

	// Apply cursor
	if (newCursor != fLastCursor) {
		//printf("Pop!\n");
		be_app->SetCursor(newCursor);
		fLastCursor = newCursor;
	}
}

// Public

filter_result _ApplicationFilter::Filter(BMessage *message, BHandler **)
{
	BCursor **system_cursor = new BCursor*;	// TODO I don't really like these,
	void **custom_cursor = new void*;		// but I couldn't find a better
	BWindow **window = new BWindow*;		// (compilable) option
	BView **view = new BView*;
	bool impose = false;
	int32 cookie = 0;
	thread_info info;
	BLooper *looper;

	switch (message->what) {

		case _MSG_CHECK_WINDOW_FILTERS:
			while (get_next_thread_info(0, &cookie, &info) == B_OK) {
				looper = BLooper::LooperForThread(info.thread);
				if (looper != NULL) {
					*window = dynamic_cast<BWindow*>(looper);
					if (*window != NULL)
						_get_window_filter(*window);
				}
			}
			break;

		case _MSG_SET_APP_CURSOR:
			message->FindBool("impose", &impose);
			if (message->FindPointer("system_cursor",
			  (void**)system_cursor) == B_OK) {
				SetAppCursor(*system_cursor, NULL, impose);
				break;
			}
			if (message->FindPointer("custom_cursor", custom_cursor) == B_OK) {
			  	SetAppCursor(NULL, *custom_cursor, impose);
			  	break;
			}
			SetAppCursor(NULL, NULL, false);
			break;

		case _MSG_SET_VIEW_CURSOR:
			if (message->FindPointer("view", (void**)view) != B_OK)
				break;
			message->FindBool("impose", &impose);
			if (message->FindPointer("system_cursor",
			  (void**)system_cursor) == B_OK) {
				SetViewCursor(*view, *system_cursor, NULL, impose);
				break;
			}
			if (message->FindPointer("custom_cursor", custom_cursor) == B_OK) {
			  	SetViewCursor(*view, NULL, *custom_cursor, impose);
			  	break;
			}
			SetViewCursor(*view, NULL, NULL, false);
			delete view;
			break;

		case _MSG_SET_WINDOW_CURSOR:
			if (message->FindPointer("window", (void**)window) != B_OK)
				break;
			message->FindBool("impose", &impose);
			if (message->FindPointer("system_cursor",
			  (void**)system_cursor) == B_OK) {
				SetWindowCursor(*window, *system_cursor, NULL, impose);
				break;
			}
			if (message->FindPointer("custom_cursor", custom_cursor) == B_OK) {
			  	SetWindowCursor(*window, NULL, *custom_cursor, impose);
			  	break;
			}
			SetWindowCursor(*window, NULL, NULL, false);
			break;

		case _MSG_UPDATE_CURSOR:
			if (message->FindPointer("window", (void**)window) != B_OK)
				break;	// TODO window-less update messages
			if (message->FindPointer("view", (void**)view) != B_OK)
				*view = NULL;
			UpdateCursor(*window, *view);
			break;
	}

	delete custom_cursor;
	delete system_cursor;
	delete view;
	delete window;

	// We may return B_DROP_MESSAGE, but you never know, maybe someone will
	// define a message with the same command...
	return B_DISPATCH_MESSAGE;
}

// +--------------------------------------------------------------------------+
// | WindowFilter                                                             |
// +--------------------------------------------------------------------------+
// | Its job is to route the relevant messages to the application filter.     |
// +--------------------------------------------------------------------------+

// Constructors and destructor

_WindowFilter::_WindowFilter(void) :
	BMessageFilter(B_PROGRAMMED_DELIVERY, B_ANY_SOURCE)
{
}

_WindowFilter::~_WindowFilter()
{
	// Nothing to do
}

// Public

filter_result _WindowFilter::Filter(BMessage *message, BHandler **target)
{
	if (message->what == B_MOUSE_MOVED || message->what == B_VIEW_MOVED ||
	  message->what == B_VIEW_RESIZED) {
	  	BMessage *msg = new BMessage(_MSG_UPDATE_CURSOR);
	  	msg->AddPointer("window", Looper());
	  	if (*target != NULL)
		  	msg->AddPointer("view", *target);
		  be_app->PostMessage(msg);
	}
	return B_DISPATCH_MESSAGE;
}

// +--------------------------------------------------------------------------+
// | Global functions                                                         |
// +--------------------------------------------------------------------------+

void SetAppCursor(const BCursor *cursor, bool impose)
{
	_install_app_filter();
	BMessage *message = new BMessage(_MSG_SET_APP_CURSOR);
	if (cursor != NULL)
		message->AddPointer("system_cursor", cursor);
	if (impose)
		message->AddBool("impose", true);
	be_app->PostMessage(message);
}

void SetAppCursor(const void *data, bool impose = false)
{
	_install_app_filter();
	BMessage *message = new BMessage(_MSG_SET_APP_CURSOR);
	if (data != NULL) {
		message->AddPointer("custom_cursor", data);
		if (impose)
			message->AddBool("impose", true);
	}
	be_app->PostMessage(message);
}

void SetViewCursor(BView *view, const BCursor *cursor)
{
	if (view == NULL) return;
	_install_app_filter();
	BMessage *message = new BMessage(_MSG_SET_VIEW_CURSOR);
	message->AddPointer("view", view);
	if (cursor != NULL)
		message->AddPointer("system_cursor", cursor);
	be_app->PostMessage(message);
}

void SetViewCursor(BView *view, const void *data)
{
	if (view == NULL) return;
	_install_app_filter();
	BMessage *message = new BMessage(_MSG_SET_VIEW_CURSOR);
	message->AddPointer("view", view);
	if (data != NULL)
		message->AddPointer("custom_cursor", data);
	be_app->PostMessage(message);
}

void SetWindowCursor(BWindow *window, const BCursor *cursor, bool impose)
{
	if (window == NULL) return;
	_install_app_filter();
	BMessage *message = new BMessage(_MSG_SET_WINDOW_CURSOR);
	message->AddPointer("window", window);
	if (cursor != NULL) {
		message->AddPointer("system_cursor", cursor);
		if (impose)
			message->AddBool("impose", true);
	}
	be_app->PostMessage(message);
}

void SetWindowCursor(BWindow *window, const void *data, bool impose)
{
	if (window == NULL) return;
	_install_app_filter();
	BMessage *message = new BMessage(_MSG_SET_WINDOW_CURSOR);
	message->AddPointer("window", window);
	if (data != NULL) {
		message->AddPointer("custom_cursor", data);
		if (impose)
			message->AddBool("impose", true);
	}
	be_app->PostMessage(message);
}

void UnsetAppCursor(void)
{
	SetAppCursor(static_cast<const void*>(NULL));
}

void UnsetViewCursor(BView *view)
{
	if (view == NULL) return;
	SetViewCursor(view, static_cast<const void*>(NULL));
}

void UnsetWindowCursor(BWindow *window)
{
	if (window == NULL) return;
	SetWindowCursor(window, static_cast<const void*>(NULL));
}
