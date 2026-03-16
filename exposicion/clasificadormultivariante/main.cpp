#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

int main() {
    // Cargar imagen
    Mat img = imread("../tulips.png");
    if (img.empty()) {
        cerr << "Error: no se pudo cargar la imagen" << endl;
        return -1;
    }

    cout << "Imagen cargada: " << img.cols << "x" << img.rows << endl;

    // ------------------------------------------------------------
    // Preparar datos para k-means
    // ------------------------------------------------------------
    Mat samples = img.reshape(1, img.total());  // total pixeles x 3 canales
    samples.convertTo(samples, CV_32F);

    // Parámetros de k-means
    int K = 3;  // Número de clusters
    TermCriteria criteria(TermCriteria::EPS + TermCriteria::COUNT, 10, 1.0);
    Mat labels, centers;

    cout << "Aplicando k-means con K=" << K << "..." << endl;
    kmeans(samples, K, labels, criteria, 3, KMEANS_PP_CENTERS, centers);

    // Reorganizar etiquetas a la forma de la imagen
    labels = labels.reshape(1, img.rows);  // matriz de etiquetas (filas x columnas)

    // ------------------------------------------------------------
    // Crear mapa de colores brillantes para cada cluster
    // ------------------------------------------------------------
    vector<Vec3b> color_map(K);
    color_map[0] = Vec3b(0, 0, 255);      // Rojo
    if (K > 1) color_map[1] = Vec3b(0, 255, 0);    // Verde
    if (K > 2) color_map[2] = Vec3b(255, 0, 0);    // Azul
    if (K > 3) color_map[3] = Vec3b(0, 255, 255);  // Amarillo
    if (K > 4) color_map[4] = Vec3b(255, 0, 255);  // Magenta
    if (K > 5) color_map[5] = Vec3b(255, 255, 0);  // Cian
    for (int i = 6; i < K; i++) {
        // Colores aleatorios brillantes para clusters adicionales
        color_map[i] = Vec3b(rand() % 156 + 100, rand() % 156 + 100, rand() % 156 + 100);
    }

    // ------------------------------------------------------------
    // Construir imagen segmentada
    // ------------------------------------------------------------
    Mat segmented(img.size(), CV_8UC3);
    for (int r = 0; r < img.rows; r++) {
        for (int c = 0; c < img.cols; c++) {
            int label = labels.at<int>(r, c);
            segmented.at<Vec3b>(r, c) = color_map[label % K];
        }
    }

    // ------------------------------------------------------------
    // Mostrar resultados
    // ------------------------------------------------------------
    namedWindow("Original", WINDOW_NORMAL);
    imshow("Original", img);

    namedWindow("Segmentacion K-Means", WINDOW_NORMAL);
    imshow("Segmentacion K-Means", segmented);

    cout << "Listo! Clusters=" << K << endl;

    waitKey(0);
    return 0;
}
