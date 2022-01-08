
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

using namespace cv;
using std::vector;

int main()
{
    // Read the image as gray-scale
Mat img = imread(samples::findFile("file/fra_martino.jpg"), IMREAD_COLOR);
// Store the edges 
Mat edges;
// // Find the edges in the image using canny detector
Canny(img, edges, 50, 200);
// // Create a vector to store lines of the image
vector<Vec4i> lines;
// // Apply Hough Transform
HoughLinesP(edges, lines, 1, CV_PI/180, 250, 10, 250);
// // Draw lines on the image
for (size_t i=0; i<lines.size(); i++) {
    Vec4i l = lines[i];
    line(img, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 1);
}
// Show result image
imshow("Result Image", img);
waitKey();
    return 0;
}