#include "SDGlobalDef.h"
#include "SDObject.h"
#include "SDNetWork.h"
#include "SDMempool.h"
#include "SDWindowMsgCallBack.h"
#include "fxkeys.h"
#include "SDFormationEditor.h"
#include "tinyxml.h"
//#include "SDGuiCallBack.h"

class MyCanvas;

Engine*						g_poEngine = NULL;
SimpleRenderObjectUtility*	g_poSROU = NULL;
MemPool<UDP_PACK>*			g_poMem = NULL;
HANDLE						g_hRecv;
Bool						g_bStop = false;
ODynamicObj*				g_WatchPlayer = NULL;
MyCanvas*				    g_Canvas = NULL;
u32							g_WindowWidth = kWINDOW_WIDTH;
u32							g_WindowHeight = kWINDOW_HEIGHT;
Bool						g_bIsShowDetailed = false;
Bool						g_bIsWatchOwner = false;
Bool						g_bIsSlowMotion = false;
Bool						g_bIsAlwaysOnTop = false;
#define kMAX_FILENAME_LENGTH	256
Char						g_zRootDir[MAX_PATH] = ".";
Char						g_zLastXMLFile[MAX_PATH] = ".\\formationdata.xml";
Char						g_zLastLUAFile[MAX_PATH] = ".\\formationdata.lua";
Bool						g_bFirstEntry = true;


const u32 kg_NumberOfTab = E_Tab_Num;
const Char* kg_TabDesc[kg_NumberOfTab] = {
	" Entity ", " Attributes ", " Statistics ", " Watch "
};
enum{
	EValueType_Smooth,
	EValueType_3,
	EValueType_4,
	EValueType_Boolean,
};
struct NameType{
	FXString name;
	FXint	 type;
};
//static const int kNumOfAIParameter = 12;
enum{
	IDX_FormationDensity,
	IDX_SideAttack,		
	IDX_DefensiveLine,	
	IDX_Width,			
	IDX_Mentality,		
	IDX_Tempo,			
	IDX_TimeWasting,	
	IDX_FocusPassing,	
	IDX_ClosingDown,	
	IDX_TargetMan,		
	IDX_PlayMaker,		
	IDX_CounterAttack,	

	kNumOfAIParameter
};
static const NameType kNameTypeOfAIParameter[kNumOfAIParameter] = {
	{"FormationDensity",	EValueType_Smooth},
	{"SideAttack",			EValueType_4},
	{"DefensiveLine",		EValueType_3},
	{"Width",				EValueType_Smooth},
	{"Mentality",			EValueType_Smooth},
	{"Tempo",				EValueType_Smooth},
	{"TimeWasting",			EValueType_Smooth},
	{"FocusPassing",		EValueType_4},
	{"ClosingDown",			EValueType_Smooth},
	{"TargetMan",			EValueType_Boolean},
	{"PlayMaker",			EValueType_Boolean},
	{"CounterAttack",		EValueType_Boolean}
};

#define HOME_TEAM_ID	-1
#define AWAY_TEAM_ID	-2
#define HOME_TEAM_START	2
#define HOME_TEAM_NUM	4
#define AWAY_TEAM_START	6
#define AWAY_TEAM_NUM	4

class PlayerTuningControl;

class MyCanvas : public FXCanvas {
	FXDECLARE(MyCanvas)

protected:
	MyCanvas();

public:
	//Construct DX viewer widget
	MyCanvas(FXComposite* p,
		FXObject* tgt = NULL,
		FXSelector sel = 0,
		FXuint opts = 0,
		FXint x = 0,
		FXint y = 0,
		FXint w = 0,
		FXint h = 0);
public:
	enum {
		ID_SENDCOMMAND = FXMainWindow::ID_LAST,
		ID_SELECTTAB,
		ID_CHANGECANVAS,
		ID_SAVEFORMATIONDATA,
		ID_LOADFORMATIONDATA,
		ID_EXPORTFORMATIONDATA,
		ID_STARTSTOPSIMULATION,
		ID_SELECTAWAYVIEW,
		ID_SETHOMETEAMSTATE,
		ID_OPENOUTPUTDIR,
		ID_STARTSTOPREALGAME,
	};
private:
	void onSIM_SelectPlayer(const Event* _poEvent);
	void onSIM_SelectPlayerInRealGame(const Event* _poEvent);
	void onAIParamUpdate(const Event* _poEvent);

	void PlayerIDToPlayerAIParamControlID(int id, bool hasGK)
	{
		
	}

public:
	long onCmdSendCommand(FXObject* sender, FXSelector sel, void* ptr);
	long onKeyPress(FXObject* sender, FXSelector sel, void* ptr);
	long onTabSelected(FXObject* sender, FXSelector sel, void* ptr);
	long onChangeCanvas(FXObject* sender, FXSelector sel, void* ptr);
	long onExportFormationData(FXObject* sender, FXSelector sel, void* ptr);
	long onSaveFormationData(FXObject* sender, FXSelector sel, void* ptr);
	long onLoadFormationData(FXObject* sender, FXSelector sel, void* ptr);
	long onSimulationControl(FXObject* sender, FXSelector sel, void* ptr);
	long onSelectAwayView(FXObject* sender, FXSelector sel, void* ptr);
	long onSetHomeTeamState(FXObject* sender, FXSelector sel, void* ptr);
	long onOpenOutputDir(FXObject* sender, FXSelector sel, void* ptr);
	long onRealGameControl(FXObject* sender, FXSelector sel, void* ptr);

	inline FXTable* GetTable(ETabIndex tab) const { return m_TabTable[tab];	}

public:
	FXTextField*			m_Command;
	Array<FXString>			m_CommandHistory;
	s32						m_CurrentCommand;
	FXTable*				m_TabTable[kg_NumberOfTab];

	FXListBox*				m_PitchListBox;
	FXListBox*				m_TeamStateListBox;
	FXListBox*				m_PositionListBox;
	FXButton*				m_SimulatingBtn;
	FXButton*				m_RealGameBtn;

	FXTabBook*				m_PlayerTuningControlTabBook;
	PlayerTuningControl*	m_PlayerTuningControl[10];

	FXTextField*			m_OutputDir;

	FXListBox*				m_SimPitchType;
	FXListBox*				m_SimTeamState;
	FXListBox*				m_SimHomeTeamList[4];
	FXListBox*				m_SimAwayTeamList[4];

	TsiU::AI::RefValue<PlayerIndividualAIParam, TsiU::AI::ERefValuFlag_ReadOnly> m_PlayerIndividualAIParamToEditor;
	TsiU::AI::RefValue<RealGameInfo, TsiU::AI::ERefValuFlag_ReadOnly> m_RealGameInfo;
	TsiU::AI::RefValue<PlayerIndividualAIParam, TsiU::AI::ERefValuFlag_Writable> m_PlayerIndividualAIParamFromEditor;
};

class PlayerTuningControl : public FXCanvas
{
	FXDECLARE(PlayerTuningControl)

protected:
	PlayerTuningControl(){}

public:
	//Construct DX viewer widget
	PlayerTuningControl(
		int id,
		FXComposite* p,
		FXObject* tgt = NULL,
		FXSelector sel = 0,
		FXuint opts = 0,
		FXint x = 0,
		FXint y = 0,
		FXint w = 0,
		FXint h = 0);

	~PlayerTuningControl();

public:
	enum{
		ID_LAST,
		ID_RESERVED
	};

public:
	struct AtomControlPair : public FXObject
	{
		FXDECLARE(AtomControlPair)

	public:
		enum{
			ID_LAST,
			ID_UPDATERFVALUE,
		};
		AtomControlPair(){}

		AtomControlPair(FXMatrix* matrix, const char* name, int type, int id, int idx)
			: m_Name(name)
			, m_ValueType(type)
			, m_SliderValue(0)
			, m_Slider(NULL)
			, m_TextField(NULL)
			, m_id(id)
			, m_idx(idx)
		{
			new FXLabel(matrix, m_Name.text(), NULL, JUSTIFY_LEFT|LAYOUT_FILL_X|LAYOUT_CENTER_Y);
			m_SliderDataTarget.connect(m_SliderValue, this, AtomControlPair::ID_UPDATERFVALUE);
			m_TextField = new FXTextField(matrix, 1, NULL, 0, FRAME_SUNKEN|FRAME_THICK|LAYOUT_CENTER_Y|LAYOUT_FILL_ROW|LAYOUT_FIX_WIDTH, 0, 0, 40);
			m_Slider = new FXSlider(matrix, &m_SliderDataTarget, FXDataTarget::ID_VALUE, LAYOUT_CENTER_Y|LAYOUT_FILL_ROW|LAYOUT_FIX_WIDTH, 0, 0, 100);
			switch(type)
			{
			case EValueType_Smooth:
				m_Slider->setRange(0, 100);
				m_Slider->setIncrement(1);
				break;
			case EValueType_3:
				m_Slider->setRange(0, 2);
				m_Slider->setIncrement(1);
				break;
			case EValueType_4:
				m_Slider->setRange(0, 3);
				m_Slider->setIncrement(1);
				break;
			case EValueType_Boolean:
				m_Slider->setRange(0, 1);
				m_Slider->setIncrement(1);
				break;
			}
			m_Slider->setValue(m_SliderValue);
			m_TextField->disable();
			//update text field;
			onUpdateRfValue(NULL, 0, NULL);
		}
		~AtomControlPair()
		{
		}
		long onUpdateRfValue(FXObject* sender, FXSelector sel, void* ptr)
		{
			if(m_id == HOME_TEAM_ID || m_id == AWAY_TEAM_ID)
			{
				if(m_id == HOME_TEAM_ID)
				{
					for(int i = HOME_TEAM_START; i < HOME_TEAM_START + HOME_TEAM_NUM; ++i)
					{
						if(g_Canvas && g_Canvas->m_PlayerTuningControl[i])
							g_Canvas->m_PlayerTuningControl[i]->CopyFrom(this, m_idx);
					}
				}
				else
				{
					for(int i = AWAY_TEAM_START; i < AWAY_TEAM_START + AWAY_TEAM_NUM; ++i)
					{
						if(g_Canvas && g_Canvas->m_PlayerTuningControl[i])
							g_Canvas->m_PlayerTuningControl[i]->CopyFrom(this, m_idx);
					}
				}
			}
			{
				FXString str;
				switch(m_ValueType)
				{
				case EValueType_Boolean:
					str.format("%s", m_SliderValue ? "True" : "False");
					break;
				case EValueType_3:
					{
						static const char* kDefensiveLine[] = {
							"Front", "Middle", "Back"
						};
						if(m_Name == "DefensiveLine")
						{
							str.format("%s", kDefensiveLine[m_SliderValue]);
						}
						else
						{
							D_CHECK(0);
						}
						break;
					}
				case EValueType_4:
					{
						static const char* kSideAttack[] = {
							"Any", "Left", "Middle", "Right"
						};
						static const char* kFocusPassing[] = {
							"Any", "Left", "Middle", "Right"
						};
						if(m_Name == "SideAttack")
						{
							str.format("%s", kSideAttack[m_SliderValue]);
						}
						else if(m_Name == "FocusPassing")
						{
							str.format("%s", kFocusPassing[m_SliderValue]);
						}
						else
						{
							D_CHECK(0);
						}
						break;
					}
				default:
					str.format("%d", m_SliderValue);
					break;
				}
				m_TextField->setText(str);

				Event evt((EventType_t)E_ET_AIParamUpdate);
				evt.AddParam(m_id);
				evt.AddParam(m_idx);
				evt.AddParam(m_SliderValue);
				g_poEngine->GetEventMod()->SendEvent(&evt);
			}
			return 1;
		}
		FXint			m_idx;
		FXint			m_id;
		FXint			m_ValueType;
		FXString		m_Name;
		FXTextField*	m_TextField;
		FXint			m_SliderValue;
		FXSlider*		m_Slider;
		FXDataTarget	m_SliderDataTarget;
	};

public:
	void InitFromData(const PlayerOtherAttributes& attr)
	{
		m_SubController[IDX_FormationDensity]->m_Slider->setValue(Math::Clamp((int)(attr.m_FormationDensity * 100), 0, 100), true);
		m_SubController[IDX_SideAttack		]->m_Slider->setValue(attr.m_SideAttack, true);
		m_SubController[IDX_DefensiveLine	]->m_Slider->setValue(attr.m_DefensiveLine, true);
		m_SubController[IDX_Width			]->m_Slider->setValue(Math::Clamp((int)(attr.m_Width * 100), 0, 100), true);
		m_SubController[IDX_Mentality		]->m_Slider->setValue(Math::Clamp((int)(attr.m_Mentality * 100), 0, 100), true);
		m_SubController[IDX_Tempo			]->m_Slider->setValue(Math::Clamp((int)(attr.m_Tempo * 100), 0, 100), true);
		m_SubController[IDX_TimeWasting		]->m_Slider->setValue(Math::Clamp((int)(attr.m_TimeWasting * 100), 0, 100), true);
		m_SubController[IDX_FocusPassing	]->m_Slider->setValue(attr.m_FocusPassing, true);
		m_SubController[IDX_ClosingDown		]->m_Slider->setValue(Math::Clamp((int)(attr.m_ClosingDown * 100), 0, 100), true);
		m_SubController[IDX_TargetMan		]->m_Slider->setValue(attr.m_TargetMan ? 1 : 0, true);
		m_SubController[IDX_PlayMaker		]->m_Slider->setValue(attr.m_Playmaker ? 1 : 0, true);
		m_SubController[IDX_CounterAttack	]->m_Slider->setValue(attr.m_CounterAttack ? 1 : 0, true);
	}
	void ToData(PlayerOtherAttributes& attr)
	{
		attr.m_FormationDensity =  Math::Clamp((float)m_SubController[IDX_FormationDensity]->m_Slider->getValue()/100.f, 0.f, 1.f);
		attr.m_SideAttack		=  m_SubController[IDX_SideAttack]->m_Slider->getValue();
		attr.m_DefensiveLine	=  m_SubController[IDX_DefensiveLine]->m_Slider->getValue();
		attr.m_Width			=  Math::Clamp((float)m_SubController[IDX_Width]->m_Slider->getValue()/100.f, 0.f, 1.f);
		attr.m_Mentality		=  Math::Clamp((float)m_SubController[IDX_Mentality]->m_Slider->getValue()/100.f, 0.f, 1.f);
		attr.m_Tempo			=  Math::Clamp((float)m_SubController[IDX_Tempo]->m_Slider->getValue()/100.f, 0.f, 1.f);
		attr.m_TimeWasting		=  Math::Clamp((float)m_SubController[IDX_TimeWasting]->m_Slider->getValue()/100.f, 0.f, 1.f);
		attr.m_FocusPassing		=  m_SubController[IDX_FocusPassing]->m_Slider->getValue();
		attr.m_ClosingDown		=  Math::Clamp((float)m_SubController[IDX_ClosingDown]->m_Slider->getValue()/100.f, 0.f, 1.f);
		attr.m_TargetMan		=  m_SubController[IDX_TargetMan]->m_Slider->getValue() ? true : false;
		attr.m_Playmaker		=  m_SubController[IDX_PlayMaker]->m_Slider->getValue() ? true : false;
		attr.m_CounterAttack	=  m_SubController[IDX_CounterAttack]->m_Slider->getValue() ? true : false;
	}
	void CopyFrom(const AtomControlPair* from, int whichSub)
	{
		D_CHECK(whichSub >= 0 && whichSub < kNumOfAIParameter);
		m_SubController[whichSub]->m_Slider->setValue(from->m_Slider->getValue(), true);
	}
	long onReserved(FXObject* sender, FXSelector sel, void* ptr){ return 1;	}

	int					m_id;
	AtomControlPair*	m_SubController[kNumOfAIParameter];
};

FXDEFMAP(PlayerTuningControl::AtomControlPair) AtomControlPairMap[]={
	FXMAPFUNC(SEL_COMMAND,		PlayerTuningControl::AtomControlPair::ID_UPDATERFVALUE,  PlayerTuningControl::AtomControlPair::onUpdateRfValue),
};
FXIMPLEMENT(PlayerTuningControl::AtomControlPair, FXObject, AtomControlPairMap, ARRAYNUMBER(AtomControlPairMap))

FXDEFMAP(PlayerTuningControl) PlayerTuningControlMap[]={
	FXMAPFUNC(SEL_COMMAND,		PlayerTuningControl::ID_RESERVED,           PlayerTuningControl::onReserved),
};
FXIMPLEMENT(PlayerTuningControl, FXCanvas, PlayerTuningControlMap, ARRAYNUMBER(PlayerTuningControlMap))

PlayerTuningControl::PlayerTuningControl(
				   int id,
				   FX::FXComposite *p, 
				   FX::FXObject *tgt, 
				   FX::FXSelector sel, 
				   FX::FXuint opts, 
				   FX::FXint x, 
				   FX::FXint y, 
				   FX::FXint w, 
				   FX::FXint h)
				   :FXCanvas(p,tgt,sel,opts,x,y,w,h)
				   ,m_id(id)
{
	flags |= FLAG_ENABLED;

	FXMatrix* matrix = new FXMatrix(p, 12, MATRIX_BY_COLUMNS|LAYOUT_FILL_X);
	for(int i = 0; i < kNumOfAIParameter; ++i)
	{
		m_SubController[i] = new AtomControlPair(matrix, kNameTypeOfAIParameter[i].name.text(), kNameTypeOfAIParameter[i].type, id, i);
	}
}
PlayerTuningControl::~PlayerTuningControl()
{
	for(int i = 0; i < kNumOfAIParameter; ++i)
	{
		D_SafeDelete(m_SubController[i]);
	}
}
FXDEFMAP(MyCanvas) MyCanvasMap[]={
	FXMAPFUNC(SEL_COMMAND,		MyCanvas::ID_SENDCOMMAND,           MyCanvas::onCmdSendCommand),
	FXMAPFUNC(SEL_COMMAND,		MyCanvas::ID_SAVEFORMATIONDATA,     MyCanvas::onSaveFormationData),
	FXMAPFUNC(SEL_COMMAND,		MyCanvas::ID_LOADFORMATIONDATA,     MyCanvas::onLoadFormationData),
	FXMAPFUNC(SEL_COMMAND,		MyCanvas::ID_EXPORTFORMATIONDATA,   MyCanvas::onExportFormationData),
	FXMAPFUNC(SEL_KEYPRESS,		MyCanvas::ID_SENDCOMMAND,			MyCanvas::onKeyPress),
	FXMAPFUNC(SEL_COMMAND,		MyCanvas::ID_SELECTTAB,				MyCanvas::onTabSelected),
	FXMAPFUNC(SEL_COMMAND,		MyCanvas::ID_CHANGECANVAS,			MyCanvas::onChangeCanvas),
	FXMAPFUNC(SEL_COMMAND,		MyCanvas::ID_STARTSTOPSIMULATION,	MyCanvas::onSimulationControl),
	FXMAPFUNC(SEL_COMMAND,		MyCanvas::ID_SELECTAWAYVIEW,		MyCanvas::onSelectAwayView),
	FXMAPFUNC(SEL_COMMAND,		MyCanvas::ID_SETHOMETEAMSTATE,		MyCanvas::onSetHomeTeamState),
	FXMAPFUNC(SEL_COMMAND,		MyCanvas::ID_OPENOUTPUTDIR,			MyCanvas::onOpenOutputDir),
	FXMAPFUNC(SEL_COMMAND,		MyCanvas::ID_STARTSTOPREALGAME,		MyCanvas::onRealGameControl),
};
// ButtonApp implementation
FXIMPLEMENT(MyCanvas, FXCanvas, MyCanvasMap, ARRAYNUMBER(MyCanvasMap))

MyCanvas::MyCanvas()
	: m_PlayerIndividualAIParamToEditor("IndividualAIParamToEditor", PlayerIndividualAIParam())
	, m_PlayerIndividualAIParamFromEditor("IndividualAIParamFromEditor", PlayerIndividualAIParam())
	, m_RealGameInfo("RealGameInfo", RealGameInfo())
{
	flags |= FLAG_ENABLED;
	m_CurrentCommand = -1;
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
						 , m_PlayerIndividualAIParamToEditor("IndividualAIParamToEditor", PlayerIndividualAIParam())
						 , m_PlayerIndividualAIParamFromEditor("IndividualAIParamFromEditor", PlayerIndividualAIParam())
						 , m_RealGameInfo("RealGameInfo", RealGameInfo())
{
	flags |= FLAG_ENABLED;
	m_CurrentCommand = -1;

	FXSplitter *poSplitterV		= new FXSplitter(p,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|LAYOUT_FILL_Y|SPLITTER_VERTICAL);
	FXVerticalFrame *poGroupV1	= new FXVerticalFrame(poSplitterV,FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y);
	FXVerticalFrame *poGroupV2	= new FXVerticalFrame(poSplitterV,FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y);

	//V1
	FXMatrix* matrix = new FXMatrix(poGroupV1, 3, MATRIX_BY_COLUMNS|LAYOUT_FILL_X);
	new FXLabel(matrix, "Command: ",NULL, JUSTIFY_LEFT|LAYOUT_FILL_X|LAYOUT_CENTER_Y);
	m_Command = new FXTextField(matrix, 1, this, ID_SENDCOMMAND, FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_CENTER_Y|LAYOUT_FILL_COLUMN);
	new FXButton(matrix, "Send", NULL, this, ID_SENDCOMMAND, FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_CENTER_Y|LAYOUT_FIX_WIDTH, 0, 0, 100);

	//V2
	FXSplitter *poSplitterH			= new FXSplitter(poGroupV2,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|LAYOUT_FILL_Y|SPLITTER_HORIZONTAL);
	FXHorizontalFrame *poGroupH1	= new FXHorizontalFrame(poSplitterH,FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0, 0, 900);
	FXHorizontalFrame *poGroupH2	= new FXHorizontalFrame(poSplitterH,FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y);

	//H1
	FXEGDIViewer    *poDXViewer	= new FXEGDIViewer(poGroupH1, NULL, 0, FRAME_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_Y);
	poDXViewer->RegisterListener(new SDWindowMsgCallBack);
	poDXViewer->RegisterListener(new FEWindowMsgCallBack);

	//H2
	FXTabBook* poTabBook = new FXTabBook(poGroupH2, this, ID_SELECTTAB, PACK_UNIFORM_WIDTH|PACK_UNIFORM_HEIGHT|LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_RIGHT);
	poTabBook->setTabStyle(TABBOOK_BOTTOMTABS);
	FXuint packing_hints = poTabBook->getPackingHints();
	packing_hints |= PACK_UNIFORM_WIDTH;
	poTabBook->setPackingHints(packing_hints);

	for(int i = 0; i < kg_NumberOfTab; ++i)
	{
		FXTabItem* poTab = new FXTabItem(poTabBook, kg_TabDesc[i] ,NULL);
		poTab->setTabOrientation(TAB_BOTTOM);
		FXHorizontalFrame* poTableFrame = new FXHorizontalFrame(poTabBook, FRAME_THICK|FRAME_RAISED);
		FXHorizontalFrame* poBoxframe = new FXHorizontalFrame(poTableFrame,FRAME_THICK|FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);
		m_TabTable[i] = new FXTable(poBoxframe,this, NULL, TABLE_COL_SIZABLE|TABLE_ROW_SIZABLE|LAYOUT_FILL_X|LAYOUT_FILL_Y|TABLE_READONLY|TABLE_NO_COLSELECT|TABLE_NO_ROWSELECT,0,0,0,0, 2,2,2,2);
		m_TabTable[i]->setBackColor(FXRGB(255,255,255));
		m_TabTable[i]->setVisibleRows(20);
		m_TabTable[i]->setVisibleColumns(2);
		m_TabTable[i]->setTableSize(0, 2);
		m_TabTable[i]->setCellColor(0,0,FXRGB(255,240,240));
		m_TabTable[i]->setCellColor(1,0,FXRGB(240,255,240));
		m_TabTable[i]->setCellColor(0,1,FXRGB(255,240,240));
		m_TabTable[i]->setCellColor(1,1,FXRGB(240,255,240));
		m_TabTable[i]->setColumnWidth(0, 155);
		m_TabTable[i]->setColumnWidth(1, 155);
		m_TabTable[i]->setRowHeaderWidth(0);
		m_TabTable[i]->setColumnText(0, "Name");
		m_TabTable[i]->setColumnText(1, "Value");
		m_TabTable[i]->setSelBackColor(FXRGB(128,128,128));
	}
	FXTabItem* poTab = new FXTabItem(poTabBook, " Formation " ,NULL);
	poTab->setTabOrientation(TAB_BOTTOM);
	FXHorizontalFrame* poTableFrame = new FXHorizontalFrame(poTabBook, FRAME_THICK|FRAME_RAISED);
	FXHorizontalFrame* poBoxframe = new FXHorizontalFrame(poTableFrame,FRAME_THICK|FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);
	FXMatrix* mainnmatrix = new FXMatrix(poBoxframe, 5, MATRIX_BY_ROWS|LAYOUT_FILL_X);

	FXMatrix* dirmatrix = new FXMatrix(mainnmatrix, 3, MATRIX_BY_COLUMNS|LAYOUT_FILL_X);
	new FXLabel(dirmatrix, "Game Dir: ",NULL, JUSTIFY_LEFT|LAYOUT_FILL_X|LAYOUT_CENTER_Y);
	m_OutputDir = new FXTextField(dirmatrix, 1, NULL, 0, TEXTFIELD_READONLY|FRAME_SUNKEN|FRAME_THICK|LAYOUT_CENTER_Y|LAYOUT_FILL_ROW|LAYOUT_FIX_WIDTH, 0, 0, 180);
	//m_OutputDir->disable();
	new FXButton(dirmatrix, "Choose...", NULL, this, ID_OPENOUTPUTDIR, FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_CENTER_Y|LAYOUT_FIX_WIDTH, 0, 0, 60);

	FXMatrix* formationmatrix = new FXMatrix(mainnmatrix, 3, MATRIX_BY_COLUMNS|LAYOUT_FILL_X);
	new FXButton(formationmatrix, "Save...", NULL, this, ID_SAVEFORMATIONDATA, FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_CENTER_Y|LAYOUT_FIX_WIDTH, 0, 0, 100);
	new FXButton(formationmatrix, "Load...", NULL, this, ID_LOADFORMATIONDATA, FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_CENTER_Y|LAYOUT_FIX_WIDTH, 0, 0, 100);
	new FXButton(formationmatrix, "Export to lua...", NULL, this, ID_EXPORTFORMATIONDATA, FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_CENTER_Y|LAYOUT_FIX_WIDTH, 0, 0, 100);

	FXMatrix* filtermatrix = new FXMatrix(mainnmatrix, 3, MATRIX_BY_COLUMNS|LAYOUT_FILL_X);
	m_PitchListBox = new FXListBox(filtermatrix, this, ID_CHANGECANVAS, FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_CENTER_Y|LAYOUT_FIX_WIDTH, 0, 0, 100);
	m_PitchListBox->appendItem("Normal");
	m_PitchListBox->appendItem("Large");
	m_TeamStateListBox = new FXListBox(filtermatrix, this, ID_CHANGECANVAS, FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_CENTER_Y|LAYOUT_FIX_WIDTH, 0, 0, 100);
	m_TeamStateListBox->appendItem("Attack");
	m_TeamStateListBox->appendItem("Defend");
	m_PositionListBox = new FXListBox(filtermatrix, this, ID_CHANGECANVAS, FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_CENTER_Y|LAYOUT_FIX_WIDTH, 0, 0, 100);
	for(int i = 0; i < 15; ++i)
	{
		m_PositionListBox->appendItem(FXStringFormat("%d", i));
	}
	m_PositionListBox->setNumVisible(15);

	FXCheckButton* checkBtn = new FXCheckButton(filtermatrix, "Away View", this, ID_SELECTAWAYVIEW);
	checkBtn->setCheck(false, true);

	new FXHorizontalSeparator(mainnmatrix, LAYOUT_SIDE_TOP|SEPARATOR_GROOVE|LAYOUT_FILL_X);
	
	FXMatrix* simPart = new FXMatrix(mainnmatrix, 3, MATRIX_BY_COLUMNS|LAYOUT_FILL_X,0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	FXMatrix* simCommonPart = new FXMatrix(simPart, 3, MATRIX_BY_ROWS|LAYOUT_FILL_X);
	
	m_SimPitchType = new FXListBox(simCommonPart, NULL, 0, FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_CENTER_Y|LAYOUT_FIX_WIDTH, 0, 0, 100);
	m_SimPitchType->appendItem("Normal");
	m_SimPitchType->appendItem("Large");
	m_SimTeamState = new FXListBox(simCommonPart, this, ID_SETHOMETEAMSTATE, FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_CENTER_Y|LAYOUT_FIX_WIDTH, 0, 0, 100);
	m_SimTeamState->appendItem("Attack");
	m_SimTeamState->appendItem("Defend");

	new FXVerticalSeparator(simPart, LAYOUT_SIDE_TOP|SEPARATOR_GROOVE|LAYOUT_FILL_Y);

	FXMatrix* simOtherPart = new FXMatrix(simPart, 2, MATRIX_BY_ROWS|LAYOUT_FILL_X, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	FXMatrix* simHomeAwayPart = new FXMatrix(simOtherPart, 2, MATRIX_BY_COLUMNS|LAYOUT_FILL_X, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	
	FXMatrix* simHomePart = new FXMatrix(simHomeAwayPart, 5, MATRIX_BY_ROWS|LAYOUT_FILL_X);
	new FXLabel(simHomePart, "Home Team",NULL, JUSTIFY_CENTER_Y|LAYOUT_FILL_X|LAYOUT_CENTER_Y);
	for(int i = 0; i < 4; ++i)
	{
		m_SimHomeTeamList[i] = new FXListBox(simHomePart, NULL, 0, FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_CENTER_Y|LAYOUT_FIX_WIDTH, 0, 0, 95);
		for(int j = -1; j < 15; ++j)
		{
			m_SimHomeTeamList[i]->appendItem(FXStringFormat("%d", j));
		}
		m_SimHomeTeamList[i]->setNumVisible(15);
	}

	FXMatrix* simAwayPart = new FXMatrix(simHomeAwayPart, 5, MATRIX_BY_ROWS|LAYOUT_FILL_X);
	new FXLabel(simAwayPart, "Away Team",NULL, JUSTIFY_CENTER_Y|LAYOUT_FILL_X|LAYOUT_CENTER_Y);
	for(int i = 0; i < 4; ++i)
	{
		m_SimAwayTeamList[i] = new FXListBox(simAwayPart, NULL, 0, FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_CENTER_Y|LAYOUT_FIX_WIDTH, 0, 0, 95);
		for(int j = -1; j < 15; ++j)
		{
			m_SimAwayTeamList[i]->appendItem(FXStringFormat("%d", j));
		}
		m_SimAwayTeamList[i]->setNumVisible(15);
	}
	
	FXMatrix* simulationmatrix = new FXMatrix(simOtherPart, 3, MATRIX_BY_COLUMNS|LAYOUT_FILL_X);
	m_SimulatingBtn = new FXButton(simulationmatrix, "Start Simulation", NULL, this, ID_STARTSTOPSIMULATION, FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_CENTER_Y|LAYOUT_FIX_WIDTH, 0, 0, 95);
	m_RealGameBtn = new FXButton(simulationmatrix, "Start Real Game", NULL, this, ID_STARTSTOPREALGAME, FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_CENTER_Y|LAYOUT_FIX_WIDTH, 0, 0, 95);

	FXVerticalFrame *poGroupV3	= new FXVerticalFrame(poSplitterV,FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0, 0, 300);
	m_PlayerTuningControlTabBook = new FXTabBook(poGroupV3, NULL, 0, PACK_UNIFORM_WIDTH|PACK_UNIFORM_HEIGHT|LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_LEFT);
	m_PlayerTuningControlTabBook->setTabStyle(TABBOOK_TOPTABS);
	packing_hints = m_PlayerTuningControlTabBook->getPackingHints();
	packing_hints |= PACK_UNIFORM_WIDTH;
	m_PlayerTuningControlTabBook->setPackingHints(packing_hints);
	for(int i = 0; i < 10; ++i)
	{
		char tabName[16] = {0};
		int id = -1;
		if(i == 0)
		{
			strcpy(tabName, " Home ");
			id = HOME_TEAM_ID;
		}
		else if(i == 1)
		{
			strcpy(tabName, " Away ");
			id = AWAY_TEAM_ID;
		}
		else if(i >= HOME_TEAM_START && i < HOME_TEAM_START + HOME_TEAM_NUM)
		{
			sprintf(tabName, " H-Player %d ", i - 1);
			id = i - HOME_TEAM_START;
		}
		else if(i >= AWAY_TEAM_START && i < AWAY_TEAM_START + AWAY_TEAM_NUM)
		{
			sprintf(tabName, " A-Player %d ", i - 5);
			id = i - HOME_TEAM_START;
		}
		FXTabItem* poTab = new FXTabItem(m_PlayerTuningControlTabBook, tabName, NULL);
		poTab->setTabOrientation(TAB_TOP);
		FXHorizontalFrame* poTableFrame = new FXHorizontalFrame(m_PlayerTuningControlTabBook, FRAME_THICK|FRAME_RAISED);
		FXHorizontalFrame* poBoxframe = new FXHorizontalFrame(poTableFrame,FRAME_THICK|FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);
		m_PlayerTuningControl[i] = new PlayerTuningControl(id, poBoxframe);
	}

	char fullPath[MAX_PATH];
	strcpy(fullPath, ".\\SoccerDebuggerV2Setting.ini");
	::GetPrivateProfileString("Settings", "Game Dir", ".", g_zRootDir, MAX_PATH, fullPath);
	m_OutputDir->setText(g_zRootDir);
	::GetPrivateProfileString("Settings", "Xml File", ".\\formationdata.xml", g_zLastXMLFile, MAX_PATH, fullPath);
	::GetPrivateProfileString("Settings", "Lua File", ".\\formationdata.lua", g_zLastLUAFile, MAX_PATH, fullPath);

	//Register Event
	g_poEngine->GetEventMod()->RegisterHandler(
		(EventType_t)(E_ET_SIM_SelectPlayerInSimulating), 
		new MEventHandler<MyCanvas>(this, &MyCanvas::onSIM_SelectPlayer));
	g_poEngine->GetEventMod()->RegisterHandler(
		(EventType_t)(E_ET_SIM_SelectPlayerInRealGame), 
		new MEventHandler<MyCanvas>(this, &MyCanvas::onSIM_SelectPlayerInRealGame));
	g_poEngine->GetEventMod()->RegisterHandler(
		(EventType_t)(E_ET_AIParamUpdate), 
		new MEventHandler<MyCanvas>(this, &MyCanvas::onAIParamUpdate));
}
long MyCanvas::onKeyPress(FXObject* sender, FXSelector sel,void* ptr)
{
	FXEvent* event=(FXEvent*)ptr;
	switch(event->code){
		case KEY_Up:
		case KEY_KP_Up:
			--m_CurrentCommand;
			m_CurrentCommand = Math::Clamp(m_CurrentCommand, 0, (s32)m_CommandHistory.Size() - 1);
			if(m_CurrentCommand >= 0 && m_CurrentCommand < (s32)m_CommandHistory.Size())
			{
				m_Command->setText(m_CommandHistory[m_CurrentCommand]);
				m_Command->selectAll();
			}
			return 1;
		case KEY_Down:
		case KEY_KP_Down:
			++m_CurrentCommand;
			m_CurrentCommand = Math::Clamp(m_CurrentCommand, 0, (s32)m_CommandHistory.Size() - 1);
			if(m_CurrentCommand >= 0 && m_CurrentCommand < m_CommandHistory.Size())
			{
				m_Command->setText(m_CommandHistory[m_CurrentCommand]);
				m_Command->selectAll();
			}
			return 1;
	}
	return 0;
}
long MyCanvas::onCmdSendCommand(FXObject* sender, FXSelector sel,void* ptr)
{
	FXString commandStr = m_Command->getText();
	if(commandStr.empty())
		return 1;

	//printf("%s\n", commandStr.text());
	if(m_CommandHistory.Size() > 0)
	{
		if(m_CommandHistory[m_CommandHistory.Size() - 1] != commandStr)
		{
			m_CommandHistory.PushBack(commandStr);
		}
	}
	else
	{
		m_CommandHistory.PushBack(commandStr);
	}
	m_CurrentCommand = m_CommandHistory.Size();

	UDP_PACK stMsg;
	stMsg.ulFilter = 0;
	stMsg.ulType = MsgType_Command;
	stMsg.bIsHidden = true;
	strncpy(stMsg.unValue._zValue, commandStr.text(), 32); 

	WSABUF stWSABuf;
	stWSABuf.buf = (char*)&stMsg;
	stWSABuf.len = sizeof(stMsg);

	DWORD cbRet = 0;
	WSASendTo(g_hSSock, &stWSABuf,1, &cbRet, 0, (struct sockaddr*)&g_stDestAddr, sizeof(struct sockaddr), NULL, NULL);

	m_Command->selectAll();

	return 1;
}
long MyCanvas::onTabSelected(FXObject* sender, FXSelector sel,void* ptr)
{
	FXint current = (FXint)(ptr);
	
	MyEngine* pMyEngine = (MyEngine*)g_poEngine;
	pMyEngine->ChangeAppMode(current == E_Tab_FormationEditor ? EAppMode_FormationEditor : EAppMode_WatchMode);
	
	if(g_bFirstEntry)
	{
		long ret = onLoadFormationData(sender, sel, ptr);
		if(ret == 2)
		{
			FXMessageBox::warning(this, MBOX_OK, tr("Warning"),tr("Use default formation data!!!!!!"));
		}
		g_bFirstEntry = false;
	}
	return 1;
}
long MyCanvas::onChangeCanvas(FXObject* sender, FXSelector sel, void* ptr)
{
	MyEngine* pMyEngine = (MyEngine*)g_poEngine;
	pMyEngine->UpdateCanvas();
	
	return 1;
}
long MyCanvas::onSaveFormationData(FXObject* sender, FXSelector sel, void* ptr)
{
	FXFileDialog savedialog(this,tr("Save Config Data"));
	savedialog.setSelectMode(SELECTFILE_ANY);
	savedialog.setPatternList("*.xml");
	savedialog.setCurrentPattern(0);
	savedialog.setFilename(g_zLastXMLFile);
	if(!savedialog.execute()) 
		return 1;

	FXString file = savedialog.getFilename();
	if(FXStat::exists(file)){
		if( MBOX_CLICKED_NO == FXMessageBox::question(this,MBOX_YES_NO,tr("Overwrite Document"),tr("Overwrite existing document: %s?"),file.text())) 
			return 1;
	}
	
	TiXmlDocument* pFormationDataFile = new TiXmlDocument();
	if(pFormationDataFile)
	{
		FormationEditor* pEditor = g_poEngine->GetSceneMod()->GetSceneObject<FormationEditor>("FormationEditor");
		pEditor->Serialize(pFormationDataFile, NULL);
		pFormationDataFile->SaveFile(file.text());
		FXMessageBox::information(this,MBOX_OK,tr("Info"),tr("Save %s successfully"), file.text());
		delete pFormationDataFile;

		//to setting
		char fullPath[MAX_PATH];
		strcpy(fullPath, ".\\SoccerDebuggerV2Setting.ini");
		::WritePrivateProfileString("Settings", "Xml File", file.text(), fullPath);
		strcpy(g_zLastXMLFile, file.text());
	}
	else
	{
		FXMessageBox::error(this,MBOX_OK,tr("Error"),tr("Save %s failed"), file.text());
	}
	return 1;
}

long MyCanvas::onLoadFormationData(FXObject* sender, FXSelector sel, void* ptr)
{
	FXFileDialog loaddialog(this,tr("Load Config Data"));
	loaddialog.setSelectMode(SELECTFILE_ANY);
	loaddialog.setPatternList("*.xml");
	loaddialog.setCurrentPattern(0);
	loaddialog.setFilename(g_zLastXMLFile);
	if(!loaddialog.execute()) 
		return 2;

	FXString file = loaddialog.getFilename();
	TiXmlDocument* pFormationDataFile = new TiXmlDocument();
	if(pFormationDataFile)
	{
		pFormationDataFile->LoadFile(file.text());
		FormationEditor* pEditor = g_poEngine->GetSceneMod()->GetSceneObject<FormationEditor>("FormationEditor");
		pEditor->Clear();
		pEditor->Deserialize(pFormationDataFile, NULL);
		MyEngine* pMyEngine = (MyEngine*)g_poEngine;
		pMyEngine->UpdateCanvas();
		FXMessageBox::information(this,MBOX_OK,tr("Info"),tr("Load %s successfully"), file.text());
		delete pFormationDataFile;

		//to setting
		char fullPath[MAX_PATH];
		strcpy(fullPath, ".\\SoccerDebuggerV2Setting.ini");
		::WritePrivateProfileString("Settings", "Xml File", file.text(), fullPath);
		strcpy(g_zLastXMLFile, file.text());
		return 1;
	}
	else
	{
		FXMessageBox::error(this,MBOX_OK,tr("Error"),tr("Load %s failed"), file.text());
		return 2;
	}
}

long MyCanvas::onExportFormationData(FXObject* sender, FXSelector sel, void* ptr)
{
	FXFileDialog savedialog(this,tr("Export to"));
	savedialog.setSelectMode(SELECTFILE_ANY);
	savedialog.setPatternList("*.lua");
	savedialog.setCurrentPattern(0);
	savedialog.setFilename(g_zLastLUAFile);
	if(!savedialog.execute()) 
		return 1;

	FXString file = savedialog.getFilename();
	if(FXStat::exists(file)){
		if( MBOX_CLICKED_NO == FXMessageBox::question(this,MBOX_YES_NO,tr("Overwrite Document"),tr("Overwrite existing document: %s?"),file.text())) 
			return 1;
	}

	File* pLuaFile = FileManager::Get().OpenFile(file.text(), E_FOM_Write | E_FOM_Text);
	if(pLuaFile)
	{
		FormationEditor* pEditor = g_poEngine->GetSceneMod()->GetSceneObject<FormationEditor>("FormationEditor");
		pEditor->Export(pLuaFile);
		FXMessageBox::information(this,MBOX_OK,tr("Info"),tr("Export %s successfully"), file.text());
		FileManager::Get().CloseFile(pLuaFile);

		//to setting
		char fullPath[MAX_PATH];
		strcpy(fullPath, ".\\SoccerDebuggerV2Setting.ini");
		::WritePrivateProfileString("Settings", "Lua File", file.text(), fullPath);
		strcpy(g_zLastLUAFile, file.text());
	}
	else
	{
		FXMessageBox::error(this,MBOX_OK,tr("Error"),tr("Export %s failed"), file.text());
	}
	return 1;
}
long MyCanvas::onOpenOutputDir(FXObject* sender, FXSelector sel, void* ptr)
{
	FXDirDialog open(this,"Open Game Directory");
	open.showFiles(FALSE);
	if(open.execute()){
		m_OutputDir->setText(open.getDirectory().text());
		strcpy(g_zRootDir, open.getDirectory().text());

		//to setting
		char fullPath[MAX_PATH];
		strcpy(fullPath, ".\\SoccerDebuggerV2Setting.ini");
		::WritePrivateProfileString("Settings", "Game Dir", g_zRootDir, fullPath);
	}
	return 1;
}
long MyCanvas::onSimulationControl(FXObject* sender, FXSelector sel, void* ptr)
{
	FormationEditor* pEditor = g_poEngine->GetSceneMod()->GetSceneObject<FormationEditor>("FormationEditor");
	if(pEditor->IsSimulating())
	{
		m_SimulatingBtn->setText("Start Simulation");
		pEditor->StopSimulation();
		MyEngine* pMyEngine = (MyEngine*)g_poEngine;
		pMyEngine->UpdateCanvas();

		m_SimPitchType->enable();
		for(int i = 0; i < 4; ++i)
		{
			m_SimHomeTeamList[i]->enable();
			m_SimAwayTeamList[i]->enable();
		}
		m_RealGameBtn->enable();
	}
	else
	{
		FESimulatedCanvas::SimulatedSettings setup;
		setup.m_RootDir = g_zRootDir;
		setup.m_PitchType = m_SimPitchType->getCurrentItem();
		setup.m_TeamState = m_SimTeamState->getCurrentItem();
		for(int i = 0; i < 4; ++i)
		{
			if(m_SimHomeTeamList[i]->getCurrentItem())
			{
				setup.m_HomePlayerPosition.PushBack(m_SimHomeTeamList[i]->getCurrentItem() - 1);
			}
			if(m_SimAwayTeamList[i]->getCurrentItem())
			{
				setup.m_AwayPlayerPosition.PushBack(m_SimAwayTeamList[i]->getCurrentItem() - 1);
			}
		}
		FEDebuggerInfo* pDebuggerInfo = g_poEngine->GetSceneMod()->GetSceneObject<FEDebuggerInfo>("FEDebuggerInfo");
		pDebuggerInfo->SetSelectedPosition(-1);

		if(pEditor->StartSimulation(setup))
		{
			m_SimulatingBtn->setText("Stop Simulation");
			m_SimPitchType->disable();
			for(int i = 0; i < 4; ++i)
			{
				m_SimHomeTeamList[i]->disable();
				m_SimAwayTeamList[i]->disable();
			}
			m_RealGameBtn->disable();

			PlayerIndividualAIParam& param = m_PlayerIndividualAIParamFromEditor.AsRawData();
			for(int i = 0; i < 10; ++i)
			{
				int playerid = -1;
				if(i >= HOME_TEAM_START && i < HOME_TEAM_START + 4)
				{
					playerid = i - HOME_TEAM_START;
				}
				else if(i >= AWAY_TEAM_START && i < AWAY_TEAM_START + 4)
				{
					playerid = i - AWAY_TEAM_START + FESimulatedPlayer::sHomePlayerCount;
				}
				if(playerid >= 0 && playerid < 8)
				{
					PlayerOtherAttributes& attr = param.m_OtherAttributes[playerid];
					m_PlayerTuningControl[i]->ToData(attr);
				}
			}
		} 
		else
		{
			FXMessageBox::error(this,MBOX_OK,tr("Error"),tr("start simulation failed"));
		}
	}
	return 1;
}

long MyCanvas::onRealGameControl(FXObject* sender, FXSelector sel, void* ptr)
{
	FormationEditor* pEditor = g_poEngine->GetSceneMod()->GetSceneObject<FormationEditor>("FormationEditor");
	if(pEditor->IsInRealGame())
	{
		m_RealGameBtn->setText("Start Real Game");
		pEditor->StopRealGame();
		MyEngine* pMyEngine = (MyEngine*)g_poEngine;
		pMyEngine->UpdateCanvas();

		m_SimulatingBtn->enable();
	}
	else
	{
		const RealGameInfo& rgInfo = m_RealGameInfo.As();
		m_RealGameBtn->setText("Stop Real Game");
		pEditor->StartRealGame(rgInfo);

		const PlayerIndividualAIParam& param = m_PlayerIndividualAIParamToEditor.As();
		for(int i = 0; i < 10; ++i)
		{
			if(rgInfo.m_Player[i].m_HasValidData && !rgInfo.m_Player[i].m_IsGK)
			{
				const PlayerOtherAttributes& attr = param.m_OtherAttributes[i];
				int idx;
				if(rgInfo.m_Player[i].m_Team == kHOME_TEAM)
				{
					idx = rgInfo.m_Team[kHOME_TEAM].m_HasGK ? 
						HOME_TEAM_START + i - 1 : 
						HOME_TEAM_START + i;
				}
				else
				{
					idx = rgInfo.m_Team[kAWAY_TEAM].m_HasGK ? 
						AWAY_TEAM_START + i - FESimulatedPlayer::sHomePlayerCount - 1 : 
						AWAY_TEAM_START + i - FESimulatedPlayer::sHomePlayerCount;
				}
				m_PlayerTuningControl[idx]->InitFromData(attr);
			}
		}
		m_SimulatingBtn->disable();

		FEDebuggerInfo* pDebuggerInfo = g_poEngine->GetSceneMod()->GetSceneObject<FEDebuggerInfo>("FEDebuggerInfo");
		pDebuggerInfo->SetSelectedPosition(-1);
	}
	return 1;
}
long MyCanvas::onSelectAwayView(FXObject* sender, FXSelector sel, void* ptr)
{
	CoordinateInfo::sAwayView = ((FXuval)ptr ? true : false);
	return 1;
}
long MyCanvas::onSetHomeTeamState(FXObject* sender, FXSelector sel, void* ptr)
{
	FormationEditor* pEditor = g_poEngine->GetSceneMod()->GetSceneObject<FormationEditor>("FormationEditor");
	pEditor->SetIsHomeAttacking((m_SimTeamState->getCurrentItem() == FormationEditor::ETeamState_Attack));
	return 1;
}
void MyCanvas::onSIM_SelectPlayer(const Event* _poEvent)
{
	s32 playerID	= _poEvent->GetParam<s32>(0);
	s32 teamID		= _poEvent->GetParam<s32>(1);
	m_PlayerTuningControlTabBook->setCurrent(teamID == kHOME_TEAM ? HOME_TEAM_START + playerID : AWAY_TEAM_START + playerID - FESimulatedPlayer::sHomePlayerCount);

	Event evt((EventType_t)E_ET_SIM_SelectRefCanvasInSimulating);
	evt.AddParam(g_Canvas->m_SimPitchType->getCurrentItem());
	evt.AddParam(g_Canvas->m_SimTeamState->getCurrentItem());
	evt.AddParam(playerID);
	evt.AddParam(teamID);
	g_poEngine->GetEventMod()->SendEvent(&evt);
}
void MyCanvas::onSIM_SelectPlayerInRealGame(const Event* _poEvent)
{
	bool isGK = _poEvent->GetParam<Bool>(2);
	if(!isGK)
	{
		s32 playerID	= _poEvent->GetParam<s32>(0);
		s32 teamID		= _poEvent->GetParam<s32>(1);
		m_PlayerTuningControlTabBook->setCurrent(teamID == kHOME_TEAM ? HOME_TEAM_START + playerID - 1 : AWAY_TEAM_START + playerID - FESimulatedPlayer::sHomePlayerCount - 1);

		s32 pitchList	= m_RealGameInfo.As().m_IsLargePitch ? FormationEditor::EPitchType_Large : FormationEditor::EPitchType_Normal;
		s32 teamState	= m_RealGameInfo.As().m_IsHomeAttacking ? FormationEditor::ETeamState_Attack : FormationEditor::ETeamState_Defend;

		Event evt((EventType_t)E_ET_SIM_SelectRefCanvasInRealGame);
		evt.AddParam(pitchList);
		evt.AddParam(teamState);
		evt.AddParam(playerID);
		evt.AddParam(teamID);
		g_poEngine->GetEventMod()->SendEvent(&evt);
	}
}
void MyCanvas::onAIParamUpdate(const Event* _poEvent)
{
	s32 id	= _poEvent->GetParam<s32>(0);
	s32 idx	= _poEvent->GetParam<s32>(1);
	s32 val = _poEvent->GetParam<s32>(2);

	FormationEditor* pEditor = g_poEngine->GetSceneMod()->GetSceneObject<FormationEditor>("FormationEditor");

	PlayerOtherAttributes* pOutAttr = NULL;
	if(pEditor->IsInRealGame())
	{
		PlayerIndividualAIParam& param = m_PlayerIndividualAIParamFromEditor.AsRawData();
		const RealGameInfo& rgInfo = m_RealGameInfo.As();
		int playerid = -1;
		if(id >= 0 && id < 4)
		{
			if(rgInfo.m_Team[kHOME_TEAM].m_HasGK)
				playerid = id + 1;
			else
				playerid = id;
		}
		else if(id >= 4 && id < 4 + 4)
		{
			if(rgInfo.m_Team[kAWAY_TEAM].m_HasGK)
				playerid = id - 4 + 1 + FESimulatedPlayer::sHomePlayerCount;
			else
				playerid = id - 4 + FESimulatedPlayer::sHomePlayerCount;
		}
		if(playerid >= 0 && playerid < 10)
			pOutAttr = &param.m_OtherAttributes[playerid];
	}
	else if(pEditor->IsSimulating())
	{
		PlayerIndividualAIParam& param = m_PlayerIndividualAIParamFromEditor.AsRawData();
		int playerid = -1;
		if(id >= 0 && id < 4)
		{
			playerid = id;
		}
		else if(id >= 4 && id < 4 + 4)
		{
			playerid = id - 4 + FESimulatedPlayer::sHomePlayerCount;
		}
		if(playerid >= 0 && playerid < 10)
			pOutAttr = &param.m_OtherAttributes[playerid];
	}
	if(pOutAttr)
	{
		switch(idx)
		{
		case IDX_FormationDensity:	{ pOutAttr->m_FormationDensity	= Math::Clamp((float)val/100.f, 0.f, 1.f); break; }
		case IDX_SideAttack:		{ pOutAttr->m_SideAttack		= val; break; }
		case IDX_DefensiveLine:		{ pOutAttr->m_DefensiveLine		= val; break; }
		case IDX_Width:				{ pOutAttr->m_Width				= Math::Clamp((float)val/100.f, 0.f, 1.f); break; }
		case IDX_Mentality:			{ pOutAttr->m_Mentality			= Math::Clamp((float)val/100.f, 0.f, 1.f); break; }
		case IDX_Tempo:				{ pOutAttr->m_Tempo				= Math::Clamp((float)val/100.f, 0.f, 1.f); break; }
		case IDX_TimeWasting:		{ pOutAttr->m_TimeWasting		= Math::Clamp((float)val/100.f, 0.f, 1.f); break; }
		case IDX_FocusPassing:		{ pOutAttr->m_FocusPassing		= val; break; }
		case IDX_ClosingDown:		{ pOutAttr->m_ClosingDown		= Math::Clamp((float)val/100.f, 0.f, 1.f); break; }
		case IDX_TargetMan:			{ pOutAttr->m_TargetMan			= val ? true : false; break; }
		case IDX_PlayMaker:			{ pOutAttr->m_Playmaker			= val ? true : false; break; }
		case IDX_CounterAttack:		{ pOutAttr->m_CounterAttack		= val ? true : false; break; }
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------------

namespace Util
{
	typedef std::map<std::string, int>::iterator NameToRowIter;
	std::map<std::string, int> g_NameToRow[E_Tab_Num];
	typedef std::map<std::string, float>::iterator NameToClockIter;
	std::map<std::string, float> g_NameToClock[E_Tab_Num];

	static const float kMaxHighlightingClock = 4.f;

	int GetItemSize(ETabIndex tab)
	{
		FXTable* poTable = g_Canvas->GetTable(tab);
		return poTable->getNumRows();
	}
	void Clear(ETabIndex tab)
	{
		FXTable* poTable = g_Canvas->GetTable(tab);
		int size = poTable->getNumRows();
		for(int i = 0; i < size; ++i)
			poTable->removeRows(0);
		g_NameToRow[tab].clear();
		g_NameToClock[tab].clear();
	}
	int GetRowByName(ETabIndex tab, const Char* name)
	{
		FXTable* poTable = g_Canvas->GetTable(tab);

		NameToRowIter iter = g_NameToRow[tab].find(name);

		int row;
		if(iter == g_NameToRow[tab].end())
		{
			row = poTable->getNumRows();
			poTable->insertRows(row);
			g_NameToRow[tab].insert(std::pair<std::string, int>(name, row));
			g_NameToClock[tab].insert(std::pair<std::string, int>(name, 0));
		}
		else
		{
			row = (*iter).second;
		}
		return row;
	}
	void DrawValue(ETabIndex tab, UDP_PACK* pack, bool shouldHighLight)
	{
		int row = GetRowByName(tab, pack->zName);

		FXTable* poTable = g_Canvas->GetTable(tab);

		poTable->setItemText(row, 0, pack->zName);
		poTable->setItemJustify(row, 0, FXTableItem::LEFT|FXTableItem::CENTER_Y);

		Char zValue[32];
		switch(pack->ulType)
		{
		case MsgType_Float:
			sprintf(zValue, "%.3f", pack->unValue._fValue);
			break;
		case MsgType_Vector:
			sprintf(zValue, "(%.3f, %.3f, %.3f)", pack->unValue._vValue.x, pack->unValue._vValue.y, pack->unValue._vValue.z);
			break;
		case MsgType_Int:
			sprintf(zValue, "%d", pack->unValue._iValue);
			break;
		case MsgType_String:
			strncpy(zValue, pack->unValue._zValue, 32);
			break;
		}
		float clock;

		NameToClockIter iter = g_NameToClock[tab].find(pack->zName);
		D_CHECK(iter != g_NameToClock[tab].end());
		if(!shouldHighLight)
		{
			clock = (*iter).second;
			clock = Math::Max(0.f, clock - g_poEngine->GetClockMod()->GetFrameElapsedSeconds());
		}
		else
		{
			clock = kMaxHighlightingClock;
		}
		(*iter).second = clock;

		if(clock > 0)
		{
			poTable->setItemStipple(row, 1, STIPPLE_WHITE);
		}
		else
		{
			poTable->setItemStipple(row, 1, STIPPLE_BLACK);
		}
		poTable->setItemText(row, 1, zValue);
		poTable->setItemJustify(row, 1, FXTableItem::LEFT|FXTableItem::CENTER_Y);
	}
}

void MyEngine::DoInit()
{
	FXApp *poApp = g_poEngine->GetRenderMod()->GetGUIApp();
	FXMainWindow *poMain = g_poEngine->GetRenderMod()->GetGUIMainWindow();
	g_Canvas = new MyCanvas(poMain);

	NetWorkInit();

	g_poMem = new MemPool<UDP_PACK>;
	g_poMem->SetMaxSize(1000);

	g_poSROU = new WinGDISimpleRenderObjectUtility((WinGDIRenderer*)g_poEngine->GetRenderMod()->GetRenderer());
	//RenderWindowMsg::RegisterMsgListener(new SDWindowMsgCallBack);
	
	g_poEngine->GetSceneMod()->AddObject("RefValueUpdater", new ORefValueUpdater);
	g_poEngine->GetSceneMod()->AddObject("FormationEditor", new FormationEditor);
	g_poEngine->GetSceneMod()->AddObject("FEDebuggerInfo", new FEDebuggerInfo);
	g_poEngine->GetSceneMod()->AddObject("ZBall", new OBall);
	g_poEngine->GetSceneMod()->AddObject("Parser", new OParser);
	g_poEngine->GetSceneMod()->AddObject("TeamHome", new OTeam(kHOME_TEAM));
	g_poEngine->GetSceneMod()->AddObject("TeamAway", new OTeam(kAWAY_TEAM));
	Object* pField = new OField;
	pField->SetZOrder(EZOrder_Bottom);
	g_poEngine->GetSceneMod()->AddObject("Field", pField);
	g_poEngine->GetSceneMod()->AddObject("Watch", new OWatch);

	g_hRecv = BEGINTHREADEX(0,0,RecvUDPPack,0,0,0);

	//memset(g_zLastOpenFile, 0, kMAX_FILENAME_LENGTH);

	ChangeAppMode(EAppMode_WatchMode);
}

void MyEngine::ChangeAppMode(u32 mode)
{
	if(mode != m_AppMode)
	{
		Array<Object*> showObj, hideObj;
		if(mode == EAppMode_FormationEditor)
		{
			showObj.PushBack(g_poEngine->GetSceneMod()->GetSceneObject<Object>("RefValueUpdater"));
			showObj.PushBack(g_poEngine->GetSceneMod()->GetSceneObject<Object>("FormationEditor"));
			showObj.PushBack(g_poEngine->GetSceneMod()->GetSceneObject<Object>("FEDebuggerInfo"));

			hideObj.PushBack(g_poEngine->GetSceneMod()->GetSceneObject<Object>("ZBall"));
			hideObj.PushBack(g_poEngine->GetSceneMod()->GetSceneObject<Object>("TeamHome"));
			hideObj.PushBack(g_poEngine->GetSceneMod()->GetSceneObject<Object>("TeamAway"));

			UpdateCanvas();
		}
		else
		{
			hideObj.PushBack(g_poEngine->GetSceneMod()->GetSceneObject<Object>("RefValueUpdater"));
			hideObj.PushBack(g_poEngine->GetSceneMod()->GetSceneObject<Object>("FormationEditor"));
			hideObj.PushBack(g_poEngine->GetSceneMod()->GetSceneObject<Object>("FEDebuggerInfo"));

			showObj.PushBack(g_poEngine->GetSceneMod()->GetSceneObject<Object>("ZBall"));
			showObj.PushBack(g_poEngine->GetSceneMod()->GetSceneObject<Object>("TeamHome"));
			showObj.PushBack(g_poEngine->GetSceneMod()->GetSceneObject<Object>("TeamAway"));

			CoordinateInfo::sLength = kPithLenghNormal;
			CoordinateInfo::sWidth = kPitchWidthNormal;
		}
		for(int i = 0; i < showObj.Size(); ++i)
		{
			if(showObj[i])
				showObj[i]->AddControlFlag(E_OCF_Active | E_OCF_Show);
		}
		for(int i = 0; i < hideObj.Size(); ++i)
		{
			if(hideObj[i])
				hideObj[i]->RemoveControlFlag(E_OCF_Active | E_OCF_Show);
		}
		m_AppMode = mode;
	}
}

void MyEngine::UpdateCanvas()
{
	FormationEditor* pEditor = g_poEngine->GetSceneMod()->GetSceneObject<FormationEditor>("FormationEditor");
	int pitchList	= g_Canvas->m_PitchListBox->getCurrentItem();
	int teamState	= g_Canvas->m_TeamStateListBox->getCurrentItem();
	int position	= g_Canvas->m_PositionListBox->getCurrentItem();

	pEditor->SetCurrentCanvas(pitchList, teamState, position);

	FEDebuggerInfo* pDebuggerInfo = g_poEngine->GetSceneMod()->GetSceneObject<FEDebuggerInfo>("FEDebuggerInfo");
	pDebuggerInfo->SetSelectedPosition(position);
}

void MyEngine::DoUnInit()
{
	g_bStop = TRUE;
	s32 nRet = WaitForSingleObject(g_hRecv,2000);
	if(nRet == WAIT_TIMEOUT)
	{
		TerminateThread(g_hRecv,0);
	}
	CloseHandle(g_hRecv);
	g_poMem->CleanBuff();

	NetWorkDeinit();

}