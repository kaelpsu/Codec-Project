#include "processing/SpatialTransformation.hpp"
#include <cmath>

SpatialTransformation::SpatialTransformation() {
    // Matriz de Quantização Clássica (Padrão JPEG para Luminância)
    // Números maiores no canto inferior direito geram mais zeros após a divisão
    quantizationMatrix = {
        {16, 11, 10, 16, 24, 40, 51, 61},
        {12, 12, 14, 19, 26, 58, 60, 55},
        {14, 13, 16, 24, 40, 57, 69, 56},
        {14, 17, 22, 29, 51, 87, 80, 62},
        {18, 22, 37, 56, 68, 109, 103, 77},
        {24, 35, 55, 64, 81, 104, 113, 92},
        {49, 64, 78, 87, 103, 121, 120, 101},
        {72, 92, 95, 98, 112, 100, 103, 99}
    };
}

double SpatialTransformation::calculateC(int val) {
    if (val == 0) {
        return 1.0 / std::sqrt(2.0);
    }
    return 1.0;
}

Block8x8 SpatialTransformation::applyDCT(const Block8x8& pixelsBlock) {
    // Cria um bloco 8x8 vazio preenchido com zeros para guardar o resultado
    Block8x8 dctBlock(8, std::vector<double>(8, 0.0));

    // u e v representam as coordenadas no domínio da frequência
    for (int u = 0; u < 8; u++) {
        for (int v = 0; v < 8; v++) {
            
            double sum = 0.0;
            
            // x e y representam as coordenadas espaciais (os pixels originais)
            for (int x = 0; x < 8; x++) {
                for (int y = 0; y < 8; y++) {
                    // O núcleo da equação da Transformada
                    double cosine1 = std::cos(((2.0 * x + 1.0) * u * PI) / 16.0);
                    double cosine2 = std::cos(((2.0 * y + 1.0) * v * PI) / 16.0);
                    
                    // Centralizamos o pixel (subtraindo 128) para equilibrar a onda em torno do zero
                    double centerPixel = pixelsBlock[x][y] - 128.0;
                    
                    sum += centerPixel * cosine1 * cosine2;
                }
            }
            
            double coeficientC = calculateC(u) * calculateC(v);
            dctBlock[u][v] = 0.25 * coeficientC * sum;
        }
    }

    return dctBlock;
}

Block8x8 SpatialTransformation::applyQuantization(const Block8x8& dctBlock) {
    Block8x8 quantizedBlock(8, std::vector<double>(8, 0.0));

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            // Dividimos o coeficiente da frequência pelo peso da tabela e ARREDONDAMOS.
            // É esse arredondamento que gera a perda de dados irremediável e cria os Zeros.
            quantizedBlock[i][j] = std::round(dctBlock[i][j] / quantizationMatrix[i][j]);
        }
    }

    return quantizedBlock;
}