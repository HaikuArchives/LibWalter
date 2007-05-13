// =============================================================================
//
// libwalter ToolbarBitmapButton.h
//
// Bitmap button control for WToolbar
//
// Revision: 20070513
// Page width 80, tab width 4, encoding UTF-8, line endings LF.
//
// Original author:
//     Gabriele Biffi - http://www.biffuz.it/
// Contributors:
//
// Released under the terms of the MIT license.
//
// =============================================================================

/*!
 * \struct WTBBBitmapSet
 * \brief Bitmap set to use with WToolbarBitmapButton.
 *
 * These are the rules for a valid set:
 * \li There must be at least the normal bitmap
 * \li Bitmaps must be square, all of the same size
 * \li All bitmaps must use the same color space
 * \li For each bitmap, a disabled version must be provided
 * \li If there is a disabled bitmap, then there must be the non-disabled
 *     version
 */
/*!
 * \var WTBBBitmapSet::disabled
 * \brief Bitmap used for disabled button, when it is not pushed.
 */
/*!
 * \var WTBBBitmapSet::normal
 * \brief Bitmap normally used for the button.
 */
/*!
 * \var WTBBBitmapSet::over
 * \brief Bitmap used for the button when the mouse is over it.
 */
/*!
 * \var WTBBBitmapSet::over_disabled
 * \brief Bitmap used for disabled button, when the mouse is over it.
 */
/*!
 * \var WTBBBitmapSet::pushed
 * \brief Bitmap used for pushed button.
 */
/*!
 * \var WTBBBitmapSet::pushed_disabled
 * \brief Bitmap used for pushed and disabled button, when the mouse is over it.
 */
/*!
 * \var WTBBBitmapSet::pushed_over
 * \brief Bitmap used for pushed button, when the mouse is over it.
 */
/*!
 * \var WTBBBitmapSet::pushed_over_disabled
 * \brief Bitmap used for pushed and disabled button, when the mouse is over it.
 */
/*!
 * \var WTBBBitmapSet::size
 * \brief Size of the bitmaps, in pixels. Width and height are the same.
 */

/*!
 * \class WToolbarBitmapButton
 * \brief The classic toolbar button, with a picture and an optional label.
 *
 * \section Bitmaps
 *
 * Like its ancestor WToolbarButton, a bitmap button can be in eight different
 * states: pushed, with the mouse over it, and disabled, and their combinations.
 * It is possible to assign a different bitmap to each of these states: we call
 * this a \b bitmap \b set, and it is represented by the structure
 * WTBBBitmapSet. All bitmaps in the set have the same size (width and height)
 * and the same color space.
 *
 * You can assign to a bitmap button as much sets as you want, as long as they
 * have a different size; the button will choose the bitmap set of the same size
 * of toolbar picture size (see WToolbar::PictureSize()), or the biggest that
 * fits; if there's no such a bitmap set, the button won't display any picture
 * (but it will respect the expected size).
 */

// Standard C++ headers
#include <typeinfo>

// BeOS headers
#include <Bitmap.h>
#include <Screen.h>

// AK headers
#include "Toolbar.h"
#include "ToolbarBitmapButton.h"
#include "ToolbarSupport.h"

// =============================================================================
// Internal functions
// =============================================================================

void __wtbbbitmapset_unarchive_bitmap(int size, color_space colors,
	BMessage *archive, const char *normalName, const char *disabledName,
	BBitmap *&normal, BBitmap *&disabled)
{
	BArchivable *archivable;
	char buffer[128];
	BMessage message;
	BBitmap *bitmap;

	// Normal bitmap
	normal = NULL;
	strcpy(buffer, "WTBBBitmapSet::");
	strcat(buffer, normalName);
	if (archive->FindMessage(buffer, &message) == B_OK) {
		archivable = instantiate_object(&message);
		if (archivable != NULL) {
			bitmap = dynamic_cast<BBitmap*>(archivable);
			if (bitmap != NULL) {
				if (bitmap->IsValid()) {
					if (bitmap->Bounds().Width() == bitmap->Bounds().Height() &&
					  bitmap->Bounds().Width() + 1.0 ==
					  static_cast<float>(size) &&
					  bitmap->ColorSpace() == colors)
						normal = bitmap;
					else
						delete bitmap;
				}
				else
					delete bitmap;
			}
			else
				delete archivable;
		}
	}

	// Disabled bitmap
	disabled = NULL;
	if (normal != NULL) {
		strcpy(buffer, "WTBBBitmapSet::");
		strcat(buffer, disabledName);
		if (archive->FindMessage(buffer, &message) == B_OK) {
			archivable = instantiate_object(&message);
			if (archivable != NULL) {
				bitmap = dynamic_cast<BBitmap*>(archivable);
				if (bitmap != NULL) {
					if (bitmap->IsValid()) {
						if (bitmap->Bounds().Width() ==
						  bitmap->Bounds().Height() &&
						  bitmap->Bounds().Width() + 1.0 ==
						  static_cast<float>(size) &&
						  bitmap->ColorSpace() == colors)
							disabled = bitmap;
						else
							delete bitmap;
					}
					else
						delete bitmap;
				}
				else
					delete archivable;
			}
		}
		if (disabled == NULL)
			disabled = WToolbarSupport::GrayscaleBitmap(normal);
	}
	if (disabled == NULL) {
		delete normal;
		normal = NULL;
	}
}

// =============================================================================
// WTBBBitmapSet
// =============================================================================

// Constructors and destructor

/*!
 * \brief Creates a new, empty set
 */
WTBBBitmapSet::WTBBBitmapSet(void)
{
	_init_object();
}

/*!
 * \brief Creates a copy of a set
 *
 * This constructor copies all the bitmaps of the original, without any check
 * about the set's validity.
 *
 * @param[in] set The set you want to copy
 */
WTBBBitmapSet::WTBBBitmapSet(WTBBBitmapSet &set) :
	BArchivable()
{
	_init_object();
	size = set.size;
	if (set.normal != NULL)
		normal = new BBitmap(set.normal);
	if (set.disabled != NULL)
		disabled = new BBitmap(set.disabled);
	if (set.over != NULL)
		over = new BBitmap(set.over);
	if (set.over_disabled != NULL)
		over_disabled = new BBitmap(set.over_disabled);
	if (set.pushed != NULL)
		pushed = new BBitmap(set.pushed);
	if (set.pushed_disabled != NULL)
		pushed_disabled = new BBitmap(set.pushed_disabled);
	if (set.pushed_over != NULL)
		pushed_over = new BBitmap(set.pushed_over);
	if (set.pushed_over_disabled != NULL)
		pushed_over_disabled = new BBitmap(set.pushed_over_disabled);
}

/*!
 * \brief Unarchive the set
 *
 * While unarchiving, it checks that the bitmaps makes a valid set.
 *
 * \todo Check with malformed archives (like invalid sets or duplicates)
 *
 * @param[in] archive The archive wich contains the bitmap set
 */
WTBBBitmapSet::WTBBBitmapSet(BMessage *archive) :
	BArchivable(archive)
{
	color_space colors = B_NO_COLOR_SPACE;
	BArchivable *archivable;
	BMessage message;
	BBitmap *bitmap;

	_init_object();

	// Normal ------------------------------------------------------------------

	if (archive->FindMessage("WTBBBitmapSet::normal", &message) == B_OK) {
		archivable = instantiate_object(&message);
		if (archivable != NULL) {
			bitmap = dynamic_cast<BBitmap*>(archivable);
			if (bitmap != NULL) {
				if (bitmap->IsValid()) {
					if (bitmap->Bounds().Width() == bitmap->Bounds().Height()) {
						normal = bitmap;
						size = static_cast<int>(bitmap->Bounds().Width() + 1.0);
						colors = bitmap->ColorSpace();
					}
					else
						delete bitmap;
				}
				else
					delete bitmap;
			}
			else
				delete archivable;
		}
	}
	if (normal == NULL)			// At least the normal bitmap is required!
		return;

	if (archive->FindMessage("WTBBBitmapSet::disabled", &message) == B_OK) {
		archivable = instantiate_object(&message);
		if (archivable != NULL) {
			bitmap = dynamic_cast<BBitmap*>(archivable);
			if (bitmap != NULL) {
				if (bitmap->IsValid()) {
					if (bitmap->Bounds().Width() == bitmap->Bounds().Height() &&
					  bitmap->Bounds().Width() + 1.0 ==
					  static_cast<float>(size) &&
					  bitmap->ColorSpace() == colors)
						disabled = bitmap;
					else
						delete bitmap;
				}
				else
					delete bitmap;
			}
			else
				delete archivable;
		}
	}
	if (disabled == NULL) {		// We have the normal, we need the disabled!
		disabled = WToolbarSupport::GrayscaleBitmap(normal);
		if (disabled == NULL) {
			delete normal;
			normal = NULL;
			size = 0;
			return;
		}
	}

	// Over --------------------------------------------------------------------

	__wtbbbitmapset_unarchive_bitmap(size, colors, archive, "over",
		"over_disabled", over, over_disabled);

	// Pushed ------------------------------------------------------------------

	__wtbbbitmapset_unarchive_bitmap(size, colors, archive, "pushed",
		"pushed_disabled", pushed, pushed_disabled);

	// Pushed_over -------------------------------------------------------------

	__wtbbbitmapset_unarchive_bitmap(size, colors, archive, "pushed_over",
		"pushed_over_disabled", over, over_disabled);
}

/*!
 * \brief Deletes the object and all the bitmaps
 */
WTBBBitmapSet::~WTBBBitmapSet()
{
	if (normal != NULL)
		delete normal;
	if (disabled != NULL)
		delete disabled;
	if (over != NULL)
		delete over;
	if (over_disabled != NULL)
		delete over_disabled;
	if (pushed != NULL)
		delete pushed;
	if (pushed_disabled != NULL)
		delete pushed_disabled;
	if (pushed_over != NULL)
		delete pushed_over;
	if (pushed_over_disabled != NULL)
		delete pushed_over_disabled;
}

// Private or protected

void WTBBBitmapSet::_init_object(void)
{
	size = 0;
	normal = NULL;
	disabled = NULL;
	over = NULL;
	over_disabled = NULL;
	pushed = NULL;
	pushed_disabled = NULL;
	pushed_over = NULL;
	pushed_over_disabled = NULL;
}

// Public

// BArchivable hooks

/*!
 * \brief Archive the bitmap set
 *
 * Archives all the bitmaps.
 *
 * @param[in,out] archive The message where to archive the bitmap set
 * @param[in] deep Unused
 *
 * \return Return B_OK if successfull, or some error code if it fails.
 */
status_t WTBBBitmapSet::Archive(BMessage *archive, bool deep) const
{
	status_t status;

	status = BArchivable::Archive(archive, deep);

	if (status == B_OK && normal != NULL) {
		BMessage bmp;
		status = normal->Archive(&bmp);
		if (status == B_OK)
			status = archive->AddMessage("WTBBBitmapSet::normal", &bmp);
	}

	if (status == B_OK && disabled != NULL) {
		BMessage bmp;
		status = disabled->Archive(&bmp);
		if (status == B_OK)
			status = archive->AddMessage("WTBBBitmapSet::disabled", &bmp);
	}

	if (status == B_OK && over != NULL) {
		BMessage bmp;
		status = over->Archive(&bmp);
		if (status == B_OK)
			status = archive->AddMessage("WTBBBitmapSet::over", &bmp);
	}

	if (status == B_OK && over_disabled != NULL) {
		BMessage bmp;
		status =  over_disabled->Archive(&bmp);
		if (status == B_OK)
			status = archive->AddMessage("WTBBBitmapSet:: over_disabled",
				&bmp);
	}

	if (status == B_OK && pushed != NULL) {
		BMessage bmp;
		status = pushed->Archive(&bmp);
		if (status == B_OK)
			status = archive->AddMessage("WTBBBitmapSet::pushed", &bmp);
	}

	if (status == B_OK && pushed_disabled != NULL) {
		BMessage bmp;
		status = pushed_disabled->Archive(&bmp);
		if (status == B_OK)
			status = archive->AddMessage("WTBBBitmapSet::pushed_disabled",
				&bmp);
	}

	if (status == B_OK && pushed_over != NULL) {
		BMessage bmp;
		status = pushed_over->Archive(&bmp);
		if (status == B_OK)
			status = archive->AddMessage("WTBBBitmapSet::pushed_over", &bmp);
	}

	if (status == B_OK && pushed_over_disabled != NULL) {
		BMessage bmp;
		status = pushed_over_disabled->Archive(&bmp);
		if (status == B_OK)
			status = archive->AddMessage("WTBBBitmapSet::pushed_over_disabled",
				&bmp);
	}

	return status;
}

/*!
 * \brief Instantiate an instance of the bitmap set.
 *
 * @param[in] archive The archive wich contains the bitmap set.
 *
 * \return Return the unarchived bitmap set, or  \c NULL if the archive doesn't
 *         contain a bitmap set.
 */
BArchivable * WTBBBitmapSet::Instantiate(BMessage *archive)
{
	return (validate_instantiation(archive, "WTBBBitmapSet") ?
		new WTBBBitmapSet(archive) : NULL);
}

// =============================================================================
// WToolbarBitmapButton
// =============================================================================

// Constructors and destructors

/*!
 * \brief Creates a new bitmap button.
 *
 * A freshly created button has only a label, not a picture. To add a picture,
 * call AddBitmapSet().
 *
 * @param[in] name The name of the control. Can be \c NULL.
 * @param[in] label The label of the button. Can be \c NULL.
 * @param[in] message The message that's sent to the target when the button is
 *                    clicked. Can be \c NULL.
 */
WToolbarBitmapButton::WToolbarBitmapButton(const char *name,
	const char *label, BMessage *message) :
	WToolbarButton(name, label, message)
{
	_init_object();
	SetLabel(label);
}

/*!
 * \brief Unarchive a button.
 *
 * \todo Check with malformed archives (like invalid sets or duplicates)
 *
 * @param[in] archive The archive to extract the button from.
 */
WToolbarBitmapButton::WToolbarBitmapButton(BMessage *archive) :
	WToolbarButton(archive)
{
	BArchivable *archivable;
	WTBBBitmapSet *set;
	BMessage message;
	unsigned sets, i;
	int32 index;

	_init_object();

	index = 0;
	while (archive->FindMessage("WToolbarBitmapButton::bitmap_set", index,
	  &message) == B_OK) {
		archivable = instantiate_object(&message);
		if (archivable != NULL) {
			set = dynamic_cast<WTBBBitmapSet*>(archivable);
			if (set != NULL) {
				set = (WTBBBitmapSet*)archivable;
				if (set->size > 0) {
					sets = fBitmaps.size();
					i = 0;
					while (i < sets) {
						if (fBitmaps[i]->size == set->size) break;
						i++;
					}
					if (i == sets)
						fBitmaps.push_back(set);
					else
						delete set;
				}
				else
					delete set;
			}
			else
				delete archivable;
		}
		index++;
	}
}

/*!
 * \brief Deletes the object and all the bitmaps
 */
WToolbarBitmapButton::~WToolbarBitmapButton()
{
	// Delete all bitmaps
	while (fBitmaps.size() > 0) {
		delete fBitmaps[fBitmaps.size() - 1];
		fBitmaps.pop_back();
	}
}

// Private

void WToolbarBitmapButton::_init_object(void)
{
}

// Protected

/*!
 * \brief Draw the bitmap of the button.
 *
 * Reimplemented from WToolbarButton to draw the correct bitmap. Alpha channel
 * is respected. If there's no bitmap to draw, the toolbar's style is
 * W_TOOLBAR_MENU, and the button's value is B_CONTROL_ON, then a check mark
 * will be drawn in place of the picture.
 *
 * @param[in] position The upper left point of the bitmap, in button's
 *                     coordinates. If the bitmap is smaller than the toolbar's
 *                     picture size, it will be centered in the expected
 *                     position.
 * @param[in] updateRect The rectangle that needs to be updated, in button's
 *                       coordinates.
 */
void WToolbarBitmapButton::DrawPicture(BPoint position, BRect updateRect)
{
	bool enabled = Enabled() && Toolbar()->Enabled();
	float pic_size = 0.0, pic_x = 0.0, pic_y = 0.0;
	BBitmap *bitmap = NULL;
	WTBBBitmapSet *set;

	// Select the picture to use. The picture will always use the toolbar's
	// picture size; if the bitmap is smaller, we will center it.
	pic_size = (float)(Toolbar()->PictureSize());
	set = GetBitmapSet((unsigned)pic_size);
	if (set != NULL) {
		bitmap = (enabled ? set->normal : set->disabled);
		if (fMouseDown && !fMouseOver && set->pushed != NULL)
			bitmap = (enabled ? set->pushed : set->pushed_disabled);
		if (!fMouseDown && fMouseOver && set->over != NULL)
			bitmap = (enabled ? set->over : set->over_disabled);
		if (fMouseDown && fMouseOver && set->pushed_over != NULL)
			bitmap = (enabled ? set->pushed_over :
				set->pushed_over_disabled);
	}

	if (bitmap != NULL) {
		bool pushed = false;

		// Fancy alpha blending!
		fCanvas->SetDrawingMode(B_OP_ALPHA);

		// Menus require a bit more handling.
		if (fStyle == W_TOOLBAR_STYLE_MENU) {
			pic_size += 6.0;
			pic_x += 2.0;
			pic_y += 2.0;
			if (Value() == B_CONTROL_ON) {
				BRect r(position, BPoint(position.x + pic_size - 1.0,
					position.y + pic_size - 1.0));
				pushed = true;
				fCanvas->SetHighColor(255, 255, 255, 128);
				fCanvas->FillRect(r, B_SOLID_HIGH);
				WToolbarSupport::Draw3DBorder(fCanvas, r, true);
			}
		}
		else {

			// Center the bitmap
			if (set->size < (unsigned)pic_size) {
				pic_x += floor((pic_size - (float)set->size) / 2.0);
				pic_y += floor((pic_size - (float)set->size) / 2.0);
			}
		}

		// Is the button pushed?
		if (fStyle != W_TOOLBAR_STYLE_MENU &&
		  ((fMouseOver && fMouseDown) || Value() == B_CONTROL_ON))
			pushed = true;

		// If the button is pushed, move the bitmap one pixel off
		if (pushed) {
			pic_x++;
			pic_y++;
		}

		// Draw picture
		fCanvas->DrawBitmap(bitmap, BPoint(position.x + pic_x,
			position.y + pic_y));
	}
	else if (fStyle == W_TOOLBAR_STYLE_MENU && Value() == B_CONTROL_ON) {
		// Ok, draw the check mark.
		BBitmap *mark;
		float width;

		mark = GetMenuCheckMark();

		// Real X position
		width = mark->Bounds().Width() + 1.0;
		pic_x = (width < pic_size ?
			position.x + floor((pic_size - width) / 2.0) :
			position.x);
		pic_x += 3.0;

		// Draw the check mark
		fCanvas->SetDrawingMode(B_OP_OVER);
		fCanvas->DrawBitmap(mark, BPoint(pic_x, position.y));

		delete mark;
	}
}

/*!
 * \brief Return the size of the picture.
 *
 * It doesn't return the actual size of the bitmap, but the toolbar's picture
 * size, plus one pixel for the pushed effect. The bitmap will be centered in
 * this size.
 * If style is menu, it reserves 3 pixels on every side to draw a small border
 * around the picture, in place of the check mark. If there's a picture, of
 * course, unless it will stick with the check mark.
 *
 * @param[out] width The width of the picture in toolbar's units (less 1.0), can
 *                   be \c NULL if you're not interested in it.
 * @param[out] height The height of the picture in toolbar's units (less 1.0),
 *                    can be \c NULL if you're not interested in it.
 */
void WToolbarBitmapButton::GetPictureSize(float *width, float *height)
{
	float picSize, w, h;
	picSize = (float)Toolbar()->PictureSize();
	if (fStyle == W_TOOLBAR_STYLE_MENU) {
		if (GetBitmapSet((unsigned)picSize) == NULL) {
			BBitmap *mark = GetMenuCheckMark();
			w = mark->Bounds().Width() + 1.0;
			h = mark->Bounds().Width() + 1.0;
			delete mark;
			if (w < picSize)
				w = picSize;
			w += 6.0;
		}
		else {
			w = picSize + 6.0;
			h = picSize + 6.0;
		}
	}
	else {
		w = picSize + 1.0;
		h = picSize + 1.0;
	}
	if (width != NULL) *width = w - 1.0;
	if (height != NULL) *height = h - 1.0;
}

// BArchivable hooks

/*!
 * \brief Archive the button
 *
 * Archives all the bitmap button's properties, as well as all the bitmaps.
 *
 * @param[in,out] archive The message where to archive the button.
 * @param[in] deep Unused.
 *
 * \return Return \c B_OK if successfull, or some error code if it fails.
 */
status_t WToolbarBitmapButton::Archive(BMessage *archive, bool deep) const
{
	const unsigned kBitmaps = fBitmaps.size();
	status_t status;

	status = WToolbarButton::Archive(archive, deep);

	if (status == B_OK && kBitmaps > 0) {
		for (unsigned i = 0; i < kBitmaps && status == B_OK; i++) {
			BMessage set;
			status = fBitmaps[i]->Archive(&set);
			if (status == B_OK)
				status = archive->AddMessage(
					"WToolbarBitmapButton::bitmap_set", &set);
		}
	}

	return status;
}

/*!
 * \brief Instantiate an instance of the button.
 *
 * @param[in] archive The archive wich contains the button.
 *
 * \return Return the unarchived button, or  \c NULL if the archive doesn't
 *         contain a bitmap button.
 */
BArchivable * WToolbarBitmapButton::Instantiate(BMessage *archive)
{
	return (validate_instantiation(archive, "WToolbarBitmapButton") ?
		new WToolbarBitmapButton(archive) : NULL);
}

// Bitmaps

#define ALL_BITMAPS		n, d, p, pd, o, od, po, pod
#define BITMAP_PARAMS	BBitmap *n, BBitmap *d, BBitmap *p, BBitmap *pd, \
						BBitmap *o, BBitmap *od, BBitmap *po, BBitmap *pod
#define CHECK_BMP(x)	if (x != NULL) { if (\
						x->Bounds().IntegerHeight() != size || \
						x->Bounds().IntegerWidth() != size || \
						x->ColorSpace() != colors) return false; }
#define DELETE_BMP(x)	if (x != NULL) delete x

void __check_disabled(BBitmap *&bitmap, BBitmap *&disabled_bitmap)
{
	// No bitmap? No disabled!
	if (bitmap == NULL) {
		if (disabled_bitmap != NULL) {
			delete disabled_bitmap;
			disabled_bitmap = NULL;
		}
		return;
	}

	// There is a bitmap AND a disabled? Why bother, then?
	if (disabled_bitmap != NULL)
		return;

	// Ok, time to get that grayscale bitmap
	disabled_bitmap = WToolbarSupport::GrayscaleBitmap(bitmap);

	// Something gone bad, delete everything.
	if (disabled_bitmap == NULL) {
		delete bitmap;
		bitmap = NULL;
	}
}

bool __check_bitmaps(int32 size, color_space colors, BITMAP_PARAMS)
{
	CHECK_BMP(n);
	CHECK_BMP(d);
	CHECK_BMP(p);
	CHECK_BMP(pd);
	CHECK_BMP(o);
	CHECK_BMP(od);
	CHECK_BMP(po);
	CHECK_BMP(pod);
	return true;
}

void __delete_bitmaps(BITMAP_PARAMS)
{
	DELETE_BMP(n);
	DELETE_BMP(d);
	DELETE_BMP(p);
	DELETE_BMP(pd);
	DELETE_BMP(o);
	DELETE_BMP(od);
	DELETE_BMP(po);
	DELETE_BMP(pod);
}

/*!
 * \brief Add a bitmap set to the button.
 *
 * First, read Bitmaps at the beginning of this chapter. The bitmap set is built
 * from the bitmaps you pass, but you have to respect some rules:
 *
 * \li You must specify at least the normal bitmap.
 * \li Bitmaps must be square (width equals height) and the size must be the
 *     same for every bitmap.
 * \li Bitmaps must use the same color space.
 * \li Bitmaps will become property of the button, even if this method fails.
 * \li If you specify a bitmap for a state but not the corresponding disabled,
 *     it will be generated for you by turning the original image in grayscale,
 *     using the same color space of the original (supported color spaces for
 *     this operation are the ones supported by
 *     WToolbarSupport::GrayscaleBitmap())
 * \li If you specify a "disabled" bitmap but not the corresponding
 *     "non-disabled" bitmap, it will be discarded (deleted).
 * \li If a bitmap set for the specified size is already present, it will be
 *     discarded and the new one will be used.
 *
 * \return Returns \c true if the set was succesfully added to the bitmap or
 *         \c false if there was some error, but please remember that in any
 *         case the bitmaps will become property of the button!
 *
 * \todo Support more color spaces.
 */
bool WToolbarBitmapButton::AddBitmapSet(BITMAP_PARAMS)
{
	WTBBBitmapSet *set, *old_set;
	color_space colors;
	unsigned size;

	// We need at least the normal bitmap
	if (n == NULL) {
		__delete_bitmaps(ALL_BITMAPS);
		return false;
	}

	// Check the sizes and color spaces
	size = (unsigned)(n->Bounds().IntegerWidth()) + 1;
	colors = n->ColorSpace();
	if (!__check_bitmaps((int32)size - 1, colors, ALL_BITMAPS)) {
		__delete_bitmaps(ALL_BITMAPS);
		return false;
	}

	// Checks and creates all the disabled bitmaps
	__check_disabled(n, d);
	__check_disabled(p, pd);
	__check_disabled(o, od);
	__check_disabled(po, pod);

	// What, no bitmaps left??
	if (n == NULL) {
		__delete_bitmaps(ALL_BITMAPS);
		return false;
	}

	// Create the new set
	set = new WTBBBitmapSet();
	set->size = size;
	set->normal = n;
	set->disabled = d;
	set->pushed = p;
	set->pushed_disabled = pd;
	set->over = o;
	set->over_disabled = od;
	set->pushed_over = po;
	set->pushed_over_disabled = pod;

	// Remove previous bitmap set
	old_set = RemoveBitmapSet(size);
	if (old_set != NULL)
		delete old_set;

	// Add the button to the set
	fBitmaps.push_back(set);

	Invalidate();
	return true;
}

#undef DELETE_BMP
#undef CHECK_BMP
#undef BITMAP_PARAMS
#undef ALL_BITMAPS

/*!
 * \brief Return the number of bitmap sets
 */
unsigned WToolbarBitmapButton::CountBitmapSets(void)
{
	return fBitmaps.size();
}

/*!
 * \brief Return the bitmap set used by the button whit the given picture size
 *
 * @param[in] size The size you want the bitmap set for
 *
 * \return The bitmap set of the same size, or the largest one that fits into
 *         it. Can be  \c NULL if no set applies. The returned set belongs to
 *         the object, don't delete it!
 */
WTBBBitmapSet * WToolbarBitmapButton::GetBitmapSet(unsigned size)
{
	if (size == 0) return NULL;
	WTBBBitmapSet *candidate = NULL;
	unsigned i = 0;
	while (i < fBitmaps.size()) {
		if (fBitmaps[i]->size == size)
			return fBitmaps[i];
		if (fBitmaps[i]->size < size) {
			if (candidate == NULL)
				candidate = fBitmaps[i];
			else {
				if (fBitmaps[i]->size > candidate->size)
					candidate = fBitmaps[i];
			}
		}
		i++;
	}
	return candidate;
}

/*!
 * \brief Return the bitmap set at given index
 *
 * @param[in] index The index of the set you want to obtain, start from 0.
 *
 * \return The bitmap set at index, or  \c NULL if there's no set at the given
 *         index. The returned set belongs to the object, don't delete it!
 */
WTBBBitmapSet * WToolbarBitmapButton::GetBitmapSetAt(unsigned index)
{
	return (index < fBitmaps.size() ? fBitmaps[index] : NULL);
}

/*!
 * \brief Removes a bitmap set from the button
 *
 * The set is removed from the button, but it is not deleted; it is up to you.
 *
 * @param[in] size The size of the set you want to remove
 *
 * \return The requested set, or  \c NULL if there's no set of the given size.
 */
WTBBBitmapSet * WToolbarBitmapButton::RemoveBitmapSet(unsigned size)
{
	WTBBBitmapSet *set;
	for (unsigned i = 0; i < fBitmaps.size(); i++) {
		set = fBitmaps[i];
		if (set->size == size) {
			fBitmaps.erase(fBitmaps.begin() + i);
			return set;
		}
	}
	return NULL;
}
