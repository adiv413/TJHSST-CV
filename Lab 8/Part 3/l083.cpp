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

Mat create_3D_Mat(double x, double y, double z) {
    Mat temp = (Mat_<double>(3, 1) << x, y, z);
    return temp;
}

Mat get_unit_vector(Mat original) {
    std::vector<double> out(original.begin<double>(), original.end<double>());
    double magnitude = sqrt(out[0] * out[0] + out[1] * out[1] + out[2] * out[2]);
    return (1.0 / magnitude) * original;
}

Mat find_projection(Mat v, Mat a, Mat e, Mat n) {
    return (v - e) * ((a - e).dot(n) / (v - e).dot(n)) + e;
}

void part3() {
    int height = 600;
    int width = 800;
    double PI = 3.141592653589793;
    // eye -> plane --> vertex
    
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
        cube.push_back(point);
    }
    
    int frames = 300;
    double angle_increment = 2 * PI / static_cast<double>(frames);
    
    Mat eye = create_3D_Mat(400.0, 400.0, 500.0);
    Mat point_on_plane = create_3D_Mat(400.0, 200.0, 300.0);
    Mat normal = create_3D_Mat(1.0, 3.0, 2.0);

    Mat origin = create_3D_Mat(0.0, 0.0, 0.0);
    Mat p0 = find_projection(origin, point_on_plane, eye, normal);
    
    Mat p1_raw = create_3D_Mat(1.0, 1.0, 1.0);
    Mat p1 = find_projection(p1_raw, point_on_plane, eye, normal);

    Mat p2_raw = create_3D_Mat(1.0, 1.0, -1.0);
    Mat p2 = find_projection(p2_raw, point_on_plane, eye, normal);

    Mat p3_raw = create_3D_Mat(1.0, -1.0, 1.0);
    Mat p3 = find_projection(p3_raw, point_on_plane, eye, normal);
    
    Mat a = p1 - p2;
    Mat b = p3 - p2; // p1 - p3
    Mat w1 = get_unit_vector(a);
    Mat c = b - a * (b.dot(a) / a.dot(a));
    Mat w2 = get_unit_vector(c);
    
    
    ofstream results("log.txt");

    std::vector<double> a_out(point_on_plane.begin<double>(), point_on_plane.end<double>());
    std::vector<double> n_out(normal.begin<double>(), normal.end<double>());
    std::vector<double> e_out(eye.begin<double>(), eye.end<double>());
    std::vector<double> p0_out(p0.begin<double>(), p0.end<double>());
    std::vector<double> w1_out(w1.begin<double>(), w1.end<double>());
    std::vector<double> w2_out(w2.begin<double>(), w2.end<double>());
    

    results << "The plane defined by (x-a)*n=0 is:\n";
    results << "\ta="<<"("<<a_out[0]<<","<<a_out[1]<<","<<a_out[2]<<")"<<"\n";
    results << "\tn="<<"("<<n_out[0]<<","<<n_out[1]<<","<<n_out[2]<<")"<<"\n";
    results << "The eye e is:\n";
    results << "\te="<<"("<<e_out[0]<<","<<e_out[1]<<","<<e_out[2]<<")"<<"\n";
    results << "The coordinates in the plane x = p0 + u*w1 + v*w2 is:\n";
    results << "\tp0="<<"("<<p0_out[0]<<","<<p0_out[1]<<","<<p0_out[2]<<")"<<"\n";
    results << "\tw1="<<"("<<w1_out[0]<<","<<w1_out[1]<<","<<w1_out[2]<<")"<<"\n";
    results << "\tw2="<<"("<<w2_out[0]<<","<<w2_out[1]<<","<<w2_out[2]<<")"<<"\n";
    
    for(int frame_num = 0; frame_num <= frames; frame_num++) {
        Mat image(height, width, CV_8UC3, Scalar(255, 255, 255));
        vector<Point> mat_to_point;
        vector<vector<double>> mat_to_point3; 

        stringstream out_3d;
        out_3d<<"The frame "<<frame_num + 1<<" in 3d has the following edges:\n";

        stringstream out_2d;
        out_2d<<"The frame "<<frame_num + 1<<" in 2d has the following edges:\n";
        
        for(int i = 0; i < cube.size(); i++) {
            Mat v_mat = scale(rotate_z(rotate_y(rotate_x(cube[i], angle_increment * frame_num), angle_increment * frame_num), angle_increment * frame_num), 150.0, 150.0, 150.0);
            std::vector<double> v_out(v_mat.begin<double>(), v_mat.end<double>());
            mat_to_point3.push_back({v_out[0] / v_out[3], v_out[1] / v_out[3], v_out[2] / v_out[3]});
            
            Mat v_actual = (Mat_<double>(3, 1) << v_out[0] / v_out[3], v_out[1] / v_out[3], v_out[2] / v_out[3]);

            // perspective projection - general case
            
            Mat pv = find_projection(v_actual, point_on_plane, eye, normal);
            double u = (pv - p0).dot(w1) / w1.dot(w1);
            double v = (pv - p0).dot(w2) / w2.dot(w2);
            
            std::vector<double> final_out{u + 400.0, v + 300.0};
            
            circle(image, Point(final_out[0], final_out[1]), 3, Scalar(0, 0, 0), 3);
            
            mat_to_point.push_back(Point(final_out[0], final_out[1]));
        }

        if(frame_num < 4) {
            out_3d<<"\t("<<mat_to_point3[0][0]<<","<<mat_to_point3[0][1]<<","<<mat_to_point3[0][2]<<"), ";
            out_3d<<"("<<mat_to_point3[1][0]<<","<<mat_to_point3[1][1]<<","<<mat_to_point3[1][2]<<")\n";

            out_3d<<"\t("<<mat_to_point3[0][0]<<","<<mat_to_point3[0][1]<<","<<mat_to_point3[0][2]<<"), ";
            out_3d<<"("<<mat_to_point3[2][0]<<","<<mat_to_point3[2][1]<<","<<mat_to_point3[2][2]<<")\n";

            out_3d<<"\t("<<mat_to_point3[0][0]<<","<<mat_to_point3[0][1]<<","<<mat_to_point3[0][2]<<"), ";
            out_3d<<"("<<mat_to_point3[3][0]<<","<<mat_to_point3[3][1]<<","<<mat_to_point3[3][2]<<")\n";

            out_3d<<"\t("<<mat_to_point3[7][0]<<","<<mat_to_point3[7][1]<<","<<mat_to_point3[7][2]<<"), ";
            out_3d<<"("<<mat_to_point3[6][0]<<","<<mat_to_point3[6][1]<<","<<mat_to_point3[6][2]<<")\n";

            out_3d<<"\t("<<mat_to_point3[7][0]<<","<<mat_to_point3[7][1]<<","<<mat_to_point3[7][2]<<"), ";
            out_3d<<"("<<mat_to_point3[5][0]<<","<<mat_to_point3[5][1]<<","<<mat_to_point3[5][2]<<")\n";

            out_3d<<"\t("<<mat_to_point3[7][0]<<","<<mat_to_point3[7][1]<<","<<mat_to_point3[7][2]<<"), ";
            out_3d<<"("<<mat_to_point3[4][0]<<","<<mat_to_point3[4][1]<<","<<mat_to_point3[4][2]<<")\n";

            out_3d<<"\t("<<mat_to_point3[4][0]<<","<<mat_to_point3[4][1]<<","<<mat_to_point3[4][2]<<"), ";
            out_3d<<"("<<mat_to_point3[3][0]<<","<<mat_to_point3[3][1]<<","<<mat_to_point3[3][2]<<")\n";

            out_3d<<"\t("<<mat_to_point3[4][0]<<","<<mat_to_point3[4][1]<<","<<mat_to_point3[4][2]<<"), ";
            out_3d<<"("<<mat_to_point3[2][0]<<","<<mat_to_point3[2][1]<<","<<mat_to_point3[2][2]<<")\n";

            out_3d<<"\t("<<mat_to_point3[5][0]<<","<<mat_to_point3[5][1]<<","<<mat_to_point3[5][2]<<"), ";
            out_3d<<"("<<mat_to_point3[3][0]<<","<<mat_to_point3[3][1]<<","<<mat_to_point3[3][2]<<")\n";

            out_3d<<"\t("<<mat_to_point3[5][0]<<","<<mat_to_point3[5][1]<<","<<mat_to_point3[5][2]<<"), ";
            out_3d<<"("<<mat_to_point3[1][0]<<","<<mat_to_point3[1][1]<<","<<mat_to_point3[1][2]<<")\n";

            out_3d<<"\t("<<mat_to_point3[6][0]<<","<<mat_to_point3[6][1]<<","<<mat_to_point3[6][2]<<"), ";
            out_3d<<"("<<mat_to_point3[2][0]<<","<<mat_to_point3[2][1]<<","<<mat_to_point3[2][2]<<")\n";

            out_3d<<"\t("<<mat_to_point3[6][0]<<","<<mat_to_point3[6][1]<<","<<mat_to_point3[6][2]<<"), ";
            out_3d<<"("<<mat_to_point3[1][0]<<","<<mat_to_point3[1][1]<<","<<mat_to_point3[1][2]<<")\n";


            out_2d<<"\t("<<mat_to_point[0].x<<","<<mat_to_point[0].y<<"), ";
            out_2d<<"("<<mat_to_point[1].x<<","<<mat_to_point[1].y<<")\n";

            out_2d<<"\t("<<mat_to_point[0].x<<","<<mat_to_point[0].y<<"), ";
            out_2d<<"("<<mat_to_point[2].x<<","<<mat_to_point[2].y<<")\n";

            out_2d<<"\t("<<mat_to_point[0].x<<","<<mat_to_point[0].y<<"), ";
            out_2d<<"("<<mat_to_point[3].x<<","<<mat_to_point[3].y<<")\n";

            out_2d<<"\t("<<mat_to_point[7].x<<","<<mat_to_point[7].y<<"), ";
            out_2d<<"("<<mat_to_point[6].x<<","<<mat_to_point[6].y<<")\n";

            out_2d<<"\t("<<mat_to_point[7].x<<","<<mat_to_point[7].y<<"), ";
            out_2d<<"("<<mat_to_point[5].x<<","<<mat_to_point[5].y<<")\n";

            out_2d<<"\t("<<mat_to_point[7].x<<","<<mat_to_point[7].y<<"), ";
            out_2d<<"("<<mat_to_point[4].x<<","<<mat_to_point[4].y<<")\n";

            out_2d<<"\t("<<mat_to_point[4].x<<","<<mat_to_point[4].y<<"), ";
            out_2d<<"("<<mat_to_point[3].x<<","<<mat_to_point[3].y<<")\n";

            out_2d<<"\t("<<mat_to_point[4].x<<","<<mat_to_point[4].y<<"), ";
            out_2d<<"("<<mat_to_point[2].x<<","<<mat_to_point[2].y<<")\n";

            out_2d<<"\t("<<mat_to_point[5].x<<","<<mat_to_point[5].y<<"), ";
            out_2d<<"("<<mat_to_point[3].x<<","<<mat_to_point[3].y<<")\n";

            out_2d<<"\t("<<mat_to_point[5].x<<","<<mat_to_point[5].y<<"), ";
            out_2d<<"("<<mat_to_point[1].x<<","<<mat_to_point[1].y<<")\n";

            out_2d<<"\t("<<mat_to_point[6].x<<","<<mat_to_point[6].y<<"), ";
            out_2d<<"("<<mat_to_point[2].x<<","<<mat_to_point[2].y<<")\n";

            out_2d<<"\t("<<mat_to_point[6].x<<","<<mat_to_point[6].y<<"), ";
            out_2d<<"("<<mat_to_point[1].x<<","<<mat_to_point[1].y<<")\n";

            results << out_3d.str() << endl;
            results << out_2d.str() << endl;
        }

        if(frame_num == 4) {
            results.close();
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
        tetrahedron.push_back(point);
    }
    
    for(int frame_num = 0; frame_num <= frames; frame_num++) {
        Mat image(height, width, CV_8UC3, Scalar(255, 255, 255));
        
        vector<Point> mat_to_point;
        
        for(int i = 0; i < tetrahedron.size(); i++) {
            Mat v_mat = scale(rotate_z(rotate_y(rotate_x(tetrahedron[i], angle_increment * frame_num), angle_increment * frame_num), angle_increment * frame_num), 350.0, 350.0, 350.0);
            std::vector<double> v_out(v_mat.begin<double>(), v_mat.end<double>());
            Mat v_actual = (Mat_<double>(3, 1) << v_out[0] / v_out[3], v_out[1] / v_out[3], v_out[2] / v_out[3]);

            // perspective projection - general case
            
            Mat pv = find_projection(v_actual, point_on_plane, eye, normal);
            double u = (pv - p0).dot(w1) / w1.dot(w1);
            double v = (pv - p0).dot(w2) / w2.dot(w2);
            
            std::vector<double> final_out{u + 400.0, v + 300.0};
            
            circle(image, Point(final_out[0], final_out[1]), 3, Scalar(0, 0, 0), 3);
            
            mat_to_point.push_back(Point(final_out[0], final_out[1]));
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
    part3();
    return 0;
}
