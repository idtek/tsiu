#include "VMGlobalDef.h"

Engine*	g_poEngine = NULL;

//--------------------------------------------------------------------------------------
class MyCanvas : public FXCanvas {
	FXDECLARE(MyCanvas)

protected:
	MyCanvas();

public:
	MyCanvas(FXComposite* p,
		FXObject* tgt = NULL,
		FXSelector sel = 0,
		FXuint opts = 0,
		FXint x = 0,
		FXint y = 0,
		FXint w = 0,
		FXint h = 0);
public:
	long onDoNothing(FXObject* sender, FXSelector sel, void* ptr);

public:
	enum {
		ID_RESEARVED = FXMainWindow::ID_LAST,
	};
};

FXDEFMAP(MyCanvas) MyCanvasMap[]={
	FXMAPFUNC(SEL_COMMAND,		MyCanvas::ID_RESEARVED,					MyCanvas::onDoNothing)
};
FXIMPLEMENT(MyCanvas,FXCanvas,MyCanvasMap,ARRAYNUMBER(MyCanvasMap))
//--------------------------------------------------------------------------------------------
MyCanvas::MyCanvas()
{
	flags |= FLAG_ENABLED;
}

MyCanvas::MyCanvas(FX::FXComposite *p, 
				   FX::FXObject *tgt, 
				   FX::FXSelector sel, 
				   FX::FXuint opts, 
				   FX::FXint x, 
				   FX::FXint y, 
				   FX::FXint w, 
				   FX::FXint h)
				   :FXCanvas(p,tgt,sel,opts,x,y,w,h)
{
	flags |= FLAG_ENABLED;

	FXMatrix* poMatrixMain = new FXMatrix(p, 2, MATRIX_BY_ROWS|LAYOUT_FILL_X|LAYOUT_FILL_Y);
}

long MyCanvas::onDoNothing(FXObject* sender, FXSelector sel, void* ptr)
{
	return 1;
}

//---------------------------------------------------------------------------------------------
void MyEngine::DoInit()
{
	FXApp *poApp = g_poEngine->GetRenderMod()->GetGUIApp();
	FXMainWindow *poMain = g_poEngine->GetRenderMod()->GetGUIMainWindow();
	new MyCanvas(poMain);
}

void MyEngine::DoUnInit()
{
}