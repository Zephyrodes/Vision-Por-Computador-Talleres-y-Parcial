#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <iostream>

using namespace cv;
using namespace std;

// =========================
// FUNCION: Dibuja bounding box y centro de masa dado una imagen y su mascara
// =========================
Mat dibujarBoundingBox(const Mat& imagen, const vector<vector<Point>>& contornos, int idxMayor)
{
    Mat resultado;
    cvtColor(imagen, resultado, COLOR_GRAY2BGR);

    if (!contornos.empty())
    {
        Rect box = boundingRect(contornos[idxMayor]);
        rectangle(resultado, box, Scalar(0, 255, 0), 2);

        Moments m = moments(contornos[idxMayor]);
        int cx = int(m.m10 / m.m00);
        int cy = int(m.m01 / m.m00);

        circle(resultado, Point(cx, cy), 5, Scalar(0, 0, 255), -1);

        cout << "Centro de masa (" << resultado.cols << "x" << resultado.rows << "): "
             << "(" << cx << ", " << cy << ")" << endl;
    }

    return resultado;
}

// =========================
// FUNCION: Encuentra el indice del contorno con mayor area
// =========================
int encontrarContornoMayor(const vector<vector<Point>>& contornos)
{
    int idxMayor = 0;
    double areaMayor = contourArea(contornos[0]);

    for (int i = 1; i < (int)contornos.size(); i++)
    {
        double area = contourArea(contornos[i]);
        if (area > areaMayor)
        {
            areaMayor = area;
            idxMayor = i;
        }
    }

    return idxMayor;
}

int main()
{
    // =========================
    // EJERCICIO 1 y 2
    // =========================

    Mat img = imread("../Data/lena.jpg");

    if (img.empty())
    {
        cout << "No hay imagen lena" << endl;
        return -1;
    }

    resize(img, img, Size(250, 250));

    // Ejercicio 1
    Mat fila1, fila2, imagenFinal;

    hconcat(vector<Mat>{img, img, img}, fila1);
    hconcat(vector<Mat>{img, img, img}, fila2);
    vconcat(fila1, fila2, imagenFinal);

    imshow("Ejercicio 1", imagenFinal);

    // Ejercicio 2A
    Mat imgEj2A = img.clone();

    int x = img.cols / 2 - 50;
    int y = img.rows / 2 - 50;
    Rect region(x, y, 100, 100);

    Mat grayA, grayABGR;
    cvtColor(imgEj2A(region), grayA, COLOR_BGR2GRAY);
    cvtColor(grayA, grayABGR, COLOR_GRAY2BGR);
    grayABGR.copyTo(imgEj2A(region));

    // Ejercicio 2B
    Mat imgEj2B = img.clone();

    Mat grayB, binary, binaryBGR;
    cvtColor(imgEj2B(region), grayB, COLOR_BGR2GRAY);
    threshold(grayB, binary, 127, 255, THRESH_BINARY);
    cvtColor(binary, binaryBGR, COLOR_GRAY2BGR);
    binaryBGR.copyTo(imgEj2B(region));

    Mat resultadoEj2;
    hconcat(imgEj2A, imgEj2B, resultadoEj2);
    imshow("Ejercicio 2", resultadoEj2);

    // =========================
    // CARGA Y MASCARA (compartida entre ejercicio 3 y 4)
    // =========================

    Mat imgA = imread("../Data/imA.bmp", IMREAD_GRAYSCALE);
    Mat imgB = imread("../Data/imB.png", IMREAD_GRAYSCALE);

    if (imgA.empty() || imgB.empty())
    {
        cout << "No hay imagenes" << endl;
        return -1;
    }

    // Diferencia, binarizado y limpieza de ruido
    Mat diff, binaria;
    absdiff(imgA, imgB, diff);
    threshold(diff, binaria, 30, 255, THRESH_BINARY);
    morphologyEx(binaria, binaria, MORPH_CLOSE,
                 getStructuringElement(MORPH_RECT, Size(5, 5)));

    // Contornos y contorno mayor
    vector<vector<Point>> contornos;
    findContours(binaria, contornos, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    int idxMayor = encontrarContornoMayor(contornos);

    // =========================
    // EJERCICIO 3 - Silueta brillante (diff) sobre fondo negro
    // =========================

    Mat imagenC3 = Mat::zeros(imgA.size(), imgA.type());
    diff.copyTo(imagenC3, binaria);
    imshow("Ejercicio 3 - Bounding Box y Centro",
           dibujarBoundingBox(imagenC3, contornos, idxMayor));

    // =========================
    // EJERCICIO 4 - Grises originales (imgA) sobre fondo blanco
    // =========================

    Mat imagenC4(imgA.size(), imgA.type(), Scalar(255));
    imgA.copyTo(imagenC4, binaria);
    imshow("Ejercicio 4 - Bounding Box y Centro",
           dibujarBoundingBox(imagenC4, contornos, idxMayor));

    waitKey(0);
    return 0;
}
