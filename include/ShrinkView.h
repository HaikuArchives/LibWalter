/***********************************************************************************************\
*	ShrinkView.h																				*
*************************************************************************************************
*	Programmer par: Patrick Henri																*
*	Derniere modification: 09-03-99																*
*************************************************************************************************
*	Cette classe implemente une BView retractable. 												*
\***********************************************************************************************/

#ifndef __SHRINK_VIEW__
#define __SHRINK_VIEW__

#include <View.h>
#include <Bitmap.h>

class ShrinkView: public BView {

public:

	typedef enum {
		B_SHRINK = 0x00080001,
		B_EXPAND = 0x00080002,
		B_NORMAL = (1 << 0),
		B_AUTO_FIT_WINDOW = (1 << 1),
	} shrink_mode;

					ShrinkView(BRect frame, char* pzLabel, bool bShrink = false,
						BBitmap* pBmp = NULL, uint32 flags = B_NORMAL);
			bool	IsShrink() { return m_bShrink; }
			void	SetMode(uint32 flags);

	//Fonction de BView redefini.
	virtual void 	Draw(BRect updateRect);
	virtual void 	AttachedToWindow();
	virtual void 	MouseDown(BPoint point);
	virtual void 	MouseMoved(BPoint point, uint32 transit, const BMessage* message);

private:
			float 		m_fFullHeight;	//La hauteur initiale de la View
			bool 		m_bShrink;		//True si le View est reduite, false sinon
			bool		m_bMouseOver;	//True si le curseur est au dessus du
										//boutton d'agrandissement et reduction
			char 		m_pzLabel[256];	//L'etiquette a affichier
			BBitmap* 	m_pBmp;
			uint32		fShrinkFlags;

			//Fonction appele pour agrandir ou reduire la View.
			void		Shrink();
			void		Expand();
			void		DrawDegrader(BRect rect, int iStartGray, int iEndGray);
};

#endif //__SHRINK_VIEW__
