cmake_minimum_required(VERSION 2.8)

project(C1Taller2)

# Buscar OpenCV
find_package(OpenCV REQUIRED)

# Incluir directorios de OpenCV
include_directories(${OpenCV_INCLUDE_DIRS})

# Crear ejecutable
add_executable(${PROJECT_NAME} main.cpp)

# Enlazar librerías de OpenCV
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

