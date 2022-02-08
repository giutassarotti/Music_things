#include "music_sheet.hpp"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>

#include <iostream>
#include <set>
#include <functional>
#include <cmath>
#include <algorithm>
#include <functional>
#include <numeric>

using namespace cv;

using std::set;
using std::endl;
using std::cout;
using std::string; 
using std::vector;
using std::array;
using music::music_sheet;

struct Box 
{
    Mat image;
    Rect rectangle;

    vector<int> x_proj;
    vector<int> y_proj;
};

struct Box_Comparator 
{
    vector<array<unsigned, 5>> lines;

    bool box_matching(const Box& box)
    {
        for (auto lines: this->lines)
        {
            if (box_inside_lines(box, lines))
            {
                return true; 
            }
        }

        return false;
    }

    inline bool box_inside_lines(const Box& box, const array<unsigned, 5>& lines) const
    {
        return box.rectangle.y <= lines.back() && (box.rectangle.y + box.rectangle.height >= lines.front());
    }

    bool operator()(const Box& lhs, const Box& rhs) const 
    { 
        for (auto l: lines)
        {
            bool l_match = box_inside_lines(lhs, l);
            bool r_match = box_inside_lines(rhs, l);

            if (l_match ^ r_match)
            {
                return lhs.rectangle.y < rhs.rectangle.y;
            }

            if (l_match && r_match)
            {
                return lhs.rectangle.x < rhs.rectangle.x;
            }
        }

        return false;
    }
};

void show_proj(const std::string& title, const vector<int>& histogram)
{
    Mat proj = Mat::zeros(histogram.size(), 1 + *std::max_element(histogram.begin(), histogram.end()), CV_64FC1);

    size_t y = 0;
    for (auto h: histogram)
    {
        line(proj, Point(0, y), Point(h, y), Scalar(255,255,255), 1, 4);
        ++y;
    }

    //Shows the projection
    //TODO usefull for a good image for presentation
    imshow(title.data(), proj);
}

//Print a single histogram 
void print_proj(const vector<int>& histogram)
{
    for (auto h: histogram)
    {
        cout << h << ", ";
    }
};

//Horizontal projection
void horizontal_projection(Mat& img, vector<int>& histogram)
{
    int count, i, j;

    for(i = 0; i < img.rows; i++) 
    {   
        for(count = 0, j = 0; j < img.cols; j++) 
        {
            count += (img.at<uint8_t>(i, j)) ? 0:1;
            //cout << (img.at<uint8_t>(i,j) ? 0:1) << " ";
        }
        //cout << endl;
        histogram.push_back(count);
    }
}

//Vertical projection
void vertical_projection(Mat& img, vector<int>& histogram)
{
    int count, i, j, k;

    for(i=0; i < img.cols; i++) 
    {   
        for(count = 0, j=0; j < img.rows; j++) 
        {
            for(k=0; k < img.channels(); k++) 
            {
                count += (img.at<uint8_t>(j, i, k)) ? 0:1;
            }
        }

        histogram.push_back(count);
    }
}

//Calculates similarity between 2 histograms
auto similarity(const vector<int>& histogram1, const vector<int>& histogram2)
{
    std::function<unsigned(unsigned, unsigned)> square_error = [](unsigned a, unsigned b) 
    {
        auto e = a-b;
        return e*e;
    };
    auto sum = std::transform_reduce(histogram1.begin(), histogram1.end(), histogram2.begin(), 0, std::plus<>(), square_error);
    auto error = std::sqrt(sum / histogram1.size());

    return error;
}

//Removes the staff from the image (a single found pixel line)
void remove_staff(Mat& img, int pixel)
{
    int sum;
    //Parametres for understand who is the staff and who's not near that pixel
    int n=3, m=1;
    for(int i=0; i<img.cols; i++) 
    {
        if(img.at<unsigned char>(pixel, i) == 0) 
        { 
            sum = 0;
            for(int y=-n; y<=n; y++) 
            {
                if(pixel + y > 0 && pixel + y < img.rows) 
                {
                    sum += img.at<unsigned char>(pixel+y, i);                    
                }
            }
            if(sum > 1000) 
            {
                for(int y=-m; y<=m; y++) 
                {
                    if(pixel + y > 0 && pixel + y < img.rows) 
                    {
                        img.at<unsigned char>(pixel+y, i) = 255;
                    }
                } 
            }
        }    
    }
}

//If 2 boxes are touching horizontally
bool boxes_h_touching(const Box& left, const Box& right)
{
    auto x1 = left.rectangle.x + left.rectangle.width;

    //cout << left.rectangle.x << ' ' << x1 << ' ' << right.rectangle.x << ' ' << right.rectangle.width << ' ' << (x1 >= right.rectangle.x && x1 <= (right.rectangle.x + right.rectangle.width)) << '\n';

    return x1 >= right.rectangle.x && left.rectangle.x <= (right.rectangle.x);
}

//If 2 boxes are touching vertically
bool boxes_v_touching(const Box& lowest, const Box& uppest)
{
    auto y1 = uppest.rectangle.y + uppest.rectangle.height;

    //cout << "v " << lowest.rectangle.x << ' ' << lowest.rectangle.y << ' ' << y1 << ' ' << uppest.rectangle.y << ' ' << uppest.rectangle.height << ' ' << (lowest.rectangle.y >= uppest.rectangle.y && y1 >= lowest.rectangle.y) << '\n';

    return lowest.rectangle.y >= uppest.rectangle.y && y1 >= lowest.rectangle.y;
}

//If 2 boxes are touching
bool boxes_touching(const Box& left, const Box& right)
{
    if (boxes_v_touching(left, right) || boxes_v_touching(right, left))
    {
        return boxes_h_touching(left, right);
    }

    return false;
}

//Finds the figures and draw a rectangle on them
vector<Box> find_boxes(Mat boxes_img, const vector<array<unsigned, 5>>& lines)
{
    static RNG rng(12385);
    
    //Final boxes
    Box_Comparator comparator;
    comparator.lines = lines;
    set<Box, Box_Comparator> boxes_set(comparator);
    vector<Box> boxes_tmp, boxes;
    
    //This function finds contours in a binary image.
    vector<vector<Point>> contours; //Each contour is stored as a vector of points
    vector<Vec4i> hierarchy;        //Optional output vector containing information about the image topology.
    findContours(boxes_img, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

    //Input vector of a 2D point, the final boxes and the final small images (resize after the contours function)
    vector<vector<Point>> contours_boxes(contours.size());

    for(int i=1; i<contours.size(); i++) 
    { 
        //approxPolyDP approximates a curve or a polygon with another curve/polygon with less vertices so that the distance between them is less or equal to the specified precision.
        approxPolyDP(Mat(contours[i]), contours_boxes[i], 3, true);
        //boundingRect returns the minimal up-right integer rectangle containing the rotated rectangle
        
        Box element; 
        element.rectangle = boundingRect(Mat(contours_boxes[i]));
        boxes_set.insert(element);
    }

    //Removing elements that are not inside the sheet
    std::copy_if(boxes_set.begin(), boxes_set.end(), std::inserter(boxes_tmp, boxes_tmp.end()), std::bind(&Box_Comparator::box_matching, &comparator, std::placeholders::_1));

    Box last = boxes_tmp.front();
    for (auto& box: boxes_tmp)
    {
        if (boxes_touching(last, box))
        {
            //cout << "touching" << endl;
            auto y1 = std::max(box.rectangle.y + box.rectangle.height, last.rectangle.y + last.rectangle.height);
            auto x1 = std::max(box.rectangle.x + box.rectangle.width, last.rectangle.x + last.rectangle.width);

            last.rectangle.x = std::min(last.rectangle.x, box.rectangle.x);
            last.rectangle.y = std::min(last.rectangle.y, box.rectangle.y);

            last.rectangle.height = y1 - last.rectangle.y;
            last.rectangle.width = x1 - last.rectangle.x;
        }
        else
        {
            last.image = boxes_img(last.rectangle);
            horizontal_projection(last.image, last.x_proj);
            
            //Shows every horizontal projection (of the little boxes)
            //show_proj(std::to_string(boxes.size()), last.x_proj);
            
            vertical_projection(last.image, last.y_proj);

            //Shows every vertical projection (of the little boxes)
            //show_proj(std::to_string(boxes.size()), last.y_proj);

            boxes.push_back(last);
            last = box;
        }
    }

    return boxes;
}

vector<array<unsigned, 5>> find_lines(Mat red_lines_img, Mat nolines_img)
{
    //Projection for find staff's lines
    vector<int> horiz_proj; 
    horizontal_projection(nolines_img, horiz_proj);

    //Single lines' position (1 pixel)
    vector<int> staff_positions;
    
    //The final lines
    //vector<int> lines;
    vector<array<unsigned, 5>> lines;
    int n_group = -1;   //number of 5-group lines
    int n_lines = 0;   //every 5 lines is a group

    //Single line's pixel in the staff
    int lines_pixel;

    //Number of the single pixel lines that make one real line in the staff
    int n_pixel = 0;

    //Last single pixel line's pixel
    int last_line;
    
    //The most length of the longest line
    int max = *max_element(horiz_proj.begin(), horiz_proj.end());

    for(int i=0; i<horiz_proj.size(); i++) 
    {
        //One line is confirmed to be a line if it's at least the max line length/3.5
        if(horiz_proj[i] > max/3.5) 
        {
            //Every line needs to be removed from the image
            remove_staff(nolines_img, i);

            //If it's a new pixel line
            if (n_pixel == 0)
            {
                if (n_lines == 0)
                {
                    lines.emplace_back();
                    n_group++;
                }

                lines_pixel = i; 
                last_line = i;
                n_pixel = 1;
            }

            //If the next pixel line is 1 pixel near
            else if (abs(i-last_line) == 1)
            {
                lines_pixel += i;
                last_line = i;
                n_pixel++;
            }

            //If the line is finished
            else
            {
                //Average pixel in a line (a line made of some pixel lines)
                lines.back()[n_lines] = static_cast<int>(static_cast<double>(lines_pixel)/n_pixel);

                //Prints the average line's pixel
                //cout << n_group << ' ' << n_lines << endl;
                
                //Let's restart and find next line
                if (n_lines == 4)
                {
                    n_lines = 0;
                }
                else
                {
                    n_lines++;
                }

                n_pixel = 0; 

                //Creates the found line for the show (the red lines show)
                line(red_lines_img, Point(0, lines[n_group][n_lines]), Point(nolines_img.size().width, lines[n_group][n_lines]), Scalar(0, 0, 255), 2);
            }
        }
    }
    
    return lines;
}

music_sheet::music_sheet (const std::string& filename)	
{
    //Try to read the original colour source image.
    Mat colour_img = imread(filename.data(), IMREAD_COLOR);
    if(colour_img.empty()) {
        cout << "I can not open it." << filename << endl;
        exit(-1);
    }

    //Converts the image from color version to black and white
    Mat gray_img;
    cvtColor(colour_img, gray_img, COLOR_RGB2GRAY);
    //Shows original b&w image
    //imshow("B&W", gray_img);
    
    //Applies a fixed-level threshold to each array element.
    threshold(gray_img, gray_img, 200, 255, THRESH_TOZERO);
    //Shows modified b&w image
    //imshow("Black and White with adjustments", gray_img);

    //I'll need a copy for a test (finding lines)
    Mat red_lines_img = colour_img.clone();

    //I need an image without lines, without modify the original
    Mat nolines_img = gray_img.clone();

    //Find lines
    vector<array<unsigned, 5>> lines = find_lines(red_lines_img, nolines_img);

    //Shows the image without lines
    imshow("Without lines", nolines_img);

    //Shows the found lines (in red)
    imshow("Red Found Lines", red_lines_img);
    
    vector<Box> boxes = find_boxes(nolines_img, lines);

    Mat boxes_img = nolines_img.clone();
    cvtColor(boxes_img, boxes_img, COLOR_BGR2BGRA);
    size_t i = 0;
    for(auto& box: boxes) 
    {
        //I need many colours for see the different boxes
        //Scalar colour = Scalar(rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255));
        Scalar colour = Scalar(0, 255, 0);
        rectangle(boxes_img, box.rectangle.tl(), box.rectangle.br(), colour, 2, 8, 0);
        
        //Shows the single match
        //imshow(std::to_string(i), boxes_img(box.rectangle));
        ++i;
    }
    //Shows the image with boxes
    imshow("Boxes", boxes_img);

/*
    int i = 0;
    for(auto& box: boxes)
    {
        cout << i << "x ";
        print_proj(box.x_proj);
        cout << endl;
        cout << i << "y ";
        print_proj(box.y_proj);
        cout << endl;
        i++;
    }
*/

    //TODO rimuovere
    waitKey();
}