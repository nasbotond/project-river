#pragma once
#include <opencv2/opencv.hpp>

namespace OpticalFlow
{
    // void denseOpticalFlow(cv::VideoCapture& inputVideo)
    // {
    //     // cv::VideoCapture capture("../sec07_mot_mpg/motor.avi");
    //     // cv::VideoCapture capture(path);

    //     const std::string NAME = "../optical_flow.avi";
    //     int ex = static_cast<int>(inputVideo.get(cv::CAP_PROP_FOURCC));

    //     // Transform from int to char via Bitwise operators
    //     char EXT[] = {(char)(ex & 0XFF) , (char)((ex & 0XFF00) >> 8),(char)((ex & 0XFF0000) >> 16),(char)((ex & 0XFF000000) >> 24), 0};

    //     cv::Size S = cv::Size((int) inputVideo.get(cv::CAP_PROP_FRAME_WIDTH), (int) inputVideo.get(cv::CAP_PROP_FRAME_HEIGHT));
    //     cv::VideoWriter outputVideo;

    //     outputVideo.open(NAME, ex, inputVideo.get(cv::CAP_PROP_FPS), S, true);

    //     cv::Mat frame1, prvs;
    //     inputVideo >> frame1;
    //     cv::cvtColor(frame1, prvs, cv::COLOR_BGR2GRAY);

    //     while(true)
    //     {
    //         cv::Mat frame2, next, original;
    //         inputVideo >> frame2;
    //         if (frame2.empty())
    //         {
    //             break;
    //         }

    //         cv::cvtColor(frame2, next, cv::COLOR_BGR2GRAY);
    //         cv::Mat flow(prvs.size(), CV_32FC2);
    //         cv::calcOpticalFlowFarneback(prvs, next, flow, 0.5, 3, 15, 3, 5, 1.2, 0);

    //         frame2.copyTo(original);

    //         // By y += 5, x += 5 you can specify the grid 
    //         for (int y = 0; y < original.rows; y += 5)
    //         {
    //             for (int x = 0; x < original.cols; x += 5)
    //             {
    //                 // get the flow from y, x position * 10 for better visibility
    //                 const cv::Point2f flowatxy = flow.at<cv::Point2f>(y, x) * 5;
    //                 // draw line at flow direction
    //                 cv::line(original, cv::Point(x, y), cv::Point(cvRound(x + flowatxy.x), cvRound(y + flowatxy.y)), cv::Scalar(255,0,0));
    //                 // draw initial point
    //                 cv::circle(original, cv::Point(x, y), 1, cv::Scalar(0, 0, 0), -1);
    //             }
    //         }
    //         outputVideo << original;
    //         prvs = next;
    //     }
    // }

    cv::Mat denseOpticalFlowMat(cv::Mat &prev, cv::Mat &next)
    {
        cv::Mat prvs;
        cv::cvtColor(prev, prvs, cv::COLOR_BGR2GRAY);

        cv::Mat nxt, original;

        cvtColor(next, nxt, cv::COLOR_BGR2GRAY);
        cv::Mat flowt(prvs.size(), CV_32FC2);

        calcOpticalFlowFarneback(prvs, nxt, flowt, 0.5, 3, 15, 3, 5, 1.2, 0);
        next.copyTo(original);

        // By y += 5, x += 5 you can specify the grid 
        for (int y = 0; y < original.rows; y += 5)
        {
            for (int x = 0; x < original.cols; x += 5)
            {
                // get the flow from y, x position * 10 for better visibility
                cv::Point2f flowatxy = flowt.at<cv::Point2f>(y, x) * 5;
                // draw line at flow direction
                cv::line(original, cv::Point(x, y), cv::Point(cvRound(x + flowatxy.x), cvRound(y + flowatxy.y)), cv::Scalar(255,0,0));
                // draw initial point
                cv::circle(original, cv::Point(x, y), 1, cv::Scalar(0, 0, 0), -1);
            }
        }
        return original;
    }
}