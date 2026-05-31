#include <iostream>
#include <opencv2/opencv.hpp>
#include "io/VideoIO.hpp"

int main() {
    std::string inputVideo = "../data/input/teste.mp4";
    std::string outputBitstream = "../data/output/teste.efk";

    std::cout << "Iniciando o Codec EFK..." << std::endl;

    // Instancia da classe de Entrada/Saida (abre o MP4 e cria o arquivo binario vazio para a saída)
    VideoIO codecIO(inputVideo, outputBitstream);

    // Grava o cabecalho de metadados customizado no arquivo binario
    codecIO.writeHeader();

    // Estrutura para receber os canais separados a cada iteracao
    FrameYUV frameAtual;
    int contadorFrames = 0;

    // 3. Loop de leitura do video
    std::cout << "Reproduzindo os canais separados. Pressione 'q' ou 'ESC' na janela do video para sair." << std::endl;
    
    while (codecIO.readNextFrame(frameAtual)) {
        contadorFrames++;

        // Exibe os tres canais em janelas separadas do OpenCV
        // O canal Y aparecera em alta resolucao e preto e branco
        cv::imshow("Canal Y (Luminancia - Brilho/Bordas)", frameAtual.Y);
        
        // Os canais Cr e Cb aparecerao menores (metade da largura e altura)
        cv::imshow("Canal Cr (Crominancia Vermelha)", frameAtual.Cr);
        cv::imshow("Canal Cb (Crominancia Azul)", frameAtual.Cb);

        // Aguarda 30 milissegundos (~33 FPS) para dar tempo de renderizar a imagem na tela
        // Se o usuario apertar 'q' (quit) ou a tecla ESC (codigo 27), interrompe o loop
        char tecla = (char)cv::waitKey(30);
        if (tecla == 'q' || tecla == 27) {
            std::cout << "Reproducao interrompida pelo usuario." << std::endl;
            break;
        }
    }

    std::cout << "Fim do processamento! Total de frames lidos: " << contadorFrames << std::endl;
    cv::destroyAllWindows(); // Fecha as janelas do OpenCV

    return 0;
}