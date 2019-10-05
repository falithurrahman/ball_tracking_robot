#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <wiringPi.h>
#include <softPwm.h>

double MapValue(double a0, double a1, double b0, double b1, double a)
{
	return b0 + ((b1 - b0) * ((a - a0) / (a1 - a0)));
}

using namespace cv;
using namespace std;

const int max_value_H = 360 / 2;
const int max_value = 255;

const String window_capture_name = "Video Capture";
const String window_detection_name = "Object Detection";
//int low_H = 8, low_S = 81, low_V = 112;
//int low_H = 9, low_S = 204, low_V = 117;
int low_H = 9, low_S = 88, low_V = 0;
//int high_H = max_value_H, high_S = max_value, high_V = max_value;
int high_H = 22, high_S = 255, high_V = 255;

//bagian PID
#define kp 1
#define ki 0.05
#define kd 0.7
int P, I, D, prev_selisih, outpid, outpidnew;


int main()
{

//-----------------inisialisasi wiringpi
//if (wiringPiSetup () == -1)
//  exit (1) ;
wiringPiSetup () ;

// set pin 7 to PWM
softPwmCreate (7, 0, 255);
pinMode (4, OUTPUT) ;
pinMode (5, OUTPUT) ;
//-----------------

	Mat frame, frame_HSV, frame_threshold, frame_hasil, frame_gray;

	Mat element = getStructuringElement(MORPH_RECT, Size(13, 13), Point(-1, -1));
	Mat element_close = getStructuringElement(MORPH_ELLIPSE, Size(5, 5), Point(-1, -1));
	Mat element_open = getStructuringElement(MORPH_ELLIPSE, Size(5, 5), Size(-1, -1));

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	vector<Rect> boundrect(contours.size());

	VideoCapture capture;
	capture.open(0);
	system("v4l2-ctl -c exposure_auto=1");
	system("v4l2-ctl -c exposure_absolute=100");

	while (true) {
		capture >> frame;
		if (frame.empty())
		{
			break;
		}

		// Convert from BGR to HSV colorspace
		resize(frame, frame, Size(200, 200),INTER_LINEAR);
		cvtColor(frame, frame_HSV, COLOR_BGR2HSV);
		GaussianBlur(frame_HSV, frame_HSV, Size(7, 7), 0);
		// Detect the object based on HSV Range Values
		inRange(frame_HSV, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), frame_threshold);

		//dilate(frame_threshold, frame_hasil, element);
		//dilate(frame_hasil, frame_hasil, element);
		morphologyEx(frame_threshold, frame_hasil, MORPH_OPEN, element_open, Point(-1, -1));
		morphologyEx(frame_hasil, frame_hasil, MORPH_CLOSE, element_close, Point(-1, -1));

		vector< vector<Point> > contours;
		findContours(frame_hasil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE); //find contours  
		//vector<double> areas(contours.size());
		//vector<Rect> boundrect(contours.size());
		
		Mat drawing = Mat::zeros(frame.size(), CV_8UC3);
		line(frame, Point(frame.cols / 2,480 ), Point(frame.cols / 2,0), Scalar(255, 128, 128), 3, 8);
		//find largest contour area  
		for (int i = 0; i < contours.size(); i++)
		{
			//areas[i] = contourArea(Mat(contours[i]));

			drawContours(frame, contours, i, Scalar(0, 128, 255), 2, 8, hierarchy, 0, Point());

			if (contours[i].size() > 100) {

				Moments m = moments(contours[i], true);
				Point p(m.m10 / m.m00, m.m01 / m.m00);
				circle(frame, p, 5, Scalar(128, 0, 0), -1);

				int selisih = (p.x - 100);
				//cout << selisih << endl;

				P = kp * selisih;
				I += ki * selisih;
				D = kd * (selisih - prev_selisih);
				prev_selisih = selisih;
				outpid = P + I + D;
				outpidnew = MapValue(-400, 400, -255, 255, outpid);
				//outpidnew = MapValue(-400, 400, -1024, 1024, outpid);
				//cout << outpidnew << endl;

				//if (outpid < -50) {
				//	//gerak kanan
				//	cout << "kanan" << endl;
				//}
				//else if (-50 < outpid && outpid < 50) {
				//	//diam
				//	cout << "diam" << endl;
				//}
				//else if (outpid > 50) {
				//	//gerak kiri
				//	cout << "kiri" << endl;
				//}

				if (outpidnew < -5) {
					//gerak kanan
					cout << "kanan" << endl;
					int outpidmin = abs(outpidnew);
					if(outpidmin>255){outpidmin=255;}
					digitalWrite (4, LOW) ;
    					digitalWrite (5,  HIGH) ;
					softPwmWrite (7, outpidmin) ;
					//pwmWrite (1, outpidnew);
					cout << outpidmin << endl;
				}
				else if (-5 < outpidnew && outpidnew < 5) {
					//diam
					cout << "diam" << endl;
					digitalWrite (4, LOW) ;
    					digitalWrite (5,  LOW) ;
					softPwmWrite (7, 0) ;
					//cout << 0 << endl;
					cout << outpidnew << endl;
				}
				else if (outpidnew > 5) {
					//gerak kiri
					cout << "kiri" << endl;
					if(outpidnew>255){outpidnew=255;}
					digitalWrite (4, HIGH) ;
    					digitalWrite (5,  LOW) ;
					softPwmWrite (7, outpidnew) ;
					//pwmWrite (1, outpidnew);
					cout << outpidnew << endl;
				}
			}
		}

		// Show the frames
		imshow(window_capture_name, frame);
		//imshow(window_detection_name, frame_hasil);
		//imshow("drawing", drawing);
		char key = (char)waitKey(1);
		if (key == 'q' || key == 27)
		{
			break;
		}
	}
	digitalWrite (4, LOW) ;
	digitalWrite (5,  LOW) ;
	softPwmWrite (7, 0) ;
	return 0;
}
