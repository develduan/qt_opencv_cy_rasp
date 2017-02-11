#include <iostream>
#include <opencv2/opencv.hpp>
#include "cyproc.h"
#include "globalParam.h"
#include "chongya.h"
#include <float.h>
#include <QVector>
#include <QDebug>

using namespace std;
using namespace cv;

/// Resize img's rows and cols


/*
 *  @brief : ���캯����ִ�г�ʼ������
*/
cy::cy()
{
    cyVarInit();
    std::cout<<"This is a cy test!"<<std::endl;
}

/*
 * @brief: ������ʼ��
*/
void cy::cyVarInit()
{
    /// ROI
    CY_ROI_x = 0;	//���������������x
    CY_ROI_y = 0;	//���������������y
    CY_ROI_L = 320;	//������򳤶�L
    CY_ROI_H = 240;	//�������߶�H

    /// CY related
    CY_bw_thresh = 127;		//�Ҷ���ֵ
    CY_r = 20;				// ��װ뾶
    CY_maxStep = 400;		// �������������
    CY_delta = 0;			// ��׼��
    CY_dist = 0;			// ��߼��
    CY_img_invert = 0;		// ��ɫѡ��
}

/*
 * @brief: ������
 * @param:
 *      img: opencv��ֵ��ͼ��
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
	Point rad;
    vout.clear();
    for (int i = 0; i < CY_maxStep; i++)
	{
        /// ����Ƿ������ЧԲ������
        /// �����ڣ���ȡһ�������ꣻ�������ڣ��˳�����ѭ��
        rad = findValueStore(img, IMGBW_BLACK);
        if (rad.x<0 || rad.y<0) break;

        /// ������������
        vout.append(rad);

        /// ��Բ���� (�뾶Ϊ��2r+delta)
		plotc(img, rad, 2*CY_r+CY_delta);
		imgEdge = edgesbw(img);
	}
    ///�õ��㼯��Ч����
    numOfPoints = vout.length();

    ///��ѹ˳���������ص㣩
    vout = pointPixSort(vout);

    ///�����ʾ
    voutf.clear();
    for(int j=0; j<numOfPoints; j++)
    {
        /// ����ӳ�������
        voutf.append(mapPoint(vout[j], Point(img.cols, img.rows), CY_ROI_x, CY_ROI_y, CY_ROI_L, CY_ROI_H));
        cout << voutf[j] << endl;
        /// ��ѹ�����Բ
        cv::circle(img_raw, vout[j], CY_r, cv::Scalar(255, 255, 255), 1);
        /// ˳����ʾ
        std::stringstream ss;
        ss << j+1;
        cv::putText(img_raw, String(ss.str()), vout[j]-cv::Point(CY_r/5.0, -CY_r/5.0), CV_FONT_HERSHEY_COMPLEX, CY_r*2.0/CY_r_Max, Scalar(255, 0, 0), 1);
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
    int imgRows = 320;
    int imgCols = 240;
    double imgFactor = 1.3; // ͼ�����
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
 *  @brief : ���ص�����
 *  @param :
 *          vin : ���ص������
*/
QVector<Point> cy::pointPixSort(QVector<cv::Point> &vin)
{
    if(vin.length()<=2)
        return vin;

    Point startPoint;
    QVector<cv::Point> vout(vin.length());
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
    vin = vout;
    return vin;
}


