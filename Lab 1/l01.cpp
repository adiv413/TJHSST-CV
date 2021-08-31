// Aditya Vasantharao, pd. 3

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <ctime>
#include <utility>
#include <algorithm>

using namespace std;

// set pixel to be 1 (black pixel)
void color_pixel(int **pixels, int x, int y) {
    pixels[y][x] = 1; // reverse the order because of arrays
}

// write the board to a ppm file
void write_board(int **pixels, int height, int width) {
    ofstream outfile;
    outfile.open("output.ppm");

    // header
    outfile << "P3 " << width << " " << height << " " << 255 << endl;

    // content
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(pixels[i][j] == 0) {
                // ppm files require one output for r, g, and b
                outfile << "0 0 0 ";
            }

            if(pixels[i][j] == 1) { // 1 represents a black pixel
                outfile << "255 255 255 ";
            }
        }
        outfile << endl;
    } 

    outfile.close();
}

// bresenham algorithm to draw a line between two given points
// note: the points must be in increasing order if x1 < x2 and y1 < y2
void _draw_line_bresenham(int **pixels, pair<int, int> p1, pair<int, int> p2) {
    int dx = p2.first - p1.first; // x2 - x1
    int dy = p2.second - p1.second; // y2 - y1
    int error = abs(dy) - abs(dx);  // negative: x driven, positive: y driven

    if(error < 0) { // x-driven
        int j = p1.second; // y1

        if(dy < 0) { // going down
            for(int i = p1.first; i <= p2.first; i++) {
                color_pixel(pixels, i, j);

                if(error >= 0) {
                    j -= 1;
                    error -= dx;
                }
                error += -dy;
            }
        }
        else { // normal case x driven
            for(int i = p1.first; i <= p2.first; i++) {
                color_pixel(pixels, i, j);

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
            int i = p1.first;

            for(int j = p1.second; j <= p2.second; j++) {
                color_pixel(pixels, i, j);

                if(error <= 0) {
                    i -= 1;
                    error += dy;
                }

                error -= -dx;
            }
        }
        else { // normal case y driven
            int i = p1.first;

            for(int j = p1.second; j <= p2.second; j++) {
                color_pixel(pixels, i, j);

                if(error <= 0) {
                    i += 1;
                    error += dy;
                }

                error -= dx;
            }
        }
    }
}

// draw a line between any two given points
void draw_line(int **pixels, pair<int, int> p1, pair<int, int> p2) {
    int dx = p2.first - p1.first; // x2 - x1
    int dy = p2.second - p1.second; // y2 - y1
    int error = abs(dy) - abs(dx);

    if(error < 0 && p2.first <= p1.first) { // reverse the points
        _draw_line_bresenham(pixels, p2, p1);
    }
    else if(error > 0 && p2.second <= p1.second) {
        _draw_line_bresenham(pixels, p2, p1);
    }
    else {
        _draw_line_bresenham(pixels, p1, p2);
    }
}

int main() {
    srand((unsigned) time(0));
    int height = 10;
    int width = 10;    

    // pair<int, int> p1 = {rand() % height, rand() % width};
    // pair<int, int> p2 = {rand() % height, rand() % width};
    // pair<int, int> p3 = {rand() % height, rand() % width};

    // pair<int, int> p1 = {9, 2};
    // pair<int, int> p2 = {1, 2};

    // pair<int, int> p1 = {1, 2};
    // pair<int, int> p2 = {9, 2};

    // pair<int, int> p1 = {0, 0};
    // pair<int, int> p2 = {9, 2};

    // pair<int, int> p1 = {9, 2};
    // pair<int, int> p2 = {0, 0};
        
    // pair<int, int> p1 = {0, 9};
    // pair<int, int> p2 = {9, 2};

    // pair<int, int> p1 = {9, 2};
    // pair<int, int> p2 = {0, 9};

    // pair<int, int> p1 = {2, 2};
    // pair<int, int> p2 = {2, 9};

    // pair<int, int> p1 = {2, 9};
    // pair<int, int> p2 = {2, 2};

    // pair<int, int> p1 = {2, 2};
    // pair<int, int> p2 = {6, 9};

    // pair<int, int> p2 = {2, 2};
    // pair<int, int> p1 = {6, 9};

    // pair<int, int> p1 = {8, 2};
    // pair<int, int> p2 = {2, 9};

    // pair<int, int> p2 = {8, 2};
    // pair<int, int> p1 = {2, 9};

    // allocate memory for pixel array
    // cleaner to keep the data type as int** and dynamically allocate
    // rather than making it fixed width and passing around int[][size] in functions

    int **pixels;
    pixels = new int* [height];
    
    for(int i = 0; i < height; i++) {
        pixels[i] = new int[width];

        // initialize all values in pixel array to default value of 0 (white pixel)
        for(int j = 0; j < width; j++) {
            pixels[i][j] = 0;
        }
    }



    // draw_line(pixels, p1, p2);


    for(int i = 0; i < height; i++) {
        // initialize all values in pixel array to default value of 0 (white pixel)
        for(int j = 0; j < width; j++) {
            cout << pixels[i][j] << " ";
        }
        cout << endl;
    }





    write_board(pixels, height, width);

    // cleanup

    for(int i = 0; i < height; i++) {
        delete[] pixels[i];
    }

    delete[] pixels;

}