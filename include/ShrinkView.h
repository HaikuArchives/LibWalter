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

#ifndef BENET_SHRINK
#define BENET_SHRINK 0x000F0001
#endif

#ifndef BENET_EXPAND
#define BENET_EXPAND 0x000F0002
#endif

class ShrinkView: public BView
{
	public:
		ShrinkView(BRect frame, char * pLabel, bool bShrink = false, BBitmap* pBmp = NULL);
		bool IsShrink() { return m_bShrink; }	
		

		//Fonction de BView redefini.
		virtual void Draw(BRect updateRect);
		virtual void AttachedToWindow();
		virtual void MouseDown(BPoint point);
		virtual void MouseMoved(BPoint point, uint32 transit, const BMessage* message);

	private:
		float m_fFullHeight;	//La hauteur initiale de la View.
		bool m_bShrink;			//True si le View est reduite, false sinon.	
		bool m_bMouseOver;		//True si le curseur est au dessus du boutton d'agrandissement et reduction.
		char m_pzLabel[256];	//L'etiquette a affichier.
		BBitmap* m_pBmp;

		//Fonction appele pour agrandir ou reduire la View.
		void Shrink();
		void Expand();
		void DrawDegrader(BRect rect, int iStartGray, int iEndGray);
};

#endif //__SHRINK_VIEW__
