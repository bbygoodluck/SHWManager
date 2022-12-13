#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED

#define EVT_MY_CUSTOM_COMMAND(custom_cmd, id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        custom_cmd, id, wxID_ANY, \
        wxCommandEventHandler(fn), \
        (wxObject *) NULL \
    ),

#define ARRSIZE(arr) (sizeof(arr) / sizeof(arr[0]))

#if defined(_UNICODE) || defined(UNICODE)
	#define CONVSTR(x)        (x.wchar_str())
    #define CONVSTR_TO_STD(x) (x.ToStdWstring())
#else
	#define CONVSTR(x)        (x.char_str())
    #define CONVSTR_TO_STD(x) (x.ToStdString())
#endif

#define CONVSTR_TO_C_STR(x)   (x.c_str())

const int wxKEY_DOT			= 46;
//const int wxKEY_SLASH		= 92;
const int WXK_REVERSE_SLASH = 92;  // [\]
const int WXK_SLASH         = 47;  // [/]
const int WXK_COLONE        = 58;  // [:]
const int WXK_QUESTION      = 63;  // [?]
const int WXK_DBQUATATION   = 34;  // ["]
const int WXK_LEFT_M_G      = 60;  // [<]
const int WXK_RIGHT_M_G     = 62;  // [>]
const int WXK_V_LINE        = 124; // [|]
const int WXK_WINDOWS_KEY   = 393;

const unsigned long ATTR_DIR	 = FILE_ATTRIBUTE_DIRECTORY;
const unsigned long ATTR_ARCHIVE = FILE_ATTRIBUTE_ARCHIVE;
const unsigned long ATTR_HIDDEN	 = FILE_ATTRIBUTE_HIDDEN;
const unsigned long ATTR_SYSTEM	 = FILE_ATTRIBUTE_SYSTEM;
const unsigned long ATTR_RDONLY	 = FILE_ATTRIBUTE_READONLY;

const long CHILD_WINDOW_STYLE = wxBORDER_NONE | wxCLIP_CHILDREN | /* wxTRANSPARENT_WINDOW  |*/ wxCLIP_SIBLINGS | wxNO_FULL_REPAINT_ON_RESIZE;
const long IMAGELIST_FLAG = SHGFI_ICON | SHGFI_SMALLICON | SHGFI_SYSICONINDEX | SHGFI_OVERLAYINDEX;// | SHGFI_USEFILEATTRIBUTES ;
const long FILELISTVIEW_STYLE = wxBORDER_NONE | wxWANTS_CHARS | wxCLIP_CHILDREN | /* wxTRANSPARENT_WINDOW  |*/ wxCLIP_SIBLINGS | wxNO_FULL_REPAINT_ON_RESIZE;

const wxString SLASH = wxString(wxFILE_SEP_PATH);

typedef struct _EXTERNAL_EDIT_PROGRAM {
	wxString _strPGName = wxT("");
	wxString _strPGPath = wxT("");
	wxString _strPGArgs = wxT("");
} EXTERNAL_EDIT_PROGRAM, *PEXTERNAL_EDIT_PROGRAM;

const int EXTERNAL_PROGRAM_START_ID = 31000;
const int EXTERNAL_PROGRAM_END_ID   = 31010;

const int wxSHC_MENU_TOP = 0;
const int wxSHC_MENU			= wxID_HIGHEST + 1001;	//wxWidgets의 가장 마지막 ID값은 wxID_HIGHEST(5999)
const int wxSHC_MENU_FILE		= wxSHC_MENU;			//파일메뉴 ID시작
const int wxSHC_MENU_EDIT		= wxSHC_MENU + 100;
const int wxSHC_MENU_PATH		= wxSHC_MENU + 200;
const int wxSHC_MENU_COMP		= wxSHC_MENU + 300;
const int wxSHC_MENU_VIEW		= wxSHC_MENU + 400;
const int wxSHC_MENU_FAVORITE	= wxSHC_MENU + 500;
const int wxSHC_MENU_TOOLS		= wxSHC_MENU + 600;
const int wxSHC_MENU_HELP		= wxSHC_MENU + 700;
const int wxSHC_MENU_SEPERATOR	= 9999;

static int wxTABWindowID		= wxSHC_MENU + 2000;
static int wxFILFELIST_VIEWID	= wxSHC_MENU + 3000;
static int wxINFOVIEW_ID		= wxSHC_MENU + 3500;
static int wxDRIVEVIEW_ID		= wxSHC_MENU + 3600;
static int wxHISTORY_ID			= wxSHC_MENU + 4000;
static int wxPATHVIEW_ID		= wxSHC_MENU + 4500;
static int wxDRIVE_ID_START		= wxSHC_MENU + 5000;		//드라이브목록ID는 100개(default)
static int wxDRIVE_ID_END		= wxDRIVE_ID_START + 100;
//즐겨찾기ID
const int wxFAVIROTE_ID_START	= wxSHC_MENU + 5500;
const int wxFAVIROTE_ID_END     = wxFAVIROTE_ID_START + 499;

static int wxDIRINFOVIEW_ID		= wxSHC_MENU + 6000;
static int wxEXTCMD_ID			= wxSHC_MENU + 7000;
static int wxPATH_NEXT_START_ID = wxSHC_MENU + 8000;
static int wxPATH_NEXT_END_ID   = wxPATH_NEXT_START_ID + 500;
static int wxTIMER_ID           = 100;

const double KILOBYTE = 1024.0;
const double MEGABYTE = (1024.0 * 1024.0);
const double GIGABYTE = (1024.0 * 1024.0 * 1024.0);
const double TERABYTE = (1024.0 * 1024.0 * 1024.0 * 1024.0);

const wxString TIME_FORMAT_DEFAULT = wxT("%04d-%02d-%02d %02d:%02d");
const wxString SPACE_FORMAT_DEFAULT = wxT("%s / %s");

const int TOOLBAR_START = 500;

const int TOOLBAR_ID_GO_ROOT                 = TOOLBAR_START + 1;
const int TOOLBAR_ID_GO_BACK                 = TOOLBAR_START + 2;
const int TOOLBAR_ID_REFRESH                 = TOOLBAR_START + 3;
const int TOOLBAR_ID_FILE_EDIT               = TOOLBAR_START + 4;
const int TOOLBAR_ID_FIND                    = TOOLBAR_START + 5;
const int TOOLBAR_ID_DIR_MANAGER             = TOOLBAR_START + 6;
const int TOOLBAR_ID_BOOKMARK                = TOOLBAR_START + 7;
const int TOOLBAR_ID_FUNCTION_KEY            = TOOLBAR_START + 8;
const int TOOLBAR_ID_ENV_SETTING             = TOOLBAR_START + 9;
const int TOOLBAR_ID_ENV_SAVE                = TOOLBAR_START + 10;
const int TOOLBAR_ID_RESOURCE_USAGE          = TOOLBAR_START + 11;
const int TOOLBAR_ID_DISK_USAGE              = TOOLBAR_START + 12;
const int TOOLBAR_ID_THIS_PROGRAM            = TOOLBAR_START + 13;

const int VIEW_SUBDIR_START_ID = 1000;
const int VIEW_SUBDIR_START_END = 1500;

#ifdef __WXMSW__
typedef struct _PROCESS_INFO {
	HANDLE _hProcess = nullptr;
	unsigned long _ulProcessID = 0x00;
	wxString _strProcessName = wxT("");
	wxString _strProcessFullPath = wxT("");
	wxString _strFileDescription = wxT("");
	wxString _strCommandLine     = wxT("");
	wxString _strCompanyName     = wxT("");
	wxString _strUserName        = wxT("");
	wxString _strDomainName      = wxT("");
	wxString _strDomainAndUser   = wxT("");

	unsigned long long _PrivateSize = 0;
	unsigned long long _WorkingSetSize = 0;

	float fCPUUsage = 0.0f;

	int iIconIndex = -1;
	int iOvelayIndex = -1;

	FILETIME _ftSysKernel;
	FILETIME _ftSysUser;
	FILETIME _ftProcKernel;
	FILETIME _ftProcUser;

	FILETIME _ftLastCPU;
	FILETIME _ftLastSysCPU;
	FILETIME _ftLastUserCPU;
/*
	ULARGE_INTEGER _lastCPU;
	ULARGE_INTEGER _lastSysCPU;
	ULARGE_INTEGER _lastUserCPU;
*/
} PROCESS_INFO, *PPROCESS_INFO;
#endif

constexpr int COMPRESS_START_ID     = 32000;
constexpr int COMPRESS_END_ID       = 32100;

#endif // DEFS_H_INCLUDED
