#ifndef NEWSTREAMRECORDER_H
#define NEWSTREAMRECORDER_H

#include <QObject>

extern "C" {
#include <ffmpeg/include/libavcodec/avcodec.h>
#include <ffmpeg/include/libavformat/avformat.h>
#include <ffmpeg/include/libswscale/swscale.h>
}

#define INBUF_SIZE 4096

class NewStreamRecorder : public QObject
{
        Q_OBJECT
    public:
        explicit NewStreamRecorder(QObject *parent = nullptr);

    signals:

    public slots:

    private:
        static void pgm_save(unsigned char *buf, int wrap, int xsize, int ysize,
                             char *filename);
        static void decode(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt,
                           const char *filename);


        void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame);
};

#endif // NEWSTREAMRECORDER_H
