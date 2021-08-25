// Aditya Vasantharao, pd. 3

#include <iostream>
#include <stdlib.h>
#include <ctime>

using namespace std;

// set pixel to be 1 (black pixel)
void color_pixel(int **pixels, int x, int y) {
    pixels[x][y] = 1;
}

int main() {
    srand((unsigned) time(0));
    int height = 800;
    int width = 800;    
    
    // allocate memory for pixel array
    // cleaner to keep the data type as int** and dynamically allocate
    // rather than making it fixed width and passing around int[][800] in functions

    int **pixels;
    pixels = new int* [height];
    
    for(int i = 0; i < height; i++) {
        pixels[i] = new int[width];

        // initialize all values in pixel array to default value of 0 (white pixel)
        for(int j = 0; j < width; j++) {
            pixels[i][j] = 0;
        }
    }


    cout << pixels[0][0] << "s;ldf" << endl;
    color_pixel(pixels, 0, 0);

    cout << pixels[0][0] << "try 2" << endl;
    cout << "sdklfj" << endl;

    // cleanup

    for(int i = 0; i < height; i++) {
        delete[] pixels[i];
    }

    delete[] pixels;

}