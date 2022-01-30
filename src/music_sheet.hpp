#ifndef MUSIC_SHEET_HPP
#define MUSIC_SHEET_HPP

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>

#include <iostream>
#include <cmath>
#include <algorithm>

namespace music
{
	class music_sheet
    {
        private:
            horizontal_projection(Mat& img, vector<int>& histo);
            vert_projection(Mat& img, vector<int>& histo);
            remove_staff(Mat& img, int pixel);
            find_boxes(int t, Mat& threshold_output, vector<Rect>& boxes);
        public:
            music_sheet(const char* filename);
    };
}

#endif