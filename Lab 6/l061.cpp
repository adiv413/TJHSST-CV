// Aditya Vasantharao, pd. 3

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <ctime>
#include <utility>
#include <algorithm>
#include <vector>
#include <cmath>
#include <climits>
#include <cfloat>
#include <iomanip>
#include <list>
#include <iterator>
#include <chrono>
#include <sstream>
#include <unordered_map>
#include <stack>

using namespace std;

int width = 0;
int height = 0;

class Point {
    private:
        double x;
        double y;

    public:
        double getX() {
            return x;
        }

        double getY() {
            return y;
        }

        Point(double first, double second) : x(first), y(second) {}
        Point(const Point &other) : x(other.x), y(other.y) {}
        Point() : x(0.0), y(0.0) {}

        double distance(Point other) {
            return sqrt(pow(other.x - x, 2) + pow(other.y - y, 2));
        }    
};

// set pixel to be 1 (black pixel)
void cast_vote(int **pixels, int x, int y, int height, int width) {
    if(x < 0 || x > height - 1 || y < 0 || y > width - 1);
    else {
        pixels[x][y] += 1;
    }
}

// write the board to a ppm file
void write_board(int **pixels, int height, int width, string filename) {
    ofstream outfile;
    outfile.open(filename + ".ppm");

    // header
    outfile << "P3 " << width << " " << height << " " << 255 << endl;

    // content
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(pixels[i][j] == 0) {
                // ppm files require one output for r, g, and b
                outfile << "255 255 255 ";
            }

            if(pixels[i][j] == 1) { // 1 represents a black pixel
                outfile << "0 0 0 ";
            }
        }
        outfile << endl;
    } 

    outfile.close();
}

class Line {
    private:
        Point first;
        Point second;
        double length;
        double slope;

        int canvas_height;
        int canvas_width;

        double threshold = 1e-10;

    public:
        double getLength() {
            return first.distance(second);
        }

        Point getFirst() {
            return first;
        }
        
        Point getSecond() {
            return second;
        }

        double getSlope() {
            return slope;
        }

        // draw a line between any two given points
        void draw_line(int **pixels) {
            // both points intersect with top right bottom or left of screen
            // check x = canvas_width, if 0 <= y && y <= canvas_height we're good
            // else plug in y = 0, solve for x
            Point scaled_first(-1.0, -1.0);
            Point scaled_second(-1.0, -1.0);

            // check 0 slope
            if((-threshold <= slope) && (slope <= threshold)) {
                scaled_first = Point(static_cast<double>(canvas_width), first.getY());
                scaled_second = Point(-static_cast<double>(canvas_width), first.getY());
            }
            // check infinite slope
            else if(100000.0 <= abs(slope)) {
                scaled_first = Point(first.getX(), static_cast<double>(canvas_height));
                scaled_second = Point(first.getX(), -static_cast<double>(canvas_height));
            }
            else {
                // check right side
                double right_y = slope * (canvas_height - first.getX()) + first.getY();
                if((0 <= right_y) && (right_y <= canvas_height)) {
                    // hits the right side of the screen
                    scaled_first = Point(canvas_height, right_y);
                }
                // check top
                double top_x = (canvas_width - first.getY()) / slope + first.getX();
                if((0 <= top_x) && (top_x <= canvas_width)) {
                    // hits the top of the screen
                    if((scaled_first.getX() == -1.0) && (scaled_first.getY() == -1.0)) {
                        scaled_first = Point(top_x, canvas_width);
                    }
                    else {
                        scaled_second = Point(top_x, canvas_height);
                    }
                }
                // check left
                double left_y = slope * (0 - first.getX()) + first.getY();
                if((0 <= left_y) && (left_y <= canvas_height)) {
                    // hits the left side of the screen
                    if((scaled_first.getX() == -1.0) && (scaled_first.getY() == -1.0)) {
                        scaled_first = Point(0, left_y); 
                    }
                    else if((scaled_second.getX() == -1.0) && (scaled_second.getY() == -1.0)){
                        scaled_second = Point(0, left_y); 
                    }
                }
                // check bottom
                double bottom_x = (0 - first.getY()) / slope + first.getX();
                if((0 <= bottom_x) && (bottom_x <= canvas_width) && ((scaled_second.getX() == -1.0) && (scaled_second.getY() == -1.0))) {
                    // hits the bottom of the screen
                    scaled_second = Point(bottom_x, 0);
                }
            }

            if(scaled_first.getX() == -1.0 || scaled_first.getY() == -1.0 || scaled_second.getX() == -1.0 || scaled_second.getY() == -1.0);
            else {
                int dx = scaled_second.getX() - scaled_first.getX(); // x2 - x1
                int dy = scaled_second.getY() - scaled_first.getY(); // y2 - y1
                int error = abs(dy) - abs(dx);

                if(error < 0 && scaled_second.getX() <= scaled_first.getX()) { // reverse the points
                    _draw_line_bresenham(pixels, scaled_second, scaled_first, canvas_height, canvas_width);
                }
                else if(error > 0 && scaled_second.getY() <= scaled_first.getY()) {
                    _draw_line_bresenham(pixels, scaled_second, scaled_first, canvas_height, canvas_width);
                }
                else {
                    _draw_line_bresenham(pixels, scaled_first, scaled_second, canvas_height, canvas_width);
                }
            }
        }

        // note: the lines cannot be parallel
        Point find_intersection(Line other) {
            if((other.slope - threshold <= slope) && (slope <= other.slope + threshold)) {
                return Point(0.0, 0.0); // no intersection between parallel lines
            }
            else if ((100000.0 <= abs(slope)) && ((-threshold <= other.slope) && (other.slope <= threshold))) { // x = ___ and y = ___
                return Point(first.getX(), other.first.getY());
            } 
            else if (((-threshold <= slope) && (slope <= threshold)) && (100000.0 <= abs(other.slope))) { // y = ___ and x = ___ 
                return Point(other.first.getX(), first.getY());
            }
            else if(100000.0 <= abs(slope)) {
                return Point(first.getX(), other.slope * (first.getX() - other.first.getX()) + other.first.getY());
            }
            else if((-threshold <= slope) && (slope <= threshold)) {
                return Point((first.getY() - other.first.getY()) / other.slope + other.first.getX(), first.getY()); 
            }
            else if(((-threshold <= other.slope) && (other.slope <= threshold)) || (100000.0 <= abs(other.slope))) {
                return other.find_intersection(getCopy());
            }
            else {
                double x = (other.slope * other.first.getX() - slope * first.getX() + first.getY() - other.first.getY()) / (other.slope - slope);
                double y = slope * (x - first.getX()) + first.getY();
                return Point(x, y);
            }
        }

        // returns a copy of this line
        Line getCopy() {
            return Line(first, second, canvas_height, canvas_width);
        }

        Line make_perpendicular(Point other) {
            double final_slope;
            if(slope == DBL_MAX) {
                final_slope = 0;
            }
            else if (slope == 0) {
                final_slope = DBL_MAX;    
            }
            else {
                final_slope = -(1.0 / slope);
            }

            return Line(other, final_slope, canvas_height, canvas_width);
        }

        // point-point definition
        Line(Point p1, Point p2, int c_height, int c_width) : first(p1), second(p2), length(p1.distance(p2)), canvas_height(c_height), canvas_width(c_width) {
            // slope: (y2 - y1) / (x2 - x1)
            if(p1.getX() == p2.getX()) {
                slope = DBL_MAX; // infinite slope
            }
            else if(p1.getY() == p2.getY()) {
                slope = 0;
            }
            else {
                slope = (p2.getY() - p1.getY()) / (p2.getX() - p1.getX());
            }
        }

        // point-slope form
        Line(Point p1, double m, int c_height, int c_width) : first(p1), slope(m), second(Point(p1.getX() + 1, p1.getY() + m)), canvas_height(c_height), canvas_width(c_width) {}

    private:
        // bresenham algorithm to draw a line between two given points
        // note: the points must be in increasing order if x1 < x2 and y1 < y2
        void _draw_line_bresenham(int **pixels, Point p1, Point p2, int canvas_height, int canvas_width) {
            int dx = p2.getX() - p1.getX(); // x2 - x1
            int dy = p2.getY() - p1.getY(); // y2 - y1
            int error = abs(dy) - abs(dx);  // negative: x driven, positive: y driven

            if(error < 0) { // x-driven
                int j = p1.getY(); // y1

                if(dy < 0) { // going down
                    for(int i = p1.getX(); i <= p2.getX(); i++) {
                        cast_vote(pixels, i, j, canvas_height, canvas_width);

                        if(error >= 0) {
                            j -= 1;
                            error -= dx;
                        }
                        error += -dy;
                    }
                }
                else { // normal case x driven
                    for(int i = p1.getX(); i <= p2.getX(); i++) {
                        cast_vote(pixels, i, j, canvas_height, canvas_width);

                        if(error >= 0) {
                            j += 1;
                            error -= dx;
                        }
                        error += dy;
                    }
                }
            }
            else { // y-driven
                if(dx < 0) { // going left
                    int i = p1.getX();

                    for(int j = p1.getY(); j <= p2.getY(); j++) {
                        cast_vote(pixels, i, j, canvas_height, canvas_width);

                        if(error <= 0) {
                            i -= 1;
                            error += dy;
                        }

                        error -= -dx;
                    }
                }
                else { // normal case y driven
                    int i = p1.getX();

                    for(int j = p1.getY(); j <= p2.getY(); j++) {
                        cast_vote(pixels, i, j, canvas_height, canvas_width);

                        if(error <= 0) {
                            i += 1;
                            error += dy;
                        }

                        error -= dx;
                    }
                }
            }
        }
};

class Pixel {
    private:
        int r;
        int g;
        int b;

    public:
        int getR() { 
            return r; 
        }

        int getG() { 
            return g; 
        }

        int getB() { 
            return b; 
        }

        void setR(int r) { 
            this->r = r; 
        }

        void setG(int g) { 
            this->g = g; 
        }

        void setB(int b) { 
            this->b = b; 
        }

        Pixel(int r, int g, int b) {
            this->r = r;
            this->g = g;
            this->b = b;
        }
};

// set pixel to be 1 (black pixel)
void color_pixel(vector<vector<Pixel>> &pixels, int x, int y, int height, int width) {
    if(x < 0 || x > height - 1 || y < 0 || y > width - 1);
    else {
        pixels[x][y] = Pixel(255, 0, 0);
    }
}

class Circle {
    private:
        Point center;
        double radius;
        int canvas_height;
        int canvas_width;

    public:

        Circle(Point p1, double r, int c_h, int c_w) : center(p1), radius(r), canvas_height(c_h), canvas_width(c_w) {}

        void draw_circle(vector<vector<Pixel>> &pixels) {
            int x, y, xmax, y2, y2_new, ty;
            xmax = (int) (radius * 0.70710678); // maximum x at radius/sqrt(2) + x0
            y = radius;
            y2 = y * y;
            ty = (2 * y) - 1; y2_new = y2;
            
            for (x = 0; x <= xmax + 1; x++) {
                if ((y2 - y2_new) >= ty) {
                    y2 -= ty;
                    y -= 1;
                    ty -= 2;
                }

                color_pixel(pixels, x + center.getX(), y + center.getY(), canvas_height, canvas_width);
                color_pixel(pixels, x + center.getX(), -y + center.getY(), canvas_height, canvas_width);
                color_pixel(pixels, -x + center.getX(), y + center.getY(), canvas_height, canvas_width);
                color_pixel(pixels, -x + center.getX(), -y + center.getY(), canvas_height, canvas_width);
                color_pixel(pixels, y + center.getX(), x + center.getY(), canvas_height, canvas_width);
                color_pixel(pixels, y + center.getX(), -x + center.getY(), canvas_height, canvas_width);
                color_pixel(pixels, -y + center.getX(), x + center.getY(), canvas_height, canvas_width);
                color_pixel(pixels, -y + center.getX(), -x + center.getY(), canvas_height, canvas_width);

                y2_new -= (2 * x) - 3;
            }
        }
};

vector<vector<Pixel>> readPPM(string infile) {
    ifstream inp(infile);
    string temp1;
    inp >> temp1; // useless information (file type, we don't need that)
    
    inp >> width >> height; // width and height of image

    int temp2;
    inp >> temp2; // more useless information (max color will always be 255)


    vector<vector<Pixel>> pixels(height, vector<Pixel>(width, Pixel(0, 0, 0)));
    // fill pixels with all pixels in the ppm

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int r, g, b;
            inp >> r >> g >> b;
            pixels[i][j] = Pixel(r, g, b);
        }
    }

    return pixels;
}

void hysteresis(vector<vector<Pixel>> &pixels, int i, int j, vector<vector<int>> &visited) {
    if(pixels[i][j].getR() != 0 && visited[i][j] != 1) {
        if(i + 1 < pixels.size()) {
            visited[i][j] = 1;
            hysteresis(pixels, i + 1, j, visited);
        }
        if(i - 1 >= 0) {
            visited[i][j] = 1;
            hysteresis(pixels, i - 1, j, visited);
        }
        if(j + 1 < pixels[0].size()) {
            visited[i][j] = 1;
            hysteresis(pixels, i, j + 1, visited);
        }
        if(j - 1 >= 0) {
            visited[i][j] = 1;
            hysteresis(pixels, i, j - 1, visited);
        }
        if(i + 1 < pixels.size() && j + 1 < pixels[0].size()) {
            visited[i][j] = 1;
            hysteresis(pixels, i + 1, j + 1, visited);
        }
        if(i - 1 >= 0 && j - 1 >= 0) {
            visited[i][j] = 1;
            hysteresis(pixels, i - 1, j - 1, visited);
        }
        if(i + 1 < pixels.size() && j - 1 >= 0) {
            visited[i][j] = 1;
            hysteresis(pixels, i + 1, j - 1, visited);
        }
        if(i - 1 >= 0 && j + 1 < pixels[0].size()) {
            visited[i][j] = 1;
            hysteresis(pixels, i - 1, j + 1, visited);
        }
    }
}

void part1(int threshold1 = 75, int threshold2 = 115, string filename = "image.ppm", int center_threshold = 80) {
    const long double pi = 3.14159265358979323846;

    // read in the ppm file
    vector<vector<Pixel>> pixels = readPPM(filename);

    // grayscale pixels
    for(int i = 0; i < pixels.size(); i++) {
        for(int j = 0; j < pixels[i].size(); j++) {
            int r = pixels[i][j].getR();
            int g = pixels[i][j].getG();
            int b = pixels[i][j].getB();
            int gray = (r + g + b) / 3;
            pixels[i][j] = Pixel(gray, gray, gray);
        }
    }

    // add zero padding=1 in preperation for convolution
    pixels.insert(pixels.begin(), vector<Pixel>(width, Pixel(0, 0, 0)));
    pixels.push_back(vector<Pixel>(width, Pixel(0, 0, 0)));

    for(int i = 0; i < pixels.size(); i++) {
        pixels[i].insert(pixels[i].begin(), Pixel(0, 0, 0));
        pixels[i].push_back(Pixel(0, 0, 0));
    }

    // gaussian blur
    vector<vector<Pixel>> blurred_pixels(height, vector<Pixel>(width, Pixel(0, 0, 0)));

    vector<vector<double>> kernel = {
        {1/16.0, 2/16.0, 1/16.0},
        {2/16.0, 4/16.0, 2/16.0},
        {1/16.0, 2/16.0, 1/16.0}
    };

    for(int i = 1; i < height + 1; i++) {
        for(int j = 1; j < width + 1; j++) {
            int value = 0;

            for(int k = 0; k <= 2; k++) {
                for(int l = 0; l <= 2; l++) {
                    value += pixels[i + (k - 1)][j + (l - 1)].getR() * kernel[k][l]; // we can use getR because it's all the same after grayscaling
                }
            }

            blurred_pixels[i - 1][j - 1] = Pixel(value, value, value);
        }
    }

    // add padding for next convolution

    blurred_pixels.insert(blurred_pixels.begin(), vector<Pixel>(width, Pixel(0, 0, 0)));
    blurred_pixels.push_back(vector<Pixel>(width, Pixel(0, 0, 0)));

    for(int i = 0; i < blurred_pixels.size(); i++) {
        blurred_pixels[i].insert(blurred_pixels[i].begin(), Pixel(0, 0, 0));
        blurred_pixels[i].push_back(Pixel(0, 0, 0));
    }

    int maxblur = 0;
    for(int i = 0; i < blurred_pixels.size(); i++) {
        for(int j = 0; j < blurred_pixels[i].size(); j++) {
            if(blurred_pixels[i][j].getR() > maxblur) {
                maxblur = blurred_pixels[i][j].getR();
            }
        }
    }

    // x convolution
    
    vector<vector<Pixel>> x_edges(height, vector<Pixel>(width, Pixel(0, 0, 0)));
    vector<vector<int>> x_kernel = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};

    for(int i = 1; i < height + 1; i++) {
        for(int j = 1; j < width + 1; j++) {
            int value = 0;

            for(int k = 0; k <= 2; k++) {
                for(int l = 0; l <= 2; l++) {
                    value += blurred_pixels[i + (k - 1)][j + (l - 1)].getR() * x_kernel[k][l]; // we can use getR because it's all the same after grayscaling
                }
            }

            x_edges[i - 1][j - 1] = Pixel(value, value, value);
        }
    }

    // y convolution
    vector<vector<Pixel>> y_edges(height, vector<Pixel>(width, Pixel(0, 0, 0)));
    vector<vector<int>> y_kernel = {{1, 2, 1}, {0, 0, 0}, {-1, -2, -1}};

    for(int i = 1; i < height + 1; i++) {
        for(int j = 1; j < width + 1; j++) {
            int value = 0;

            for(int k = 0; k <= 2; k++) {
                for(int l = 0; l <= 2; l++) {
                    value += blurred_pixels[i + (k - 1)][j + (l - 1)].getR() * y_kernel[k][l]; // we can use getR because it's all the same after grayscaling
                }
            }

            y_edges[i - 1][j - 1] = Pixel(value, value, value);
        }
    }

    // combine x and y edges
    vector<vector<Pixel>> edges(height, vector<Pixel>(width, Pixel(0, 0, 0)));
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            int value = sqrt(x_edges[i][j].getR() * x_edges[i][j].getR() + y_edges[i][j].getR() * y_edges[i][j].getR());
            edges[i][j] = Pixel(value, value, value);
        }
    }

    // find angles from x and y edges

    vector<vector<int>> angles(height, vector<int>(width, 0));

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            double angle = atan2(y_edges[i][j].getR(), x_edges[i][j].getR()) * 180 / pi;
            if(angle <= -157.5) {
                angles[i][j] = 0;
            }
            else if(-157.5 <= angle && angle <= -112.5) {
                angles[i][j] = 45;
            }
            else if(-112.5 <= angle && angle <= -67.5) {
                angles[i][j] = 90;
            }
            else if(-67.5 <= angle && angle <= -22.5) {
                angles[i][j] = 135;
            }
            else if(-22.5 <= angle && angle <= 22.5) {
                angles[i][j] = 0;
            }
            else if(22.5 <= angle && angle <= 67.5) {
                angles[i][j] = 45;
            }
            else if(67.5 <= angle && angle <= 112.5) {
                angles[i][j] = 90;
            }
            else if(112.5 <= angle && angle <= 157.5) {
                angles[i][j] = 135;
            }
            else if(angle >= 157.5) {
                angles[i][j] = 0;
            }
        }
    }

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(angles[i][j] == 0) {
                int max = 0;

                if(j > 0) {
                    if(edges[i][j - 1].getR() > max) {
                        max = edges[i][j - 1].getR();
                    }
                }
                if(j < width - 1) {
                    if(edges[i][j + 1].getR() > max) {
                        max = edges[i][j + 1].getR();
                    }
                }

                if(edges[i][j].getR() < max) {
                    edges[i][j] = Pixel(0, 0, 0);
                }
            }
            else if(angles[i][j] == 45) {
                int max = 0;

                if(i < height - 1 && j > 0) {
                    if(edges[i + 1][j - 1].getR() > max) {
                        max = edges[i + 1][j - 1].getR();
                    }
                }
                if(i > 0 && j < width - 1) {
                    if(edges[i - 1][j + 1].getR() > max) {
                        max = edges[i - 1][j + 1].getR();
                    }
                }

                if(edges[i][j].getR() < max) {
                    edges[i][j] = Pixel(0, 0, 0);
                }
            }
            else if(angles[i][j] == 90) {
                int max = 0;

                if(i > 0) {
                    if(edges[i - 1][j].getR() > max) {
                        max = edges[i - 1][j].getR();
                    }
                }
                if(i < height - 1) {
                    if(edges[i + 1][j].getR() > max) {
                        max = edges[i + 1][j].getR();
                    }
                }

                if(edges[i][j].getR() < max) {
                    edges[i][j] = Pixel(0, 0, 0);
                }
            }
            else if(angles[i][j] == 135) {
                int max = 0;

                if(i < height - 1 && j < width - 1) {
                    if(edges[i + 1][j + 1].getR() > max) {
                        max = edges[i + 1][j + 1].getR();
                    }
                }
                if(i > 0 && j > 0) {
                    if(edges[i - 1][j - 1].getR() > max) {
                        max = edges[i - 1][j - 1].getR();
                    }
                }

                if(edges[i][j].getR() < max) {
                    edges[i][j] = Pixel(0, 0, 0);
                }
            }
        }
    }

    // apply thresholding and hysteresis

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(edges[i][j].getR() > threshold2) {
                edges[i][j] = Pixel(255, 255, 255);
            } 
            else if(edges[i][j].getR() > threshold1) {
                edges[i][j] = Pixel(128, 128, 128);
            }
            else {
                edges[i][j] = Pixel(0, 0, 0);
            }
        }
    }

    // vector<vector<int>> visited(height, vector<int>(width, 0));
    // vector<vector<Pixel>> new_edges(height, vector<Pixel>(width, Pixel(0, 0, 0)));


    vector<vector<int>> visited(height, vector<int>(width, 0));

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(edges[i][j].getR() == 255) {
                hysteresis(edges, i, j, visited);
            }
        }
    }

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(visited[i][j] == 1) {
                edges[i][j] = Pixel(255, 255, 255);
            }
            else {
                edges[i][j] = Pixel(0, 0, 0);
            }
        }
    }

    for(int j = 0; j < width; j++) {
        edges[0][j] = Pixel(0, 0, 0);
        edges[height - 1][j] = Pixel(0, 0, 0);
        edges[1][j] = Pixel(0, 0, 0);
        edges[height - 2][j] = Pixel(0, 0, 0);

        x_edges[0][j] = Pixel(0, 0, 0);
        x_edges[height - 1][j] = Pixel(0, 0, 0);
        x_edges[1][j] = Pixel(0, 0, 0);
        x_edges[height - 2][j] = Pixel(0, 0, 0);

        y_edges[0][j] = Pixel(0, 0, 0);
        y_edges[height - 1][j] = Pixel(0, 0, 0);
        y_edges[1][j] = Pixel(0, 0, 0);
        y_edges[height - 2][j] = Pixel(0, 0, 0);
    }

    for(int i = 0; i < edges.size(); i++) {
        edges[i][0] = Pixel(0, 0, 0);
        edges[i][width - 1] = Pixel(0, 0, 0);
        edges[i][1] = Pixel(0, 0, 0);
        edges[i][width - 2] = Pixel(0, 0, 0);

        x_edges[i][0] = Pixel(0, 0, 0);
        x_edges[i][width - 1] = Pixel(0, 0, 0);
        x_edges[i][1] = Pixel(0, 0, 0);
        x_edges[i][width - 2] = Pixel(0, 0, 0);

        y_edges[i][0] = Pixel(0, 0, 0);
        y_edges[i][width - 1] = Pixel(0, 0, 0);
        y_edges[i][1] = Pixel(0, 0, 0);
        y_edges[i][width - 2] = Pixel(0, 0, 0);
    }

    // output final edge detections to imagef.ppm
    ofstream out4("imagef.ppm");
    out4 << "P3" << endl;
    out4 << width << " " << height << endl;
    out4 << "1" << endl;
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(edges[i][j].getR() == 0) {
                out4 << 0 << " " << 0 << " " << 0 << " ";
            }
            else {
                out4 << 1 << " " << 1 << " " << 1 << " ";
            }
        }
        out4 << endl;
    }
    out4.close();

    int **votes;
    votes = new int*[height];
    
    for(int i = 0; i < height; i++) {
        votes[i] = new int[width];

        // initialize all values in pixel array to default value of 0 (white pixel)
        for(int j = 0; j < width; j++) {
            votes[i][j] = 0;
        }
    }

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(edges[i][j].getR() != 0) { 
                double angle = atan2(y_edges[i][j].getR(), x_edges[i][j].getR()) * 180 / pi;
                double slope = sin(angle * pi / 180) / cos(angle * pi / 180);

                Line l(Point(i, j), slope, height, width);
                Line x = l.make_perpendicular(Point(i, j));
                
                
                x.draw_line(votes);
            }
        }
    }

    for(int j = 0; j < width; j++) {
        votes[0][j] = 0;
        votes[height - 1][j] = 0;
        votes[1][j] = 0;
        votes[height - 2][j] = 0;
    }

    for(int i = 0; i < height; i++) {
        votes[i][0] = 0;
        votes[i][width - 1] = 0;
        votes[i][1] = 0;
        votes[i][width - 2] = 0;
    }

    // show the lines perpendicular to circle when drawing final voting results
    int max = 0;

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(votes[i][j] > max) {
                max = votes[i][j];
            }
        }
    }

    ofstream out5("imagev.ppm");
    out5 << "P3" << endl;
    out5 << width << " " << height << endl;
    out5 << max << endl;
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            out5 << votes[i][j] << " " << votes[i][j] << " " << votes[i][j] << " ";
        }
        out5 << endl;
    }
    out5.close();

    // threshold votes

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(votes[i][j] < center_threshold) {
                votes[i][j] = 0;
            }
        }
    }

    vector<vector<Pixel>> output = readPPM(filename);

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(votes[i][j] != 0) {
                Circle c(Point(i, j), 1, height, width);
                Circle c1(Point(i, j), 2, height, width);
                Circle c2(Point(i, j), 3, height, width);
                Circle c3(Point(i, j), 4, height, width);

                c.draw_circle(output);
                c1.draw_circle(output);
                c2.draw_circle(output);
                c3.draw_circle(output);
            }
        }
    }

    ofstream out6("imageCC.ppm");
    out6 << "P3" << endl;
    out6 << width << " " << height << endl;
    out6 << 255 << endl;
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            out6 << output[i][j].getR() << " " << output[i][j].getB() << " " << output[i][j].getG() << " ";
        }
        out6 << endl;
    }
    out6.close();


    for(int i = 0; i < height; i++) {
        delete[] votes[i];
    }

    delete[] votes;

}

int main(int argc, char** argv) {
    if(argc > 8) {
        part1(stoi(argv[2]), stoi(argv[4]), argv[6], stoi(argv[8]));
    }
    else if(argc > 6) { 
        part1(stoi(argv[2]), stoi(argv[4]), argv[6]);
    }
    else if(argc > 4) {
        part1(stoi(argv[2]), stoi(argv[4]));
    }
    else if(argc > 2) {
        part1(stoi(argv[2]));
    }
    else {
        part1();
    }
}