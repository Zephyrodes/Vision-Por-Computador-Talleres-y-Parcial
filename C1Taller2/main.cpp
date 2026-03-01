#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main()
{
    // =========================
    // PUNTO 1 y 2
    // =========================

    Mat src = imread("../Data/tutor.png");
    if (src.empty()) return -1;

    Mat gray;
    cvtColor(src, gray, COLOR_BGR2GRAY);

    Mat binary;
    threshold(gray, binary, 180, 255, THRESH_BINARY);

    Mat kernel5 = getStructuringElement(MORPH_ELLIPSE, Size(5,5));

    Mat dilatado, erosionado;
    dilate(binary, dilatado, kernel5);
    erode(binary, erosionado, kernel5);

    Mat dilatadoBGR, erosionadoBGR;
    cvtColor(dilatado, dilatadoBGR, COLOR_GRAY2BGR);
    cvtColor(erosionado, erosionadoBGR, COLOR_GRAY2BGR);

    Mat ventanaFinal;
    vector<Mat> imgs1 = {src, dilatadoBGR, erosionadoBGR};
    vconcat(imgs1, ventanaFinal);

    imshow("Base / Dilatado / Erosionado", ventanaFinal);

    // =========================
    // PUNTO 3
    // =========================

    Mat ave2 = imread("../Data/ave2.png", IMREAD_GRAYSCALE);
    if (ave2.empty()) return -1;

    Mat kernel10 = getStructuringElement(MORPH_ELLIPSE, Size(10,10));

    Mat ave2Limpia;
    morphologyEx(ave2, ave2Limpia, MORPH_OPEN, kernel10);

    Mat ave2BGR, ave2LimpiaBGR;
    cvtColor(ave2, ave2BGR, COLOR_GRAY2BGR);
    cvtColor(ave2Limpia, ave2LimpiaBGR, COLOR_GRAY2BGR);

    vector<Mat> imgs3 = {ave2BGR, ave2LimpiaBGR};
    Mat ventanaP3;
    vconcat(imgs3, ventanaP3);

    imshow("Ruido Externo", ventanaP3);

    // =========================
    // PUNTO 4
    // =========================

    Mat ave = imread("../Data/ave.jpg", IMREAD_GRAYSCALE);
    if (ave.empty()) return -1;

    Mat aveLimpia;
    morphologyEx(ave, aveLimpia, MORPH_CLOSE, kernel10);

    Mat aveBGR, aveLimpiaBGR;
    cvtColor(ave, aveBGR, COLOR_GRAY2BGR);
    cvtColor(aveLimpia, aveLimpiaBGR, COLOR_GRAY2BGR);

    vector<Mat> imgs4 = {aveBGR, aveLimpiaBGR};
    Mat ventanaP4;
    vconcat(imgs4, ventanaP4);

    imshow("Ruido Interno", ventanaP4);

    // =========================
    // PUNTO 5
    // =========================

    Mat imgBGR = imread("../Data/imagenBGR.png");
    if (imgBGR.empty()) return -1;

    vector<Mat> canales;
    split(imgBGR, canales);

    Mat zeros = Mat::zeros(imgBGR.size(), CV_8UC1);

    Mat soloR, soloG, soloB;
    merge({zeros, zeros, canales[2]}, soloR);
    merge({zeros, canales[1], zeros}, soloG);
    merge({canales[0], zeros, zeros}, soloB);

    Mat ventanaP5;
    vector<Mat> imgs5 = {soloR, soloG, soloB};
    hconcat(imgs5, ventanaP5);

    imshow("Canales", ventanaP5);

    // =========================
    // PUNTO 6
    // =========================

    Mat lenaNoise = imread("../Data/lenanoise.png", IMREAD_GRAYSCALE);
    if (lenaNoise.empty()) return -1;

    resize(lenaNoise, lenaNoise, Size(400,400));

    Mat lenaLimpia;
    medianBlur(lenaNoise, lenaLimpia, 5);

    Mat lenaNoiseBGR, lenaLimpiaBGR;
    cvtColor(lenaNoise, lenaNoiseBGR, COLOR_GRAY2BGR);
    cvtColor(lenaLimpia, lenaLimpiaBGR, COLOR_GRAY2BGR);

    vector<Mat> imgs6 = {lenaNoiseBGR, lenaLimpiaBGR};
    Mat ventanaP6;
    vconcat(imgs6, ventanaP6);

    imshow("Mediana", ventanaP6);

    // =========================
    // PUNTO 7
    // =========================

    Mat mes = imread("../Data/mes.jpg");
    if (mes.empty()) return -1;

    Mat hsv;
    cvtColor(mes, hsv, COLOR_BGR2HSV);

    Mat grayMes, grayMesBGR;
    cvtColor(mes, grayMes, COLOR_BGR2GRAY);
    cvtColor(grayMes, grayMesBGR, COLOR_GRAY2BGR);

    auto resaltarColor = [&](double h, double r)
    {
        double h1 = fmod(h - r/2 + 180, 180);
        double h2 = fmod(h + r/2 + 180, 180);

        Mat mask;

        if(h1 <= h2)
            inRange(hsv, Scalar(h1,50,50), Scalar(h2,255,255), mask);
        else
        {
            Mat m1, m2;
            inRange(hsv, Scalar(h1,50,50), Scalar(179,255,255), m1);
            inRange(hsv, Scalar(0,50,50), Scalar(h2,255,255), m2);
            bitwise_or(m1,m2,mask);
        }

        Mat result = grayMesBGR.clone();
        mes.copyTo(result, mask);

        return result;
    };

    Mat verde = resaltarColor(60,40);
    Mat azul  = resaltarColor(110,20);
    Mat rojo  = resaltarColor(170,20);

    Mat ventanaP7;
    vector<Mat> imgs7 = {verde, azul, rojo};
    hconcat(imgs7, ventanaP7);

    imshow("HSV", ventanaP7);

    waitKey(0);
    return 0;
}
