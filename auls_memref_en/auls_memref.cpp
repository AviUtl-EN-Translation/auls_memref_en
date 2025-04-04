////////////////////////////////////////////////////////////////////////////////
// // auls_memref.cpp
// memory reference plug-in
// -----------------------------------------------------------------------------
// Provides a way to reference memory for extended editing to other plug-ins.
// When the memory address changes due to an extended edit update,
// All you need to do is update this plug-in.
////////////////////////////////////////////////////////////////////////////////

#include <aulslib/aviutl.h>
#include <aulslib/exedit.h>
#include <aulslib/memref.h>

using namespace auls;





static const LPSTR FILTER_INFORMATION = "Auls Memory Reference for 1.00~/0.92";

static const int TARGET_AVIUTL_VERSION = 10000; // ver 1.00
static const LPCSTR TARGET_EXEDIT_INFORMATION = "Adv. Editing(exedit) version 0.92 by Mr. Ken";

// exedit Memory Offset
struct EXEDIT_MEMORY_OFFSET {
	static const DWORD StaticFilterTable = 0x0A3E28;
	static const DWORD SortedObjectTable_LayerIndexEnd = 0x135AC8;
	static const DWORD AliasNameBuffer = 0x135C70;
	static const DWORD ObjDlg_CommandTarget = 0x14965C;
	static const DWORD SortedObjectTable_LayerIndexBegin = 0x149670;
	static const DWORD ObjDlg_FilterStatus = 0x158D38;
	static const DWORD SortedObjectTable = 0x168FA8;
	static const DWORD ObjDlg_ObjectIndex = 0x177A10;
	static const DWORD SceneSetting = 0x177A50;
	static const DWORD LoadedFilterTable = 0x187C98;
	static const DWORD LayerSetting = 0x188498;
	static const DWORD SceneDisplaying = 0x1A5310;
	static const DWORD TextBuffer = 0x1A6BD0;
	static const DWORD TraScript_ProcessingTrackBarIndex = 0x1B21F4;
	static const DWORD TraScript_ProcessingObjectIndex = 0x1B2B04;
	static const DWORD ScriptProcessingFilter = 0x1B2B10;
	static const DWORD LuaState = 0x1BACA8;
	static const DWORD ObjectBufferInfo = 0x1E0F9C;
	static const DWORD CameraZBuffer = 0x1EC7AC;
	static const DWORD UndoInfo = 0x244E08;
};

enum ERRORCODE {
	ERRORCODE_NOERROR,
	ERRORCODE_INITIALIZE_FAILURE,
	ERRORCODE_AVIUTL_VERSION_WRONG,
	ERRORCODE_NO_EXEDIT,
	ERRORCODE_EXEDIT_VERSION_WRONG,
	ERRORCODE_MAX,
};

#define pre "[memory reference plug-in]\r\n"
static const LPCSTR ERROR_STRING[] = {
	pre"There are no errors.",
	pre"Initialization failed.",
	pre"Unsupported version of AviUtl.",
	pre"Could not find the exedit plug-in.",
	pre"Unsupported version of the exedit plug-in.",
};
#undef pre

DWORD g_exedit_inst;
int g_errorcode;

extern "C" __declspec(dllexport) int Init(FILTER* fp)
{
	static bool initialized = false;
	if (initialized) return ERRORCODE_NOERROR;

	if (!fp) return ERRORCODE_INITIALIZE_FAILURE;

	if (AviUtl_GetVersionNumber(fp) < TARGET_AVIUTL_VERSION) return ERRORCODE_AVIUTL_VERSION_WRONG;

	FILTER* exedit = Exedit_GetFilter(fp);
	if (!exedit) return ERRORCODE_NO_EXEDIT;
	if (strcmp(exedit->information, TARGET_EXEDIT_INFORMATION)) return ERRORCODE_EXEDIT_VERSION_WRONG;

	g_exedit_inst = (DWORD)exedit->dll_hinst;

	initialized = true;
	return ERRORCODE_NOERROR;
}

extern "C" __declspec(dllexport) LPCSTR GetErrorString(int e)
{
	if (e >= 0 && e < ERRORCODE_MAX) return ERROR_STRING[e];
	else return NULL;
}

#define def(type, name)\
	extern "C" __declspec(dllexport) type Exedit_##name(void)\
	{\
		return (type)(EXEDIT_MEMORY_OFFSET::name + g_exedit_inst);\
	}

def(EXEDIT_FILTER**, StaticFilterTable);
def(int*, SortedObjectTable_LayerIndexEnd);
def(LPSTR, AliasNameBuffer);
def(int*, ObjDlg_CommandTarget);
def(int*, SortedObjectTable_LayerIndexBegin);
def(BYTE*, ObjDlg_FilterStatus);
def(EXEDIT_OBJECT**, SortedObjectTable);
def(int*, ObjDlg_ObjectIndex);
def(SCENE_SETTING*, SceneSetting);
def(EXEDIT_FILTER**, LoadedFilterTable);
def(LAYER_SETTING*, LayerSetting);
def(int*, SceneDisplaying);
def(wchar_t*, TextBuffer);
def(int*, TraScript_ProcessingTrackBarIndex);
def(int*, TraScript_ProcessingObjectIndex);
def(EXEDIT_FILTER*, ScriptProcessingFilter);
def(lua_State*, LuaState);
def(OBJECT_BUFFER_INFO*, ObjectBufferInfo);
def(CAMERA_ZBUFFER**, CameraZBuffer);
def(UNDO_INFO*, UndoInfo);
#undef def


////////////////////////////////////////////////////////////////////////////////
// initialization
////////////////////////////////////////////////////////////////////////////////

BOOL func_init(FILTER* fp)
{
	Init(fp);
	return TRUE;
}

FILTER_DLL filter = {
	FILTER_FLAG_ALWAYS_ACTIVE | FILTER_FLAG_NO_CONFIG | FILTER_FLAG_EX_INFORMATION,
	0, 0,
	MEMREF_FILTER_NAME,
	0, NULL, NULL, NULL, NULL,
	0, NULL, NULL,
	NULL,
	func_init,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	FILTER_INFORMATION,
};

extern "C" __declspec(dllexport) FILTER_DLL* GetFilterTable()
{
	return &filter;
}
