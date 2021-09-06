#include <iostream>

#include <opencv2/highgui/highgui.hpp>

#include <pupiltracker/PupilTracker.h>
#include <pupiltracker/cvx.h>

void imshowscale(const std::string& name, cv::Mat& m, double scale)
{
    cv::Mat res;
    cv::resize(m, res, cv::Size(0, 0), scale, scale, cv::INTER_AREA);
    cv::imshow(name, res);
}

int main(int argc, char* argv[])
{
    double imgscale;
    if (argc == 2) {
        std::cout << "img scaling parameter " << argv[1] << std::endl;
        imgscale = strtod(argv[1], NULL);
        // return 1;
    }

    // std::cout << "Opening " << argv[1] << std::endl;
    cv::VideoCapture vc(0);
    if (!vc.isOpened()) {
        std::cerr << "Could not open camera" << std::endl;
        return 2;
    }


    cv::Mat m, n;
    while (true)
    {
        vc >> m;
        if (m.empty())
        {
            vc.release();
            vc.open(0);
            if (!vc.isOpened()) {
                std::cerr << "Could not open camera" << std::endl;
                return 2;
            }
            vc >> n;
            cv::resize(n, m, cv::Size(0, 0), imgscale, imgscale, cv::INTER_AREA);
            if (m.empty()) {
                return 1;
            }
        }


        pupiltracker::TrackerParams params;
        params.Radius_Min = 80;
        params.Radius_Max = 150;

        params.CannyBlur = 1.5;
        params.CannyThreshold1 = 20;
        params.CannyThreshold2 = 40;
        params.StarburstPoints = 0;

        params.PercentageInliers = 30;
        params.InlierIterations = 2;
        params.ImageAwareSupport = true;
        params.EarlyTerminationPercentage = 95;
        params.EarlyRejection = true;
        params.Seed = -1;

        pupiltracker::findPupilEllipse_out out;
        pupiltracker::tracker_log log;
        pupiltracker::findPupilEllipse(params, m, out, log); 

        pupiltracker::cvx::cross(m, out.pPupil, 5, pupiltracker::cvx::rgb(255,255,0));
        cv::ellipse(m, out.elPupil, pupiltracker::cvx::rgb(255,0,255));


        // imshowscale("Haar Pupil", out.mHaarPupil, 3);
        // imshowscale("Pupil", out.mPupil, 3);
        // //imshowscale("Thresh Pupil", out.mPupilThresh, 3);
        // imshowscale("Edges", out.mPupilEdges, 3);
        cv::imshow("Result", m);

        if (cv::waitKey(10) != -1)
            break;
    }
}
