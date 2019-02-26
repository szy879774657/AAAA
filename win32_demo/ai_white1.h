#ifndef _AI_WHITE_H_
#define _AI_WHITE_H_

#ifndef AI_EXPORT_WHITE
#define AI_EXPORT_WHITE	extern "C" __declspec(dllimport)
#endif

#define STEP_W				8
#define STEP_H				6
#define MAX_PARTICLE_CNT	200
typedef struct _tag_particle
{
	int x;
	int y;
	int neighbor_cnt;
	int is_invalid;
	int val;
}PARTICLE_POS;

AI_EXPORT_WHITE int blemish_proc(const BYTE *bmp_y, BYTE *dest, int img_w, int img_h,
	int distance, int threshold, int close_val);

AI_EXPORT_WHITE int particle_proc(BYTE *bmp_y, int img_w, int img_h, float limit_low, float limit_up,
								 float &result_low, float &result_up, int &dark_cnt, int &white_cnt,
								 PARTICLE_POS *particle_dark, PARTICLE_POS *particle_white);
AI_EXPORT_WHITE void oc_proc(BYTE *bmp_y24, int img_w, int img_h, int threshold, int &center_x, int &center_y);
namespace ai_blemish
{
	AI_EXPORT_WHITE void get_sub_image_size(int img_w, int img_h, int divider, int &sub_w, int &sub_h);
	AI_EXPORT_WHITE void shrink_image(const BYTE *bmp_y, BYTE *dest, int img_w, int img_h, int divider, int sub_w, int sub_h);
	AI_EXPORT_WHITE int  median(int x1, int x2, int x3);
	AI_EXPORT_WHITE void scan_image(const BYTE *source, BYTE *dest, int sub_w, int sub_h, int distance, int threshold);
	AI_EXPORT_WHITE int  filter_close_image(const BYTE *source, BYTE *dest, int sub_w, int sub_h,
		int pattern_x, int pattern_y, int close_val);
	AI_EXPORT_WHITE int  close_image_ex(const BYTE *source, BYTE *dest, int sub_w, int sub_h,
		int pattern_x, int pattern_y, int threshold);
	AI_EXPORT_WHITE void combine_image(const BYTE *source, BYTE *dest, int img_w, int img_h, int sub_w, int sub_h);
}

namespace ai_oc
{
	AI_EXPORT_WHITE void equalize_filter(BYTE *enhance_buf, BYTE *bmp_buf, int img_w, int img_h);
	AI_EXPORT_WHITE void median_filter(BYTE *median_buf, BYTE *bmp_buf, int img_w, int img_h);
	AI_EXPORT_WHITE void binary_image(BYTE *bmp_buf, int img_w, int img_h, int threshold);
	AI_EXPORT_WHITE void calc_oc(BYTE *bin_buf, int img_w, int img_h, int &center_x, int &center_y);
}

namespace filter_rvc_line
{
	AI_EXPORT_WHITE void f_line( BYTE *bmp,BYTE *bmp_fy,int img_w, int img_h);
}





#endif