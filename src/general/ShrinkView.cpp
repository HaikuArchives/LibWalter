/***********************************************************************************************\
*	ShrinkView.cpp																				*
*************************************************************************************************
*	Programmer par: Patrick Henri																*
*	Derniere modification: 09-03-99																*
*************************************************************************************************
*	Cette classe implemente une BView retractable. 												*
\***********************************************************************************************/

#ifndef __SHRINK_VIEW__
#include "ShrinkView.h"
#endif

#include <Window.h>

/*=============================================================================================*\
|	ShrinkView																					|
+-----------------------------------------------------------------------------------------------+
|	Effet: Appelle le constructeur de BView et initialise les valeurs de bases.					|
|	Entre: 																						|
|		BRect frame: Rectagle de la View lorsqu'elle est agrandit.								|
|		char * pzLabel: Etiquette de la view.													|
|		bool bShrink: True si la view doit etre initialiment reduite, false sinon.				|
\*=============================================================================================*/
ShrinkView::ShrinkView(BRect frame, char * pzLabel, bool bShrink, BBitmap * pBmp)
		: BView::BView(frame, "ShrinkView", B_FOLLOW_NONE, B_WILL_DRAW)	
{
	rgb_color color = {192, 192, 192, 0};
	SetViewColor(color);
	m_fFullHeight = frame.Height();
	m_bShrink = bShrink;
	m_bMouseOver = false;
	strcpy(m_pzLabel, pzLabel);
	
	if(m_bShrink)
	{
		ResizeTo(Bounds().Width(), 16);
	}
	m_pBmp = pBmp;	
}//End of ShrinkView.

/*=============================================================================================*\
|	Draw																						|
+-----------------------------------------------------------------------------------------------+
|	Effet: Redessiner une partie de la view.													|
|	Entre: 																						|
|		BRect frame: Rectagle qui a besoin d'etre redessine.									|
\*=============================================================================================*/
void ShrinkView::Draw(BRect updateRect)
{
	BPoint pPointList[8];		//Utilise pour dessiner les polygone.
	rgb_color color = {0,0,0,0};
	rgb_color colorHigh = {0,0,0,0};
	
	SetDrawingMode(B_OP_COPY);
	if(m_pBmp)
	{
		uint32 * uiBits = (uint32*)m_pBmp->Bits();
		uint32 uiColor = uiBits[141];
		uint32 uiColorHigh = uiBits[289];
		
		color.red = ( uiColor & 0xFF0000) / 0x10000;
		color.green = (uiColor & 0xFF00) / 0x100;
		color.blue = (uiColor & 0xFF) ;
		colorHigh.red = ( uiColorHigh & 0xFF0000) / 0x10000;
		colorHigh.green = (uiColorHigh & 0xFF00) / 0x100;
		colorHigh.blue = (uiColorHigh & 0xFF) ;
			
	}

	//if(updateRect.left < 10)
	{
		if(m_pBmp)
		{
			DrawBitmap(m_pBmp, BRect(140,15,147,15), BRect(Bounds().Width() - 7, 16, Bounds().Width(), Bounds().Height() - 7));				 
		}

	}
	//if(updateRect.right > Bounds().Width()-10)
	{
		if(m_pBmp)
		{
			DrawBitmap(m_pBmp, BRect(140,14,147,14), BRect(0, 16, 7, Bounds().Height() - 7));

		}
	}
	
	
	//Dessiner l'etiquette si necessaire.
	if(updateRect.top < 16 && updateRect.right >= 16)
	{
		
		if(m_pBmp)
		{
			
			if(m_bShrink && m_bMouseOver)
			{
				DrawBitmap(m_pBmp, BRect(80,0,95,15), BRect(Bounds().Width() - 15,0,Bounds().Width(),15));
				DrawBitmap(m_pBmp, BRect(137,0,137,15), BRect(16, 0, Bounds().Width() - 15, 15)); 
			}
			else if(m_bShrink)
			{
				DrawBitmap(m_pBmp, BRect(64,0,79,15), BRect(Bounds().Width() - 15,0,Bounds().Width(),15));
				DrawBitmap(m_pBmp, BRect(136,0,136,15), BRect(16, 0, Bounds().Width() - 15, 15)); 
			}
			else if(m_bMouseOver)
			{
				DrawBitmap(m_pBmp, BRect(112,0,127,15), BRect(Bounds().Width() - 15,0,Bounds().Width(),15));
				DrawBitmap(m_pBmp, BRect(139,0,139,15), BRect(16, 0, Bounds().Width() - 15, 15)); 
			}
			else
			{
				DrawBitmap(m_pBmp, BRect(96,0,111,15), BRect(Bounds().Width() - 15,0,Bounds().Width(),15));
				DrawBitmap(m_pBmp, BRect(138,0,138,15), BRect(16, 0, Bounds().Width() - 15 , 15)); 
			}
			SetFont(be_bold_font);
					
			if(m_bMouseOver)
			{
				SetHighColor(colorHigh);
			}
			else
			{
				SetHighColor(color);
			}	
			SetDrawingMode(B_OP_OVER);
			DrawString(m_pzLabel, BPoint(18,12), NULL);	
			SetDrawingMode(B_OP_COPY);	
		}
		else
		{
			if(m_bShrink)
			{
				DrawDegrader(BRect(16,0,Bounds().Width(),4), 255, 192);
				DrawDegrader(BRect(16,11,Bounds().Width(),15), 192, 128);
			
				//Remplir le milieu de l'etiquette en gris
				SetHighColor(192, 192, 192, 0);
				FillRect(BRect(16, 5, Bounds().Width(), 10));
			}
			else	//Degrader du blanc au gris
			{
				DrawDegrader(BRect(16,0,Bounds().Width(),15), 255, 192);
			}
			SetFont(be_bold_font);
			SetHighColor(0, 0, 0, 0);
			SetDrawingMode(B_OP_OVER);
			DrawString(m_pzLabel, BPoint(18,12), NULL);
		}
	}
		
	//Redessiner le boutton si nessesaire.
	if(updateRect.left < 16 && updateRect.top < 16)
	{
		if(m_pBmp)
		{
			if(m_bShrink && m_bMouseOver)
			{
				DrawBitmap(m_pBmp, BRect(16,0,31,15), BRect(0,0,15,15));
			}
			else if(m_bShrink)
			{
				DrawBitmap(m_pBmp, BRect(0,0,15,15), BRect(0,0,15,15));
			}
			else if(m_bMouseOver)
			{
				DrawBitmap(m_pBmp, BRect(48,0,63,15), BRect(0,0,15,15));
			}
			else
			{
				DrawBitmap(m_pBmp, BRect(32,0,47,15), BRect(0,0,15,15));
			}
		
		}
		else
		{
			//Redessiner le degrader
			if(m_bShrink)
			{
				DrawDegrader(BRect(0, 0, 16, 4), 255, 192);
				DrawDegrader(BRect(0, 11, 16, 15), 192, 128);
			
				//Remplir le milieu de l'etiquette en gris
				SetHighColor(192, 192, 192, 0);
				FillRect(BRect(0, 5, 15, 10));
			}
			else	//Degrader du blanc au gris
			{
				DrawDegrader(BRect(0,0,15,15), 255, 192);
			}
		
			//Dessiner le Boutton si le curseur est au dessus du triangle.
			if( m_bMouseOver)
			{	
			SetHighColor(64, 64, 64, 0);
				pPointList[0] = BPoint(1,3);
				pPointList[1] = BPoint(3,1);
				pPointList[2] = BPoint(12,1);
				pPointList[3] = BPoint(14,3);
				pPointList[4] = BPoint(14,12);
				pPointList[5] = BPoint(12,14);
				pPointList[6] = BPoint(3,14);
				pPointList[7] = BPoint(1,12);
				StrokePolygon(pPointList, 8, true,  B_SOLID_HIGH);	
		
				SetHighColor(255, 255, 255, 0);
				pPointList[0] = BPoint(2,12);
				pPointList[1] = BPoint(2,3);
				pPointList[2] = BPoint(3,2);
				pPointList[3] = BPoint(12,2);
				StrokePolygon(pPointList, 4, false,  B_SOLID_HIGH);	
		
				SetHighColor(192, 192, 192, 0);
				pPointList[0] = BPoint(3,3);
				pPointList[1] = BPoint(12,3);
				pPointList[2] = BPoint(12,12);
				pPointList[3] = BPoint(3,12);
				FillPolygon(pPointList, 4,  B_SOLID_HIGH);	
		
				SetHighColor(128, 128, 128, 0);
				pPointList[0] = BPoint(13,3);
				pPointList[1] = BPoint(13,12);
				pPointList[2] = BPoint(12,13);
				pPointList[3] = BPoint(3,13);
				StrokePolygon(pPointList, 4, false,  B_SOLID_HIGH);	
			}
	
			//Dessiner le triangle vers la droite.
			// La View est reduite.
			if(m_bShrink)
			{
			SetHighColor(0, 0, 0, 256);
				pPointList[0] = BPoint(9,7);
				pPointList[1] = BPoint(6,4);
				pPointList[2] = BPoint(6,11);
				pPointList[3] = BPoint(9,8);
				FillPolygon(pPointList, 4, B_SOLID_HIGH);
			}
			//Dessiner le triangle vers le bas.
			// La View est agrandi.
			else
			{
				SetHighColor(0, 0, 0, 256);
				pPointList[0] = BPoint(7,9);
				pPointList[1] = BPoint(4,6);
				pPointList[2] = BPoint(11,6);
				pPointList[3] = BPoint(8,9);
				FillPolygon(pPointList, 4, B_SOLID_HIGH);
			}
		}
	}
	
	//Redessiner le degrader du bas si necessaire
	if(!m_bShrink && updateRect.bottom > Bounds().Height() - 8)
	{
		if(m_pBmp)
		{
			DrawBitmap(m_pBmp, BRect(128,0,135,7), BRect(0,Bounds().Height()-7,7,Bounds().Height()));
			DrawBitmap(m_pBmp, BRect(128,8,135,15), BRect(Bounds().Width() - 7,Bounds().Height()-7,Bounds().Width(),Bounds().Height()));
			DrawBitmap(m_pBmp, BRect(140,0,140,7), BRect(8, Bounds().Height()-7, Bounds().Width() - 7, Bounds().Height())); 
		}
		else
		{
			DrawDegrader(BRect(0,Bounds().bottom - 8,Bounds().Width(),Bounds().bottom), 192, 128);
		}
	} 
}//End of Draw.


/*=============================================================================================*\
|	MouseDown																					|
+-----------------------------------------------------------------------------------------------+
|	Effet: Intercepter les click de sourie au dessus du boutton de la view.						|
|	Entre: 																						|
|		BPoint point: Location ou curseur.														|
\*=============================================================================================*/
void ShrinkView::MouseDown(BPoint point)
{	
	BView::MouseDown(point);
	if( point.x >= 0 && point.x < Bounds().Width() && point.y >= 0 && point.y < 16)
	{
		if(m_bShrink)
		{
			Expand();
		}	
		else
		{
			Shrink();
		}
		Draw(BRect(0,0,Bounds().Width(),15));
	}
}//End of MouseDown.


/*=============================================================================================*\
|	MouseMoved																					|
+-----------------------------------------------------------------------------------------------+
|	Effet: Intercepter les deplacement de la sourie pour connaitre a quelle moments le curseur 	|
|			se retrouve au dessus du boutton de la View.										|
|	Entre: 																						|
|		BPoint point: Location ou curseur.														|
|		uint32 transit: Code de transition.														|
|		const BMessage *message: Non utilise dans la function.									|
\*=============================================================================================*/
void ShrinkView::MouseMoved(BPoint point, uint32 transit, const BMessage *message)
{
	BView::MouseMoved(point, transit, message);
	
	//Verifie si le curseur est au dessus du boutton.
	if( point.x >= 0 && point.x < Bounds().Width() && point.y >= 0 && point.y < 16)
	{
		//Le curseur vien d'arriver au dessus du curseur.
		if(!m_bMouseOver)
		{
			m_bMouseOver = true;
			Draw(BRect(0,0,Bounds().Width(),15));
		}
	}
	else
	{
		//Le curseur vien de quitter le boutton.
		if(m_bMouseOver)
		{
			m_bMouseOver = false;
			Draw(BRect(0,0,Bounds().Width(),15));
		}	
	}
	
	//Le curseur est sortie de la view.
	if(transit == B_EXITED_VIEW)
	{
		m_bMouseOver = false;
		Draw(BRect(0,0,Bounds().Width(),15));
	}
}//End of MouseMoved


/*=============================================================================================*\
|	Shrink																						|
+-----------------------------------------------------------------------------------------------+
|	Effet: Reduire la view, la fenetre mere et deplacer les autre view si necessaire.			|
\*=============================================================================================*/
void ShrinkView::Shrink()
{
	BView * pTempView;	//Utiliser pour parcourir les View attacher a la meme fenetre mere.
	
	//Reduire la View.
	ResizeTo(Frame().Width(), 16.0);
	m_bShrink = true;

	//Redimentionner la fenetre mere.
	if( Window() != NULL )
	{
		Window()->ResizeBy(0, - (m_fFullHeight - 15.0));
	}
	
	//Deplacer, si necessaire, les autre View. 
	pTempView = this;
	while( pTempView->PreviousSibling() != NULL)
	{
		pTempView = pTempView->PreviousSibling();
	}
	while(pTempView != NULL)
	{
		if(Frame().top + 16.0 <= pTempView->Frame().top )
		{	
			pTempView->MoveBy(0, -(m_fFullHeight - 15.0));
		}
		pTempView = pTempView->NextSibling();
	}
	
	Draw(Bounds());
}//End of Shrink.

/*=============================================================================================*\
|	Expand																						|
+-----------------------------------------------------------------------------------------------+
|	Effet: Reduire la view, la fenetre mere et deplacer les autre view si necessaire.			|
\*=============================================================================================*/
void ShrinkView::Expand()
{
	BView * pTempView;	//Utiliser pour parcourir les View attacher a la meme fenetre mere.
	
	//Agradir la View.
	ResizeTo(Frame().Width(), m_fFullHeight);
	m_bShrink = false;

	//Redimentionner la fenetre mere
	if( Window() != NULL )
	{
		Window()->ResizeBy(0, m_fFullHeight - 15.0 );		
	}
	
	//Deplacer, si necessaire, les autre View. 
	pTempView = this;
	while( pTempView->PreviousSibling() != NULL)
	{
		pTempView = pTempView->PreviousSibling();
	}
	while(pTempView != NULL)
	{
		if(Frame().top + 16.0 <= pTempView->Frame().top )
		{	
			pTempView->MoveBy(0, m_fFullHeight - 15.0);
		}
		pTempView = pTempView->NextSibling();
	}
	
	Draw(Bounds());
}//End of Expand.


/*=============================================================================================*\
|	DrawDegrader																				|
+-----------------------------------------------------------------------------------------------+
|	Effet: Dessiner un degrader de gris dans la region demander.								|
|	Entre: 																						|
|		BRect rect: Rectagle ou doit etre dessine le degrader.									|
|		int iStartGray: Intensite du gris de depart (top).										|
|		int iEndGray: Intensite du gris de la fin (bottom).										|
\*=============================================================================================*/
void ShrinkView::DrawDegrader(BRect rect, int iStartGray, int iEndGray)
{
	int i, iGray;
	
	for(i = 0; i <= rect.Height(); i++)
	{
		iGray = iStartGray + ((iEndGray - iStartGray) / (int)rect.Height()) * i;
		SetHighColor(iGray, iGray, iGray ,0);
		StrokeLine(BPoint(rect.left, i + rect.top), BPoint(rect.right, i + rect.top));
	}
}

void ShrinkView::AttachedToWindow()
{
	uint32 * pSource;
	
	if(!m_pBmp) return;
				
	pSource = (uint32*)m_pBmp->Bits();
		
	if(m_pBmp->ColorSpace() == B_RGB32)
	{
		for(int i = 0; i < m_pBmp->BitsLength()/4; i++)
		{
			if(pSource[i] == B_TRANSPARENT_MAGIC_RGBA32)
			{
				pSource[i] = ViewColor().red * 0x10000 + ViewColor().green * 0x100 + ViewColor().blue;
			}
		}
	}
	if(m_pBmp->ColorSpace() == B_RGB32_BIG)
	{
		for(int i = 0; i < m_pBmp->BitsLength()/4; i++)
		{
			if(pSource[i] == B_TRANSPARENT_MAGIC_RGBA32_BIG)
			{
				pSource[i] = ViewColor().red * 0x10000 + ViewColor().green * 0x100 + ViewColor().blue;
			}
		}
	}
}

