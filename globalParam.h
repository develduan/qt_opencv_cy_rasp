#ifndef __GLOBALPARAM_H
#define __GLOBALPARAM_H

#include <opencv2/opencv.hpp>

#define CAM_INDEX 0             //���indexֵ
#define IMGBW_BLACK 0           //opencv��ɫֵ
#define IMGBW_WITHE 255         //opencv��ɫֵ
#define IMGBW_THRESH_B2W 200    //�ķ����ֵͼ��Ե�����ֵ

//#define IMG_RESIZE      //�ߴ�����Ƿ�����ߴ�  --Not recommend now

#define CY_bw_thresh_Max 255		// �Ҷ���ֵ���ֵ
#define CY_r_Max 100				// ��װ뾶���ֵ
#define CY_delta_Max 100			// ��׼�����ֵ
#define CY_dist_Max 100				// ��߼�����ֵ
#define CY_img_invert_Max 1         // ��ɫѡ�����ֵ

#define CY_ROI_x_Max 1000	//���������������x���ֵ
#define CY_ROI_y_Max 1000	//���������������y���ֵ
#define CY_ROI_L_Max 1000	//������򳤶�L���ֵ
#define CY_ROI_H_Max 1000	//�������߶�H���ֵ

#endif // !__GLOBALPARAM_H
