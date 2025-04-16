/**
 * @file VideoReaderFFMPEG.hpp
 * @brief Class wrapper around FFmpeg for reading and writing video frames.
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
 * @class VideoReaderFFMPEG
 * @brief A class for handling video reading using FFmpeg.
 */
class VideoReaderFFMPEG {
public:
    /**
     * @brief Constructs the VideoReaderFFMPEG object and opens the video file.
     * @param filename The path to the input video file.
     */
    explicit VideoReaderFFMPEG(const std::string& filename);

    /**
     * @brief Destructor that releases FFmpeg resources.
     */
    ~VideoReaderFFMPEG();

    /**
     * @brief Reads the next RGBA frame from the video.
     * @param output_buffer A vector to store the raw RGBA data of the frame.
     * @return True if a frame was successfully read, false if end of stream.
     */
    bool read_next_frame(std::vector<uint8_t>& output_buffer);

    /**
     * @brief Gets the width of the video frames.
     * @return The width of the video.
     */
    int get_width() const;

    /**
     * @brief Gets the height of the video frames.
     * @return The height of the video.
     */
    int get_height() const;

    /**
     * @brief Gets the total number of frames in the video (if known).
     * @return The number of frames.
     */
    int64_t get_frame_count() const;

private:
    std::string filename_;               ///< Path to the video file
    AVFormatContext* format_ctx_;       ///< Format context
    AVCodecContext* codec_ctx_;         ///< Codec context
    AVCodecParameters* codecpar_;       ///< Codec parameters
    const AVCodec* codec_;              ///< Codec
    AVFrame* frame_;                    ///< Original frame
    AVFrame* rgba_frame_;               ///< Converted RGBA frame
    AVPacket* packet_;                  ///< Packet
    SwsContext* sws_ctx_;               ///< Software scaler context

    int video_stream_index_;            ///< Index of the video stream
    int width_;                         ///< Frame width
    int height_;                        ///< Frame height
    int64_t frame_count_;               ///< Total number of frames (if known)
    int64_t current_frame_;             ///< Current frame index
    int fps_;                          ///< Frame per second
    int64_t duration_;                  ///< Duration of the video in microseconds

    std::vector<uint8_t> buffer_;       ///< Buffer for RGBA frame data
};
 