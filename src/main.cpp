#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include "opencv2/opencv.hpp"
#include <opencv2/imgproc.hpp>
#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <numeric>
#include <vector>
#include <cmath>
#include <stdint.h>
#include <functional> 

int main(int argc, const char** argv)
{
    // add your file name
    cv::VideoCapture cap("../sec07_mot_mpg/halak1.mpg");
    cv::Mat flow, frame;
    // some faster than mat image container
    cv::UMat  flowUmat, prevgray;
    for (;;)
    {
        bool Is = cap.grab();
        if (Is == false) 
        {
            // if video capture failed
            std::cout << "Video Capture Fail" << std::endl;
            break;
        }
        else 
        {
            cv::Mat img;
            cv::Mat original;
            // capture frame from video file
            cap.retrieve(img, cv::CAP_OPENNI_BGR_IMAGE);
            resize(img, img, cv::Size(640, 480));
            // save original for later
            img.copyTo(original);
            // just make current frame gray
            cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
            // For all optical flow you need a sequence of images.. Or at least 2 of them. Previous
                                    //and current frame
            // if there is no current frame
            // go to this part and fill previous frame
            //else {
            // img.copyTo(prevgray);
            //   }
            // if previous frame is not empty.. There is a picture of previous frame. Do some 
            //optical flow alg. 

            if (prevgray.empty() == false) 
            {
                // calculate optical flow 
                cv::calcOpticalFlowFarneback(prevgray, img, flowUmat, 0.4, 1, 12, 2, 8, 1.2, 0);
                // copy Umat container to standard Mat
                flowUmat.copyTo(flow);
                // By y += 5, x += 5 you can specify the grid 
                for (int y = 0; y < original.rows; y += 5) 
                {
                    for (int x = 0; x < original.cols; x += 5)  
                    {
                        // get the flow from y, x position * 10 for better visibility
                        const cv::Point2f flowatxy = flow.at<cv::Point2f>(y, x) * 10;
                        // draw line at flow direction
                        cv::line(original, cv::Point(x, y), cv::Point(cvRound(x + flowatxy.x), cvRound(y + flowatxy.y)), cv::Scalar(255,0,0));
                        // draw initial point
                        cv::circle(original, cv::Point(x, y), 1, cv::Scalar(0, 0, 0), -1);
                    }
                }
                // draw the results
                cv::namedWindow("prew", cv::WINDOW_AUTOSIZE);
                cv::imshow("prew", original);
                // fill previous image again
                img.copyTo(prevgray);
            }
            else 
            {
                // fill previous image in case prevgray.empty() == true
                img.copyTo(prevgray);
            }
            int key1 = cv::waitKey(20);
        }
    }
}