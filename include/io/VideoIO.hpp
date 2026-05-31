#pragma once

#include <opencv2/opencv.hpp>
#include <string>
#include <fstream>
#include <iostream>

// Estrutura que representa um Frame após a subamostragem de cor (4:2:0)
struct FrameYUV {
    cv::Mat Y;  // Luminância (Tamanho original: Largura x Altura)
    cv::Mat Cr; // Crominância Vermelha (Tamanho reduzido: Largura/2 x Altura/2)
    cv::Mat Cb; // Crominância Azul (Tamanho reduzido: Largura/2 x Altura/2)
};

class VideoIO {
private:
    cv::VideoCapture cap;
    std::ofstream bitstream;
    
    int width;
    int height;
    double fps;
    int totalFrames;

public:
    // Construtor
    VideoIO(const std::string& inputPath, const std::string& outputPath);
    
    // Destrutor
    ~VideoIO();

    // Lê o próximo frame do vídeo, faz a conversão de cor e a subamostragem 4:2:0
    bool readNextFrame(FrameYUV& outFrame);

    // Grava o cabeçalho customizado no início do arquivo comprimido
    void writeHeader();

    // Getters úteis
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    double getFps() const { return fps; }
};