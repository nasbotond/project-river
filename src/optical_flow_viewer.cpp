#include "optical_flow_viewer.hpp"

OpticalFlowViewer::OpticalFlowViewer(std::vector<cv::Mat>& mats)
{
	// video = &videoCapture;
    // video->set(CAP_PROP_POS_FRAMES, 1);
    // cout << (int)video->get(CAP_PROP_POS_FRAMES) << endl;
	// *video >> mat_curr;
    // cout << (int)video->get(CAP_PROP_POS_FRAMES) << endl;
    // *video >> mat_next;
    frames = mats;
    mat_curr = frames.at(0);
    mat_next = frames.at(1);
    currentFrame = 0;

    flow = denseOpticalFlowMat(mat_curr, mat_next);
    std::cout << "worked" << std::endl;

	matViewer1 = new MatViewer("Optical Flow Video", flow);
}

void OpticalFlowViewer::addToGUI(bool manualPlayback, bool imageControls)
{
    static bool loop = false;

    if(manualPlayback)
    {
        int frame = 0;
        frame = getCurrentFrame();
        if (ImGui::SliderInt("Video position", &frame, 0, numFrames()-1))
        {
            isPlaying = false;
            setCurrentFrame(frame);
        }

        ImGui::Checkbox("Loop", &loop);
        if(isPlaying)
        {
            if (ImGui::Button("Stop"))
            {
                isPlaying = false;
            }
        }
        else if(ImGui::Button("Play"))
        {
            isPlaying = true;
        }

        if (ImGui::Button("Next frame"))
        {
            nextFrame();
        }
    }

    if (isPlaying)
    {
        if (!nextFrame())
        {
            if(!loop)isPlaying = false;
            setCurrentFrame(1);
        }
    }
    matViewer1->addToGUI(imageControls);
}

int OpticalFlowViewer::getCurrentFrame()
{
    return currentFrame;
}

void OpticalFlowViewer::setCurrentFrame(int desiredFrame)
{
	if (desiredFrame == 0)
	{
        currentFrame = 0;
		nextFrame();
		return;
	}
	desiredFrame--;
    currentFrame = desiredFrame;
	nextFrame();
}

bool OpticalFlowViewer::nextFrame()
{
    if(currentFrame >= numFrames()-1)
    {
        return false;
    }

    mat_curr = frames.at(currentFrame);
    mat_next = frames.at(currentFrame+1);
    currentFrame++;

	if(mat_curr.empty() || mat_next.empty())
	{
		std::cerr << "Mat is empty" << std::endl;
		return false;
	}
    // *video >> mat_next;    
    flow = denseOpticalFlowMat(mat_curr, mat_next);

	matViewer1->update();
	return true;
}

int OpticalFlowViewer::numFrames()
{
	// return (int)video->get(CAP_PROP_FRAME_COUNT);
    return frames.size();
}

cv::Mat OpticalFlowViewer::denseOpticalFlowMat(cv::Mat &prev, cv::Mat &next)
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
