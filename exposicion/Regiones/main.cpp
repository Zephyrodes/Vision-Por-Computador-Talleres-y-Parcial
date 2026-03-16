#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

int main() {
    // ------------------------------------------------------------
    // PASO 1: Cargar imagen y umbral de Otsu
    // ------------------------------------------------------------
    Mat img = imread("../coins.png");
    if (img.empty()) {
        cerr << "Error: no se pudo cargar coins.png" << endl;
        return -1;
    }
    cout << "Imagen cargada: " << img.cols << "x" << img.rows << endl;

    namedWindow("1 - Original", WINDOW_NORMAL);
    imshow("1 - Original", img);

    Mat gray;
    cvtColor(img, gray, COLOR_BGR2GRAY);
    Mat thresh;
    threshold(gray, thresh, 0, 255, THRESH_BINARY_INV + THRESH_OTSU);

    namedWindow("2 - Thresh Otsu", WINDOW_NORMAL);
    imshow("2 - Thresh Otsu", thresh);

    // ------------------------------------------------------------
    // PASO 2: Eliminación de ruido con apertura morfológica
    // ------------------------------------------------------------
    Mat kernel = Mat::ones(3, 3, CV_8U);
    Mat opening;
    morphologyEx(thresh, opening, MORPH_OPEN, kernel, Point(-1,-1), 2);

    namedWindow("3 - Opening (sin ruido)", WINDOW_NORMAL);
    imshow("3 - Opening (sin ruido)", opening);

    // ------------------------------------------------------------
    // PASO 3: Fondo seguro (dilatación)
    // ------------------------------------------------------------
    Mat sure_bg;
    dilate(opening, sure_bg, kernel, Point(-1,-1), 3);

    namedWindow("4 - Sure Background", WINDOW_NORMAL);
    imshow("4 - Sure Background", sure_bg);

    // ------------------------------------------------------------
    // PASO 4: Frente seguro (transformada de distancia)
    // ------------------------------------------------------------
    Mat dist_transform;
    distanceTransform(opening, dist_transform, DIST_L2, 5);

    // Normalizar para visualización
    Mat dist_vis;
    normalize(dist_transform, dist_vis, 0, 255, NORM_MINMAX, CV_8U);

    namedWindow("5 - Distance Transform", WINDOW_NORMAL);
    imshow("5 - Distance Transform", dist_vis);

    Mat sure_fg;
    double maxVal;
    minMaxLoc(dist_transform, nullptr, &maxVal);
    threshold(dist_transform, sure_fg, 0.7 * maxVal, 255, THRESH_BINARY);
    sure_fg.convertTo(sure_fg, CV_8U);

    namedWindow("6 - Sure Foreground", WINDOW_NORMAL);
    imshow("6 - Sure Foreground", sure_fg);

    // ------------------------------------------------------------
    // PASO 5: Región desconocida
    // ------------------------------------------------------------
    Mat unknown;
    subtract(sure_bg, sure_fg, unknown);

    namedWindow("7 - Region Desconocida", WINDOW_NORMAL);
    imshow("7 - Region Desconocida", unknown);

    // ------------------------------------------------------------
    // PASO 6: Etiquetado de marcadores (imagen fondo azul)
    // ------------------------------------------------------------
    Mat markers;
    int numComponents = connectedComponents(sure_fg, markers);
    cout << "Componentes encontrados: " << numComponents << endl;

    markers = markers + 1;
    markers.setTo(0, unknown == 255);

    // Visualizar marcadores coloreados (fondo azul)
    Mat markerVis = Mat::zeros(markers.size(), CV_8UC3);
    markerVis.setTo(Vec3b(100, 0, 0), markers == 1);  // fondo azul oscuro
    RNG rng(42);
    for (int i = 2; i <= numComponents; i++) {
        Vec3b color(rng.uniform(0,255), rng.uniform(0,255), rng.uniform(0,255));
        markerVis.setTo(color, markers == i);
    }

    namedWindow("8 - Markers coloreados", WINDOW_NORMAL);
    imshow("8 - Markers coloreados", markerVis);

    // ------------------------------------------------------------
    // PASO 7: Aplicar Watershed
    // ------------------------------------------------------------
    watershed(img, markers);

    // Segmentación coloreada (fondo rojo)
    Mat segmented = Mat::zeros(markers.size(), CV_8UC3);
    segmented.setTo(Vec3b(0, 0, 180), markers == 1);   // fondo rojo
    segmented.setTo(Vec3b(0, 0, 0),   markers == -1);  // bordes negros
    RNG rng2(123);
    for (int i = 2; i <= numComponents; i++) {
        Vec3b color(rng2.uniform(0,255), rng2.uniform(0,255), rng2.uniform(0,255));
        segmented.setTo(color, markers == i);
    }

    namedWindow("9 - Segmentacion coloreada", WINDOW_NORMAL);
    imshow("9 - Segmentacion coloreada", segmented);

    // Resultado final: imagen original con bordes rojos
    img.setTo(Scalar(0, 0, 255), markers == -1);

    namedWindow("10 - Resultado Watershed", WINDOW_NORMAL);
    imshow("10 - Resultado Watershed", img);

    cout << "Listo! " << numComponents - 1 << " objetos detectados." << endl;

    waitKey(0);
    return 0;
}
