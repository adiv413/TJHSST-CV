// Aditya Vasantharao, pd. 4

#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core.hpp>     // Basic OpenCV structures (cv::Mat)
#include <opencv2/videoio.hpp>  // Video write
#include <math.h>


using namespace std;
using namespace cv;

Mat translate(Mat original, double x, double y, double z) {
    // create transformation matrix
    std::vector<std::vector<double>> transformation_vector = {{1, 0, 0, x},
                                                              {0, 1, 0, y},
                                                              {0, 0, 1, z},
                                                              {0, 0, 0, 1}};
    
    Mat transform_Mat = (Mat_<double>(4, 4) << transformation_vector[0][0], transformation_vector[0][1], transformation_vector[0][2], transformation_vector[0][3], transformation_vector[1][0], transformation_vector[1][1], transformation_vector[1][2], transformation_vector[1][3], transformation_vector[2][0], transformation_vector[2][1], transformation_vector[2][2], transformation_vector[2][3], transformation_vector[3][0], transformation_vector[3][1], transformation_vector[3][2], transformation_vector[3][3]);
    
    return transform_Mat * original;
}

Mat scale(Mat original, double x_scale, double y_scale, double z_scale) {
    // create transformation matrix
    std::vector<std::vector<double>> transformation_vector = {{x_scale, 0, 0, 0},
                                                             {0, y_scale, 0, 0},
                                                             {0, 0, z_scale, 0},
                                                             {0, 0, 0, 1}};
    
    Mat transform_Mat = (Mat_<double>(4, 4) << transformation_vector[0][0], transformation_vector[0][1], transformation_vector[0][2], transformation_vector[0][3], transformation_vector[1][0], transformation_vector[1][1], transformation_vector[1][2], transformation_vector[1][3], transformation_vector[2][0], transformation_vector[2][1], transformation_vector[2][2], transformation_vector[2][3], transformation_vector[3][0], transformation_vector[3][1], transformation_vector[3][2], transformation_vector[3][3]);
    
    return transform_Mat * original;
}

Mat rotate_x(Mat original, double angle) { // angle is in radians
    // create transformation matrix
    std::vector<std::vector<double>> transformation_vector = {{1, 0, 0, 0},
                                                             {0, cos(angle), -sin(angle), 0},
                                                             {0, sin(angle), cos(angle), 0},
                                                             {0, 0, 0, 1}};
    
    Mat transform_Mat = (Mat_<double>(4, 4) << transformation_vector[0][0], transformation_vector[0][1], transformation_vector[0][2], transformation_vector[0][3], transformation_vector[1][0], transformation_vector[1][1], transformation_vector[1][2], transformation_vector[1][3], transformation_vector[2][0], transformation_vector[2][1], transformation_vector[2][2], transformation_vector[2][3], transformation_vector[3][0], transformation_vector[3][1], transformation_vector[3][2], transformation_vector[3][3]);
    
    return transform_Mat * original;
}

Mat rotate_y(Mat original, double angle) { // angle is in radians
    // create transformation matrix
    std::vector<std::vector<double>> transformation_vector = {{cos(angle), 0, sin(angle), 0},
                                                             {0, 1, 0, 0},
                                                             {-sin(angle), 0, cos(angle), 0},
                                                             {0, 0, 0, 1}};
    
    Mat transform_Mat = (Mat_<double>(4, 4) << transformation_vector[0][0], transformation_vector[0][1], transformation_vector[0][2], transformation_vector[0][3], transformation_vector[1][0], transformation_vector[1][1], transformation_vector[1][2], transformation_vector[1][3], transformation_vector[2][0], transformation_vector[2][1], transformation_vector[2][2], transformation_vector[2][3], transformation_vector[3][0], transformation_vector[3][1], transformation_vector[3][2], transformation_vector[3][3]);
    
    return transform_Mat * original;
}

Mat rotate_z(Mat original, double angle) { // angle is in radians
    // create transformation matrix
    std::vector<std::vector<double>> transformation_vector = {{cos(angle), -sin(angle), 0, 0},
                                                             {sin(angle), cos(angle), 0, 0},
                                                             {0, 0, 1, 0},
                                                             {0, 0, 0, 1}};
    
    Mat transform_Mat = (Mat_<double>(4, 4) << transformation_vector[0][0], transformation_vector[0][1], transformation_vector[0][2], transformation_vector[0][3], transformation_vector[1][0], transformation_vector[1][1], transformation_vector[1][2], transformation_vector[1][3], transformation_vector[2][0], transformation_vector[2][1], transformation_vector[2][2], transformation_vector[2][3], transformation_vector[3][0], transformation_vector[3][1], transformation_vector[3][2], transformation_vector[3][3]);
    
    return transform_Mat * original;
}

void part1() {
    int height = 600;
    int width = 800;
    double PI = 3.141592653589793;
    
    // create cube list of vertices
    
    std::vector<std::vector<double>> vertices = {{1, 1, 1, 1},
                                                 {1, 1, -1, 1},
                                                 {1, -1, 1, 1},
                                                 {-1, 1, 1, 1},
                                                 {-1, -1, 1, 1},
                                                 {-1, 1, -1, 1},
                                                 {1, -1, -1, 1},
                                                 {-1, -1, -1, 1}};
    
    vector<Mat> cube;
    int fps = 60;
    
    VideoWriter writer;
    writer.open("rotation.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, Size(width, height));
    
    for(int i = 0; i < vertices.size(); i++) {
        Mat point = (Mat_<double>(4, 1) << vertices[i][0], vertices[i][1], vertices[i][2], vertices[i][3]);
        cube.push_back(scale(point, 50, 50, 50));
    }
    
    int frames = 300;
    double angle_increment = 2 * PI / static_cast<double>(frames);
    
    ofstream results("coordinates.txt");
    
    for(int frame_num = 0; frame_num <= frames; frame_num++) {
        Mat image(height, width, CV_8UC3, Scalar(255, 255, 255));
        
        vector<Mat> temp_cube;
        vector<Point> mat_to_point;
        
        for(int i = 0; i < cube.size(); i++) {
            Mat m = translate(rotate_z(rotate_y(rotate_x(cube[i], angle_increment * frame_num), angle_increment * frame_num), angle_increment * frame_num), 400, 300, 0);
            temp_cube.push_back(m);
            std::vector<double> out(m.begin<double>(), m.end<double>());
            circle(image, Point(out[0], out[1]), 3, Scalar(0, 0, 0), 3);
            
            if(frame_num < 4) {
                if(i != cube.size() - 1) {
                    results << "(" << out[0] << "," << out[1] << "," << out[2] << "), ";
                }
                else {
                    results << "(" << out[0] << "," << out[1] << "," << out[2] << ")" << endl;
                }
            }

            if(frame_num == 4) {
                results.close();
            }
        }
        
        for(int i = 0; i < temp_cube.size(); i++) {
            std::vector<double> out(temp_cube[i].begin<double>(), temp_cube[i].end<double>());
            mat_to_point.push_back(Point(out[0], out[1]));
        }
        
        line(image, mat_to_point[0], mat_to_point[1], Scalar(0, 0, 0));
        line(image, mat_to_point[0], mat_to_point[2], Scalar(0, 0, 0));
        line(image, mat_to_point[0], mat_to_point[3], Scalar(0, 0, 0));
        
        line(image, mat_to_point[7], mat_to_point[6], Scalar(0, 0, 0));
        line(image, mat_to_point[7], mat_to_point[5], Scalar(0, 0, 0));
        line(image, mat_to_point[7], mat_to_point[4], Scalar(0, 0, 0));
        
        line(image, mat_to_point[4], mat_to_point[3], Scalar(0, 0, 0));
        line(image, mat_to_point[4], mat_to_point[2], Scalar(0, 0, 0));
        
        line(image, mat_to_point[5], mat_to_point[3], Scalar(0, 0, 0));
        line(image, mat_to_point[5], mat_to_point[1], Scalar(0, 0, 0));
        
        line(image, mat_to_point[6], mat_to_point[2], Scalar(0, 0, 0));
        line(image, mat_to_point[6], mat_to_point[1], Scalar(0, 0, 0));
        
        
        writer << image;
    }
    
    // create tetrahedron list of vertices
    
    std::vector<std::vector<double>> tetrahedron_vertices = {{1.0 / sqrt(3), 0, 0, 1},
                                                            {0, 0, 2 / sqrt(6), 1},
                                                            {-sqrt(3) / 6.0, 0.5, 0, 1},
                                                            {-sqrt(3) / 6.0, -0.5, 0, 1}};
    
    vector<Mat> tetrahedron;
    
    for(int i = 0; i < tetrahedron_vertices.size(); i++) {
        Mat point = (Mat_<double>(4, 1) << tetrahedron_vertices[i][0], tetrahedron_vertices[i][1], tetrahedron_vertices[i][2], tetrahedron_vertices[i][3]);
        tetrahedron.push_back(scale(point, 200, 200, 200));
    }
    
    for(int frame_num = 0; frame_num <= frames; frame_num++) {
        Mat image(height, width, CV_8UC3, Scalar(255, 255, 255));
        
        vector<Mat> temp_tetrahedron;
        vector<Point> mat_to_point;
        
        for(int i = 0; i < tetrahedron.size(); i++) {
            Mat m = translate(rotate_z(rotate_y(rotate_x(tetrahedron[i], angle_increment * frame_num), angle_increment * frame_num), angle_increment * frame_num), 400, 300, 0);
            temp_tetrahedron.push_back(m);
            std::vector<double> out(m.begin<double>(), m.end<double>());
            circle(image, Point(out[0], out[1]), 3, Scalar(0, 0, 0), 3);
        }
        
        for(int i = 0; i < temp_tetrahedron.size(); i++) {
            std::vector<double> out(temp_tetrahedron[i].begin<double>(), temp_tetrahedron[i].end<double>());
            mat_to_point.push_back(Point(out[0], out[1]));
        }
        
        line(image, mat_to_point[1], mat_to_point[0], Scalar(0, 0, 0));
        line(image, mat_to_point[1], mat_to_point[2], Scalar(0, 0, 0));
        line(image, mat_to_point[1], mat_to_point[3], Scalar(0, 0, 0));
        
        line(image, mat_to_point[2], mat_to_point[0], Scalar(0, 0, 0));
        line(image, mat_to_point[2], mat_to_point[3], Scalar(0, 0, 0));
        
        line(image, mat_to_point[0], mat_to_point[3], Scalar(0, 0, 0));
        
        
        writer << image;
    }
}

int main(int argc, char** argv) {
    part1();
    return 0;
}