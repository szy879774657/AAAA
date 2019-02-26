#ifndef _AI_IMG_H_
#define _AI_IMG_H_

#ifndef AI_EXPORT_IMG 
#define AI_EXPORT_IMG __declspec(dllimport) 
#endif

#define HIGH_CLIP(x,limit)		( ((x)>(limit)) ? (limit):(x) )
#define LOW_CLIP(x,limit)		( ((x)<(limit)) ? (limit):(x) )
#define CLIP(x,l,h)				HIGH_CLIP(LOW_CLIP(x,l),h)

//////////////////////////////////////////////////////////////////////////
#define PIXEL_R(x,y,w,h) ((((h)-(y)-1)*(w)+(x))*3+2)
#define PIXEL_G(x,y,w,h) ((((h)-(y)-1)*(w)+(x))*3+1)
#define PIXEL_B(x,y,w,h) ((((h)-(y)-1)*(w)+(x))*3)
#define PIXEL_Y(x,y,w,h) (((h)-(y)-1)*(w)+(x))

#define B_CHANNEL			0
#define G_CHANNEL			1
#define R_CHANNEL			2

#define YUV_OUTPUT_YCBYCR	1
#define YUV_OUTPUT_YCRYCB	2
#define YUV_OUTPUT_CBYCRY	3
#define YUV_OUTPUT_CRYCBY	4

#define RAW_OUTPUT_BGGR		1
#define RAW_OUTPUT_RGGB		2
#define RAW_OUTPUT_GBRG		3
#define RAW_OUTPUT_GRBG		4

#define IMG_TYPE_RAW						0
#define IMG_TYPE_BMP						1
#define IMG_TYPE_JPG						2
#define IMG_TYPE_PNG						3
#define IMG_TYPE_TIFF						4
#define IMG_TYPE_GIF						5
#define IMG_TYPE_MIPIRAW					6

#define RAW_YUV								0
#define RAW_8BIT							1
#define RAW_10BIT							2
#define RAW_12BIT							3
#define RAW_MIPI10BIT						4

#define COLOR_BLACK				RGB(0,     0,   0)
#define COLOR_WHITE				RGB(255, 255, 255)
#define COLOR_RED				RGB(255,   0,   0)
#define COLOR_GREEN				RGB(0,   255,   0)
#define COLOR_BLUE				RGB(0,     0, 255)
#define COLOR_DODGER_BLUE		RGB( 30, 144, 255)
#define COLOR_MEDIUM_STATE_BLUE	RGB(123, 104, 238)
#define COLOR_MAGENTA			RGB(255,   0, 255)
#define COLOR_YELLOW_GREEN		RGB(154, 205,  50)
#define COLOR_TOMATO			RGB(255,  99,  71)
#define COLOR_GHOST_WHITE		RGB(248, 248, 255)
#define COLOR_LIGHT_DARK		RGB(68,   68,  68)
#define COLOR_YELLOW			RGB(255, 255,   0)
#define COLOR_DARK_ORANGE		RGB(255, 140,   0)
#define COLOR_SADDLE_BROWN		RGB(139,  69,  19)

#define COLOR_LIME_GREEN		RGB( 50, 205,  50)
#define COLOR_DEEP_PINK			RGB(255,  20, 147)
#define COLOR_FIRE_BRICK		RGB(178,  34,  34)
#define COLOR_GOLD				RGB(255, 215,   0)
#define COLOR_PLUM				RGB(255, 215,   0)

typedef struct _tagImgProp
{
	int format;
	int sub_type;
	int width;
	int height;
	int pixel_order;
}ImgProp;

namespace ai_img
{
	class AI_EXPORT_IMG Convert
	{
	public:
		Convert();
		~Convert();
		BOOL init_lib();
		void bmp24_to_y8(const BYTE *bmp_source, int bmp_w, int bmp_h, RECT *y_rect, BYTE *y_data);
		void yuv422_to_bmp24(int data_order, BYTE *yuv, BYTE *bmp, int img_w, int img_h);
		void raw8_to_bmp24_y(int data_order, BYTE *raw8, BYTE *bmp_y, int img_w, int img_h);

	private:
		BYTE *table_y;
		BYTE *table_r;
		BYTE *table_g;
		BYTE *table_b;
		int is_table_inited;

		void  create_yuv_table(void);	
		void  create_rgb_table(void);
	};

	class AI_EXPORT_IMG Output
	{
	public:
		Output();
		~Output();
		virtual void draw_on_window(HDC memdc, LPVOID temp){return;}
		int display(HWND hwnd, const BYTE *bmp_source, int img_w, int img_h, int img_ch=3, int start_x=0, int start_y=0, int is_full_sized=FALSE, int stretch_mode=HALFTONE, LPVOID temp=NULL);
		BOOL save_dib_to_bmp(const char *path, int img_w, int img_h);
		void clear_bmp_stream(void);
		HBITMAP get_hbitmap(void);

		static void draw_crossline(HDC memdc, int center_x, int center_y, int width, int height);
		static void draw_simple_rect(HDC memdc, int center_x, int center_y, int width, int height);

	protected:
		BOOL	is_memory_built;
		HBITMAP hbitmap;
		BYTE *  bi_buf;
	};

	AI_EXPORT_IMG void raw8_to_bmp24_2x2(int data_order, BYTE *raw8, BYTE *bmp, int img_w, int img_h);
	AI_EXPORT_IMG void raw8_to_bmp24_3x3(int data_order, BYTE *raw8, BYTE *bmp, int img_w, int img_h);

	AI_EXPORT_IMG void mipi10_to_raw8(BYTE *mipi10, BYTE *raw8, int img_w, int img_h);
	AI_EXPORT_IMG void mipi10_to_raw10(BYTE *mipi10, BYTE *raw10, int img_w, int img_h);
	AI_EXPORT_IMG void mipi12_to_raw12(BYTE *mipi12, BYTE *raw12, int img_w, int img_h);
	AI_EXPORT_IMG void raw10_to_raw8(BYTE *raw10, BYTE *raw8, int img_w, int img_h);
	AI_EXPORT_IMG void raw12_to_raw8(BYTE *raw12, BYTE *raw8, int img_w, int img_h);

	AI_EXPORT_IMG float get_bmp_roi_avg_val(const BYTE *bmp_source, int bmp_w, int bmp_h, RECT *roi, int ch = B_CHANNEL);
	AI_EXPORT_IMG BOOL  get_bmp_roi_avg_val(const BYTE *bmp_source, int bmp_w, int bmp_h, RECT *roi, float *avg_b, float *avg_g, float *avg_r);
	AI_EXPORT_IMG BOOL  get_bmp_roi_info(const BYTE *bmp_source, int bmp_w, int bmp_h, RECT *roi, float *avg, float *min, float *max);
	
	AI_EXPORT_IMG BOOL save_bmp24_image(const char *img_path, BYTE *bmp, int img_w, int img_h);
	AI_EXPORT_IMG BOOL save_bmp8_image(const char *img_path, BYTE *bmp, int img_w, int img_h);
	AI_EXPORT_IMG BOOL save_raw_image(const char *raw_path, BYTE *raw, int img_w, int img_h, int pixel_width);
	AI_EXPORT_IMG BOOL save_drawing_image(HWND hdlg, char *img_path);

	AI_EXPORT_IMG BOOL  read_bmp(const char *bmp_path, BYTE *bmp_source, int &bmp_w, int &bmp_h, int &bmp_channel);
	AI_EXPORT_IMG float read_raw(const char *raw_path, BYTE *raw_source, int img_w, int img_h, int header = 0, BYTE *h_buf=NULL);

	AI_EXPORT_IMG void  roi_column_to_row(BYTE *roi_data, int roi_w, int roi_h);

	AI_EXPORT_IMG BYTE *get_bmp24_pointer(BYTE *bmp, int img_w, int img_h, int x, int y);

	AI_EXPORT_IMG void get_bmp24_avg_yuv(BYTE *bmp, int img_w, int img_h, float &y, float &u, float &v, RECT *roi = NULL);
	AI_EXPORT_IMG void get_bmp24_avg_lab(const BYTE *bmp, int img_w, int img_h, double &l, double &a, double &b, RECT *roi = NULL);
	AI_EXPORT_IMG void rgb_to_lab(int r, int g, int b, double &lab_l, double &lab_a, double &lab_b);
	AI_EXPORT_IMG void rgb_to_yuv(int r, int g, int b, float &y, float &u, float &v);


}

#endif