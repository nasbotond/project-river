#include "klt_viewer.hpp"

KLTViewer::KLTViewer(std::vector<cv::Mat>& mats)
{
    originalFrames = mats;
    currentFrame = 0;
    kltFrames = calculateKLT(originalFrames);
    mat = kltFrames.at(currentFrame);
	matViewer = new MatViewer("KLT Video", mat);
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

    // if(ImGui::Button("Calc"))
    // {
    //     calculateKLT(video);
    //     cv::VideoCapture kltCapture = cv::VideoCapture("../klt_points.avi");
    //     video = &kltCapture;
    //     video->set(cv::CAP_PROP_POS_FRAMES, 0);
    //     *video >> mat;
    //     matViewer = new MatViewer("KLT Video", mat);

        
    // }
    matViewer->addToGUI(imageControls);
}

int KLTViewer::getCurrentFrame()
{
    return currentFrame;
}

void KLTViewer::updateFrame(int pyr_size)
{
    kltFrames = calculateKLT(originalFrames);
    mat = kltFrames.at(currentFrame);

	matViewer->update();
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

	matViewer->update();
	return true;
}

int KLTViewer::numFrames()
{
    return kltFrames.size();
}

std::vector<cv::Mat> KLTViewer::calculateKLT(std::vector<cv::Mat>& mats)
{
    std::vector<cv::Mat> result = std::vector<cv::Mat>(mats.size());
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
    old_frame = mats.at(frameIndex);
    frameIndex++;

    cv::cvtColor(old_frame, old_gray, cv::COLOR_BGR2GRAY);
    cv::goodFeaturesToTrack(old_gray, p0, 100, 0.3, 7, cv::Mat(), 7, false, 0.04);

    // Create a mask image for drawing purposes
    cv::Mat mask = cv::Mat::zeros(old_frame.size(), old_frame.type());
    while(frameIndex < mats.size()-1)
    {
        cv::Mat frame, frame_gray;
        frame = mats.at(frameIndex);
        frameIndex++;
        if(frame.empty())
            break;
        cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
        // calculate optical flow
        std::vector<uchar> status;
        std::vector<float> err;
        cv::TermCriteria criteria = cv::TermCriteria((cv::TermCriteria::COUNT) + (cv::TermCriteria::EPS), 10, 0.03);
        calcOpticalFlowPyrLK(old_gray, frame_gray, p0, p1, status, err, cv::Size(15,15), 2, criteria);
        std::vector<cv::Point2f> good_new;
        for(uint i = 0; i < p0.size(); i++)
        {
            // Select good points
            if(status[i] == 1) 
            {
                good_new.push_back(p1[i]);
                // draw the tracks
                cv::line(mask,p1[i], p0[i], colors[i], 2);
                cv::circle(frame, p1[i], 5, colors[i], -1);
            }
        }
        cv::Mat img;
        cv::add(frame, mask, img);

        // Now update the previous frame and previous points
        old_gray = frame_gray.clone();
        p0 = good_new;
        result.at(frameIndex-1) = img;

    }
    return result;
}