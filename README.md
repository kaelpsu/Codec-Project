# Codec-Project
Implementing a simplified codec as a project for the Image Processing course.

An experimental video encoder and decoder built from scratch in C++. This project has educational purposes and focuses on demonstrating, in practice, the mathematics and engineering behind digital video compression (spatial and temporal compression).

## The Pipeline (Overview)

The codec transforms videos into a custom binary format (`.efk`) through the following stages:

1. **Input and Pre-processing (`VideoIO`):** The original video (currently, only supports mp4 format) is read frame by frame. The color space is converted from RGB to YCbCr, and **Chroma Subsampling (4:2:0)** is applied, drastically reducing color data while preserving luminance (brightness and edges).

2. **Spatial Compression (I-Frames) - *In development*:** The image is sliced into 8x8 pixel blocks. We apply the **Discrete Cosine Transform (DCT)** to bring the data into the frequency domain, followed by **Quantization** (where controlled data loss occurs) and zero-grouping via *Run-Length Encoding* (RLE).

3. **Temporal Compression (P-Frames) - *In development*:** For moving videos, the algorithm doesn't save the entire image. It performs **Motion Estimation** (searching for patterns in previous frames) and saves only the Motion Vectors and the residual matrix (the subtle differences between frames).

4. **The Final File (`.efk`):** The compressed data is packaged into a custom binary format, protected by the signature (Magic Number) `EFK1`. This ensures the decoder knows exactly how to read the metadata and reconstruct the video on the user's screen.

## Prerequisites

To compile and run the project, you will need to have installed on your machine:
* **C++17 Compiler** (GCC or Clang)
* **CMake** (version 3.10 or higher)
* **OpenCV (version 4.x)** (used exclusively for I/O, color conversion, and testing interface; all compression math is implemented manually).

## How to Compile and Run

1. Clone the repository and enter the project folder;

2. Create the build folder and enter it:

```bash
mkdir build
cd build
```

3. Generate the build files using CMake and compile:
```bash
cmake ..
make
```

4. Run the program:
```bash
./my_codec
```

(Make sure there is a video named teste.mp4 inside the data/input/ directory before running the program).