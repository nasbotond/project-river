#pragma once

#include "mat_viewer.hpp"
#include "imgui.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

class KLTViewer
{
    public:
    /*
    Binds this VideoViewer to an existing VideoCapture instance.
    */
    KLTViewer(std::vector<cv::Mat> &mats);

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
    cv::Mat mat;

    void calculateKLT(std::vector<cv::Mat> &originalFrames, std::vector<cv::Mat> &result, int &pyr_size, int &window_size, int &max_corners, int &min_distance, int &block_size);
    /*Current frame index*/
    int currentFrame;
    /*Vector of frames as Mats*/
    std::vector<cv::Mat> originalFrames;
    /*Vector of frames as Mats*/
    std::vector<cv::Mat> kltFrames;
    // int algorithm;
    void updateFrame(int pyr_size, int window_size, int max_corners, int min_distance, int block_size);
    
private:
    /*This class relies on MatViewer to render the matrix*/
    MatViewer matViewer;
    /*True if playing*/
    bool isPlaying = false;

    int pyr_size;
    int window_size;
    int max_corners;
    int min_distance;
    int block_size;
};