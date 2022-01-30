#include "music_sheet.hpp"

using music::music_sheet;

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
        if(img.at<uchar>(pixel, i) == 0) 
        { 
            sum = 0;
            for(int y=-n; y<=n; y++) 
            {
                if(pixel + y > 0 && pixel + y < img.rows) 
                {
                    sum += img.at<uchar>(pixel+y, i);                    
                }
            }
            if(sum > 1000) 
            {
                for(int y=-m; y<=m; y++) 
                {
                    if(pixel + y > 0 && pixel + y < img.rows) 
                    {
                        img.at<uchar>(pixel+y, i) = 255;
                    }
                } 
            }
        }    
    }
}

//Finds the figures and draw a rectangle on them
void find_boxes(int t, Mat& threshold_output, vector<Rect>& boxes)
{
    //Finds contours
    //This function finds contours in a binary image.
    vector<vector<Point>> contours; //Each contour is stored as a vector of points
    vector<Vec4i> hierarchy;        //Optional output vector containing information about the image topology.
    findContours(threshold_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

    //Input vector of a 2D point and the final boxes (resize after the contours function)
    vector<vector<Point>> contours_boxes(contours.size());
    boxes.resize(contours.size());

    for(int i=0; i<contours.size(); i++) 
    { 
        //approxPolyDP approximates a curve or a polygon with another curve/polygon with less vertices so that the distance between them is less or equal to the specified precision.
        approxPolyDP(Mat(contours[i]), contours_boxes[i], 3, true);
        //boundingRect returns the minimal up-right integer rectangle containing the rotated rectangle
        boxes[i] = boundingRect(Mat(contours_boxes[i]));
    }

    //The image with boxes
    Mat boxes_img = Mat::zeros(threshold_output.size(), CV_8UC3);

    for(int i=0; i<contours.size(); i++) 
    {
        //I need many colours for see the different boxes
        Scalar colour = Scalar(rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255));
        drawContours(boxes_img, contours_boxes, i, colour, 1, 8, vector<Vec4i>(), 0, Point());
        rectangle(boxes_img, boxes[i].tl(), boxes[i].br(), colour, 2, 8, 0);
    }

    //Shows the image with boxes
    imshow("Boxes", boxes_img);
}

music_sheet::music_sheet (const char* filename)	
{
    const char* filename = argc >= 2 ? argv[1] : "test.jpg";

    //Try to read the original colour source image.
    Mat colour_img = imread(filename, IMREAD_COLOR);
    if(colour_img.empty()) {
        help();
        cout << "can not open " << filename << endl;
        return -1;
    }

    //Converts the image from color version to black and white
    Mat gray_img;
    cvtColor(colour_img, gray_img, COLOR_RGB2GRAY);
    //Stamps original b&w image
    //imshow("B&W1", gray_img);

    //I'll need a copy for a test (finding lines)
    Mat red_lines_img = colour_img.clone();
    
    //Applies a fixed-level threshold to each array element.
    threshold(gray_img, gray_img, 200, 255, THRESH_TOZERO);

    //Stamps modified b&w image
    //imshow("Black and White with adjustments", gray_img);

    //Projection for find staff's lines
    vector<int> horiz_proj; 
    horizontal_projection(gray_img, horiz_proj);

    //Single lines' position (1 pixel)
    vector<int> staff_positions;
    //The final lines
    vector<int> lines;

    //Single line's pixel in the staff
    int lines_pixel;

    //Number of the single pixel lines that make one real line in the staff
    int n_lines;

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
            remove_staff(gray_img, i);

            //If it's a new line
            if (n_lines == 0)
            {
                lines_pixel = i; 
                last_line = i;
                n_lines = 1;
            }

            //If the next pixel line is 1 pixel near
            else if (abs(i-last_line) == 1)
            {
                lines_pixel += i;
                last_line = i;
                n_lines++;
            }

            //If the line is finished
            else
            {
                //Average pixel in a line (a line made of some pixel lines)
                lines.push_back(static_cast<int>(static_cast<double>(lines_pixel)/n_lines));
                
                //Stamps the average line's pixel
                //cout << lines.back() << ' ' << n_lines << endl;
                
                //Let's restart and find next line
                n_lines = 0; 

                //Creates the found line for the show (the red lines show)
                line(red_lines_img, Point(0, lines.back()), Point(gray_img.size().width, lines.back()), Scalar(0, 0, 255), 2);
            }
        }
    }
    //Shows the image without lines
    imshow("Without lines", gray_img);

    //Shows the found lines (in red)
    //imshow("Red Found Lines", red_lines_img);

    //Every figure will be in a rectangle (i'll call it boxe)
    vector<Rect> boxes;
    find_boxes(thresh, gray_img, boxes);
}