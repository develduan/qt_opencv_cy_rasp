#ifndef __GLOBALPARAM_H
#define __GLOBALPARAM_H

#include <opencv2/opencv.hpp>

#define CAM_INDEX 0

#define IMGBW_BLACK 0
#define IMGBW_WITHE 255
#define IMGBW_THRESH_B2W 200

//#define IMG_RESIZE //�ߴ�����Ƿ�����ߴ�  --Not recommend now
#define CY_SHOW_PROC    //��ʾ������� --Can be use for debug


#define CY_bw_thresh_Max 255		// �Ҷ���ֵ���ֵ
#define CY_r_Max 100				// ��װ뾶���ֵ
#define CY_delta_Max 100			// ��׼�����ֵ
#define CY_dist_Max 100				// ��߼�����ֵ
#define CY_img_invert_Max 1		// ��ɫѡ��

#define CY_ROI_x_Max 1000	//���������������x���ֵ
#define CY_ROI_y_Max 1000	//���������������y���ֵ
#define CY_ROI_L_Max 1000	//������򳤶�L���ֵ
#define CY_ROI_H_Max 1000	//�������߶�H���ֵ



/// Global parameter clarations
/// -Definition in chongya.cpp-
extern int CY_bw_thresh;		// �Ҷ���ֵ
extern int CY_r;				// ��װ뾶
extern int CY_maxStep;		// �������������
extern int CY_delta;			// ��׼��
extern int CY_dist;				// ��߼��
extern int CY_img_invert;		// ��ɫѡ��

extern int CY_ROI_x;	//���������������x
extern int CY_ROI_y;	//���������������y
extern int CY_ROI_L;	//������򳤶�L
extern int CY_ROI_H;	//�������߶�H

//extern cv::Point_<double> RadArray[400+1];

#endif // !__GLOBALPARAM_H
