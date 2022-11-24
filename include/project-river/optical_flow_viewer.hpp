#pragma once

#include "mat_viewer.hpp"
#include "imgui.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
// #include "optical_flow.hpp"

class OpticalFlowViewer
{
    public:
        /*
        Binds this VideoViewer to an existing VideoCapture instance.
        */
        OpticalFlowViewer(std::vector<cv::Mat>& mats);

        /*
        Uses ImGui to render an image preview.

        The first parameter, manualPlayback, adds human-readable controls for the playback of the 
        video, including advancing to the next frame and scrubbing through a video feed.

        The second parameter, imageControls, adds display controls like the apparent size of the image
        on the display.
        */
        void addToGUI(bool manualPlayback = true, bool imageControls = true);

        /*Returns the index of the current frame*/
        int getCurrentFrame();
        /*Sets the index of the current frame*/
        void setCurrentFrame(int desiredFrame);
        /*Attempts to advance to the next frame. This is a blocking function and may take time*/
        bool nextFrame();
        /*Returns the number of accessible frames in the video feed*/
        int numFrames();
        /*The current OpenCV image*/
        cv::Mat mat_curr;
        /*The next OpenCV image*/
        cv::Mat mat_next;
        /*The flow Mat to display*/
        cv::Mat flow;
        /*Dense optical flow algorithm*/
        cv::Mat denseOpticalFlowMat(cv::Mat &prev, cv::Mat &next);
        /*KLT optical flow algorithm*/
        cv::Mat KLTOpticalFlow(cv::Mat &prev, cv::Mat &next);
        /*Current frame index*/
        int currentFrame;
        /*Vector of frames as Mats*/
        std::vector<cv::Mat> frames;
        
    private:
        /*This class relies on MatViewer to render the matrix*/
        MatViewer* matViewer1;
        /*True if playing*/
        bool isPlaying = false;
};