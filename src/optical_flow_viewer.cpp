#include "optical_flow_viewer.hpp"

OpticalFlowViewer::OpticalFlowViewer(std::vector<cv::Mat> &mats)
{
    std::cout << mats.size() << std::endl;
    frames = mats;
    mat_curr = frames.at(0);
    mat_next = frames.at(1);
    currentFrame = 0;
    pyr_size = 2;
    window_size = 15;
    iterations = 3;
    denseOpticalFlowMat(mat_curr, mat_next, flow, pyr_size, window_size, iterations);

	matViewer1 = MatViewer("Optical Flow Video", flow);
}

void OpticalFlowViewer::addToGUI(bool manualPlayback, bool imageControls)
{
    if(ImGui::Button("Save to video"))
    {
        denseOpticalFlowToVideo(frames, pyr_size, window_size, iterations);
    }
    static bool loop = false;

    if(manualPlayback)
    {
        int frame = 0;
        frame = getCurrentFrame();
        if (ImGui::SliderInt("Video position", &frame, 0, numFrames()))
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

    if(isPlaying)
    {
        if (!nextFrame())
        {
            if(!loop)isPlaying = false;
            setCurrentFrame(1);
        }
    }

    if(ImGui::SliderInt("Pyramid Levels", &pyr_size, 2, 10))
    {
        updateFrame(pyr_size, window_size, iterations);
    }
    if(ImGui::SliderInt("Window Size", &window_size, 3, 15))
    {
        updateFrame(pyr_size, window_size, iterations);
    }
    if(ImGui::SliderInt("Iterations", &iterations, 3, 15))
    {
        updateFrame(pyr_size, window_size, iterations);
    }

    // matViewer1->addToGUI(imageControls);
    matViewer1.addToGUI(imageControls);
}

int OpticalFlowViewer::getCurrentFrame()
{
    return currentFrame;
}

void OpticalFlowViewer::updateFrame(int pyr_size, int window_size, int iterations)
{
    denseOpticalFlowMat(mat_curr, mat_next, flow, pyr_size, window_size, iterations);

	// matViewer1->update();
    matViewer1.update();
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

    denseOpticalFlowMat(mat_curr, mat_next, flow, pyr_size, window_size, iterations);

	// matViewer1->update();
    matViewer1.update();
	return true;
}

int OpticalFlowViewer::numFrames()
{
    return frames.size();
}

void OpticalFlowViewer::denseOpticalFlowMat(cv::Mat &prev, cv::Mat &next, cv::Mat &flow, int &pyr_size, int &window_size, int &iterations)
{
    cv::Mat prvs;
    cv::cvtColor(prev, prvs, cv::COLOR_BGR2GRAY);

    cv::Mat nxt;
    cv::cvtColor(next, nxt, cv::COLOR_BGR2GRAY);
    cv::Mat flowt(prvs.size(), CV_32FC2);

    calcOpticalFlowFarneback(prvs, nxt, flowt, 0.5, pyr_size, window_size, iterations, 5, 1.1, 0);
    next.copyTo(flow);

    // By y += 5, x += 5 you can specify the grid 
    for (int y = 0; y < flow.rows; y += 5)
    {
        for (int x = 0; x < flow.cols; x += 5)
        {
            // get the flow from y, x position * 10 for better visibility
            cv::Point2f flowatxy = flowt.at<cv::Point2f>(y, x) * 5;
            // draw line at flow direction
            cv::line(flow, cv::Point(x, y), cv::Point(cvRound(x + flowatxy.x), cvRound(y + flowatxy.y)), cv::Scalar(255,0,0));
            // draw initial point
            cv::circle(flow, cv::Point(x, y), 1, cv::Scalar(0, 0, 0), -1);
        }
    }
    // return flow;
}

void OpticalFlowViewer::denseOpticalFlowToVideo(std::vector<cv::Mat> &frames, int &pyr_size, int &window_size, int &iterations)
{
    std::string name_suffix = "_" + std::to_string(pyr_size) + "_" + std::to_string(window_size) + "_" + std::to_string(iterations);
    const std::string NAME = "../example_results/dense" + name_suffix + ".avi";
    int ex = static_cast<int>(cv::VideoWriter::fourcc('M','J','P','G'));

    // Transform from int to char via Bitwise operators
    char EXT[] = {(char)(ex & 0XFF) , (char)((ex & 0XFF00) >> 8),(char)((ex & 0XFF0000) >> 16),(char)((ex & 0XFF000000) >> 24), 0};

    cv::Size S = cv::Size((int) frames.at(0).cols, (int) frames.at(0).rows);
    cv::VideoWriter outputVideo;

    outputVideo.open(NAME, ex, 5, S, true);

    cv::Mat frame1, prvs;
    int frameIndex = 0;
    frame1 = frames.at(frameIndex);
    frameIndex++;
    cv::cvtColor(frame1, prvs, cv::COLOR_BGR2GRAY);

    while(frameIndex < frames.size())
    {
        cv::Mat frame2, nxt, flow;
        frame2 = frames.at(frameIndex);
        frameIndex++;
        if (frame2.empty())
        {
            break;
        }

        cv::cvtColor(frame2, nxt, cv::COLOR_BGR2GRAY);
        cv::Mat flowt(prvs.size(), CV_32FC2);

        calcOpticalFlowFarneback(prvs, nxt, flowt, 0.5, pyr_size, window_size, iterations, 5, 1.1, 0);
        frame2.copyTo(flow);

        // By y += 5, x += 5 you can specify the grid 
        for (int y = 0; y < flow.rows; y += 5)
        {
            for (int x = 0; x < flow.cols; x += 5)
            {
                // get the flow from y, x position * 10 for better visibility
                cv::Point2f flowatxy = flowt.at<cv::Point2f>(y, x) * 5;
                // draw line at flow direction
                cv::line(flow, cv::Point(x, y), cv::Point(cvRound(x + flowatxy.x), cvRound(y + flowatxy.y)), cv::Scalar(255,0,0));
                // draw initial point
                cv::circle(flow, cv::Point(x, y), 1, cv::Scalar(0, 0, 0), -1);
            }
        }
        outputVideo << flow;
        prvs = nxt;
    }
}