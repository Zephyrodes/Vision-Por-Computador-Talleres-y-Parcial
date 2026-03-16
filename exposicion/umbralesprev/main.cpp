#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>

using namespace cv;
using namespace std;

// Función auxiliar para crear una grilla de imágenes
Mat createGrid(vector<Mat>& images, vector<string>& titles, int cols, int cellW, int cellH) {
    int rows = (int)ceil((double)images.size() / cols);
    int titleBarH = 30;
    Mat grid(rows * (cellH + titleBarH), cols * cellW, CV_8U, Scalar(50));

    for (size_t idx = 0; idx < images.size(); idx++) {
        int r = idx / cols;
        int c = idx % cols;

        // Redimensionar imagen al tamaño de celda
        Mat resized;
        resize(images[idx], resized, Size(cellW, cellH));

        // Copiar imagen en la celda
        Rect roi(c * cellW, r * (cellH + titleBarH) + titleBarH, cellW, cellH);
        resized.copyTo(grid(roi));

        // Escribir título
        putText(grid, titles[idx],
                Point(c * cellW + 5, r * (cellH + titleBarH) + 20),
                FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255), 1);
    }
    return grid;
}

// -------------------------------------------------------
// Script 1: Tipos de umbralización global (gradient.png)
// -------------------------------------------------------
void script1() {
    Mat img = imread("../gradient.png", IMREAD_GRAYSCALE);
    if (img.empty()) { cerr << "Error: no se pudo cargar gradient.png" << endl; return; }

    Mat thresh1, thresh2, thresh3, thresh4, thresh5;
    threshold(img, thresh1, 127, 255, THRESH_BINARY);
    threshold(img, thresh2, 127, 255, THRESH_BINARY_INV);
    threshold(img, thresh3, 127, 255, THRESH_TRUNC);
    threshold(img, thresh4, 127, 255, THRESH_TOZERO);
    threshold(img, thresh5, 127, 255, THRESH_TOZERO_INV);

    vector<Mat> images = {img, thresh1, thresh2, thresh3, thresh4, thresh5};
    vector<string> titles = {"Original", "BINARY", "BINARY INV", "TRUNC", "TOZERO", "TOZERO INV"};

    Mat grid = createGrid(images, titles, 3, img.cols, img.rows);

    namedWindow("Script 1 - Umbrales Globales", WINDOW_NORMAL);
    imshow("Script 1 - Umbrales Globales", grid);
}

// -------------------------------------------------------
// Script 2: Umbralización adaptativa (sudoku.png)
// -------------------------------------------------------
void script2() {
    Mat img = imread("../sudoku.png", IMREAD_GRAYSCALE);
    if (img.empty()) { cerr << "Error: no se pudo cargar sudoku.png" << endl; return; }

    medianBlur(img, img, 5);

    Mat th1, th2, th3;
    threshold(img, th1, 127, 255, THRESH_BINARY);
    adaptiveThreshold(img, th2, 255, ADAPTIVE_THRESH_MEAN_C,     THRESH_BINARY, 11, 2);
    adaptiveThreshold(img, th3, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 11, 2);

    vector<Mat> images = {img, th1, th2, th3};
    vector<string> titles = {"Original", "Global (v=127)", "Adaptive Mean", "Adaptive Gaussian"};

    Mat grid = createGrid(images, titles, 2, img.cols, img.rows);

    namedWindow("Script 2 - Umbrales Adaptativos", WINDOW_NORMAL);
    imshow("Script 2 - Umbrales Adaptativos", grid);
}

int main() {
    script1();
    script2();
    waitKey(0);
    return 0;
}
