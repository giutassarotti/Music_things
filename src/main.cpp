
// #include <opencv2/imgcodecs.hpp>
// #include <opencv2/highgui.hpp>
// #include <opencv2/imgproc.hpp>
// #include <vector>

// using namespace cv;
// using std::vector;

// bool use_mask;
// Mat img; Mat templ; Mat result;
// const char* image_window = "Source Image";
// const char* result_window = "Result window";
// int match_method;
// int max_Trackbar = 5;


// void MatchingMethod( int, void* )
// {
//   Mat img_display;
//   img.copyTo( img_display );
//   int result_cols = img.cols - templ.cols + 1;
//   int result_rows = img.rows - templ.rows + 1;
//   result.create( result_rows, result_cols, CV_32FC1 );



//   matchTemplate( img, templ, result, match_method);

  
//   normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );
  
//   double minVal; double maxVal; Point minLoc; Point maxLoc;
//   Point matchLoc;
//   minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
//   if( match_method  == TM_SQDIFF || match_method == TM_SQDIFF_NORMED )
//     { matchLoc = minLoc; }
//   else
//     { matchLoc = maxLoc; }
//   rectangle( img_display, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );
//   rectangle( result, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );




//   imshow( image_window, img_display );
//   imshow( result_window, result );
//   return;
// }

// int main()
// {
//     // Read the image as gray-scale
// img = imread(samples::findFile("file/fra_martino.jpg"), IMREAD_COLOR);

// templ = imread(samples::findFile("file/nota.jpg"), IMREAD_COLOR );
// resize(templ, templ, Size(15,15));
// // // Store the edges 
// // Mat edges;
// // // // Find the edges in the image using canny detector
// // Canny(img, edges, 50, 200);
// // // // Create a vector to store lines of the image
// // vector<Vec4i> lines;
// // vector<Vec3f> circles;
// // // // Apply Hough Transform
// // HoughLinesP(edges, lines, 1, CV_PI/180, 250, 10, 250);


// // // // Draw lines on the image
// // for (size_t i=0; i<lines.size(); i++) {
// //     Vec4i l = lines[i];
// //     //line(img, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 1);
// // }

// namedWindow( image_window, WINDOW_AUTOSIZE );
//   namedWindow( result_window, WINDOW_AUTOSIZE );

// const char* trackbar_label = "Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED";
//   createTrackbar( trackbar_label, image_window, &match_method, max_Trackbar, MatchingMethod );

//   MatchingMethod( 0, 0 );
//   waitKey(0);

// // Show result image
// //imshow("Result Image", img);

//     return 0;
// }

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp> // For HoG.
#include <opencv2/photo.hpp>

#include <iostream>
#include <cmath>
#include <algorithm>

using namespace cv;
using namespace std;

RNG rng(12345);
int thresh = 100;

void help()
{ 
    cout << "\nThis program reads and plays sheet music from a scanned image.\n"
         << "Usage:\n"
         << "./sightread <image_name>, Default is test.jpg\n"
         << endl;
}

float median(vector<float> &v)
{
    size_t n = v.size() / 2;
    nth_element(v.begin(), v.begin()+n, v.end());
    return v[n];
}

void horiz_projection(Mat& img, vector<int>& histo)
{
    //Mat proj = Mat::zeros(img.rows, img.cols, img.type());
    int count, i, j;

    for(i=0; i < img.rows; i++) {   
        for(count = 0, j=0; j < img.cols; j++) {
            count += (img.at<int>(i, j)) ? 0:1;
        }

        histo.push_back(count);
        //line(proj, Point(0, i), Point(count, i), Scalar(0,0,0), 1, 4);
    }

    //imshow("proj", proj);
}

void vert_projection(Mat& img, vector<int>& histo)
{
    Mat proj = Mat::zeros(img.rows, img.cols, img.type());
    int count, i, j, k;

    for(i=0; i < img.cols; i++) {   
        for(count = 0, j=0; j < img.rows; j++) {
            for(k=0; k < img.channels(); k++) {
                count += (img.at<int>(j, i, k)) ? 0:1;
            }
        }

        histo.push_back(count);
        //line(proj, Point(i, 0), Point(i, count/img.channels()), Scalar(0,0,0), 1, 4);
    }

    //imshow("proj", proj);
}

void remove_staff(Mat& img, int index)
{
    for(int x = 0; x < img.cols; x++) {
        if(img.at<uchar>(index, x) == 0) { 
            int sum = 0;
            for(int y = -3; y <= 3; y++) {
                if(index + y > 0 && index + y < img.rows) {
                    sum += img.at<uchar>(index+y, x);                    
                }
            } 
            if(sum >1000) {
                for(int y = -2; y <= 2; y++) {
                    if(index + y > 0 && index + y < img.rows) {
                        img.at<uchar>(index+y, x) = 255;
                    }
                } 
            }
        }    
    }
}

void flood_line(Mat& img, Point seed, vector<Point>& line_pts)
{
    // If point isn't black, return.
    if( img.at<uchar>(seed) != 0 ) { return; }
    if( seed.x < 0 || seed.x > img.rows || seed.y < 0 || seed.y > img.cols );
    if( std::find(line_pts.begin(), line_pts.end(), seed)!=line_pts.end() ) { return; }

    line_pts.push_back(seed);

    img.at<uchar>(seed) = 255;
    flood_line(img, Point(seed.x - 1, seed.y), line_pts);
    flood_line(img, Point(seed.x + 1, seed.y), line_pts);
    flood_line(img, Point(seed.x, seed.y - 1), line_pts);
    flood_line(img, Point(seed.x, seed.y + 1), line_pts);
}

void remove_staff2(Mat& orig, Mat& img, int index)
{
    char b = rng.uniform(0, 255);
    char g = rng.uniform(0, 255);
    char r = rng.uniform(0, 255);

    for(int x = 0; x < img.cols; x++) {
        if(img.at<uchar>(index, x) == 0) { 
            vector<Point> pts;
            flood_line(img, Point(x, index), pts);            
            for( vector<Point>::iterator it = pts.begin(); it != pts.end(); ++it) {
                orig.at<Vec3b>(it->y, it->x)[0] = b;
                orig.at<Vec3b>(it->y, it->x)[1] = g;
                orig.at<Vec3b>(it->y, it->x)[2] = r;
            }
        }    
    }
}

void find_contours(int t, Mat& threshold_output, vector<Rect>& boundRect)
{
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    // Find contours
    findContours( threshold_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );

    // Approximate contours to polygons + get bounding rects and circles
    vector<vector<Point> > contours_poly( contours.size() );
    vector<Point2f>center( contours.size() );
    vector<float>radius( contours.size() );
    boundRect.resize( contours.size() );

    for( int i = 0; i < contours.size(); i++ ) { 
        approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
        boundRect[i] = boundingRect( Mat(contours_poly[i]) );
    }

    // Draw polygonal contour + bounding rects + circles
    Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );
    for( int i = 0; i < contours.size(); i++ ) {
       Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
       drawContours( drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
       rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
    }

    imshow( "Contours", drawing );
}

int main(int argc, char** argv)
{
    const char* filename = argc >= 2 ? argv[1] : "test.jpg";

    Mat c_src;      // Color source image.
    Mat g_src;      // Grayscale source image.
    Mat edges;      // Edge detected source image.

    c_src = imread(filename, IMREAD_COLOR);

    if(c_src.empty()) {
        help();
        cout << "can not open " << filename << endl;
        return -1;
    }

    cvtColor(c_src, g_src, COLOR_RGB2GRAY);

    Canny(g_src, edges, 50, 200, 3);

    threshold(g_src, g_src, 200, 255, THRESH_TOZERO);

    imshow("B&W", g_src);
    vector<int> horiz_proj; 
    
    horiz_projection(g_src, horiz_proj);

    vector<int> staff_positions;
    vector<int> lines;
    int lines_sum = 0, n_lines = 0, last_line;
    int max = *std::max_element(horiz_proj.begin(), horiz_proj.end());
    std::sort(staff_positions.begin(), staff_positions.end());
    for(int i = 0; i < horiz_proj.size(); i++) {
        if(horiz_proj[i] > max/3.5) {
            remove_staff(g_src, i); 
            //cout << i << endl;

            if (n_lines == 0)
            {
                lines_sum = i; 
                last_line = i;
                n_lines = 1;
            }

            if (abs(i - last_line) == 1)
            {
                lines_sum += i;
                last_line = i;
                n_lines++;
            }
            else if (i != last_line)
            {
                lines.push_back(static_cast<int>(static_cast<double>(lines_sum)/n_lines));
                cout << lines.back() << ' ' << n_lines << endl;
                n_lines = 0; 

                //Fa vedere le linee trovate
                //line(c_src, Point(0, lines.back()), Point(g_src.size().width, lines.back()), Scalar(0, 0, 255), 2);
            }
        }
    }
    imshow("No Staff", g_src);

    //Linee rosse
    //imshow("Lines", c_src);

    vector<Rect> bboxes;
    Mat bw_temp = g_src.clone();
    find_contours(thresh, bw_temp, bboxes);

    HOGDescriptor hog;
    vector<float> descriptorsValues;
    vector<Point> locations;

    int i = 7;
    Mat img;
    g_src(Rect(bboxes[i].x, bboxes[i].y, bboxes[i].width, bboxes[i].height)).copyTo(img);

    hog.compute(g_src , descriptorsValues, Size(0,0), Size(0,0), locations);
    waitKey();

    cout << "HOG descriptor size is " << hog.getDescriptorSize() << endl;
    cout << "img dimensions: " << img.cols << " width x " << img.rows << " height" << endl;
    cout << "Found " << descriptorsValues.size() << " descriptor values" << endl;
    cout << "Nr of locations specified : " << locations.size() << endl;
/*
    for(int i = 0; i<bboxes.size(); i++) {
        if(bboxes[i].width * bboxes[i].height < 100) { continue; }
        imshow("small", g_src(range(bboxes[i].y, bboxes[i].y+bboxes[i].height), range(bboxes[i].x, bboxes[i].x + bboxes[i].width)));    
        waitKey();
    }*/
    waitKey();


    return 0;
}
