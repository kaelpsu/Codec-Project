#pragma once

#include <vector>
#include <cmath>

// Um Block8x8 é uma matriz (vetor de vetores) contendo doubles
typedef std::vector<std::vector<double>> Block8x8;

class SpatialTransformation {
private:
    Block8x8 quantizationMatrix;
    const double PI = 3.14159265358979323846;

    // Função matemática auxiliar da equação da DCT
    double calculateC(int val);

public:
    // Construtor: vai carregar a matriz de quantização padrão
    SpatialTransformation();

    // Recebe um bloco 8x8 de pixels espaciais e retorna um bloco 8x8 de frequências
    Block8x8 applyDCT(const Block8x8& pixelBlock);

    // Recebe o bloco DCT e divide os valores pela matriz de quantização (compressão com perda)
    Block8x8 applyQuantization(const Block8x8& dctBlock);
};