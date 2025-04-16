/**
 * @file VideoWriterFFMPEG.hpp
 * @brief Header of the VideoWriterFFMPEG class using FFmpeg.
 */
#pragma once

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

#include <string>
#include <vector>
#include <cstdint>

/**
 * @class VideoWriterFFMPEG
 * @brief A class for writing video frames to a file using FFmpeg.
 */
class VideoWriterFFMPEG {
public:
    /**
     * @brief Constructs the VideoWriterFFMPEG object and initializes the output file.
     * @param filename The path to the output video file.
     * @param width The width of the video frames.
     * @param height The height of the video frames.
     * @param fps The frame rate of the output video.
     */
    VideoWriterFFMPEG(const std::string& filename, int width, int height, int fps);

    /**
     * @brief Destructor that finalizes the video file and releases resources.
     */
    ~VideoWriterFFMPEG();

    /**
     * @brief Writes a single RGBA frame to the video file.
     * @param rgba_data A pointer to the raw RGBA pixel data.
     */
    void write_frame(const uint8_t* rgba_data);

private:
    std::string filename_;
    int width_;
    int height_;
    int fps_;
    int frame_index_;

    AVFormatContext* format_ctx_;
    AVStream* video_stream_;
    AVCodecContext* codec_ctx_;
    const AVCodec* codec_;
    AVFrame* frame_;
    AVPacket* pkt_;
    SwsContext* sws_ctx_;
};
