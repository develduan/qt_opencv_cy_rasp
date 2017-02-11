#include <iostream>
#include <opencv2/opencv.hpp>
#include "cyproc.h"
#include "globalParam.h"
#include "delay.h"
#include "chongya.h"
#include <cfloat>
#include <string>
#include <QVector>
#include <QDebug>

using namespace std;
using namespace cv;

/// Resize img's rows and cols
int imgRows = 320;
int imgCols = 240;
double imgFactor = 1.3; // ͼ�����

/// ROI
int CY_ROI_x = 0;	//���������������x
int CY_ROI_y = 0;	//���������������y
int CY_ROI_L = 320;	//������򳤶�L
int CY_ROI_H = 240;	//�������߶�H

/// Global parameter definition
int CY_bw_thresh = 127;		//�Ҷ���ֵ
int CY_r = 20;				// ��װ뾶
int CY_maxStep = 400;		// �������������
int CY_delta = 0;			// ��׼��
int CY_dist = 0;			// ��߼��
int CY_img_invert = 0;		// ��ɫѡ��

Point_<double> RadArray[400+1];
Point RadArrayInt[400+1];

/*
 * @brief: main process
 * @param:
 *      img: opencv binary img
*/
cv::Mat cy::chongya(cv::Mat& img)
{
    Mat img_raw,imgEdge;
	unsigned int rows, cols;
    int numOfPoints;

    /// ͼ����Ч�Լ��
	if (img.empty())
	{
		cerr << "Image read error!" << endl;
        return img;
	}
    img.copyTo(img_raw);

    /// ��ʾͼ����Ϣ
	rows = img.rows;
	cols = img.cols;
    cout <<"ROI size(pix): "<< rows << "," << cols << endl;

    /// �õ�ͼ���Ե
	imgEdge = edgesbw(img);

    /// �ӱ߿�߽磬������Ҫ���Ƴ������������
    imgEdge.row(0) = IMGBW_WITHE;
    imgEdge.row(rows-1) = IMGBW_WITHE;
    imgEdge.col(0) = IMGBW_WITHE;
    imgEdge.col(cols - 1) = IMGBW_WITHE;
	
    /// �õ��׸�Բ������
	img = img | circleSub(imgEdge, CY_r+ CY_dist);

    /// ������ѭ���������������㼯��
    int i;
	Point rad;
    for (i = 0; i < CY_maxStep; i++)
	{
        /// ����Ƿ������ЧԲ������
        /// �����ڣ���ȡһ�������ꣻ�������ڣ��˳�����ѭ��
        rad = findValueStore(img, IMGBW_BLACK);
//        rad = findValueStoreS(img, IMGBW_BLACK);
        if (rad.x<0 || rad.y<0) break;

        /// ������������
        RadArrayInt[i] = rad;

        /// ��Բ���� (�뾶Ϊ��2r+delta)
		plotc(img, rad, 2*CY_r+CY_delta);
		imgEdge = edgesbw(img);
	}
    ///��ӽ������־λ,�õ��㼯��Ч����
    RadArray[i] = Point(-1, -1);
    numOfPoints = i;

    ///��ѹ˳���������ص㣩
    pointPixSort(RadArrayInt, numOfPoints);


    ///�����ʾ
    voutf.clear();
    for(int j=0; j<numOfPoints; j++)
    {
        /// ����ӳ�������
        RadArray[j] = mapPoint(RadArrayInt[j], Point(img.cols, img.rows), CY_ROI_x, CY_ROI_y, CY_ROI_L, CY_ROI_H);
        voutf.append(mapPoint(RadArrayInt[j], Point(img.cols, img.rows), CY_ROI_x, CY_ROI_y, CY_ROI_L, CY_ROI_H));
        cout << RadArray[j] << endl;
        /// ��ѹ�����Բ
        cv::circle(img_raw, RadArrayInt[j], CY_r, cv::Scalar(255, 255, 255), 1);
        /// ˳����ʾ
        std::stringstream ss;
        ss << j+1;
        cv::putText(img_raw, String(ss.str()), RadArrayInt[j]-cv::Point(CY_r/5.0, -CY_r/5.0), CV_FONT_HERSHEY_COMPLEX, CY_r*2.0/CY_r_Max, Scalar(255, 0, 0), 1);
    }

    /// �ܸ�����ʾ
    cout << "Total : " << numOfPoints << endl;

    /// ��ʾ�����ͼ��(For debug)
    // imshow("img_raw", img_raw);

    return img_raw;
}


/*
*	@brief :[���ųߴ�],�õ���ֵ��ͼ��
*	@param :
*           imgin: ԭͼ������
*/
cv::Mat cy::resize2bw(cv::Mat& imgin)
{
	Mat imgout(imgin);

#ifdef IMG_RESIZE
	if (imgin.cols > imgCols)
	{
		imgFactor = double(imgin.cols) / imgin.rows;
		imgCols = ceil(imgRows*imgFactor);
		resize(imgin, imgout, Size(imgCols, imgRows));
	}
#endif // IMG_RESIZE

	cvtColor(imgout, imgout, CV_RGB2GRAY);
	threshold(imgout, imgout, CY_bw_thresh, 255, THRESH_BINARY);
	if (CY_img_invert)
		imgout = ~imgout;
	return imgout;
}

/*
*	@brief :ӳ������㣬����������ӳ�䵽ʵ������
*	@param :
*           pin: ��������
*           pinMax: �����������ֵ
*           x, y: ROI��������
*           L, H: ROIʵ�ʳߴ�,��L, ��H
*   @return: ʵ������
*/
cv::Point_<double> cy::mapPoint(cv::Point pin, cv::Point pinMax, double x, double y, double L, double H)
{
	cv::Point_<double> pout;
	pout = Point_<double>(x, y) + Point_<double>(L*double(pin.x)/pinMax.x, H*double(pin.y)/pinMax.y);
	return pout;
}


/*
*	@brief :ӳ��ߴ磬��ʵ�ʳߴ�ӳ�䵽���سߴ�
*	@param :
*           realScale: ʵ�ʳߴ�
*           pinMax: �����������ֵ
*           L, H: ROIʵ�ʳߴ�,��L, ��H
*   @return: ���سߴ�
*/
int cy::mapScale(double realScale, cv::Point pinMax, double L, double H)
{
    int pixScale;
    double scaleFactor;
    scaleFactor = (double(pinMax.x)/L+double(pinMax.y)/H)/2;
    pixScale = realScale*scaleFactor;
    return pixScale;
}


/*
 * @brief: ���Բ������
 *         ѡ��һ����㣬��ʣ�����ѡ�����ξ�������ĵ㣬��Ϊ�������һ������µ���㣬�ظ��˲������㼯��
 * @param:
 *          rawPoint:����㼯
 *          n: �㼯Ԫ�ظ���
 * @note: �˺������ı�����㼯
*/
template <typename T>
T* cy::pointSort(T *rawPoint, int n)
{
    /// Array to QVector
    QVector<T> vin(n);
    qCopy(rawPoint, rawPoint+n, vin.begin());
    QVector<T> vout;

    T startPoint;
    startPoint = vin[0];
    vout.append(startPoint);
    vin.erase(&vin[0]);

    while(vin.length()>0)
    {
        double distMin = INT_MAX;
        int indexDistMin = 0;
        for(int i=0; i<vin.length(); i++)
        {
            double dist = pow(vin[i].x-startPoint.x, 2)+pow(vin[i].y-startPoint.y, 2);
            if(dist<distMin)
            {
                distMin = dist;
                indexDistMin = i;
            }
        }
        startPoint = vin[indexDistMin];  //���ڵ������ĸ��£�����startPoint����ֵ
        vout.append(startPoint);
        vin.erase(&vin[indexDistMin]);
    }
    qCopy(vout.begin(), vout.begin()+vout.length(), rawPoint);
    return rawPoint;
}


QVector<Point> cy::pointPixSort(Point *rawPoint, int n)
{
    /// Array to QVector
    QVector<Point> vin(n);
    qCopy(rawPoint, rawPoint+n, vin.begin());

    Point startPoint;
    startPoint = vin[0];
    vout.clear();
    vout.append(startPoint);
    vin.erase(&vin[0]);

    while(vin.length()>0)
    {
        double distMin = INT_MAX;
        int indexDistMin = 0;
        for(int i=0; i<vin.length(); i++)
        {
            double dist = pow(vin[i].x-startPoint.x, 2)+pow(vin[i].y-startPoint.y, 2);
            if(dist<distMin)
            {
                distMin = dist;
                indexDistMin = i;
            }
        }
        startPoint = vin[indexDistMin];  //���ڵ������ĸ��£�����startPoint����ֵ
        vout.append(startPoint);
        vin.erase(&vin[indexDistMin]);
    }
    qCopy(vout.begin(), vout.begin()+vout.length(), rawPoint);

    return vout;
}

cy::cy()
{
    std::cout<<"This is a cy test!"<<std::endl;
}




/*
%% �㷨ԭ��
%{
#1 �õ�ɨ���ֵͼ
#2 ���հ뾶������ʴ���õ��׸�Բ������
#3 ���Բ�������Ƿ���ڣ������˳�
#4 ��Բ�������Ե���ѡȡһ��P����ӡP������
#5 ��ȥP��Բ�ģ�2r�뾶��Բ���õ��µ�Բ�������ظ�#3, 4, 5����
%}
*/

/*
NOTE :
opencv �е�λ����������0��255�������߼�ֵ����0��1

---���кܶ�����Ż��ĵط�---
1.���ڵĶ�ֵ����ȡ���Ǽ򵥵ĻҶȻ���ֵ������ֵ�����ȣ�̫����һ���ֻ��Ȳ���
	�����ͼ��ת����HSV����YCrCbɫ���ϣ�ͨ������ͨ������ֵ���ж�ֵ�����Դﵽ���ȽϺõķֻ���
	��ɫ��ȡҲ���ô˷�����Ч���е�ƫ��

2.��ֵ������Ҫһϵ�еı�Ե������Ѱ�Ұ׵㣨���ߺڵ㣩�����ڲ�ȡ�򵥵Ĵ�ͷ������������ʵ��Ӧ��
	���Ա�����һ���ҵ��ĵ㣬��Ϊ�˴εı�����ʼ�㣬���Խ���ʱ�临�Ӷ�

3.�ڽ��ж�ֵͼ��Ե��ȡ��ʱ����һ���ر���Ҫע�⣺
	opencv�е���ȡ��Ե�����и�������Ǵ���"����������"
	������ȡ�ı�Ե��ԭͼ�񣨻�ԭͼ���ȡ�������������㣬---��*��
	�õ���ͼ���ٴν��У�*��������ͼ��ĺ�ɫ�����߰�ɫ���᲻�ϵ�����������չ
	���µõ������Ч������㣬��Ӧ������Ϊopencv�ı�Ե��ȡ�����϶�������Ԥ��Ϊ��������õ�
	��Ե�ģ�����д��һ���Լ��ġ��ķ����ֵͼ��Ե��⡯��cv::Mat edgesbw(cv::Mat imgbw)���˷���������


4.���ڵĳ�������Բ��Ϊ��ѹͼ��ģ�����ת��������ת�������κ���������ԭ���ǲ���ģ�����Ҫ�������Ĵ��룬
	���Ǳ�Ե�����Ĵ�����Ҫ�޸ģ�ԭ����ֽ���Ƶ��ϣ�

5.�����������ұ�Ե�Ĵ�����̫�������Ҫ�󣬵������Ҫ�����ĸ���Ե���ǵ������������ֻҪ�ڵõ���ֵͼ���
��ֵͼ�����������ĸ���/�е���ɫ��Ϊ��ɫֵ�����ǲ��ϵ���ɫֵ���Ϳ����ˣ���opencv�м򵥼������Ϳ���ʵ�֡�

6.����ִ��кܶ�Ҫ�Ż��ĵط�������

*/

/*
{
1.ԭʼ��������
2.����s��״·��
3.ŷ����þ��� ->Ŀǰ��ʱ��������
}
*/
