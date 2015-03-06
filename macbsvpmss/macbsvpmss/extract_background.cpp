﻿/*
	计算背景的函数，基本引用自高云晖师兄的毕业设计代码
	计算公式：backImage = (1-learningRate)*background + learningRate*gray;
*/
#include "macbsvpmss.h"

const int g_adj = 50;
char g_savepath[] = "F:\\img_avg_gray_";

//input[]代表视频的绝对路径字符串，k用于第K幅生成文件命名
//返回值是IplIamge*存储类型的背景图
IplImage* ExtractBackground(char input[],int k)
{

	/**********************************************************	

	实现功能：从视频中截取连续的多帧转化为灰度图像然后求平均值	

	*/
	CvCapture* capture = cvCreateFileCapture(input);	//读入视频（运行前须保证该路径下存在该名称的视频，否则会报错）

	IplImage* frame = NULL;
	IplImage* frame_gray = NULL;

	CvMat* frame_grayMat = NULL;
	CvMat* img_avgMat = NULL;
	//所截取的帧的起始和终止位置，当所设定用于求平均的帧数较大时程序运行时间可能会较长
	int start = g_adj;
	cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, start);	//将视频帧的位置调到所设的位置
	int end = cvGetCaptureProperty(capture,CV_CAP_PROP_FRAME_COUNT) - g_adj;
	int count = start;

	while (cvGrabFrame(capture) && (count <= end))
	{
		frame = cvRetrieveFrame(capture);	//获取当前帧

		if (frame_gray == NULL)
		{
			frame_gray = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
			cvZero(frame_gray);
		}
		cvCvtColor(frame, frame_gray, CV_BGR2GRAY);		//将当前帧转成灰度图

		if (frame_grayMat == NULL)
		{
			frame_grayMat = cvCreateMat(frame -> height, frame -> width, CV_32FC1);
			cvZero(frame_grayMat);
		}
		cvConvert(frame_gray, frame_grayMat);		//将灰度图转化为32位浮点数元素的矩阵格式以方便计算

		if (img_avgMat == NULL)
		{
			img_avgMat = cvCreateMat(frame -> height, frame -> width, CV_32FC1);
			cvZero(img_avgMat);
		}
		float learning = 1.0 / (float)(end - start + 1);
		cvAddWeighted(img_avgMat, 1-learning, frame_grayMat, learning, 0, img_avgMat);	//将灰度图按平均数的权值进行叠加，即求平均

		count++;

	}

	IplImage* img_avg = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);	//将灰度图从矩阵格式转回图像格式
	cvConvert(img_avgMat, img_avg);		
	// 显示该灰度图 
	cvNamedWindow("img_avg_gray", CV_WINDOW_AUTOSIZE);
	cvShowImage("img_avg_gray", img_avg);
	//保存该灰度图 
	char savename[50];
	sprintf(savename, "%s%d%d%s%d%s", g_savepath, k,start, "_to_", end, ".png");
	cvSaveImage(savename, img_avg);

	cvWaitKey(0);	//按任意键结束程序

	cvReleaseCapture(&capture);
	cvDestroyAllWindows();
	return img_avg;
}