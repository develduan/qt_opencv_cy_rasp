#include "cyproc.h"
#include "globalParam.h"
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;

/*
*	@brief : ��Ե��Բ
*	@note : ����Ϊ��Ե��ֵͼ����ɫΪ��Ե
*
*/
cv::Mat circleSub(cv::Mat& imgbw, int r)
{
    cv::Mat img;
    imgbw.copyTo(img);
    int rows, cols;
    rows = imgbw.rows;
    cols = imgbw.cols;
    int i, j;
    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < cols; j++)
        {
            if (imgbw.at<uchar>(i, j) > IMGBW_THRESH_B2W) //withe area
            {
                cv::Point center = cv::Point(j, i);
                cv::circle(img, center, r, cv::Scalar(255, 255, 255), -1);
            }
        }
    }
    return img;
}

/*
*	@brief : �ķ����ֵͼ��Ե���
*	@note : ��Ե��Ϊ��ɫ������ԭͼ��ɫ��
*			opencv �Դ���Ե������������������
*			����Զ�ֵͼд�˱�Ե��⡣
*/
cv::Mat edgesbw(cv::Mat imgbw)
{
    cv::Mat img;
    imgbw.copyTo(img);
    img = cv::Scalar(IMGBW_BLACK);
    int rows, cols;

    rows = imgbw.rows;
    cols = imgbw.cols;

    int left_index;
    int right_index;
    int up_index;
    int down_index;
    int i, j;
    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < cols; j++)
        {
            if (imgbw.at<uchar>(i, j) > IMGBW_THRESH_B2W)
            {
                //Index boundary check
                left_index = j > 0 ? j - 1 : j;
                right_index = j <cols-1 ? j + 1 : j;
                up_index = i > 0 ? i - 1 : i;
                down_index = i <rows - 1 ? i + 1 : i;
                //Get imgbw edge
                if (imgbw.at<uchar>(i, left_index) < IMGBW_THRESH_B2W \
                    || imgbw.at<uchar>(i, right_index) < IMGBW_THRESH_B2W\
                    || imgbw.at<uchar>(up_index, j) < IMGBW_THRESH_B2W\
                    || imgbw.at<uchar>(down_index, j) < IMGBW_THRESH_B2W)
                {
                    img.at<uchar>(i, j) = IMGBW_WITHE;
                }
            }
        }
    }
    return img;
}

/*
*	@brief : ��Mat������Ѱ�ҵ�һ��valueֵ������������
*	@note : ����Ϊpoint����
*
*/
cv::Point findValue(const cv::Mat &mat, uchar value)
{
    int i;
    cv::Point rad(-1,-1);
    const uchar *p;
    for (i = 0; i < mat.rows; i++)
    {
        const uchar* row = mat.ptr<uchar>(i);
        if ((p=std::find(row, row + mat.cols, value)) != row + mat.cols)
        {
            rad.y = i;
            rad.x = int(p - row);
            return rad;
        }
    }
    return rad;
}

/*
*	@brief : ��Mat������Ѱ�ҵ�һ��valueֵ������������,���Դ�����һ�����rawֵ��Ϊ��һ���Ŀ�ʼ�㣬����ʱ�临�Ӷ�
*	@note : ����Ϊpoint����
*
*/
cv::Point findValueStore(const cv::Mat &mat, uchar value)
{
    static int lastLine = 0;
    cv::Point rad(-1, -1);
    int i;

    const uchar *p;
    for (i = lastLine; i < mat.rows; i++)
    {
        if (i == mat.rows - 1)//find in last line
            lastLine = 0;
        else
            lastLine = i;
        const uchar* row = mat.ptr<uchar>(i);

        if ((p = std::find(row, row + mat.cols, value)) != row + mat.cols)
        {
            rad.y = i;
            rad.x = int(p - row);
            return rad;
        }
    }
    return rad;
}

/*
*	@brief : S��״·��,��Mat������Ѱ�ҵ�һ��valueֵ������������,���Դ�����һ�����rawֵ��Ϊ��һ���Ŀ�ʼ�㣬����ʱ�临�Ӷ�
*	@note : ����Ϊpoint����
*
*/
cv::Point findValueStoreS(const cv::Mat &mat, uchar value)
{
    static int lastLine = 0;
    static cv::Point lastPoint(0, 0);
    cv::Point rad(-1, -1);
    int i;
    static bool last_dir = false;
    bool now_dir = false;

    const uchar *p;
    for (i = lastLine; i < mat.rows; i++)
    {
        if (i == mat.rows - 1)  //find in last line
            lastLine = 0;
        else
            lastLine = i;

        /// Derection control
        if(i == lastPoint.y)
        {
            now_dir = last_dir;
        }
        else
        {
            if(lastPoint.x < mat.cols/2)
                now_dir = false;
            else
                now_dir = true;
        }
        last_dir = now_dir;    //update last_dir

        const uchar* row = mat.ptr<uchar>(i);
        if ((p = findDir(row, row + mat.cols, value, now_dir)) != row + mat.cols)
        {
            rad.y = i;
            rad.x = int(p - row);
            lastPoint = rad;
            return rad;
        }
    }
    return rad;
}

/*
 * @brief: ���������find����
 *         range: [first, last)
*/
template<typename T>
const T* findDir(const T* _first, const T* _last, T val, bool back)
{
    T *first, *last;
    first = (T *)_first;
    last = (T *)_last;

    while (first!=last)
    {
        if(back)
        {
            if (*(last-1)==val)
            return last-1;
            --last;
        }
        else
        {
            if (*first==val)
            return first;
            ++first;
        }
    }
    return _last;
}

/*
*	@brief :��һ��ʵ��Բ
*	@note :
*
*/
cv::Mat plotc(cv::Mat& img, cv::Point rad, int r)
{
    cv::circle(img, rad, r, cv::Scalar(255, 255, 255), -1);
    return img;
}
