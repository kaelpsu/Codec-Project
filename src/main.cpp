#include <iostream>
#include <opencv2/opencv.hpp>
#include "io/VideoIO.hpp"
#include "processing/SpatialTransformation.hpp"

// --- FLAG DE CONTROLE VISUAL ---
// DPermite visualizar os canais separados e uma amostra de bloco quantizado no terminal
// Mudar para 'false' quando for medir a velocidade máxima do codec
bool visualDebugMode = true;

int main() {
    std::string inputVideo = "../data/input/teste.mp4";
    std::string outputBitstream = "../data/output/teste.efk";

    std::cout << "Iniciando o Codec EFK..." << std::endl;

    // Instancia da classe de Entrada/Saida (abre o MP4 e cria o arquivo binario vazio para a saída)
    VideoIO codecIO(inputVideo, outputBitstream);

    // Grava o cabecalho de metadados customizado no arquivo binario
    codecIO.writeHeader();

    SpatialTransformation spatialTransformer; // Instancia da classe de Transformacao Espacial (DCT e Quantizacao)

    // Estrutura para receber os canais separados a cada iteracao
    FrameYUV currentFrame;
    int frameCounter = 0;
    
    if (visualDebugMode) {
        std::cout << "Reproduzindo os canais separados. Selecione a janela do video e SEGURE 'q' ou 'ESC' para sair." << std::endl;
    }

    while (codecIO.readNextFrame(currentFrame)) {
        frameCounter++;

        // Trabalharemos com o Canal Y (Luminância)
        cv::Mat channelY = currentFrame.Y;
        int height = channelY.rows;
        int width = channelY.cols;

        // (Isso será entregue ao módulo de Entropia depois)
        std::vector<Block8x8> quantizedFrameBlocks;

        // Loop de fatiamento em blocos(Pulando de 8 em 8)
        for (int y = 0; y < height; y += 8) {
            for (int x = 0; x < width; x += 8) {
                
                // Extração do Sub-bloco (ROI - Region of Interest)
                // O OpenCV cria uma "janela" 8x8 sem duplicar dados na RAM
                cv::Rect area(x, y, 8, 8);
                cv::Mat subMatrixCV = channelY(area);

                // Conversão de Tipos (De byte para double)
                Block8x8 blocoPixels(8, std::vector<double>(8, 0.0));
                for (int i = 0; i < 8; i++) {
                    for (int j = 0; j < 8; j++) {
                        // IMPORTANTE: O OpenCV guarda pixels em escala de cinza como 'uint8_t' (unsigned char).
                        // Precisamos converter isso para 'double' para a equação da DCT não falhar.
                        blocoPixels[i][j] = static_cast<double>(subMatrixCV.at<uint8_t>(i, j));
                    }
                }

                // 3. A Compressão Matemática
                Block8x8 blocoDCT = spatialTransformer.applyDCT(blocoPixels);
                Block8x8 blocoQuantizado = spatialTransformer.applyQuantization(blocoDCT);

                // 4. Salva o bloco destruído e pronto para empacotamento
                quantizedFrameBlocks.push_back(blocoQuantizado);
            }
        }

        
        // // Condição de saída rápida para testes
        // if (frameCounter >= 5) {
            //     std::cout << "Parando no frame 5 para testes rapidos..." << std::endl;
            //     break; 
            // }
            

        if (visualDebugMode) {
            // Para não travar o terminal com milhares de blocos, imprime o resultado matemático do PRIMEIRO BLOCO do PRIMEIRO FRAME:
            if (frameCounter == 1) {
                std::cout << "\n--- Amostra do Primeiro Bloco Quantizado ---" << std::endl;
                Block8x8 amostra = quantizedFrameBlocks[0];
                for (int i = 0; i < 8; i++) {
                    for (int j = 0; j < 8; j++) {
                        printf("%4.0f ", amostra[i][j]); 
                    }
                    std::cout << std::endl;
                }
                std::cout << "--------------------------------------------\n" << std::endl;
            }

            // Exibe os tres canais em janelas separadas do OpenCV
            // O canal Y aparecera em alta resolucao e preto e branco
            cv::imshow("Canal Y (Luminancia - Brilho/Bordas)", currentFrame.Y);
            
            // Os canais Cr e Cb aparecerao menores (metade da largura e altura)
            cv::imshow("Canal Cr (Crominancia Vermelha)", currentFrame.Cr);
            cv::imshow("Canal Cb (Crominancia Azul)", currentFrame.Cb);

            // Aguarda 30 milissegundos (~33 FPS) para dar tempo de renderizar a imagem na tela
            // Se o usuario apertar 'q' (quit) ou a tecla ESC (codigo 27), interrompe o loop
            char tecla = (char)cv::waitKey(30);
            if (tecla == 'q' || tecla == 27) {
                std::cout << "Reproducao interrompida pelo usuario." << std::endl;
                break;
            }
        }
    }

    std::cout << "Fim do processamento! Total de frames lidos: " << frameCounter << std::endl;
    cv::destroyAllWindows(); // Fecha as janelas do OpenCV

    return 0;
}