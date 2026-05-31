#include "io/VideoIO.hpp"

VideoIO::VideoIO(const std::string& inputPath, const std::string& outputPath) {
    // Abre o vídeo original
    cap.open(inputPath);
    if (!cap.isOpened()) {
        std::cerr << "Erro: Nao foi possivel abrir o video de entrada: " << inputPath << std::endl;
        exit(1);
    }

    // Coleta metadados
    width = (int)cap.get(cv::CAP_PROP_FRAME_WIDTH);
    height = (int)cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    fps = cap.get(cv::CAP_PROP_FPS);
    totalFrames = (int)cap.get(cv::CAP_PROP_FRAME_COUNT);

    // Abre o arquivo de saida em modo Binário
    bitstream.open(outputPath, std::ios::binary);
    if (!bitstream.is_open()) {
        std::cerr << "Erro: Nao foi possivel criar o arquivo de saida: " << outputPath << std::endl;
        exit(1);
    }

    std::cout << "Video carregado! Res: " << width << "x" << height << " | FPS: " << fps << std::endl;
}

VideoIO::~VideoIO() {
    if (cap.isOpened()) cap.release();
    if (bitstream.is_open()) bitstream.close();
}

void VideoIO::writeHeader() {
    // Gravando a "assinatura" do nosso formato customizado
    const char magicNumber[4] = {'E', 'F', 'K', '1'};
    bitstream.write(magicNumber, 4);

    // Gravando os metadados (garantindo o tamanho das variaveis com tipos fixos para leitura futura)
    int32_t w = width;
    int32_t h = height;
    float_t f = static_cast<float_t>(fps);

    // Gravando metadados em formato binario no bitstream. (4 bytes para largura, 4 bytes para altura, 4 bytes para fps)
    // Usamos reinterpret_cast para escrever os bytes brutos da variavel.
    bitstream.write(reinterpret_cast<const char*>(&w), sizeof(w));
    bitstream.write(reinterpret_cast<const char*>(&h), sizeof(h));
    bitstream.write(reinterpret_cast<const char*>(&f), sizeof(f));
    
    std::cout << "Cabecalho customizado gravado no bitstream." << std::endl;
}

bool VideoIO::readNextFrame(FrameYUV& outFrame) {
    cv::Mat frameBGR;
    cap >> frameBGR; // Le o proximo frame do .mp4

    // Se o frame estiver vazio, o video acabou
    if (frameBGR.empty()) {
        return false; 
    }

    // Converte de BGR para YCrCb (O padrao do OpenCV equivalente ao YUV)
    cv::Mat frameYCrCb;
    cv::cvtColor(frameBGR, frameYCrCb, cv::COLOR_BGR2YCrCb);

    // Separa os canais
    std::vector<cv::Mat> channels;
    cv::split(frameYCrCb, channels);

    // channels[0] = Y (Luminancia)
    // channels[1] = Cr (Crominancia Vermelha)
    // channels[2] = Cb (Crominancia Azul)

    // O canal Y mantem o tamanho original
    outFrame.Y = channels[0].clone();

    // Subamostragem de cores (4:2:0)
    // Reduz a largura e a altura dos canais de cor pela metade usando interpolacao (a cada bloco de 2x2 pixels, calcula média dos valores e salva resultado em 1 pixel)
    cv::resize(channels[1], outFrame.Cr, cv::Size(width / 2, height / 2), 0, 0, cv::INTER_LINEAR);
    cv::resize(channels[2], outFrame.Cb, cv::Size(width / 2, height / 2), 0, 0, cv::INTER_LINEAR);

    return true;
}