/*
 * Copyright 2007 Matthias Lindner, aka Paradoxon
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include "SplitterView.h"
#include <Message.h>
#include <stdio.h>

SplitterView::SplitterView(BRect frame,orientation dr,uint32 resizingMode,uint32 flags): BView(frame,"ViewSplitter", resizingMode,flags)
{
  direction=dr;
  anz=0;
  SetViewColor(216,216,216,0);
}

void 
SplitterView::AddChild(BView *view)
{
    uint32 i;
    anz++;
    if (direction==B_HORIZONTAL){
        stepWidth=Bounds().Height()/(anz);
        view->ResizeTo(Bounds().Width(),stepWidth-10);
        if (anz>1)
            ChildAt(CountChildren()-1)->ResizeTo(Bounds().Width(),stepWidth-10);
    }
    else {
        stepWidth=Bounds().Width()/(anz);
        view->ResizeTo(stepWidth-10,Bounds().Height());
        if (anz>1)
            ChildAt(CountChildren()-1)->ResizeTo(stepWidth-10,Bounds().Height());
    }

    if (anz==1) {
        view->MoveTo(0,0);
        //Korrektur weil wir ja bei dem ersten keinen Divider haben...
        if (direction==B_HORIZONTAL)
            view->ResizeBy(0,10);
        else
            view->ResizeBy(10,0);

        BView::AddChild(view);
    }
    else {
        /* alle vorhergehenden kleiner machen... 
        Die Divider haben immer einen Index der ab 1 anfängt und dann
        in Zweierschritten hochgeht (es liegt ja immer ein BView zwischen den
        Dividern */
        for(i=0;i<(anz-2);i++)
            ((Divider *)ChildAt((i*2)+1))->SetLocation((i+1)*stepWidth);

        BView::AddChild(new Divider(ChildAt(CountChildren()-1),view,direction));
        ((Divider *)ChildAt(CountChildren()-1))->SetLocation((anz-1)*stepWidth);
        BView::AddChild(view);
    }
}


bool 
SplitterView::ForceRemove(int32 i)
{
    bool ok = false;
    float d;

    //wir berechnen nun mal wieviel jeder View dazubekommt  
    if (direction==B_HORIZONTAL)
        d=ChildAt(i*2)->Bounds().Height()/anz;
    else
        d=ChildAt(i*2)->Bounds().Width()/anz;

    //entfernen alles nötige: View und Divider
    ok = BView::RemoveChild(ChildAt(i*2)); //test obs geklappt hat

    //da der Divider jetz eine Stelle vorgerückt ist ist er auf dem selben Index... Fehlersuche ca 1/2 Tag!!!!!
    if (ok)
        ok=BView::RemoveChild(ChildAt(i*2));

    //wenns ordentlich entfernt wurde dann teilen wir jetzt neu zu..

    if (ok){
        //wenn alle geklappt hat, dann haben wir ersteinmal ein View weniger
        anz--;

        for (uint q=0;q<anz;q++){
            //View holen und entsprechend der akt Ausrichtung den freigeworden Platz aufteilen
            if (direction==B_HORIZONTAL)
                ChildAt(q*2)->ResizeBy(0,d);
            else
                ChildAt(q*2)->ResizeBy(d,0);
        }

        /* So jetzt müssen wir aber noch die Divider updaten.. das machen wir über einen Trick
        SetDirection macht das für uns.. ;-)*/
        for (uint32 i=0;i<(anz-1);i++) 
            ((Divider *)ChildAt((i*2)+1))->SetDirection(direction);
    }
    return ok;
}

bool 
SplitterView::RemoveChild(BView *view)
{
    bool ok=false,found=false;
    uint i=0;
    //den Index des zu entferndenden Views suchen..

    while ((!found)&&(i<anz))
    {
        found=(ChildAt(i*2)==view);
        i++;
    }

    // wenn es gefunden wurde
    if (found){
        //Korrektur da wir ja immer am Ende +1 gerechtet haben..
        i--;
        //so dann testen wir ob die zu entfernenden View das erste oder das letzte View ist wenn nicht dann
        if ((i>1)&&(i<(anz-1))){
            ((Divider *)ChildAt((i*2)-1))->SetSecondView(ChildAt((i*2)+2));
            ok=ForceRemove(i);
        }
        else{
            //wir brauchen nur ne Korrektur falls mehr als ein BView noch existiert
            if (anz>1){

                if (i==0){
                    ChildAt((i*2)+2)->MoveTo(0,0);
                    ok=ForceRemove(i);
                }
                else{
                    /*kleiner Trick... wenn wir das letze löschen muss ja der Divider davor auch gelöscht werden
                    unser ForceRemove löscht aber immer i und das View i+1 also müssen wir nur View und Divider 
                    vertauschen und dann "normal" mit ForceRemove(index) löschen. Erspart viel Ärger und
                    Programmieraufwand*/
                    BView *tmp1=ChildAt((i*2)-2);
                    BView *tmp2=ChildAt((i*2)-1);
                    BView::RemoveChild(tmp1);
                    BView::RemoveChild(tmp2);
                    BView::AddChild(tmp2);
                    BView::AddChild(tmp1);
                    ok=ForceRemove(i-1);
                }
            }
            else
                // wenn wir nur noch ein View haben brauchen wir nur das entfernen
                BView::RemoveChild(view);
            }
    }
    return ok;
}


BView*
SplitterView::GetView(uint32 indx)
{
    if (indx<anz) {
        return ChildAt(indx*2);
    }
    else 
        return NULL;
}


orientation	SplitterView::GetDirection()
{
    printf("B_HORIZONTAL ist %d\n",B_HORIZONTAL);
    printf("B_VERTICAL ist %d\n",B_VERTICAL);
    printf("Aktuelle Richtung ist: ist %d\n",direction);

    return direction;
}


void 
SplitterView::SetDivPos(uint32 indx,float location)
{
    if (indx<(anz-1)){
        ((Divider *)ChildAt((indx*2)+1))->SetLocation(location);
    }
}


void
SplitterView::SetDirection(orientation dr)
{
    uint i;
    BView* tmpView;
    float px,py;
    //alle Views "umarbeiten" entsprechend ihrer Größe und Position
    if (direction!=dr){
        direction=dr;
        if (direction==B_HORIZONTAL)
            for (i=0;i<anz;i++){
                tmpView=ChildAt(i*2);
                //relative x Position bestimmen
                px=tmpView->Frame().left/Bounds().Width();
                //umrechen in eine entsprechende relative y Position umrechen
                py=Bounds().Height()*px;
                //und unser View dahin bewegen
                tmpView->MoveTo(0,py);
                //relative Größe ermitteln
                px=tmpView->Frame().Width()/Bounds().Width();
                // umrechnen und setzen
                py=Bounds().Height()*px;
                tmpView->ResizeTo(Bounds().Width(),py);
            }
        else
            //Erklärung siehe oben
            for (i=0;i<anz;i++){
                tmpView=ChildAt(i*2);
                py=tmpView->Frame().top/Bounds().Height();
                px=Bounds().Width()*py;
                tmpView->MoveTo(px,0);
                py=tmpView->Frame().Height()/Bounds().Height();
                px=Bounds().Width()*py;
                tmpView->ResizeTo(px,Bounds().Height());
            }
        //so jetzt noch allen Dividern sagen, das sich die Richtung geändert hat!
        for (i=0;i<(anz-1);i++){ 
            ((Divider *)ChildAt((i*2)+1))->SetDirection(direction);
        }
    }
}
