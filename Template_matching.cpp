#include <iostream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define CVUI_IMPLEMENTATION
#include "cvui.h"


#define WINDOW_NAME "Controller"
#define WINDOW2_NAME "ROI"

using namespace cv;
using namespace std;

// release 모드로 해야 잘된다.
// CV_WINDOW_AUTOSIZE는 C 버전
// CV_LOAD_IMAGE_COLOR는 IplImage*  용.
// imread할 때 이미지가 프로젝트 파일 안에 있어야 한다.
// x86 -> x64로 바꾸면 또 프로젝트에서 opencv 포함시켜줘야 한다.


int main(int argc, char **arv)
{
	// Read image
	Mat img = imread("pusan.jpg");
	Mat img_cpy;
	Mat img_gray;
	Mat templ;
	int method = TM_CCORR_NORMED;
	float threshold = 0.8;
	
	cvtColor(img, img_gray, COLOR_RGB2GRAY);


	cv::namedWindow(WINDOW_NAME);
	cv::namedWindow(WINDOW2_NAME);

	Mat frame = cv::Mat(img.rows + 120, img.cols + 10  , CV_8UC3);
	cvui::init(WINDOW_NAME);
	cvui::watch(WINDOW2_NAME);

	Point anchor;

	Rect ROI(0, 0, 0, 0);
	Rect ROI2(0, 0, 0, 0);

	while (1) {

		img.copyTo(img_cpy);

		// Select ROI
		cvui::context(WINDOW2_NAME);
		if (cvui::mouse(cvui::DOWN)) {
			// Position the rectangle at the mouse pointer.
			anchor.x = cvui::mouse().x;
			anchor.y = cvui::mouse().y;
			
		}

		// Is any mouse button down (pressed)?
		if (cvui::mouse(cvui::IS_DOWN)) {

			//cvui::printf(img_cpy, anchor.x + 5, anchor.y + 5, 0.3, 0xff0000, "(%d,%d)", anchor.x, anchor.y);

			int mouse_x = cvui::mouse().x;
			int mouse_y = cvui::mouse().y;

			mouse_x = mouse_x < 0 ? 0 : mouse_x;
			mouse_y = mouse_y < 0 ? 0 : mouse_y;

			int width = mouse_x - anchor.x;
			int height = mouse_y - anchor.y;

			ROI.x = width < 0 ? anchor.x + width : anchor.x;
			ROI.y = height < 0 ? anchor.y + height : anchor.y;

			ROI.width = std::abs(width);
			ROI.height = std::abs(height);

			ROI.width = ROI.x + ROI.width > img_cpy.cols ? ROI.width + img_cpy.cols - (ROI.x + ROI.width) : ROI.width;
			ROI.height = ROI.y + ROI.height > img_cpy.rows ? ROI.height + img_cpy.rows - (ROI.y + ROI.height) : ROI.height;


			// Show the rectangle coordinates and size
			cvui::printf(img_cpy, ROI.x + 5, ROI.y + 5, 0.3, 0x0000ff, "(%d,%d)", ROI.x, ROI.y);
			cvui::printf(img_cpy, cvui::mouse().x + 5, cvui::mouse().y + 5, 0.3, 0xff0000, "w:%d, h:%d", ROI.width, ROI.height);
			cv::rectangle(img_cpy, ROI, Scalar(0, 0, 255));
		}




		if (cvui::mouse(cvui::UP)) {
			// We are done working with the ROI.
			ROI2 = ROI;
			templ = img_gray(ROI2);
			
		}
		cv::rectangle(img_cpy, ROI2, Scalar(0, 0, 255));
		cvui::imshow(WINDOW2_NAME, img_cpy);






		cvui::context(WINDOW_NAME);
		frame = Scalar(49, 52, 49);
		cvui::printf(frame, 10, 10, 0.3, 0x00ff00, "Threshold");
		cvui::trackbar(frame, 10, 30, 200, &threshold, (float)0., (float)1.);
		cvui::printf(frame, 250, 10, 0.3, 0x00ff00, "Mode select");
		cvui::trackbar(frame, 250, 30, 200, &method, (int)0, (int)5);
		if(method == 0)
			cvui::printf(frame, 350, 10, 0.3, 0x00ff00, "TM_SQDIFF");
		else if(method == 1)
			cvui::printf(frame, 350, 10, 0.3, 0x00ff00, "TM_SQDIFF_NORMED");
		else if (method == 2)
			cvui::printf(frame, 350, 10, 0.3, 0x00ff00, "TM_CCORR");
		else if (method == 3)
			cvui::printf(frame, 350, 10, 0.3, 0x00ff00, "TM_CCORR_NORMED");
		else if (method == 4)
			cvui::printf(frame, 350, 10, 0.3, 0x00ff00, "TM_CCOEFF");
		else if (method == 5)
			cvui::printf(frame, 350, 10, 0.3, 0x00ff00, "TM_CCOEFF_NORMED");
		cvui::imshow(WINDOW_NAME, frame);


		// Crop image
		
		// if (templ.empty()) 
		//	continue;
	



























		if (!templ.empty()) {

			Mat result;
			int result_cols = img.cols - templ.cols + 1;
			int result_rows = img.rows - templ.rows + 1;
			result.create(result_rows, result_cols, CV_8UC1);


			matchTemplate(img_gray, templ, result, method);
			normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());



			Mat general_mask = Mat::ones(result.rows, result.cols, CV_8UC1);
			Mat template_mask = Mat::zeros(templ.rows, templ.cols, CV_8UC1);
			Mat img_result;
			img.copyTo(img_result);
			int match_num = 0;


			while (1) {

				double minVal; double maxVal; Point minLoc; Point maxLoc;
				Point matchLoc;

				// general mask가 0이 아닌 곳에서만 template matching 시작.
				cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, general_mask);



				if (method == TM_SQDIFF || method == TM_SQDIFF_NORMED)
				{
					matchLoc = minLoc;
					if (minVal > 1-threshold || match_num > 20)
						break;
				}
				else
				{
					matchLoc = maxLoc;
					if (maxVal < threshold || match_num > 20)
						break;
				}


				float k_overlapping = 2.0f;
				int template_w = ceil(k_overlapping * templ.cols);
				int template_h = ceil(k_overlapping * templ.rows);
				int x = matchLoc.x - template_w / 2;
				int y = matchLoc.y - template_h / 2;

				//shrink template-mask size to avoid boundary violation
				if (y < 0) y = 0;
				if (x < 0) x = 0;

				//will template come beyond the mask?:if yes-cut off margin; 
				if (template_w + x > general_mask.cols)
					template_w = general_mask.cols - x;
				if (template_h + y > general_mask.rows)
					template_h = general_mask.rows - y;

				//최고지점 주변을 마스킹 하여 그 근처에서 또 매칭 잡는 일 없게.
				Mat template_mask = Mat::zeros(template_h, template_w, CV_8UC1);
				template_mask.copyTo(general_mask(cv::Rect(x, y, template_w, template_h)));
				//template_mask.copyTo(result(cv::Rect(x, y, template_w, template_h)));


				cv::rectangle(img_result, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
				cv::circle(result, matchLoc, 2, Scalar::all(0), 2, 8, 0);
				match_num++;
			}


			// Display Cropped Image
			//imshow("templ", templ);
			cvtColor(result, result, COLOR_GRAY2RGB);
			result.convertTo(result, CV_8UC3, 255, 0);
			result.copyTo(frame(Rect(frame.cols/2 - result.cols/2 , 100, result.cols, result.rows)));
			
			cv::imshow("result", img_result);
		}





















		cvui::imshow(WINDOW_NAME, frame);
		if (waitKey(20) == 27) {
			break;
		}
		
	}
	return 0;
}