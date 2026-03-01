# Vision Por Computador Talleres y Parcial

## Instalación y ejecución

Pasos para clonar el repositorio, instalar las dependencias y ejecutar uno de los talleres en una máquina virtual o sistema Linux compatible.

### 1. Clonar el repositorio

Descargue el proyecto desde GitHub:

```bash
git clone https://github.com/Zephyrodes/Vision-Por-Computador-Talleres-y-Parcial
```

### 2. Entrar al directorio del proyecto

```bash
cd Vision-Por-Computador-Talleres-y-Parcial
```

### 3. Dar permisos de ejecución a los scripts

Esto permite ejecutar los scripts de instalación y ejecución:

```bash
chmod +x setup.sh run.sh
```

### 4. Instalar dependencias y compilar el proyecto

El siguiente script instalará automáticamente todas las dependencias necesarias, incluyendo OpenCV, y compilará los ejecutables:

```bash
./setup.sh
```

### 5. Ejecutar un taller

Puede ejecutar un taller específico usando el script `run.sh` seguido del nombre del taller.

Ejemplo:

```bash
./run.sh taller1
```

También puede ejecutar:

```bash
./run.sh taller2
```

### Resultado

Después de la compilación, los ejecutables se generan en el directorio `build/`. El script `run.sh` permite ejecutar cada taller de forma independiente sin necesidad de recompilar.
