#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std; 

class myBackgroundSubstractor: public cv::BackgroundSubtractorMOG2
	{
	public:
		void setbackgroundRatio(float a){backgroundRatio = a;}
};

void convex_hull (cv::Mat &, cv :: Mat &);

int main () 
{

	cv::Mat image, no_back_image, final_image; 
	cv::VideoCapture cap(0); 
	cap.set(CV_CAP_PROP_BRIGHTNESS,1);

	myBackgroundSubstractor bg; 
	bg.set("nmixtures",3);// set number of gaussian mixtures
	bg.set("detectShadows", false); //if false: turn shadow detection off
	bg.setbackgroundRatio(0.0001);
	vector<vector<cv::Point> > contours;

   if (!cap.isOpened())
        cerr << "Cannot open video device" << endl;


	while (1) 
	{
		cap>>image;
		cvtColor( image, image, CV_BGR2GRAY );
		cv::flip(image,image,1);

		bg.operator ()(image,no_back_image);

		cv::erode(no_back_image,no_back_image,cv::Mat());
        cv::dilate(no_back_image,no_back_image,cv::Mat());
        cv::findContours(no_back_image,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
        cv::drawContours(image,contours,-1,cv::Scalar(0,0,255),2);
		image.copyTo(no_back_image, no_back_image);



//convex_hull		

		convex_hull ( no_back_image, final_image); 
 
		
		//SHOW THE DIFFERENT IMAGES
		cv::imshow ("ORIGINAL IMAGE" , image); 
		cv::imshow ("NO BACKGROUND IMAGE" , no_back_image); 
		cv::imshow ("FINAL IMAGE" , final_image); 


		if (cv::waitKey(1)== 27)		//press ESC to exit camera
			break;          
	} 	                             
   
	return 0;
}



void convex_hull (cv::Mat &src_gray, cv :: Mat & result)
{
	int thresh = 100;
 	int max_thresh = 255;
	int maxelem=0; 

	cv::Mat threshold_output;
	vector<vector<cv::Point> > contours;

	vector<cv::Vec4i> hierarchy;

	/// Detect edges using Threshold
	cv::threshold( src_gray, threshold_output, thresh, 255, cv::THRESH_BINARY );

	/// Find contours
	cv::findContours( threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );


	vector<vector<cv::Point> > apoly(contours.size());
	for( int i = 0; i < contours.size(); i++ )		
		approxPolyDP (cv::Mat(contours[i]), apoly[i], 3, true); 

	double contour_area; 
	for (int i=0; i<apoly.size(); i++)
	{
		contour_area = cv::contourArea(apoly[i]) ;
        if (contour_area > maxelem)
			maxelem=i; 
	}

	vector<cv::Vec4i> defects;
	for (int i=0; i<contours.size(); i++)
	//	convexityDefects(contours[i], apoly[i], defects);

   /// Draw contours + hull results
  	result =cv::Mat::zeros( threshold_output.size(), CV_8UC3 );

	cv:: drawContours( result, apoly, maxelem, cv::Scalar(0,0,255), CV_FILLED, 8, vector<cv::Vec4i>(), 0, cv::Point() );
}
