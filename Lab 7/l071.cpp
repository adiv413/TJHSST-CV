#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;
using namespace cv;

// coins_easy ./l071 -F coins1.jpg -L 30 -U 50 -TC 15 -min_r 75 -max_r 140 -max_d 81 -max_p 94 -max_n 109 -max_q 133 -gauss_size 21 -gauss_kernel 6
// coins medium ./l071 -F coins2.jpg -L 30 -U 51 -TC 19 -min_r 70 -max_r 140 -max_d 75 -max_p 94 -max_n 104 -max_q 133 -gauss_size 21 -gauss_kernel 6
// coins hard ./l071 -F coins3.jpg -L 10 -U 31 -TC 15 -min_r 56 -max_r 106 -max_d 86 -max_p 94 -max_n 106 -max_q 133 -gauss_size 33 -gauss_kernel 8 -max_silver_dollar 175

void part1(string filename = "coins1.jpg", int lower_threshold = 30, int upper_threshold = 50, int center_threshold = 15, int min_radius = 75, int max_radius = 140, int max_dime = 81, int max_penny = 94, int max_nickel = 109, int max_quarter = 133, int gauss_size = 21, int gauss_kernel = 6, int max_silver_dollar = 0) {
    Mat image, gray, canny_edge;
    image = imread(filename, 1);

    if ( !image.data )
    {
        printf("No image data \n");
        return;
    }
    
    // create grayscale image
    
    cout << "Create grayscale" << endl;
    gray = imread( filename, IMREAD_GRAYSCALE );
    imwrite("./imageg.jpg", gray);  
    
    // blur image using Gaussian Blur
    
    GaussianBlur(gray, gray, Size(gauss_size, gauss_size), gauss_kernel, gauss_kernel);
    
    // display canny edge
    
    Canny(gray, canny_edge, lower_threshold, upper_threshold);
    imwrite("./imagef.jpg", canny_edge);  
    
    // apply Hough Circle Transform
    
    vector<Vec3f> circles;
    HoughCircles(gray, circles, HOUGH_GRADIENT, 1, min_radius, upper_threshold, center_threshold, min_radius, max_radius);
    
    // classify and display circles that were detected in Hough Circle Transform
    
    int num_pennies = 0;
    int num_dimes = 0;
    int num_nickels = 0;
    int num_quarters = 0;
    int num_silver_dollars = 0;
    
    for(int i = 0; i < circles.size(); i++) {
        Vec3i c = circles[i];
        Point center = Point(c[0], c[1]);
        int radius = c[2];
        
        Vec3b pixel = image.at<Vec3b>(center);

        int b = static_cast<int>(pixel.val[0]);
        int g = static_cast<int>(pixel.val[1]);
        int r = static_cast<int>(pixel.val[2]);
        
        int avg = (r + g + b) / 3;

        if(r >= g + static_cast<int>(avg / 6) && radius <= static_cast<int>((max_penny + max_nickel) / 2)) { // penny, based on color
            for(int e = 0; e <= 3; e++) {
                circle(image, center, radius + e, Scalar(0,0,255), 3, LINE_AA);
            }
            num_pennies++;
        }
        else { // look at radius sizes
            if(max_quarter <= radius && radius <= max_radius) { // silver dollar
                for(int e = 0; e <= 3; e++) {
                    circle(image, center, radius + e, Scalar(0, 255, 255), 3, LINE_AA);
                }
                num_silver_dollars++;
            }
            else if(max_nickel <= radius && radius < max_quarter) { // quarter
                for(int e = 0; e <= 3; e++) {
                    circle(image, center, radius + e, Scalar(0, 255, 0), 3, LINE_AA);
                }
                num_quarters++;
            }
            else if(max_penny <= radius && radius < max_nickel) { // nickel
                for(int e = 0; e <= 3; e++) {
                    circle(image, center, radius + e, Scalar(255,0,255), 3, LINE_AA);
                }
                num_nickels++;
            }
            else if(max_dime <= radius && radius < max_penny) { // penny
                for(int e = 0; e <= 3; e++) {
                    circle(image, center, radius + e, Scalar(0,0,255), 3, LINE_AA);
                }
                num_pennies++;
            }
            else if(min_radius <= radius && radius < max_dime) { // dime
                for(int e = 0; e <= 3; e++) {
                    circle(image, center, radius + e, Scalar(255, 0, 0), 3, LINE_AA);
                }
                num_dimes++;
            }
        }
    }
    
    if(max_silver_dollar > max_radius) { // if we're detecting silver dollars
        vector<Vec3f> silver_dollars;
        HoughCircles(gray, silver_dollars, HOUGH_GRADIENT, 1, min_radius, 43, 12, 152, max_silver_dollar); // run separate hough transform for silver dollar
        
        for(int i = 0; i < silver_dollars.size(); i++) {
            Vec3i c = silver_dollars[i];
            Point center = Point(c[0], c[1]);
            int radius = c[2];
            
            if(max_quarter <= radius && radius <= max_silver_dollar) { // silver dollar
                for(int e = 0; e <= 3; e++) {
                    circle(image, center, radius + e, Scalar(0, 255, 255), 3, LINE_AA);
                }
                num_silver_dollars++;
            }
        }
    }
    
    cout << num_pennies << " Pennies" << endl;
    cout << num_nickels << " Nickels" << endl;
    cout << num_dimes << " Dimes" << endl;
    cout << num_quarters << " Quarters" << endl;
    cout << num_silver_dollars << " Silver Dollars" << endl;

    cout << "Total sum: $" << 0.01 * num_pennies + 0.05 * num_nickels + 0.1 * num_dimes + 0.25 * num_quarters + 1 * num_silver_dollars << endl;

    ofstream results("results.txt");

    results << num_pennies << " Pennies" << endl;
    results << num_nickels << " Nickels" << endl;
    results << num_dimes << " Dimes" << endl;
    results << num_quarters << " Quarters" << endl;
    results << num_silver_dollars << " Silver Dollars" << endl;

    results << "Total sum: $" << 0.01 * num_pennies + 0.05 * num_nickels + 0.1 * num_dimes + 0.25 * num_quarters + 1 * num_silver_dollars << endl;

    results.close();
    
    imwrite("./coins.jpg", image);  
    
}

int main(int argc, char** argv) {
    if(argc > 26) {
        part1(argv[2], stoi(argv[4]), stoi(argv[6]), stoi(argv[8]), stoi(argv[10]), stoi(argv[12]), stoi(argv[14]), stoi(argv[16]), stoi(argv[18]), stoi(argv[20]), stoi(argv[22]), stoi(argv[24]), stoi(argv[26]));
    }
    else if(argc > 24) {
        part1(argv[2], stoi(argv[4]), stoi(argv[6]), stoi(argv[8]), stoi(argv[10]), stoi(argv[12]), stoi(argv[14]), stoi(argv[16]), stoi(argv[18]), stoi(argv[20]), stoi(argv[22]), stoi(argv[24]));
    }
    else if(argc > 22) {
        part1(argv[2], stoi(argv[4]), stoi(argv[6]), stoi(argv[8]), stoi(argv[10]), stoi(argv[12]), stoi(argv[14]), stoi(argv[16]), stoi(argv[18]), stoi(argv[20]), stoi(argv[22]));
    }
    else if(argc > 20) {
        part1(argv[2], stoi(argv[4]), stoi(argv[6]), stoi(argv[8]), stoi(argv[10]), stoi(argv[12]), stoi(argv[14]), stoi(argv[16]), stoi(argv[18]), stoi(argv[20]));
    }
    else if(argc > 18) {
        part1(argv[2], stoi(argv[4]), stoi(argv[6]), stoi(argv[8]), stoi(argv[10]), stoi(argv[12]), stoi(argv[14]), stoi(argv[16]), stoi(argv[18]));
    }
    else if(argc > 16) {
        part1(argv[2], stoi(argv[4]), stoi(argv[6]), stoi(argv[8]), stoi(argv[10]), stoi(argv[12]), stoi(argv[14]), stoi(argv[16]));
    }
    else if(argc > 14) {
        part1(argv[2], stoi(argv[4]), stoi(argv[6]), stoi(argv[8]), stoi(argv[10]), stoi(argv[12]), stoi(argv[14]));
    }
    else if(argc > 12) {
        part1(argv[2], stoi(argv[4]), stoi(argv[6]), stoi(argv[8]), stoi(argv[10]), stoi(argv[12]));
    }
    else if(argc > 10) {
        part1(argv[2], stoi(argv[4]), stoi(argv[6]), stoi(argv[8]), stoi(argv[10]));
    }
    else if(argc > 8) {
        part1(argv[2], stoi(argv[4]), stoi(argv[6]), stoi(argv[8]));
    }
    else if(argc > 6) {
        part1(argv[2], stoi(argv[4]), stoi(argv[6]));
    }
    else if(argc > 4) {
        part1(argv[2], stoi(argv[4]));
    }
    else if(argc > 2) {
        part1(argv[2]);
    }
    else {
        part1();
    }
    
    return 0;
}