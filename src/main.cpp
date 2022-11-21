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
    cv::VideoCapture capture("../sec07_mot_mpg/motor.avi");

    if (!capture.isOpened())
    {
        // error in opening the video input
        std::cerr << "Unable to open file!" << std::endl;
        return 0;
    }

    cv::Mat frame1, prvs;
    capture >> frame1;
    cv::cvtColor(frame1, prvs, cv::COLOR_BGR2GRAY);

    while(true)
    {
        cv::Mat frame2, next, original;
        capture >> frame2;
        if (frame2.empty())
        {
            break;
        }

        cv::cvtColor(frame2, next, cv::COLOR_BGR2GRAY);
        cv::Mat flow(prvs.size(), CV_32FC2);
        cv::calcOpticalFlowFarneback(prvs, next, flow, 0.5, 3, 15, 3, 5, 1.2, 0);

        frame2.copyTo(original);

        // By y += 5, x += 5 you can specify the grid 
        for (int y = 0; y < original.rows; y += 5)
        {
            for (int x = 0; x < original.cols; x += 5)
            {
                // get the flow from y, x position * 10 for better visibility
                const cv::Point2f flowatxy = flow.at<cv::Point2f>(y, x) * 5;
                // draw line at flow direction
                cv::line(original, cv::Point(x, y), cv::Point(cvRound(x + flowatxy.x), cvRound(y + flowatxy.y)), cv::Scalar(255,0,0));
                // draw initial point
                cv::circle(original, cv::Point(x, y), 1, cv::Scalar(0, 0, 0), -1);
            }
        }

        cv::namedWindow("Out", cv::WINDOW_AUTOSIZE);
        cv::imshow("Out", original);

        int keyboard = cv::waitKey(30);
        if (keyboard == 'q' || keyboard == 27)
        {
            break;
        }

        prvs = next;
    }
}