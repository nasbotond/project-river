#include "klt_viewer.hpp"

KLTViewer::KLTViewer(std::vector<cv::Mat> &mats)
{
    originalFrames = mats;
    currentFrame = 0;
    pyr_size = 2;
    window_size = 21;
    max_corners = 100;
    min_distance = 7;
    block_size = 7;
    // std::vector<cv::Mat> result = std::vector<cv::Mat>(originalFrames.size());
    kltFrames = std::vector<cv::Mat>(originalFrames.size());
    calculateKLT(originalFrames, kltFrames, pyr_size, window_size, max_corners, min_distance, block_size);
    std::cout << kltFrames.at(0).size() << std::endl;
    mat = kltFrames.at(currentFrame);
	matViewer = MatViewer("KLT Video", mat);
}

void KLTViewer::addToGUI(bool manualPlayback, bool imageControls)
{
	static bool loop = false;

	if(manualPlayback)
	{
		int frame = 0;
		frame = getCurrentFrame();
		if(ImGui::SliderInt("Video position", &frame, 0, numFrames()))
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

		if(ImGui::Button("Next frame"))
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

    if(ImGui::SliderInt("Pyramid Levels", &pyr_size, 2, 10))
    {
        updateFrame(pyr_size, window_size, max_corners, min_distance, block_size);
    }
    if(ImGui::SliderInt("Window Size", &window_size, 3, 21))
    {
        updateFrame(pyr_size, window_size, max_corners, min_distance, block_size);
    }
    if(ImGui::SliderInt("Maximum Corners", &max_corners, 80, 120))
    {
        updateFrame(pyr_size, window_size, max_corners, min_distance, block_size);
    }
    if(ImGui::SliderInt("Minimum Distance", &min_distance, 3, 15))
    {
        updateFrame(pyr_size, window_size, max_corners, min_distance, block_size);
    }
    if(ImGui::SliderInt("Block Size", &block_size, 3, 15))
    {
        updateFrame(pyr_size, window_size, max_corners, min_distance, block_size);
    }

    // matViewer->addToGUI(imageControls);
    matViewer.addToGUI(imageControls);
}

int KLTViewer::getCurrentFrame()
{
    return currentFrame;
}

void KLTViewer::updateFrame(int pyr_size, int window_size, int max_corners, int min_distance, int block_size)
{
    calculateKLT(originalFrames, kltFrames, pyr_size, window_size, max_corners, min_distance, block_size);
    mat = kltFrames.at(currentFrame);

	// matViewer->update();
    matViewer.update();
}

void KLTViewer::setCurrentFrame(int desiredFrame)
{
	if(desiredFrame == 0)
	{
        currentFrame = 0;
		nextFrame();
		return;
	}
	desiredFrame--;
    currentFrame = desiredFrame;
	nextFrame();
}

bool KLTViewer::nextFrame()
{
    if(currentFrame >= numFrames()-1)
    {
        return false;
    }

    mat = kltFrames.at(currentFrame);
    currentFrame++;

	if(mat.empty())
	{
		std::cerr << "Mat is empty" << std::endl;
		return false;
	}

	// matViewer->update();
    matViewer.update();
	return true;
}

int KLTViewer::numFrames()
{
    return kltFrames.size();
}

void KLTViewer::calculateKLT(std::vector<cv::Mat> &originalFrames, std::vector<cv::Mat> &result, int &pyr_size, int &window_size, int &max_corners, int &min_distance, int &block_size)
{
    // std::vector<cv::Mat> result = std::vector<cv::Mat>(mats.size());
    // Create some random colors
    std::vector<cv::Scalar> colors;
    cv::RNG rng;
    for(int i = 0; i < 100; i++)
    {
        int r = rng.uniform(0, 256);
        int g = rng.uniform(0, 256);
        int b = rng.uniform(0, 256);
        colors.push_back(cv::Scalar(r,g,b));
    }
    int frameIndex = 0;
    cv::Mat old_frame, old_gray;
    std::vector<cv::Point2f> p0, p1;
    // Take first frame and find corners in it
    old_frame = originalFrames.at(frameIndex);
    frameIndex++;

    cv::cvtColor(old_frame, old_gray, cv::COLOR_BGR2GRAY);
    cv::goodFeaturesToTrack(old_gray, p0, max_corners, 0.1, min_distance, cv::Mat(), block_size, false, 0.04);

    // Create a mask image for drawing purposes
    cv::Mat mask = cv::Mat::zeros(old_frame.size(), old_frame.type());
    while(frameIndex < originalFrames.size()-1)
    {
        cv::Mat frame, frame_gray;
        frame = originalFrames.at(frameIndex);
        frameIndex++;
        if(frame.empty())
            break;
        cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
        cv::Mat frame_copy;
        frame.copyTo(frame_copy);
        // calculate optical flow
        std::vector<uchar> status;
        std::vector<float> err;
        cv::TermCriteria criteria = cv::TermCriteria((cv::TermCriteria::COUNT) + (cv::TermCriteria::EPS), 10, 0.03);
        cv::calcOpticalFlowPyrLK(old_gray, frame_gray, p0, p1, status, err, cv::Size(window_size, window_size), pyr_size, criteria);
        std::vector<cv::Point2f> good_new;
        for(uint i = 0; i < p0.size(); i++)
        {
            // Select good points
            if(status[i] == 1) 
            {
                good_new.push_back(p1[i]);
                // draw the tracks
                // cv::line(mask, p1[i], p0[i], colors[i], 2);
                // cv::circle(frame_copy, p1[i], 5, colors[i], -1);
                cv::circle(frame_copy, p1[i], 4, cv::Scalar(0, 0, 255), -1);
            }
        }
        cv::Mat img;
        cv::add(frame_copy, mask, img);

        // Now update the previous frame and previous points
        old_gray = frame_gray.clone();
        p0 = good_new;
        result.at(frameIndex-2) = img;
    }
    // return result;
}