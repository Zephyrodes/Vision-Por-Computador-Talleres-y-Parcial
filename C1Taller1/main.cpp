#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <iostream>

using namespace cv;
using namespace std;

// =========================
// FUNCION: Dibuja bounding box y centro de masa
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

        cout << "Centro de masa: (" << cx << ", " << cy << ")" << endl;
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
    // CARGA DE IMAGENES
    // =========================

    Mat imgA = imread("../Data/imA.bmp", IMREAD_GRAYSCALE);
    Mat imgB = imread("../Data/imB.png", IMREAD_GRAYSCALE);

    if (imgA.empty() || imgB.empty())
    {
        cout << "No hay imagenes" << endl;
        return -1;
    }

    // =========================
    // EJERCICIO 3 - Silueta brillante (diff) sobre fondo negro
    // =========================

    Mat diff, binaria;
    absdiff(imgA, imgB, diff);
    threshold(diff, binaria, 30, 255, THRESH_BINARY);
    morphologyEx(binaria, binaria, MORPH_CLOSE,
                 getStructuringElement(MORPH_RECT, Size(5, 5)));

    Mat imagenC3 = Mat::zeros(imgA.size(), imgA.type());
    diff.copyTo(imagenC3, binaria);

    vector<vector<Point>> contornos3;
    findContours(binaria, contornos3, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    int idxMayor3 = encontrarContornoMayor(contornos3);

    imshow("Ejercicio 3 - Bounding Box y Centro",
           dibujarBoundingBox(imagenC3, contornos3, idxMayor3));

    // =========================
    // EJERCICIO 4 - Division aritmetica imgA / imgB
    // =========================

    Mat imgAf, imgBf, divResult;
    imgA.convertTo(imgAf, CV_32F);
    imgB.convertTo(imgBf, CV_32F);

    // +1 para evitar division por cero
    imgBf += 1.0f;

    // Division: donde imgA < imgB (triceratops oscuro sobre piso claro) → valor pequeño → oscuro
    //           donde imgA ≈ imgB (piso, otros dinos) → valor ≈ 1 → blanco tras escalar
    divide(imgAf * 255.0f, imgBf, divResult);

    // Clamp a 255 y convertir a 8 bits
    min(divResult, 255.0f, divResult);

    Mat imagenC4;
    divResult.convertTo(imagenC4, CV_8U);

    // Contornos sobre la imagen resultante (umbral bajo porque el triceratops es oscuro)
    Mat binaria4;
    threshold(imagenC4, binaria4, 200, 255, THRESH_BINARY_INV);
    morphologyEx(binaria4, binaria4, MORPH_CLOSE,
                 getStructuringElement(MORPH_RECT, Size(5, 5)));

    vector<vector<Point>> contornos4;
    findContours(binaria4, contornos4, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    int idxMayor4 = encontrarContornoMayor(contornos4);

    imshow("Ejercicio 4 - Bounding Box y Centro",
           dibujarBoundingBox(imagenC4, contornos4, idxMayor4));

    waitKey(0);
    return 0;
}
