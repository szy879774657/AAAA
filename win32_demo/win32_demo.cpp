#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdio.h>
#include <direct.h>
#include "ai_win.h"
#include "ai_img.h"
#include "resource.h"
#include "s_chart.h"
#include "s_white.h"
#include "FreeImage.h"

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
#pragma comment(lib, "FreeImage.lib")

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
BYTE bmp_large_x[MAX_IMAGE_W*MAX_IMAGE_H] = {0};
BYTE bmp_large_y[MAX_IMAGE_W*MAX_IMAGE_H] = {0};
BYTE bmp_bin[MAX_IMAGE_W*MAX_IMAGE_H] = {0};
BYTE bmp_cut[MAX_IMAGE_W*MAX_IMAGE_H] = {0};
BYTE bmp_cut_left[MAX_IMAGE_W*MAX_IMAGE_H] = {0};
BYTE bmp_cut_right[MAX_IMAGE_W*MAX_IMAGE_H] = {0};
BYTE bmp_cut_test[MAX_IMAGE_W*MAX_IMAGE_H] = {0};

BYTE bmp_cut_left_y[MAX_IMAGE_W*MAX_IMAGE_H] = {0};
BYTE bmp_cut_right_y[MAX_IMAGE_W*MAX_IMAGE_H] = {0};


int x1;
int x2;
int area_width;

int img_w = 0;
int img_h = 0;
int img_ch = 0;
int ref_f ;
int ref_threshold ;

int sub_w = 0;
int sub_h = 0;
int avg = 0;

int shrink_size		= 1;
int scan_distance   = 0;
int scan_threshold  = 0;
int filter_close    = 0;
int close_pattern_x = 0;
int close_pattern_y = 0;

int   start_x = 0;
int   start_y = 0;
int   end_x   = 0;
int   end_y   = 0;
int   particle_x = 0;
int   particle_y = 0;
float particle_low_limit = 0.0f;
float particle_up_limit = 0.0f;
float particle_low_result = 0.0f;
float particle_up_result = 0.0f;

int particle_dark_cnt = 0;
int particle_white_cnt = 0;
int particle_dark_cnt_left = 0;
int particle_white_cnt_left = 0;

int start_x1=0;
int start_y1=0;
int start_x2=0;
int start_y2=0;



/*#define MAX_PARTICLE_CNT	2000*/
typedef struct particle
{
	int x;
	int y;
}PARTICLE;

PARTICLE_POS particle_dark[MAX_PARTICLE_CNT];
PARTICLE_POS particle_white[MAX_PARTICLE_CNT];
PARTICLE_POS particle_dark_all[MAX_PARTICLE_CNT];
PARTICLE_POS particle_white_all[MAX_PARTICLE_CNT];
PARTICLE left_areas[MAX_PARTICLE_CNT];
PARTICLE right_areas[MAX_PARTICLE_CNT];
int draw_particle = FALSE;
#define IMG_RAW								0
#define IMG_BMP								1
#define IMG_JPG								2
#define IMG_PNG								3
#define IMG_TIFF							4
#define IMG_GIF								5
#define IMG_MIPIRAW							6
int parse_normal_img(const char *img_path, BYTE *img_buf, int buf_size, int img_format)
{
	int result = TRUE;
	FREE_IMAGE_FORMAT fif;
	char img_str[20] = {0};
	int img_size = 0;
	switch(img_format)
	{
	case IMG_JPG:	fif = FIF_JPEG; sprintf(img_str, "Jpeg");	  break;
	case IMG_PNG:	fif = FIF_PNG;  sprintf(img_str, "Png");	  break;
	case IMG_TIFF: fif = FIF_TIFF; sprintf(img_str, "Tif/Tiff"); break;
	case IMG_GIF:  fif = FIF_GIF;  sprintf(img_str, "Gif");	  break;
	default: result = FALSE; break;
	}
	if (result == TRUE){
		FreeImage_Initialise();
		FIBITMAP *fimg = FreeImage_Load(fif, img_path);
		BYTE * img = FreeImage_GetBits(fimg);
		img_w  = FreeImage_GetWidth(fimg);
		img_h = FreeImage_GetHeight(fimg);
		img_ch  = FreeImage_GetBPP(fimg)/8;
		img_size = (img_w*img_h*img_ch);

		if (fimg != NULL && img_w>0 && img_h>0 && buf_size>img_size){
			log("Read %s:%s", img_str, img_path);
			log("%s W:%d, H:%d, %dbit", img_str, img_w, img_h, img_ch*8);
			memcpy(img_buf, img, img_size);
		}
		else{
			log("Read %s Fail.", img_str);
		}
		FreeImage_Unload(fimg);
		FreeImage_DeInitialise();
	}

	return result;
}



int particle_proc(BYTE *bmp_y, int img_w, int img_h, float limit_low, float limit_up, 
	float &result_low, float &result_up, int &dark_cnt, int &white_cnt, 
	PARTICLE_POS *particle_dark, PARTICLE_POS *particle_white){
		int index = 0;
		float sum_val = 0;
		float avg_val = 0.000f;
		float target_percent = 0.000f;

		float max_val = 0.000;
		float min_val = 1.000;

		int over_limit = FALSE;

		for (int j=0; j<img_h-STEP_H; j += STEP_H){
			for (int i=0; i<img_w-STEP_W; i += STEP_W){
				index = j*img_w+i;
				sum_val = 0;
				for (int n=0; n<STEP_H; n++){
					for (int m=0; m<STEP_W; m++){
						sum_val += bmp_y[index + n*img_w+m];
					}
				}
				avg_val = (float)sum_val/(STEP_W*STEP_H);
				for (int n=0; n<STEP_H; n++){
					for (int m=0; m<STEP_W; m++){
						target_percent = bmp_y[index + n*img_w+m]/avg_val;
						if (target_percent<min_val)	min_val = target_percent;
						if (target_percent>max_val)	max_val = target_percent;
						if (over_limit == FALSE){
							if (target_percent<limit_low){
								particle_dark[dark_cnt].val = bmp_y[index + n*img_w+m];
								particle_dark[dark_cnt].x = i+m; 
								particle_dark[dark_cnt].y = img_h-1-(j+n);
								dark_cnt++;	
								if (dark_cnt>=MAX_PARTICLE_CNT){
									//return FALSE;
									over_limit = TRUE;
								}
							}
							else if(target_percent>limit_up){
								particle_white[white_cnt].val = bmp_y[index + n*img_w+m];
								particle_white[white_cnt].x = i+m;
								particle_white[white_cnt].y = img_h-1-(j+n);						
								white_cnt++;
								if (white_cnt>=MAX_PARTICLE_CNT){
									//	return FALSE;
									over_limit = TRUE;
								}
							}
						}


					}
				}
			}
		}

		result_low = min_val;
		result_up  = max_val;

		if (over_limit) return FALSE;

		return TRUE;
}






void shrink_image(const BYTE *source, BYTE *image, int cx, int cy, int &sub_w, int &sub_h)
{
	if(shrink_size<1) shrink_size = 1;
	int x = cx/shrink_size;
	int y = cy/shrink_size;
	int offset_x = x%8;
	int offset_y = y%6;
	sub_w = x - offset_x;
	sub_h = y - offset_y;
//	log("cx=%d,cy=%d",sub_h,sub_w);


	int sum = 0;
	log("sum=%d",200+source[(15*shrink_size+5)*cx + 20*shrink_size+3]);
	for (int j=0; j<sub_h; j++){
		for (int i=0; i<sub_w; i++){
			sum = 0;
			for (int m=0; m<shrink_size; m++){
				for (int n=0; n<shrink_size; n++){
					sum += source[(j*shrink_size+m)*cx + i*shrink_size+n];
				}
			}
			image[j*sub_w+i] = sum/(shrink_size*shrink_size);

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
                    //shrink_y_buf, scan_y_buf, sub_w, sub_h, scan_distance, scan_threshold, use_x, use_y
void scan_image(const BYTE *source, BYTE *dest, int cx, int cy, int distance, int threshold, int use_x, int use_y)
{
	int avg_l = 0;
	int avg_r = 0;
	int avg_c = 0;
	int avg_i = 0;
// 	BYTE *bmp_large_x = new BYTE[3*cx,cy];
// 	BYTE *bmp_large_y = new BYTE[cx,3*cy];
	/*BYTE *dest = new BYTE[cx*cy];*/




	log("cx=%d,cy=%d",cx,cy);
	log("sub_w=%d,sub_h=%d",sub_w,sub_h);
	int sum=0;
	for(int i=0;i<cy;i++){
		for(int j=0;j<distance;j++){
			bmp_large_x[i*(cx+2*cx)+j+cx-distance]=source[i*cx];
			bmp_large_x[(i+1)*(cx+2*cx)-cx+j]=source[(i+1)*cx-1];
			sum++;
			
// 			log("source[%d]=%d,%d",(i+1)*cx-1,source[(i+1)*cx-1],sum);
//  			log("bmpx[%d]=%d",(i+1)*(cx+2*cx)-cx+j,bmp_large_x[(i+1)*(cx+2*cx)-cx+j]);
//  			
 			/*log("bmpx[%d]=%d",1025,bmp_large_x[1025]);*/
		}
	}//扩充边界
//	log("step1 done.");

	for(int i=0;i<cy;i++){
		for(int j=0;j<cx;j++){
			bmp_large_x[i*(cx+2*cx)+cx+j]=source[i*cx+j];
		//	log("large_num=%d,source_num=%d",i*(cx+2*cx)+cx+j,i*cx+j);
		}
	}//赋值
//	log("step2 done.");
	if (use_x){
		for (int j=1; j<cy-1; j++){
			for (int i=cx; i<cx+cx; i++){
				if(bmp_large_x[(j-1)*(cx+2*cx)+i-distance]!=0&&bmp_large_x[(j-1)*(cx+2*cx)+i+distance]!=0&&bmp_large_x[(j-1)*(cx+2*cx)+i]!=0){
					avg_l = median(bmp_large_x[(j-1)*(cx+2*cx)+i-distance], bmp_large_x[j*(cx+2*cx)+i-distance], bmp_large_x[(j+1)*(cx+2*cx)+i-distance]);
					avg_r = median(bmp_large_x[(j-1)*(cx+2*cx)+i+distance], bmp_large_x[j*(cx+2*cx)+i+distance], bmp_large_x[(j+1)*(cx+2*cx)+i+distance]);
					avg_c = /*mysum(bmp_large[(j-1)*cx+i],*/ bmp_large_x[j*(cx+2*cx)+i]/*, bmp_large[(j+1)*cx+i])*/;   //IXY

					avg_i = (avg_l+avg_r)-avg_c/*source[j*cx+i]*/*2;

					dest[(j-1)*cx+i-cx] = (avg_i)/*abs(avg_i)*/>=threshold?1/*source[j*cx+i]*/:255;
					

					/*log("dest_num=%d",j*cx+i-cx);*/
					/*if (avg_i>=threshold||avg_i<-threshold)
					{
						dest[(j-1)*cx+i-cx]=255;
					}
					else
						dest[(j-1)*cx+i-cx]=0;*/


				}
			}
		}

	}
//	log("step3 done.");
// 	for(int i=0;i<MAX_IMAGE_H;i++){
// 		for(int j=0;j<MAX_IMAGE_W;j++){
// 			bmp_large[i*MAX_IMAGE_H+j]=0;
// 		}
// 	}//clear

	for(int i=0;i<cx;i++){
		for(int j=0;j<distance;j++){
			bmp_large_y[(j+cy-distance)*cx+i]=source[i];
			bmp_large_y[(j+cy+cy)*cx+i]=source[cx*(cy-1)+i];
		}
	}
	for(int i=0;i<cy;i++){
		for(int j=0;j<cx;j++){
			bmp_large_y[(i+cy)*cx+j]=source[i*cx+j];
		}
	}
//	log("step4 done.");
	if (use_y){
		int avg_b = 0;
		int avg_t = 0;
		int offset = 0;
		int y;

		for (int i=1; i<cx-1; i++){
			for (int j=cy; j<cy-offset+cy; j++){
				if(bmp_large_y[(j-distance)*cx+i]!=0&&bmp_large_y[j*cx+i]!=0&&bmp_large_y[(j+distance)*cx+i]!=0){
					/*log("ok.t=%d,c=%d,b=%d",bmp_large_y[(j-distance)*cx+i],bmp_large_y[j*cx+i],bmp_large_y[(j+distance)*cx+i]);*/

  					avg_b = median(bmp_large_y[(j-distance)*cx+i-1], bmp_large_y[(j-distance)*cx+i], bmp_large_y[(j-distance)*cx+i+1]);
  					avg_t = median(bmp_large_y[(j+distance)*cx+i-1], bmp_large_y[(j+distance)*cx+i], bmp_large_y[(j+distance)*cx+i+1]);		
					avg_c = /*mysum(bmp_large[(j)*cx+i-1], */bmp_large_y[j*cx+i]/*, bmp_large[(j)*cx+i+1])*/;
			//		avg_i = (avg_b+avg_t) - source[j*cx+i]*2;
					avg_i = (avg_b+avg_t)- avg_c*2;
					
					if (dest[(j-cy)*cx+i] != 1){
						dest[(j-cy)*cx+i] =  (avg_i)>=threshold?1:255;
					}
					
					/*if (avg_i>=threshold||avg_i<-threshold)
					{
						dest[(j-cy)*cx+i]=255;
					}
					else
						dest[(j-cy)*cx+i]=0;*/
				}
// 				else
// 					log("err.t=%d,c=%d,b=%d",bmp_large_y[(j-distance)*cx+i],bmp_large_y[j*cx+i],bmp_large_y[(j+distance)*cx+i]);
			}
		}
	}
//	log("scan1 done.");
// 	delete [] bmp_large_x;
// 	delete [] bmp_large_y;
// 	
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

	return avg;



}

void image_cut_y(BYTE *buf,int w,int h,int leftupx,int leftupy,int CutWidth,int CutHeight,BYTE *buf1)
{

	for (int i = 0; i < CutHeight; i++)
	{
		for (int j = 0; j < CutWidth; j++)
		{


			buf1[(i*CutWidth+j)]  =buf[(leftupx+j)+w*(leftupy+i)];


		}		
	}
}

int select_areas(BYTE *buf_y,BYTE *buf_bin,BYTE *buf_cut ,int w, int h,int &x1,int &x2,int &area_width,PARTICLE_POS *particle_dark_all, PARTICLE_POS *particle_white_all)
{
	int test_width;
	int test_height;
	int dark_cnt_all=0;
	int white_cnt_all=0;
	int left_number=0;
	int right_number=0;



	/********************************************************/ //二值化灰度图
	for (int i=0; i<w*h; i++){
		buf_bin[i] = ((buf_y[i]>120)?255:0);
	}
	sprintf(config_path,"%s\\image\\s1.bmp",dir);
	ai_img::save_bmp8_image(config_path,buf_bin,w,h);
	log("save image:%s", config_path);
	/********************************************************/
	for (int j=0;j<w;j++)
	{
		if (buf_bin[j]==255)
		{
			x1=j;
			break;
		}
		
	}
	for (int j=x1;j<w-x1;j++)
	{
		if (buf_bin[j]==0)
		{
			x2=j;
			break;
		}
	}
	if ((x2-x1)%4!=0)
	{
		area_width=x2-x1-((x2-x1)%4);
	}
	else
	{
		area_width=x2-x1;
	}
	if (x1%4!=0)
	{
		x1=x1-(x1%4);
	}
 	if (x2%4!=0)
 	{
 		x2=x2+(x2%4);
 	}
	image_cut_y(buf_y,w,h,x1,0,area_width,h,buf_cut);
	sprintf(config_path,"%s\\image\\s.bmp",dir);
	ai_img::save_bmp8_image(config_path,buf_cut,area_width,h);
	log("save image:%s", config_path);

	image_cut_y(buf_bin,w,h,0,0,x1,h,bmp_cut_left);
	image_cut_y(buf_bin,w,h,x2,0,w-x2,h,bmp_cut_right);
	image_cut_y(buf_y,w,h,0,0,x1,h,bmp_cut_left_y);
	image_cut_y(buf_y,w,h,x2,0,w-x2,h,bmp_cut_right_y);
	sprintf(config_path,"%s\\image\\left.bmp",dir);
	ai_img::save_bmp8_image(config_path,bmp_cut_left,x1,h);
	log("save image:%s", config_path);
	sprintf(config_path,"%s\\image\\right.bmp",dir);
	ai_img::save_bmp8_image(config_path,bmp_cut_right,w-x2,h);
	log("save image:%s", config_path);

	particle_dark_cnt = 0;
	particle_white_cnt = 0;
 	for (int i=0; i<MAX_PARTICLE_CNT; i++){
 		particle_dark[i].x = 0;
 		particle_dark[i].y = 0;
 		particle_dark[i].val = 0;
 
 		particle_white[i].x = 0;
 		particle_white[i].y = 0;
 		particle_white[i].val = 0;
 	}
 
 	    particle_proc(buf_cut, area_width, img_h, particle_low_limit, particle_up_limit,
 		particle_low_result, particle_up_result, particle_dark_cnt, particle_white_cnt, 
 		particle_dark, particle_white);
 	for (int i=0; i<particle_dark_cnt; i++){
 		particle_dark_all[dark_cnt_all].x = particle_dark[i].x+x1;
 		particle_dark_all[dark_cnt_all].y = particle_dark[i].y;
 		dark_cnt_all++;
 	}
 
 	for (int i=0; i<particle_white_cnt; i++){
 		particle_white_all[white_cnt_all].x =particle_white[i].x+x1;
 		particle_white_all[white_cnt_all].y =particle_white[i].y;
 		white_cnt_all++;
 	}
	
 
 	for (int j=0; j<=x1-8; j=j+8)
	{
 		for(int i=0;i<h;i++)
		{
 			if (bmp_cut_left[i*x1+j]==255)
 			{
 				right_areas[right_number].x=j;
 				right_areas[right_number].y=i;
				image_cut_y(bmp_cut_left_y,x1,h,j,i,8,h-2*i,bmp_cut_test);
 				for (int i=0; i<MAX_PARTICLE_CNT; i++){
 					particle_dark[i].x = 0;
 					particle_dark[i].y = 0;
 					particle_dark[i].val = 0;
 
 					particle_white[i].x = 0;
 					particle_white[i].y = 0;
 					particle_white[i].val = 0;
 				}
 				particle_proc(bmp_cut_test, 8, h-2*i, particle_low_limit, particle_up_limit,
 					particle_low_result, particle_up_result, particle_dark_cnt, particle_white_cnt, 
 					particle_dark, particle_white);
 				for (int a=0; a<particle_dark_cnt; a++){
 					particle_dark_all[dark_cnt_all].x = particle_dark[a].x+j;
 					particle_dark_all[dark_cnt_all].y = particle_dark[a].y;
 					dark_cnt_all++;
 				}
 
 				for (int a=0; a<particle_white_cnt; a++){
 					particle_white_all[white_cnt_all].x =particle_white[a].x+j;
 					particle_white_all[white_cnt_all].y =particle_white[a].y;
 					white_cnt_all++;
 				}

				sprintf(config_path,"%s\\image\\%d.bmp",dir,right_number);
				ai_img::save_bmp8_image(config_path,bmp_cut_test, 8, h-2*i);
 				right_number++;
				
				break;
 			}		
 		}
 	}
 
 	log("%d",right_number);
	return 0;
}

void mirror_y(const BYTE *bmp_source, int bmp_w, int bmp_h, BYTE *y_data){

	for(int i=0; i< bmp_h; i++)
		for(int j=0; j< bmp_w; j++)
			y_data[(bmp_h-1-i)*bmp_w+j] = bmp_source[i*bmp_w+j];

}

class MyImg:public ai_img::Output
{
public:

	virtual void draw_on_window(HDC memdc, LPVOID temp);

private:
	HPEN pen, old_pen;

};

void MyImg::draw_on_window(HDC memdc, LPVOID temp)
{
	SetBkMode(memdc, TRANSPARENT);
	SelectObject(memdc, GetStockObject(NULL_BRUSH));
	pen		 = CreatePen(PS_SOLID, 3, RGB(255, 255, 0));
	old_pen  = (HPEN)SelectObject(memdc, pen);

	Rectangle(memdc, start_x, start_y, end_x, end_y);		

	if (draw_particle == TRUE){
		log("start draw on img.");
		int start_draw_x = 0;
		int start_draw_y = 0;
		int box_size = 15;
		DeleteObject(pen);
		SelectObject(memdc, (HPEN)old_pen);
		pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		old_pen  = (HPEN)SelectObject(memdc, pen);
		for (int i=0; i<particle_dark_cnt; i++){
			Rectangle(memdc, start_draw_x+particle_dark_all[i].x - box_size, start_draw_y+particle_dark_all[i].y-box_size,
				start_draw_x+particle_dark_all[i].x+box_size, start_draw_y+particle_dark_all[i].y+box_size);
		}
		pen = CreatePen(PS_SOLID, 1, RGB(255, 255, 0));
		old_pen  = (HPEN)SelectObject(memdc, pen);
		for (int i=0; i<particle_white_cnt; i++){
			Rectangle(memdc, start_draw_x+particle_white_all[i].x - box_size, start_draw_y+particle_white_all[i].y-box_size,
				start_draw_x+particle_white_all[i].x+box_size, start_draw_y+particle_white_all[i].y+box_size);
		}
	}


	DeleteObject(pen);
}

MyImg myimg;


class MainDlg:public ai_win::Dlg
{
public:
	virtual BOOL init(HWND hdlg, HWND hfocus, LPARAM linit);
	virtual void command(HWND hdlg, int id, HWND hctrl, unsigned int code_notify);
	virtual void hscroll(HWND hdlg, HWND hctrl, UINT code, int pos);
/*	virtual void draw_on_window(HDC memdc, LPVOID temp);*/
	virtual void drawrect(HWND hdlg,int x,int y,int w,int h);

private:
	char file_path[MAX_PATH];
	int is_dlg_init_finished;
/*	HPEN pen, old_pen;*/
};

void MainDlg::drawrect(HWND hdlg,int x,int y,int w,int h)
	{

		HDC memdc = GetDC(GetDlgItem(hdlg,IDC_DISPLAY));
		HPEN pen		= CreatePen(PS_SOLID, 2, COLOR_GREEN);
		HPEN old_pen = (HPEN)SelectObject(memdc, pen);
		SelectObject(memdc,GetStockObject(NULL_BRUSH));
		RECT rect;
		GetClientRect(GetDlgItem(hdlg,IDC_DISPLAY),&rect);
		int rw,rh;
		rw=rect.right-rect.left;
		rh=rect.bottom-rect.top;
		x=(rw*x)/img_w;
		y=rh-((rh*y)/img_h)-(rh*h)/img_h;

	

	
	 Rectangle(memdc,x,y,x+w,y+h);
	
	}


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
	is_dlg_init_finished = FALSE;
	happ = hdlg;
	RECT rt;
	GetClientRect(hdlg, &rt);
	MoveWindow(hdlg, 100, 100, rt.right, rt.bottom, FALSE);
	ai_win::set_icon(hdlg, IDI_LOGO, IDI_LOGO);

	img_cvt.init_lib();

	ai_win::get_current_directory(dir);
	sprintf(config_path, "%s\\config.ini", dir);

	shrink_size    = ai_win::read_integer_key(config_path, "blemish", "shrink_size");
	scan_distance  = ai_win::read_integer_key(config_path, "blemish", "scan_distance");
	scan_threshold = ai_win::read_integer_key(config_path, "blemish", "scan_threshold");
	filter_close   = ai_win::read_integer_key(config_path, "blemish", "filter_close");

	close_pattern_x = ai_win::read_integer_key(config_path, "blemish", "close_pattern_x");
	close_pattern_y = ai_win::read_integer_key(config_path, "blemish", "close_pattern_y");

	particle_low_limit = ai_win::read_float_key(config_path, "particle", "particle_low_limit");
	particle_up_limit  = ai_win::read_float_key(config_path, "particle", "particle_up_limit");
	ai_win::set_dlg_item_float(hdlg, IDC_PARTICLE_LOW_LIMIT, particle_low_limit, FALSE);
	ai_win::set_dlg_item_float(hdlg, IDC_PARTICLE_UP_LIMIT,  particle_up_limit,  FALSE);
	ai_win::set_dlg_item_int  (hdlg, IDC_SHRINK_SIZE,        shrink_size);

// 	ai_win::write_float_key(config_path,"particle","particle_low_limit",particle_low_limit);
// 	ai_win::write_float_key(config_path,"particle","particle_up_limit",particle_up_limit);
// 	ai_win::write_integer_key(config_path,"blemish","shrink_size",shrink_size);

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
	is_dlg_init_finished = TRUE;
	return TRUE;
}


void MainDlg::command(HWND hdlg, int id, HWND hctrl, unsigned int code_notify)
{
	switch (id){
	case IDC_OPEN:
		{
			draw_particle = FALSE;
			char path[MAX_PATH] = {0};
			ai_win::open_file(hdlg, path, FILE_TYPE_ALL);
			//parse_normal_img(path, bmp_buf, sizeof(bmp_buf), IMG_JPG);
			ai_img::read_bmp(path, bmp_buf, img_w, img_h, img_ch);
			log("load image path:%s", path);
			log("img w:%d, h:%d", img_w, img_h);
			img_cvt.bmp24_to_y8(bmp_buf, img_w, img_h, NULL, bmp_y_buf);
			//bmp_buf_f=ai_chart::filter_line( bmp_buf, img_w, img_h, n, k);
			/*filter_rvc_line::f_write_line(bmp_buf,bmp_y_buf,img_w,img_h);*/
			img_out.clear_bmp_stream();
			img_out.display(GetDlgItem(hdlg, IDC_DISPLAY), bmp_y_buf, img_w, img_h, 1);	

			select_areas(bmp_y_buf,bmp_bin,bmp_cut,img_w,img_h,x1,x2,area_width,particle_dark_all,particle_white_all);
			log("%d,%d,%d",x1,x2,area_width);
			
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
			shrink_size = ai_win::get_dlg_item_int(hdlg,IDC_SHRINK_SIZE);
			ai_win::write_integer_key(config_path,"blemish","shrink_size",shrink_size);
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
			ref_f = GetDlgItemInt(hdlg,IDC_EDIT1,0,0);
			ref_threshold = GetDlgItemInt(hdlg,IDC_EDIT2,0,0);
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
			ai_img::save_bmp24_image("d:\\expand1.bmp", filter_close_buf, img_w, img_h);
			filter_close_image(scan_y_buf, filter_close_buf, sub_w, sub_h, close_pattern_x, close_pattern_y, filter_close);
		//	filter_rvc_line::f_write_line(filter_close_buf,bmp_y,sub_w,sub_h);
			
			img_out.clear_bmp_stream();
			img_out.display(GetDlgItem(hdlg, IDC_DISPLAY), filter_close_buf, sub_w, sub_h, 3);
//			ai_img::save_bmp24_image("d:\\expand.bmp", filter_close_buf, img_w, img_h);
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

	case IDC_TEST:
		shrink_size = ai_win::get_dlg_item_int(hdlg,IDC_SHRINK_SIZE);
		ai_win::write_integer_key(config_path,"blemish","shrink_size",shrink_size);
		shrink_image(bmp_y_buf, shrink_y_buf, img_w, img_h, sub_w, sub_h);
		//GET_CHECKER(hdlg,IDC_SCAN_DIRECTION_X);
		scan_image(shrink_y_buf, scan_y_buf, sub_w, sub_h, scan_distance, scan_threshold, GET_CHECKER(hdlg,IDC_SCAN_DIRECTION_X), GET_CHECKER(hdlg,IDC_SCAN_DIRECTION_Y));
		avg=filter_close_image(scan_y_buf, filter_close_buf, sub_w, sub_h, close_pattern_x, close_pattern_y, filter_close);
		log("avg=%d",avg);
		img_out.clear_bmp_stream();
		img_out.display(GetDlgItem(hdlg, IDC_DISPLAY), filter_close_buf, sub_w, sub_h, 3);

		break;

	case IDC_PARTICLE:
		
	//	img_cvt.bmp24_to_y8(bmp_buf, img_w, img_h, NULL, bmp_y_buf);
		particle_low_limit = ai_win::get_dlg_item_float(hdlg,IDC_PARTICLE_LOW_LIMIT);
		particle_up_limit  = ai_win::get_dlg_item_float(hdlg,IDC_PARTICLE_UP_LIMIT);
		ai_win::write_float_key(config_path,"particle","particle_low_limit",particle_low_limit);
		ai_win::write_float_key(config_path,"particle","particle_up_limit",particle_up_limit);


// 		ai_win::set_dlg_item_float(hdlg, IDC_PARTICLE_LOW_RESULT, particle_low_result, 2);
// 		ai_win::set_dlg_item_float(hdlg, IDC_PARTICLE_UP_RESULT, particle_up_result, 2);
		log("particle low result:%f ,low cnt=%d,up result:%f ,up cnt=%d",particle_low_result,particle_dark_cnt,particle_up_result,particle_white_cnt);
		draw_particle = TRUE;
		myimg.clear_bmp_stream();
		myimg.display(GetDlgItem(hdlg, IDC_DISPLAY), bmp_buf, img_w,img_h, 3);	

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


