#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdio.h>
#include <direct.h>
#include "ai_win.h"
#include "ai_img.h"
#include "resource.h"
#include "ai_chart.h"
#include "ai_white.h"

#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#pragma comment(lib, "ai_win.lib")
#pragma comment(lib, "ai_img.lib")
#pragma comment(lib, "ai_white.lib")

ai_img::Convert img_cvt;
ai_img::Output img_out;

HWND happ = NULL;
void log(const char *format, ...);

#define MAX_IMAGE_W		5500
#define MAX_IMAGE_H		4500

char dir[MAX_PATH] = {0};
char config_path[MAX_PATH] = {0};

BYTE bmp_buf[MAX_IMAGE_W*MAX_IMAGE_H*3] = {0};
BYTE bmp_y_buf[MAX_IMAGE_W*MAX_IMAGE_H]		   = {0};
BYTE shrink_y_buf[MAX_IMAGE_W*MAX_IMAGE_H]	   = {0};
BYTE scan_y_buf[MAX_IMAGE_W*MAX_IMAGE_H]	   = {0};
BYTE filter_close_buf[MAX_IMAGE_W*MAX_IMAGE_H*3] = {0};
BYTE bmp_f[MAX_IMAGE_W*MAX_IMAGE_H*3] = {0};
//BYTE bmp_large[MAX_IMAGE_W*MAX_IMAGE_H] = {0};

int img_w = 0;
int img_h = 0;
int img_ch = 0;

int sub_w = 0;
int sub_h = 0;

int scan_distance   = 0;
int scan_threshold  = 0;
int filter_close    = 0;
int close_pattern_x = 0;
int close_pattern_y = 0;

#define SHRINK_SIZE			16


void shrink_image(const BYTE *source, BYTE *image, int cx, int cy, int &sub_w, int &sub_h)
{
	int x = cx/SHRINK_SIZE;
	int y = cy/SHRINK_SIZE;
	int offset_x = x%8;
	int offset_y = y%6;
	sub_w = x - offset_x;
	sub_h = y - offset_y;
//	log("cx=%d,cy=%d",sub_h,sub_w);


	int sum = 0;
	log("sum=%d",200+source[(15*SHRINK_SIZE+5)*cx + 20*SHRINK_SIZE+3]);
	for (int j=0; j<sub_h; j++){
		for (int i=0; i<sub_w; i++){
			sum = 0;
			for (int m=0; m<SHRINK_SIZE; m++){
				for (int n=0; n<SHRINK_SIZE; n++){
					sum += source[(j*SHRINK_SIZE+m)*cx + i*SHRINK_SIZE+n];
				}
			}
			image[j*sub_w+i] = sum/(SHRINK_SIZE*SHRINK_SIZE);

		}
	}
}

int median(int x1, int x2, int x3)
{
	int x[3] = {x1, x2, x3};
	int temp = 0;
	for (int i=0; i<3; i++){
		for (int j=1; j<3; j++){
			if (x[j-1]>x[j]){
				temp = x[j-1];
				x[j-1] = x[j];
				x[j] = temp;
			}
		}
	}

	return x[1];
}

int mysum(int x1, int x2, int x3)
{
	return (x1+x2+x3);
}

void scan_image(const BYTE *source, BYTE *dest, int cx, int cy, int distance, int threshold, int use_x, int use_y)
{
	int avg_l = 0;
	int avg_r = 0;
	int avg_c = 0;
	int avg_i = 0;
	BYTE *bmp_large_x = new BYTE[3*cx,cy];
	BYTE *bmp_large_y = new BYTE[cx,3*cy];
	/*BYTE *dest = new BYTE[cx*cy];*/




	log("cx=%d,cy=%d",cx,cy);
	log("sub_w=%d,sub_h=%d",sub_w,sub_h);

	for(int i=0;i<cy;i++){
		for(int j=0;j<distance;j++){
			bmp_large_x[i*(cx+2*distance)+j]=source[i*cx];
			bmp_large_x[(i+1)*(cx+2*distance)-distance+j]=source[(i+1)*cx-1];

//  			log("bmpx[%d]=%d",i*(cx+2*distance)+j,bmp_large_x[i*(cx+2*distance)+j]);
//  			log("source[%d]=%d",i*cx,source[i*cx]);
 			/*log("bmpx[%d]=%d",1025,bmp_large_x[1025]);*/
		}
	}//À©³ä±ß½ç
	log("step1 done.");

	for(int i=0;i<cy;i++){
		for(int j=0;j<cx;j++){
			bmp_large_x[i*(cy+2*distance)+distance+j]=source[i*cy+j];
		}
	}//¸³Öµ
	log("step2 done.");
	if (use_x){
		for (int j=1; j<cy-1; j++){
			for (int i=distance; i<cx+distance; i++){
				if(bmp_large_x[(j-1)*(cx+2*distance)+i-distance]!=0&&bmp_large_x[(j-1)*(cx+2*distance)+i+distance]!=0&&bmp_large_x[(j-1)*(cx+2*distance)+i]!=0){
					avg_l = median(bmp_large_x[(j-1)*(cx+2*distance)+i-distance], bmp_large_x[j*(cx+2*distance)+i-distance], bmp_large_x[(j+1)*(cx+2*distance)+i-distance]);
					avg_r = median(bmp_large_x[(j-1)*(cx+2*distance)+i+distance], bmp_large_x[j*(cx+2*distance)+i+distance], bmp_large_x[(j+1)*(cx+2*distance)+i+distance]);
					avg_c = /*mysum(bmp_large[(j-1)*cx+i],*/ bmp_large_x[j*(cx+2*distance)+i]/*, bmp_large[(j+1)*cx+i])*/;

					avg_i = (avg_l+avg_r)-avg_c/*source[j*cx+i]*/*2;
					dest[(j-1)*cx+i-distance] =/* avg_i*/abs(avg_i)>=threshold?1/*source[j*cx+i]*/:255;
				}
			}
		}

	}
	log("step3 done.");
// 	for(int i=0;i<MAX_IMAGE_H;i++){
// 		for(int j=0;j<MAX_IMAGE_W;j++){
// 			bmp_large[i*MAX_IMAGE_H+j]=0;
// 		}
// 	}//clear

	for(int i=0;i<cx;i++){
		for(int j=0;j<distance;j++){
			bmp_large_y[j*cx+i]=source[i];
			bmp_large_y[(j+distance+cy)*cx+i]=source[cx*(cy-1)+i];
		}
	}
	for(int i=0;i<cy;i++){
		for(int j=0;j<cx;j++){
			bmp_large_y[(i+distance)*cy+j]=source[i*cy+j];
		}
	}
	log("step4 done.");
	if (use_y){
		int avg_b = 0;
		int avg_t = 0;
		int offset = 0;

		for (int i=1; i<cx-1; i++){
			for (int j=distance; j<cy-offset+distance; j++){
				if(bmp_large_y[(j-distance)*cx+i]!=0&&bmp_large_y[j*cx+i]!=0&&bmp_large_y[(j+distance)*cx+i]!=0){
					log("ok.t=%d,c=%d,b=%d",bmp_large_y[(j-distance)*cx+i],bmp_large_y[j*cx+i],bmp_large_y[(j+distance)*cx+i]);

  					avg_b = median(bmp_large_y[(j-distance)*cx+i-1], bmp_large_y[(j-distance)*cx+i], bmp_large_y[(j-distance)*cx+i+1]);
  					avg_t = median(bmp_large_y[(j+distance)*cx+i-1], bmp_large_y[(j+distance)*cx+i], bmp_large_y[(j+distance)*cx+i+1]);		
					avg_c = /*mysum(bmp_large[(j)*cx+i-1], */bmp_large_y[j*cx+i]/*, bmp_large[(j)*cx+i+1])*/;
			//		avg_i = (avg_b+avg_t) - source[j*cx+i]*2;
					avg_i = (avg_b+avg_t)- avg_c*2;

					if (dest[(j-distance)*cx+i] != 1){
						dest[(j-distance)*cx+i] = (abs(avg_i)>=threshold?1:255);
					}
				}
				else
					log("err.t=%d,c=%d,b=%d",bmp_large_y[(j-distance)*cx+i],bmp_large_y[j*cx+i],bmp_large_y[(j+distance)*cx+i]);
			}
		}
	}
	delete [] bmp_large_x;
	delete [] bmp_large_y;
	
	log("scan done.");
}



int filter_close_image(const BYTE *source, BYTE *dest, int cx, int cy, int pattern_x, int pattern_y, int close_val)
{
	int sum = 0;
	int sub_sum = 0;
	int avg = 0, sub_avg = 0;
	int w= pattern_x;
	int h= pattern_y;

	BYTE *dest_t = new BYTE[cx*cy];

	memset(dest_t, 255, cx*cy);
	int min_val = 255;

	int pixel_count = 0;

	for (int j=h; j<cy-h; j+=h){
		for (int i=w; i<cx-w; i+=w){
			
			pixel_count = 0;
			for (int n=j; n<j+h*2; n++){
				for (int m=i; m<i+w*2; m++){
					if (source[n*cx+m] <= 1) pixel_count++;
				}
			}

			if (pixel_count>close_val){
				for (int n=j; n<j+h*2; n++){
					for (int m=i; m<i+w*2; m++){
						dest_t[n*cx+m] =source[n*cx+m];
					}
				}
			}
		}
	}

	sum = 0;
	for (int i=0; i<cx*cy; i++){
		if (dest_t[i] == 255){
			dest[i*3] = 255;
			dest[i*3+1] = 255;
			dest[i*3+2] = 255;
		}
		else{
			dest[i*3] = 0;
			dest[i*3+1] = 0;
			dest[i*3+2] = 255;
		}
		sum += dest[i*3];
	}
	avg = sum/(cx*cy);
	delete [] dest_t;

	return min_val;
}

class MainDlg:public ai_win::Dlg
{
public:
	virtual BOOL init(HWND hdlg, HWND hfocus, LPARAM linit);
	virtual void command(HWND hdlg, int id, HWND hctrl, unsigned int code_notify);
	virtual void hscroll(HWND hdlg, HWND hctrl, UINT code, int pos);

private:
	char file_path[MAX_PATH];
};

MainDlg main;
BOOL CALLBACK main_dlg_process(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_DLGMSG(hdlg, WM_INITDIALOG, main.init	);
		HANDLE_DLGMSG(hdlg, WM_COMMAND,	   main.command	);
		HANDLE_DLGMSG(hdlg, WM_HSCROLL,	   main.hscroll );
		HANDLE_DLGMSG(hdlg, WM_CLOSE,	   main.close	);
		break;

	}

	return FALSE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	if (DialogBox(hInstance, (LPCTSTR)IDD_MAIN, NULL, main_dlg_process) == -1){//bug
		MessageBox(NULL, TEXT("Program init fail."), TEXT("Warning"), 0);
		return 1;
	}

	return 0;
}

BOOL MainDlg::init(HWND hdlg, HWND hfocus, LPARAM linit)
{
	happ = hdlg;
	RECT rt;
	GetClientRect(hdlg, &rt);
	MoveWindow(hdlg, 100, 100, rt.right, rt.bottom, FALSE);
	ai_win::set_icon(hdlg, IDI_LOGO, IDI_LOGO);

	img_cvt.init_lib();

	ai_win::get_current_directory(dir);
	sprintf(config_path, "%s\\config.ini", dir);

	scan_distance = ai_win::read_integer_key(config_path, "blemish", "scan_distance");
	scan_threshold = ai_win::read_integer_key(config_path, "blemish", "scan_threshold");
	filter_close   = ai_win::read_integer_key(config_path, "blemish", "filter_close");

	close_pattern_x = ai_win::read_integer_key(config_path, "blemish", "close_pattern_x");
	close_pattern_y = ai_win::read_integer_key(config_path, "blemish", "close_pattern_y");

	SET_SLIDER_RANGE(hdlg, IDC_SLIDER_SCAN_DISTANCE, 0, 50);
	SET_SLIDER_POS(hdlg, IDC_SLIDER_SCAN_DISTANCE, scan_distance);
	SetDlgItemInt(hdlg, IDC_SCAN_DISTANCE_VAL, scan_distance, FALSE);

	SET_SLIDER_RANGE(hdlg, IDC_SLIDER_SCAN_THRESHOLD, 1, 50);
	SET_SLIDER_POS(hdlg, IDC_SLIDER_SCAN_THRESHOLD, scan_threshold);
	SetDlgItemInt(hdlg, IDC_SCAN_THRESHOLD_VAL, scan_threshold, FALSE);

	SET_SLIDER_RANGE(hdlg, IDC_SLIDER_FILTER_CLOSE, 1, 255);
	SET_SLIDER_POS(hdlg, IDC_SLIDER_FILTER_CLOSE, filter_close);
	SetDlgItemInt(hdlg, IDC_FILTER_CLOSE_VAL, filter_close, FALSE);

	SET_CHECKER(hdlg, IDC_SCAN_DIRECTION_X, TRUE);
	SET_CHECKER(hdlg, IDC_SCAN_DIRECTION_Y, TRUE);

	return TRUE;
}


void MainDlg::command(HWND hdlg, int id, HWND hctrl, unsigned int code_notify)
{
	switch (id){
	case IDC_OPEN:
		{
			char path[MAX_PATH] = {0};
			ai_win::open_file(hdlg, path, FILE_TYPE_BMP_ONLY);
			ai_img::read_bmp(path, bmp_buf, img_w, img_h, img_ch);
			log("load image path:%s", path);
			log("img w:%d, h:%d", img_w, img_h);
			img_cvt.bmp24_to_y8(bmp_buf, img_w, img_h, NULL, bmp_y_buf);
			//bmp_buf_f=ai_chart::filter_line( bmp_buf, img_w, img_h, n, k);
			/*filter_rvc_line::f_write_line(bmp_buf,bmp_y_buf,img_w,img_h);*/
			img_out.clear_bmp_stream();
			img_out.display(GetDlgItem(hdlg, IDC_DISPLAY), bmp_y_buf, img_w, img_h, 1);	
		}
		break;
	case IDC_SHOW_IMG:
		{
			img_out.clear_bmp_stream();
			img_out.display(GetDlgItem(hdlg, IDC_DISPLAY), bmp_f, img_w*2, img_h*2, 1);
// 			for(int i=0;i<MAX_IMAGE_H;i++){
// 				for(int j=0;j<MAX_IMAGE_W;j++){
// 					bmp_large[i*MAX_IMAGE_H+j]=0;
// 				}
// 			}
		}

	case IDC_SHRINK:
		{
			sub_w = img_w;
			sub_h = img_h;
			memcpy(shrink_y_buf, bmp_y_buf, sub_w*sub_h);
			shrink_image(bmp_y_buf, shrink_y_buf, img_w, img_h, sub_w, sub_h);
			log("sub_w=%d, sub_h=%d", sub_w, sub_h);
			img_out.clear_bmp_stream();
			img_out.display(GetDlgItem(hdlg, IDC_DISPLAY), shrink_y_buf, sub_w, sub_h, 1);	
		//	ai_img::save_bmp8_image("d:\\1.bmp", shrink_y_buf, sub_w, sub_h);
		}
		break;

	case IDC_SCAN:
		{
			
			int use_x = GET_CHECKER(hdlg, IDC_SCAN_DIRECTION_X);
			int use_y = GET_CHECKER(hdlg, IDC_SCAN_DIRECTION_Y);
// 			for(int i=0;i<sub_h;i++){
// 				for(int j=0;j<sub_w;j++){
// 					for(int k=0;k<use_x;k++){
// 						for(int m=0;m<use_y;m++){
// 							bmp_large[]=shrink_y_buf[]
// 						}
// 					}
// 				}
// 			}

			log("scan_distance:%d, threshold:%d", scan_distance, scan_threshold);
			memset(scan_y_buf, 255, sub_w*sub_h);
			scan_image(shrink_y_buf, scan_y_buf, sub_w, sub_h, scan_distance, scan_threshold, use_x, use_y);
			img_out.clear_bmp_stream();
			img_out.display(GetDlgItem(hdlg, IDC_DISPLAY), scan_y_buf, sub_w, sub_h, 1);	
		}
		break;

	case IDC_FILTER:
		{
			BYTE *bmp_y=new BYTE[img_w*img_h];
			log("close_pattern_x:%d, y:%d, close_val=%d", close_pattern_x, close_pattern_y, filter_close);
			filter_close_image(scan_y_buf, filter_close_buf, sub_w, sub_h, close_pattern_x, close_pattern_y, filter_close);
		//	filter_rvc_line::f_write_line(filter_close_buf,bmp_y,sub_w,sub_h);
			filter_rvc_line::f_read_line(bmp_buf,bmp_f,sub_w,sub_h);

			img_out.clear_bmp_stream();
			img_out.display(GetDlgItem(hdlg, IDC_DISPLAY), bmp_f, sub_w, sub_h, 3);
			delete[] bmp_y;
		}
		break;

	case IDC_SAVE_IMG:
		{
		//	filter_rvc_line::f_read_line(filter_close_buf,bmp_f,sub_w,sub_h);
			SYSTEMTIME st;
			GetLocalTime(&st);
			char mypath[MAX_PATH] = {0};
			sprintf(mypath, "%s\\%2d%2d%2d%3d.bmp", dir, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
			ai_img::save_bmp24_image(mypath, bmp_f, sub_w, sub_h);
			log("save image:%s", mypath);


		}

		break;
	}

	switch (code_notify)
	{
	case LBN_DBLCLK:
		if (hctrl == GetDlgItem(hdlg, IDC_LOG)){
			LISTBOX_RESET(hdlg, IDC_LOG);
		}
		break;
	}
}

void MainDlg::hscroll(HWND hdlg, HWND hctrl, UINT code, int pos)
{

	int x = 0;
	int value = 0;
	if (hctrl == GetDlgItem(hdlg, IDC_SLIDER_SCAN_DISTANCE)){
		value = GET_SLIDER_POS(hdlg, IDC_SLIDER_SCAN_DISTANCE);
		SetDlgItemInt(hdlg, IDC_SCAN_DISTANCE_VAL, value, FALSE);
		scan_distance = value;
		ai_win::write_integer_key(config_path, "blemish", "scan_distance", scan_distance);
		log("write scan_distance=%d", scan_distance);

		x = ai_win::read_integer_key(config_path, "blemish", "scan_distance");
		log("x=%d", x);

		memset(scan_y_buf, 255, sub_w*sub_h);
		int use_x = GET_CHECKER(hdlg, IDC_SCAN_DIRECTION_X);
		int use_y = GET_CHECKER(hdlg, IDC_SCAN_DIRECTION_Y);
		log("scan_distance:%d, threshold:%d", scan_distance, scan_threshold);
		scan_image(shrink_y_buf, scan_y_buf, sub_w, sub_h, scan_distance, scan_threshold, use_x, use_y);
// 		for(int i=0;i<MAX_IMAGE_H;i++){
// 			for(int j=0;j<MAX_IMAGE_W;j++){
// 				bmp_large[i*MAX_IMAGE_H+j]=0;
// 			}
// 		}
		img_out.clear_bmp_stream();
		img_out.display(GetDlgItem(hdlg, IDC_DISPLAY), scan_y_buf, sub_w, sub_h, 1);
	}
	else if (hctrl == GetDlgItem(hdlg, IDC_SLIDER_SCAN_THRESHOLD)){
		value = GET_SLIDER_POS(hdlg, IDC_SLIDER_SCAN_THRESHOLD);
		SetDlgItemInt(hdlg, IDC_SCAN_THRESHOLD_VAL, value, FALSE);
		scan_threshold = value;
		ai_win::write_integer_key(config_path, "blemish", "scan_threshold", scan_threshold);

		memset(scan_y_buf, 255, sub_w*sub_h);
		int use_x = GET_CHECKER(hdlg, IDC_SCAN_DIRECTION_X);
		int use_y = GET_CHECKER(hdlg, IDC_SCAN_DIRECTION_Y);
		log("scan_distance:%d, threshold:%d", scan_distance, scan_threshold);
		scan_image(shrink_y_buf, scan_y_buf, sub_w, sub_h, scan_distance, scan_threshold, use_x, use_y);
		img_out.clear_bmp_stream();
		img_out.display(GetDlgItem(hdlg, IDC_DISPLAY), scan_y_buf, sub_w, sub_h, 1);
	}
	else if (hctrl == GetDlgItem(hdlg, IDC_SLIDER_FILTER_CLOSE)){
		value = GET_SLIDER_POS(hdlg, IDC_SLIDER_FILTER_CLOSE);
		SetDlgItemInt(hdlg, IDC_FILTER_CLOSE_VAL, value, FALSE);
		filter_close = value;
		ai_win::write_integer_key(config_path, "blemish", "filter_close", filter_close);

		filter_close_image(scan_y_buf, filter_close_buf, sub_w, sub_h, close_pattern_x, close_pattern_y, filter_close);
		img_out.clear_bmp_stream();
		img_out.display(GetDlgItem(hdlg, IDC_DISPLAY), filter_close_buf, sub_w, sub_h, 3);
	}
}


void log(const char *format, ...)
{
	HWND hlog = GetDlgItem(happ, IDC_LOG);

	int n = COMBO_GETCOUNT(hlog, IDC_LOG);
	//		if (n>100) COMBO_RESET(hlog, IDC_LOG);
	char temp[1024] = {0};
	char log_str[1024] = {0};
	va_list args;
	va_start(args,format);

	vsprintf(temp,format,args);
	sprintf(log_str, "> %s", temp);
	int index =SendMessage(hlog, LB_ADDSTRING, 0, (LPARAM) log_str);
	SendMessage(hlog, LB_SETCURSEL, index, 0);
	ai_win::save_log(log_str, "log", "log", 512);
}


