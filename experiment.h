#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include <QDebug>
#include <cassert>
#include <fstream>
#include "frame.h"
#include "filenotfoundexception.h"

// TODO usar QFile
// TODO usar QString en las excepciones
// TODO file.read devuelve un código de error que hay que leer

class Experiment : public QObject
{
    Q_OBJECT // For QDebug
private:
    uint32_t width;  // Full frame width
    uint32_t height; // Full frame height (2x subframes)
    uint32_t bpp;    // Bits per pixel
    Frame<double> dark;
    Frame<double> gain;
    std::vector<Frame<double>> frames;

public:
    explicit Experiment(const std::string &path);
    void load(const std::string &path);

private:

    template<class readAs, class convertTo>
    Frame<convertTo> readNextFrame(std::ifstream& file, uint32_t width, uint32_t height, bool has_timestamp = false) {
        std::vector<readAs> vdata(width * height);
        Frame<readAs>::TimestampData timestamp = 0; // TODO Frame::TimestampData

        if (has_timestamp) {
            file.read(reinterpret_cast<char*>(&timestamp), sizeof(Frame<readAs>::TimestampData));
        }
        file.read(reinterpret_cast<char*>(vdata.data()), vdata.size() * sizeof(readAs));

        return Frame<readAs>(vdata, width, height, timestamp).cast<convertTo>();
    }
 };

#endif // EXPERIMENT_H
