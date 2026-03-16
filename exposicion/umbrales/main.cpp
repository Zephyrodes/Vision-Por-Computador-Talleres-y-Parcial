#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>

using namespace cv;
using namespace std;

/**
 * Segmentación de imágenes usando el modelo ICM (Intersecting Cortical Model)
 */
int main() {
    // Parámetros fijos
    string imgPath = "../tulips.png";
    int MAX_ITERATION = 20;
    float h = 20.0f;

    // Cargar imagen
    Mat img = imread(imgPath);
    if (img.empty()) {
        cerr << "Error: No se pudo cargar la imagen " << imgPath << endl;
        cerr << "Asegúrate de que 'tulips.png' esté en el directorio de trabajo del proyecto." << endl;
        return -1;
    }

    cout << "Imagen cargada: " << img.cols << "x" << img.rows << endl;
    cout << "Parámetros: Iteraciones=" << MAX_ITERATION << ", h=" << h << endl;

    // Mostrar imagen original
    imshow("Original", img);

    // ------------------------------------------------------------
    // Inicializar parámetros del modelo ICM
    // ------------------------------------------------------------
    float delta = 1.0f / MAX_ITERATION;

    Mat net = Mat::zeros(img.rows, img.cols, CV_32FC1);
    Mat output = Mat::zeros(img.rows, img.cols, CV_32FC1);
    Mat threshold = Mat::ones(img.rows, img.cols, CV_32FC1);
    Mat image = Mat::zeros(img.rows, img.cols, CV_32FC1);

    vector<Mat> iterationsOutput;

    // ------------------------------------------------------------
    // Normalizar imagen a escala de grises en rango [0, 1]
    // ------------------------------------------------------------
    Mat gray;
    cvtColor(img, gray, COLOR_BGR2GRAY);
    gray.convertTo(image, CV_32FC1, 1.0 / 255.0);

    // ------------------------------------------------------------
    // Iterar el modelo ICM
    // ------------------------------------------------------------
    for (int k = 1; k <= MAX_ITERATION; k++) {
        cout << "Iteración " << k << "/" << MAX_ITERATION << endl;

        float lamda = 1.0f / (MAX_ITERATION + k);

        for (int i = 1; i < image.rows - 1; i++) {
            for (int j = 1; j < image.cols - 1; j++) {
                float centerVal = image.at<float>(i, j);

                float weight =
                    (1 - abs(centerVal - image.at<float>(i-1, j))) +
                    (1 - abs(centerVal - image.at<float>(i+1, j))) +
                    (1 - abs(centerVal - image.at<float>(i, j-1))) +
                    (1 - abs(centerVal - image.at<float>(i, j+1))) +
                    0.5f * (1 - abs(centerVal - image.at<float>(i-1, j-1))) +
                    0.5f * (1 - abs(centerVal - image.at<float>(i-1, j+1))) +
                    0.5f * (1 - abs(centerVal - image.at<float>(i+1, j-1))) +
                    0.5f * (1 - abs(centerVal - image.at<float>(i+1, j+1)));

                net.at<float>(i, j) = net.at<float>(i, j) - lamda + centerVal + weight;

                threshold.at<float>(i, j) = threshold.at<float>(i, j) - delta +
                                           h * output.at<float>(i, j);

                output.at<float>(i, j) = (net.at<float>(i, j) > threshold.at<float>(i, j)) ? 1.0f : 0.0f;
            }
        }

        Mat iterOutput;
        output.convertTo(iterOutput, CV_8U, 255.0);
        iterationsOutput.push_back(iterOutput.clone());
    }

    // ------------------------------------------------------------
    // Mostrar resultados
    // ------------------------------------------------------------
    int colsPerRow = 5;
    int rowsGrid = (int)ceil((double)iterationsOutput.size() / colsPerRow);
    Mat grid(rowsGrid * img.rows, colsPerRow * img.cols, CV_8U);
    grid.setTo(0);

    for (size_t idx = 0; idx < iterationsOutput.size(); idx++) {
        int r = idx / colsPerRow;
        int c = idx % colsPerRow;
        Rect roi(c * img.cols, r * img.rows, img.cols, img.rows);
        iterationsOutput[idx].copyTo(grid(roi));
    }

    namedWindow("Evolucion ICM", WINDOW_NORMAL);
    imshow("Evolucion ICM", grid);

    namedWindow("Segmentacion Final ICM", WINDOW_NORMAL);
    imshow("Segmentacion Final ICM", iterationsOutput.back());

    waitKey(0);
    return 0;
}
