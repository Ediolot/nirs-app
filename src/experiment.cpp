#include "experiment.h"

Experiment::Experiment(const QString &path)
{
    load(path);
}

void Experiment::load(const QString &path)
{
    assert(sizeof(float ) == 4); // 32 bit Float  // TODO que ambos sean por configuración
    assert(sizeof(double) == 8); // 64 bit Double

    QFile file(path);

    if (!file.open(QIODevice::ReadOnly)) {
        throw FileNotFoundException(path);
    }

    // Width, Height, Bpp. If any fails, throw exception
    if (file.read(reinterpret_cast<char*>(&height), sizeof(uint32_t)) < 0) throw FileReadErrorException(path);
    if (file.read(reinterpret_cast<char*>(&width),  sizeof(uint32_t)) < 0) throw FileReadErrorException(path);
    if (file.read(reinterpret_cast<char*>(&bpp),    sizeof(uint32_t)) < 0) throw FileReadErrorException(path);
    bpp = getStandardBpp(bpp);

    // Frames
    dark = Frame<float>(file, width, height, FrameConstants::NO_TIMESTAMP).cast<double>();
    gain = Frame<float>(file, width, height, FrameConstants::NO_TIMESTAMP).cast<double>();

    int nframes = (file.size() - file.pos()) / (width * height * bpp / 8);
    for (int i = 0; i < nframes; ++i) {
             if (bpp <=  8) frames.push_back(Frame< int8_t>(file, width, height, FrameConstants::HAS_TIMESTAMP).cast<double>());
        else if (bpp <= 16) frames.push_back(Frame<int16_t>(file, width, height, FrameConstants::HAS_TIMESTAMP).cast<double>());
        else if (bpp <= 32) frames.push_back(Frame<int32_t>(file, width, height, FrameConstants::HAS_TIMESTAMP).cast<double>());
        else if (bpp <= 64) frames.push_back(Frame<int64_t>(file, width, height, FrameConstants::HAS_TIMESTAMP).cast<double>());
        else throw FrameBPPTooBig(bpp);

        emit loadPercent((i + 1.0f) / nframes);
    }

    file.close();
}

int Experiment::getStandardBpp(int bpp)
{
         if (bpp <=  8) return 8;
    else if (bpp <= 16) return 16;
    else if (bpp <= 32) return 32;
    else if (bpp <= 64) return 64;
    else                return bpp;
}


