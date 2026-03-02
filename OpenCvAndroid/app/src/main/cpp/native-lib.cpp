#include <jni.h>
#include <opencv2/opencv.hpp>

using namespace cv;

extern "C"
JNIEXPORT void JNICALL
Java_com_example_opencvandroid_MainActivity_procesarFrame(
        JNIEnv *env,
        jobject thiz,
        jlong addrFrame,
        jint effectId)
{
    Mat &frame = *(Mat *) addrFrame;

    if (effectId == 1) {
        // Efecto 1: Escala de Grises + Desenfoque (Efecto Suave/Antiguo)
        cvtColor(frame, frame, COLOR_RGBA2GRAY);
        GaussianBlur(frame, frame, Size(7, 7), 1.5);
        cvtColor(frame, frame, COLOR_GRAY2RGBA);
    }
    else if (effectId == 2) {
        // Efecto 2: Detección de Bordes (Efecto Boceto/Sketch)
        Mat gray, edges;
        cvtColor(frame, gray, COLOR_RGBA2GRAY);
        GaussianBlur(gray, gray, Size(3, 3), 0);
        Canny(gray, edges, 50, 150);
        cvtColor(edges, frame, COLOR_GRAY2RGBA);
    }
    else if (effectId == 3) {
        // Efecto 3: Mapa de Calor (Térmico)
        Mat gray, thermal;
        // 1. Convertir a Gris (intensidad de luz)
        cvtColor(frame, gray, COLOR_RGBA2GRAY);
        // 2. Aplicar un mapa de color (COLORMAP_JET o COLORMAP_HOT)
        applyColorMap(gray, thermal, COLORMAP_JET);
        // 3. Convertir de BGR (salida de applyColorMap) a RGBA
        cvtColor(thermal, frame, COLOR_BGR2RGBA);
    }
}
