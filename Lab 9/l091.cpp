// Aditya Vasantharao, pd. 4

#include <stdio.h>
#include <iostream>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core.hpp>     
#include <opencv2/videoio.hpp>  

using namespace std;
using namespace cv;

void part1(){
    VideoCapture video("withChessBoard.MOV"); 
    Size frame_size(static_cast<int>(video.get(CAP_PROP_FRAME_WIDTH)), static_cast<int>(video.get(CAP_PROP_FRAME_HEIGHT)));
    VideoWriter out("vr.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), static_cast<int>(video.get(CAP_PROP_FPS)), frame_size);
    
    Mat frame;
    Mat prev;
    Mat gray_frame;
    Mat gray_prev;
    vector<Point2f> old_vertices;
    vector<vector<Point2f>> raw_corners_for_calibration;
    
    // find all projected 2d corners to use for calibration, but it would take to long to actually run calibration on corners from all iterations,
    // so only use 20% of iterations for calibration
    
    vector<Point3f> target_corner_values = {{-1, 1, 0}, {1, 1, 0}, {1, -1, 0}, {-1, -1, 0}}; // this is what we want the calibrated base corners to be
    vector<vector<Point3f>> target_corners_for_calibration;
    vector<vector<Point2f>> all_corners;
        
    int i = 0;
    
    while(true) {
        video >> frame;
        if(frame.empty()) {
            break;
        }
        
        rotate(frame, frame, 1); // frame starts out rotated so un-rotate it
        vector<Point2f> vertices;
        
        bool foundCorners = findChessboardCorners(frame, Size(7, 7), vertices);
        if(!foundCorners) { // if the corners cannot be detected, use optical flow to estimate corner locations
            vector<uchar> status;
            vector<float> error;
            
            cvtColor(prev, gray_prev, COLOR_BGR2GRAY);
            cvtColor(frame, gray_frame, COLOR_BGR2GRAY);
            
            calcOpticalFlowPyrLK(gray_prev, gray_frame, old_vertices, vertices, status, error); 
        }
        
        old_vertices = vertices;
        frame.copyTo(prev);
        
        vector<Point2f> corners = {vertices[16], vertices[18], vertices[32], vertices[30]};
        
        if(i % 5 == 0) { // build lists of points to use in calibration
            raw_corners_for_calibration.push_back(corners);
            target_corners_for_calibration.push_back(target_corner_values);
        }
        
        all_corners.push_back(corners); // store the corner values so that we don't have to compute them again after calibration
        
        i++;
    }
    
    // calibrate camera from 2d corners
    Mat cameraMatrix;
    Mat distCoeffs;
    Mat rvecs;
    Mat tvecs;
    Mat stdDeviationsIntrinsics;
    Mat stdDeviationsExtrinsics;
    Mat perViewErrors;

    calibrateCamera(target_corners_for_calibration, raw_corners_for_calibration, frame_size, cameraMatrix, distCoeffs, rvecs, tvecs, stdDeviationsIntrinsics, stdDeviationsExtrinsics, perViewErrors);
                    
    // project points
                    
    // raw top and bottom corners - we will project these using rvecs and tvecs to find the actual corner values in the video
    vector<Point3f> raw_all_corners = {{-1, 1, 0}, {1, 1, 0}, {1, -1, 0}, {-1, -1, 0}, {-1, 1, 2}, {1, 1, 2}, {1, -1, 2}, {-1, -1, 2}};
    i = 0;
    
    VideoCapture video1("withChessBoard.MOV");
    
    while(true) {
        video1 >> frame;
        if(frame.empty()) {
            break;
        }
        
        rotate(frame, frame, 1); // frame starts out rotated so un-rotate it
        
        // solve PnP problem using the calibrated camera matrix to find rvecs and tvecs to use to project the raw corners
        solvePnP(target_corner_values, all_corners[i], cameraMatrix, distCoeffs, rvecs, tvecs); 
        
        // use rvecs and tvecs to project raw corners and find final points to draw
        vector<Point2f> projected_all_corners;
        projectPoints(raw_all_corners, rvecs, tvecs, cameraMatrix, distCoeffs, projected_all_corners);
        
        // draw the final cube
        Point p1_bottom(projected_all_corners[0].x, projected_all_corners[0].y);
        Point p2_bottom(projected_all_corners[1].x, projected_all_corners[1].y);
        Point p3_bottom(projected_all_corners[2].x, projected_all_corners[2].y);
        Point p4_bottom(projected_all_corners[3].x, projected_all_corners[3].y);
        
        Point p1_top(projected_all_corners[4].x, projected_all_corners[4].y);
        Point p2_top(projected_all_corners[5].x, projected_all_corners[5].y);
        Point p3_top(projected_all_corners[6].x, projected_all_corners[6].y);
        Point p4_top(projected_all_corners[7].x, projected_all_corners[7].y);

        line(frame, p1_bottom, p2_bottom, Scalar(255, 0, 0), 3);
        line(frame, p2_bottom, p3_bottom, Scalar(255, 0, 0), 3);
        line(frame, p3_bottom, p4_bottom, Scalar(255, 0, 0), 3);
        line(frame, p4_bottom, p1_bottom, Scalar(255, 0, 0), 3);
        
        line(frame, p1_top, p2_top, Scalar(255, 0, 0), 3);
        line(frame, p2_top, p3_top, Scalar(255, 0, 0), 3);
        line(frame, p3_top, p4_top, Scalar(255, 0, 0), 3);
        line(frame, p4_top, p1_top, Scalar(255, 0, 0), 3);
        
        line(frame, p1_bottom, p1_top, Scalar(255, 0, 0), 3);
        line(frame, p2_bottom, p2_top, Scalar(255, 0, 0), 3);
        line(frame, p3_bottom, p3_top, Scalar(255, 0, 0), 3);
        line(frame, p4_bottom, p4_top, Scalar(255, 0, 0), 3);

        out << frame; 
        i++;
    }
}

int main(){
    part1();
    return 0;
}