#ifndef _AI_WIN_H_
#define _AI_WIN_H_
#include <WINDOWSX.H>
#include <COMMCTRL.H>

#ifndef AI_EXPORT_WIN
#define AI_EXPORT_WIN __declspec(dllimport) 
#endif
//////////////////////////////////////////////////////////////////////////
//
#define MENU_GRAYED(hwnd, id)						EnableMenuItem(GetMenu(hwnd), (id), MF_GRAYED   )
#define MENU_ENABLED(hwnd, id)					    EnableMenuItem(GetMenu(hwnd), (id), MF_ENABLED  )
#define MENU_CHECK_ON(hwnd, id)					    CheckMenuItem (GetMenu(hwnd), (id), MF_CHECKED  )
#define MENU_CHECK_OFF(hwnd, id)					CheckMenuItem (GetMenu(hwnd), (id), MF_UNCHECKED)
#define MENU_BMP(hwnd, id, bmp_id)					SetMenuItemBitmaps(GetMenu(hwnd), (id), MF_BYCOMMAND, LoadBitmap((HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), MAKEINTRESOURCE(bmp_id)), NULL)


#define ITEM_GRAYED(hdlg, idd)						EnableWindow(GetDlgItem(hdlg, idd), FALSE)
#define ITEM_ENABLED(hdlg, idd)						EnableWindow(GetDlgItem(hdlg, idd), TRUE)

#define ITEM_ACTIVE(hdlg, idd)						SendMessage(hdlg, WM_COMMAND, idd, 0)
#define ITEM_POSTACTIVE(hdlg, idd)					PostMessage(hdlg, WM_COMMAND, idd, 0)

#define ITEM_HIDE(hdlg, idd)						ShowWindow(GetDlgItem(hdlg, idd), SW_HIDE)
#define ITEM_SHOW(hdlg, idd)						ShowWindow(GetDlgItem(hdlg, idd), SW_NORMAL)

#define GET_CHECKER(hdlg, idd)						(int)SendMessage(GetDlgItem(hdlg, idd), BM_GETCHECK, 0, 0)

#define SET_CHECKER_ON(hdlg, idd)					SendMessage(GetDlgItem(hdlg, idd), BM_SETCHECK, BST_CHECKED, 0)		
#define SET_CHECKER_OFF(hdlg, idd)					SendMessage(GetDlgItem(hdlg, idd), BM_SETCHECK, BST_UNCHECKED, 0)
#define SET_CHECKER(hdlg, idd, b)					SendMessage(GetDlgItem(hdlg, idd), BM_SETCHECK, b, 0)

#define COMBO_ADDSTR(hdlg, idd, str)				SendDlgItemMessage(hdlg, idd, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)str)
#define COMBO_GETCOUNT(hdlg, idd)					SendDlgItemMessage(hdlg, idd, CB_GETCOUNT, 0, 0)
#define COMBO_SETCURSEL(hdlg, idd, num)				SendDlgItemMessage(hdlg, idd, CB_SETCURSEL, num, 0L)
#define COMBO_GETCURSEL(hdlg, idd)					SendDlgItemMessage(hdlg, idd, CB_GETCURSEL, 0, 0L)
#define COMBO_GETTEXT(hdlg, idd, num, str)			SendDlgItemMessage(hdlg, idd, CB_GETLBTEXT, num, (LPARAM)str)
#define COMBO_RESET(hdlg, idd)					    SendDlgItemMessage(hdlg, idd, CB_RESETCONTENT, 0, 0)

#define LISTBOX_RESET(hdlg, idd)				    SendDlgItemMessage(hdlg, idd, LB_RESETCONTENT, 0, 0)
#define LISTBOX_ADDSTR(hdlg, idd, str)			    SendDlgItemMessage(hdlg, idd, LB_ADDSTRING, 0, (LPARAM)(LPCTSTR)str)
#define LISTBOX_DELSTR(hdlg, idd, num)				SendDlgItemMessage(hdlg, idd, LB_DELETESTRING, (WPARAM)(int)(num), 0L)
#define LISTBOX_INSERTSTR(hdlg, idd, num, str)		SendDlgItemMessage(hdlg, idd, LB_INSERTSTRING, (WPARAM)(int)(num), (LPARAM)(LPCTSTR)(str))
#define LISTBOX_SETCURSEL(hdlg, idd, num)			SendDlgItemMessage(hdlg, idd, LB_SETCURSEL, num, 0L)
#define LISTBOX_GETCURSEL(hdlg, idd)				SendDlgItemMessage(hdlg, idd, LB_GETCURSEL, 0, 0L)
#define LISTBOX_GETTEXT(hdlg, idd, num, str)		SendDlgItemMessage(hdlg, idd, LB_GETTEXT, num, (LPARAM)str)
#define LISTBOX_TOTALCOUNT(hdlg, idd)				SendDlgItemMessage(hdlg, idd, LB_GETCOUNT, 0, 0L)
#define LISTBOX_SETSEL(hdlg, idd, num)				SendDlgItemMessage(hdlg, idd, LB_SETSEL, TRUE, (LPARAM)(LPCTSTR)(num))
#define LISTBOX_GETLHEIGHT(hdlg, idd)				SendDlgItemMessage(hdlg, idd, LB_GETITEMHEIGHT, 0, 0)
#define LISTBOX_SETLHEIGHT(hdlg, idd, n)			SendDlgItemMessage(hdlg, idd, LB_SETITEMHEIGHT, 0, n)

#define SET_SPINNER_RANGE(hDlg, idd, nMin, nMax)	\
SendMessage(GetDlgItem(hDlg, idd), UDM_SETRANGE, 0, (LPARAM)MAKELONG((short)nMin, (short)nMax))

#define SET_SLIDER_RANGE(hdlg, idd, minval, maxval)	SendDlgItemMessage(hdlg, idd, TBM_SETRANGE, TRUE, MAKELPARAM(minval, maxval))
#define GET_SLIDER_POS(hdlg, idd)					SendDlgItemMessage(hdlg, idd, TBM_GETPOS, 0, 0)
#define SET_SLIDER_POS(hdlg, idd, val)				SendDlgItemMessage(hdlg, idd, TBM_SETPOS, TRUE, (LPARAM)(LONG)val)	

#define SET_STATUSBAR_TEXT(hdlg, n, str)			SendMessage(hdlg, SB_SETTEXT, n, (LPARAM)str)

#define HANDLE_DLGMSG(hWnd, Msg, Fn)	\
	case (Msg): return (SetDlgMsgResult(hWnd, Msg,	HANDLE_##Msg((hWnd), (wParam), (lParam), (Fn))))

#ifndef HANDLE_WM_NOTIFY
#define HANDLE_WM_NOTIFY(hWnd, wParam, lParam, fn)						\
	case WM_NOTIFY: return (fn)((hWnd), (int)(wParam), (LPNMHDR)(lParam))
#endif

//////////////////////////////////////////////////////////////////////////
#define FOLDER_NAME_LENGTH		50

#define DLGTYPE_CHECK_BOOL		0
#define DLGTYPE_INPUT_INT		1
#define DLGTYPE_INPUT_FLOAT		2
#define DLGTYPE_INPUT_STRING	3
#define DLGTYPE_INPUT_HEX		4
#define DLGTYPE_COMBO_INT		5
#define DLGTYPE_COMBO_INPUT_INT 6
#define DLGTYPE_INPUT_HEX4		7
#define DLGTYPE_RADIO_BOOL		8
#define DLGTYPE_INPUT_DOUBLE	9

#define FILE_TYPE_ALL			0
#define FILE_TYPE_BMP_RAW		1
#define FILE_TYPE_BMP_ONLY		2
#define FILE_TYPE_INI			3
#define FILE_TYPE_CSV			4
#define FILE_TYPE_IMAGES		5

#define BUTTON_IMG_NONE			0X0000
#define BUTTON_IMG_ICON			0X0001
#define BUTTON_IMG_BMP			0X0002

#define BUTTON_INNER_TEXTONLY	0X0000
#define BUTTON_INNER_IMGONLY	0X0010
#define BUTTON_INNER_IMGTEXT	0X0020

#define BUTTON_FRAME_GRAY		0X0000
#define BUTTON_FRAME_FLAT		0X0100
#define BUTTON_FRAME_NONE		0X0200	
#define BUTTON_FRAME_BG_COLOR	0X0300

typedef struct _INIEdit 
{
	int idc;
	int type;
	void *var;
	char name[50];
	
}INIEdit;

namespace ai_win
{
	class AI_EXPORT_WIN Dlg
	{
	public:
		virtual BOOL init(HWND hdlg, HWND hfocus, LPARAM linit);						
		virtual void command(HWND hdlg, int id, HWND hctrl, UINT code_notify);	
		virtual void paint(HWND hdlg);													
		virtual BOOL notify(HWND hdlg, int idctrl, LPNMHDR pnmh);						
		virtual void hscroll(HWND hdlg, HWND hctrl, UINT code, int pos);			
		virtual void timer(HWND hdlg, unsigned int id);									
		virtual void mouse_move(HWND hdlg, int x, int y, unsigned int key_flags);	
		virtual void draw_item(HWND hdlg, const DRAWITEMSTRUCT *pdis);				
		virtual void size(HWND hdlg, unsigned int state, int width, int height);			 
		virtual void close(HWND hdlg);													
		
		virtual LRESULT ctlcolor_dlg(HWND hdlg, HDC hdc, HWND hcolor, int mark=CTLCOLOR_DLG);		
		virtual LRESULT ctlcolor_static(HWND hdlg, HDC hdc, HWND hcolor, int mark=CTLCOLOR_STATIC);
		virtual LRESULT ctlcolor_edit(HWND hdlg, HDC hdc, HWND hcolor, int mark=CTLCOLOR_EDIT);	
		virtual LRESULT ctlcolor_listbox(HWND hdlg, HDC hdc, HWND hcolor, int mark=CTLCOLOR_LISTBOX);
	};

	class AI_EXPORT_WIN Timer
	{
	public:
		void start(void);
		double end(void);

	private:
		LARGE_INTEGER start_time;
		LARGE_INTEGER end_time;
		LARGE_INTEGER freq;
	};

	class AI_EXPORT_WIN Painting
	{
	public:
		Painting();
		HDC create(HWND hdlg, int cx, int cy, int first_x, int last_x, int first_y, int last_y, COLORREF cr = RGB(0xdd, 0xdd, 0xdd));
		void draw_axis_x_int(HDC hdc, int factor, int interval, char *str, int start_x = 0, int multi = 1, COLORREF cr = RGB(0x88, 0x88, 0x88));
		void draw_axis_y_int(HDC hdc, int factor, int interval, char *str, int start_y = 0, int mutli = 1, COLORREF cr = RGB(0x88, 0x88, 0x88));
		void draw_axis_x_float(HDC hdc, float factor, float interval, char *str, float start_x = 0.0f, float multi = 1.0f, COLORREF cr = RGB(0x88, 0x88, 0x88));
		void draw_axis_y_float(HDC hdc, float factor, float interval, char *str, float start_y = 0.0f, float multi = 1.0f, COLORREF cr = RGB(0x88, 0x88, 0x88));
		void release(HWND hdlg, HDC memdc);
		void set_axis(int my_font_size, int *my_axis_x_offset, int *my_axis_y_offset);

	private:
		HDC hdc;
		int drawing_size[2];
		int begin_offset[2];
		int end_offset[2];
		int full_size[2];
		int old_map_mode;
		int dlg_size[2];
		HBITMAP hbitmap;
		
		int font_size;
		int axis_x_offset[4];
		int axis_y_offset[4];
	};

	class AI_EXPORT_WIN MoveROI
	{
	public:
		MoveROI();
		int  button_down(HWND hdlg, int img_w, int img_h, RECT rt, int mouse_x, int mouse_y);
		void mouse_move(int &start_x, int &start_y, int mouse_x, int mouse_y);
		void button_up(void);
	private:
		float ratio[2];
		float shift[2];
		int start[2];
		int end[2];
		int is_roi_actived;
	};


	//////////////////////////////////////////////////////////////////////////
	AI_EXPORT_WIN HMENU create_menu(HWND hwnd, int id);
	AI_EXPORT_WIN void  create_toolbar(HWND hwnd, const int *bmp, const int *pid, int cnt);
	AI_EXPORT_WIN HWND  create_statusbar(HWND hwnd, const int *sub_w, int cnt, UINT id);

	AI_EXPORT_WIN void  set_icon(HWND hwnd, int large_icon_id, int small_icon_id);
	AI_EXPORT_WIN void  move_window(HWND hrefer, HWND htarget, int offset_x, int offset_y);
	AI_EXPORT_WIN void  get_version(HMODULE hmodule, BOOL show_buildnum, char *ver);
	AI_EXPORT_WIN void  keep_combo_record(HWND hdlg, int idc, char (*list)[256], int list_length);
	AI_EXPORT_WIN void  get_client_pos(HWND hwnd, int idc, RECT &item_rect);
	AI_EXPORT_WIN BOOL  point_in_rect(POINT pt, RECT rt, int offset);

	AI_EXPORT_WIN void  draw_own_button (HWND hdlg, const DRAWITEMSTRUCT *pdis, int img_id, int style);
	AI_EXPORT_WIN void  draw_own_button (HWND hdlg, const DRAWITEMSTRUCT *pdis, int style, int id_release, int id_pressed);

	AI_EXPORT_WIN void  set_dlg_item_int(HWND hdlg, int id, int value);
	AI_EXPORT_WIN int   get_dlg_item_int(HWND hdlg, int id);
	
	AI_EXPORT_WIN void  set_dlg_item_float(HWND hdlg, int id, double value, int decimal_count);
	AI_EXPORT_WIN float get_dlg_item_float(HWND hdlg, int id);
	
	AI_EXPORT_WIN void  set_dlg_item_hex(HWND hdlg, int id, long value, int length);
	AI_EXPORT_WIN long  get_dlg_item_hex(HWND hdlg, int id);
	
	AI_EXPORT_WIN int   parse_hex_str_to_char(char *input, char *out_hex, int out_size);
	AI_EXPORT_WIN int   parse_char_to_hex_str(char *input, int input_size, char *out_str);

	AI_EXPORT_WIN void  uppercase(char *str);
	AI_EXPORT_WIN int   replace_char(char *buf, int buf_size, char dest_char, char new_char);
	AI_EXPORT_WIN int   clear_str(char *buf, int buf_size, char select_char = -1);
	AI_EXPORT_WIN void  get_current_directory( char *path);
	AI_EXPORT_WIN BOOL  search_directory(const char *path, const char *name);
	AI_EXPORT_WIN BOOL  open_file(HWND hdlg, char *path, int type = FILE_TYPE_ALL);
	AI_EXPORT_WIN BOOL  save_file(HWND hdlg, char *path, int type); 
	AI_EXPORT_WIN BOOL  search_file(const char *path, const char *name);
	AI_EXPORT_WIN int   get_sub_folders_name(const char *path, char (*name)[FOLDER_NAME_LENGTH], int limit_cnt);
	AI_EXPORT_WIN int   get_sub_files_name(const char *path, char (*name)[FOLDER_NAME_LENGTH], int limit_cnt, char *rule_name_str);

	AI_EXPORT_WIN int   read_integer_key(const char *path, const char *section, const char *key);
	AI_EXPORT_WIN BOOL  write_integer_key(const char *path, const char *section, const char *key, int new_v);
	
	AI_EXPORT_WIN float read_float_key(const char *path, const char *section, const char *key);
	AI_EXPORT_WIN BOOL  write_float_key(const char *path, const char *section, const char *key, float new_v);
	
	AI_EXPORT_WIN UINT  read_string_key(const char *path, const char *section, const char *key, char *read_buf, unsigned long buf_size);
	AI_EXPORT_WIN BOOL  write_string_key(const char *path, const char *section, const char *key, char *new_str);
	
	AI_EXPORT_WIN void  load_keys(const char *path, const char *section, HWND hdlg, INIEdit *edit, int item_cnt);
	AI_EXPORT_WIN void  save_keys(const char *path, const char *section, HWND hdlg, INIEdit *edit, int item_cnt);

	AI_EXPORT_WIN void  save_log(char *str, char *folder_name, char *log_name, int limit_file_size);

	AI_EXPORT_WIN HANDLE create_csv(const char *filename);
	AI_EXPORT_WIN HANDLE create_define_path_csv(const char *path);
	AI_EXPORT_WIN BOOL   write_csv(HANDLE hfile, const char *format, ...);

	AI_EXPORT_WIN void  delay(int ms);
	AI_EXPORT_WIN void  sleep_ex(int ms);
	AI_EXPORT_WIN float get_cycles_per_second(int interval_time);
}



#endif