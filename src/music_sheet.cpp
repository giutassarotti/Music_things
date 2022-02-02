#include "music_sheet.hpp"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>

#include <iostream>
#include <cmath>
#include <algorithm>

using namespace cv;

using std::endl;
using std::cout;
using std::string; 
using std::vector;
using std::array;
using music::music_sheet;

struct Boxe 
{
    Mat image;
    Rect rectangle;
};

void horizontal_projection(Mat& img, vector<int>& histo)
{
    Mat proj = Mat::zeros(img.rows, img.cols, img.type());
    int count, i, j;

    for(i = 0; i < img.rows; i++) 
    {   
        for(count = 0, j = 0; j < img.cols; j++) 
        {
            count += (img.at<int>(i, j)) ? 0:1;
        }

        histo.push_back(count);
        //line(proj, Point(0, i), Point(count, i), Scalar(255,255,255), 1, 4);
    }

    //Shows the projection
    //TODO usefull for a good image for presentation
    //imshow("o projection", proj);
}

//Vertical projection
void vert_projection(Mat& img, vector<int>& histo)
{
    Mat proj = Mat::zeros(img.rows, img.cols, img.type());
    int count, i, j, k;

    for(i=0; i < img.cols; i++) 
    {   
        for(count = 0, j=0; j < img.rows; j++) 
        {
            for(k=0; k < img.channels(); k++) 
            {
                count += (img.at<int>(j, i, k)) ? 0:1;
            }
        }

        histo.push_back(count);
        //line(proj, Point(i, 0), Point(i, count/img.channels()), Scalar(255,255,255), 1, 4);
    }

    //imshow("v projection", proj);
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

//Finds the figures and draw a rectangle on them
vector<Boxe> find_boxes(Mat boxes_img)
{
    static RNG rng(12385);
    
    //Final boxes
    vector<Boxe> boxes;
    
    //This function finds contours in a binary image.
    vector<vector<Point>> contours; //Each contour is stored as a vector of points
    vector<Vec4i> hierarchy;        //Optional output vector containing information about the image topology.
    findContours(boxes_img, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

    //Input vector of a 2D point, the final boxes and the final small images (resize after the contours function)
    vector<vector<Point>> contours_boxes(contours.size());
    boxes.resize(contours.size());

    for(int i=0; i<contours.size(); i++) 
    { 
        //approxPolyDP approximates a curve or a polygon with another curve/polygon with less vertices so that the distance between them is less or equal to the specified precision.
        approxPolyDP(Mat(contours[i]), contours_boxes[i], 3, true);
        //boundingRect returns the minimal up-right integer rectangle containing the rotated rectangle
        
        boxes.emplace_back();
        boxes[i].rectangle = boundingRect(Mat(contours_boxes[i]));
        boxes[i].image = boxes_img(boxes[i].rectangle);
    }

    for(int i=0; i<contours.size(); i++) 
    {
        //I need many colours for see the different boxes
        Scalar colour = Scalar(rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255));
        rectangle(boxes_img, boxes[i].rectangle.tl(), boxes[i].rectangle.br(), colour, 2, 8, 0);
        
        //Shows the single match
        imshow(std::to_string(i).data(), boxes_img(boxes[i].rectangle));
    }

    //Shows the image with boxes
    imshow("Boxes", boxes_img);

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
                //line(red_lines_img, Point(0, lines[n_group][n_lines]), Point(nolines_img.size().width, lines[n_group][n_lines]), Scalar(0, 0, 255), 2);
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
    //imshow("Red Found Lines", red_lines_img);

    vector<Boxe> boxes = find_boxes(nolines_img);

    //TODO rimuovere
    waitKey();
}