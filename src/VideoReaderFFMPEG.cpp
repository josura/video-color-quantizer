/**
 * @file VideoReaderFFMPEG.cpp
 * @brief Implementation of the VideoReaderFFMPEG class using FFmpeg.
 */

#include "VideoReaderFFMPEG.hpp"
#include <stdexcept>
#include <iostream>

VideoReaderFFMPEG::VideoReaderFFMPEG(const std::string& filename)
    : filename_(filename), format_ctx_(nullptr), codec_ctx_(nullptr),
    codecpar_(nullptr), codec_(nullptr), frame_(nullptr),
    rgba_frame_(nullptr), packet_(nullptr), sws_ctx_(nullptr),
    video_stream_index_(-1), width_(0), height_(0), frame_count_(0) {

    if (avformat_open_input(&format_ctx_, filename.c_str(), nullptr, nullptr) < 0) {
        throw std::runtime_error("Failed to open video file: " + filename);
    }

    if (avformat_find_stream_info(format_ctx_, nullptr) < 0) {
        avformat_close_input(&format_ctx_);
        throw std::runtime_error("Failed to retrieve stream info");
    }

    for (unsigned i = 0; i < format_ctx_->nb_streams; i++) {
        if (format_ctx_->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index_ = i;
            break;
        }
    }

    if (video_stream_index_ == -1) {
        avformat_close_input(&format_ctx_);
        throw std::runtime_error("No video stream found");
    }

    codecpar_ = format_ctx_->streams[video_stream_index_]->codecpar;
    codec_ = avcodec_find_decoder(codecpar_->codec_id);
    if (!codec_) {
        avformat_close_input(&format_ctx_);
        throw std::runtime_error("Unsupported codec");
    }

    codec_ctx_ = avcodec_alloc_context3(codec_);
    avcodec_parameters_to_context(codec_ctx_, codecpar_);
    avcodec_open2(codec_ctx_, codec_, nullptr);

    width_ = codec_ctx_->width;
    height_ = codec_ctx_->height;
    frame_count_ = format_ctx_->streams[video_stream_index_]->nb_frames;
    current_frame_ = 0;

    frame_ = av_frame_alloc();
    rgba_frame_ = av_frame_alloc();
    packet_ = av_packet_alloc();

    // read fps and duration
    fps_ = av_q2d(format_ctx_->streams[video_stream_index_]->avg_frame_rate);
    duration_ = format_ctx_->duration;
    std::cout << "[LOG] Video opened: " << filename_ << "\n";
    std::cout << "[LOG] Video stream index: " << video_stream_index_ << "\n";
    std::cout << "[LOG] Video width: " << width_ << "\n";
    std::cout << "[LOG] Video height: " << height_ << "\n";
    std::cout << "[LOG] Video frame count: " << frame_count_ << "\n";
    std::cout << "[LOG] Video fps: " << fps_ << "\n";
    double duration_in_seconds = static_cast<double>(duration_) / AV_TIME_BASE;
    std::cout << "[LOG] Video duration: " << duration_in_seconds << " seconds\n";
    // additional logging for debugging
    AVColorSpace color_space = codecpar_->color_space;
    AVColorPrimaries color_primaries = codecpar_->color_primaries;
    AVColorTransferCharacteristic color_trc = codecpar_->color_trc;
    AVColorRange color_range = codecpar_->color_range;
    std::cout << "[DEBUG] Color Space: " << av_color_space_name(color_space) << "\n";
    std::cout << "[DEBUG] Color Primaries: " << av_color_primaries_name(color_primaries) << "\n";
    std::cout << "[DEBUG] Transfer Characteristics: " << av_color_transfer_name(color_trc) << "\n";
    std::cout << "[DEBUG] Color Range: " << av_color_range_name(color_range) << "\n";
    // get pixel format
    int pixel_format = codecpar_->format;
    auto pixel_format_name = av_get_pix_fmt_name(static_cast<AVPixelFormat>(pixel_format));
    // print pixel format
    std::cout << "[DEBUG] Pixel Format: " << pixel_format_name << "\n";

    // Compute the expected frame count
    expected_frame_count_ = static_cast<int64_t>(fps_) * duration_in_seconds;
    std::cout << "[LOG] Expected frame count: " << expected_frame_count_ << "\n";

    // int num_bytes = av_image_get_buffer_size(AV_PIX_FMT_RGB32, width_, height_, 1); // RGBA format, will be stored as BGRA on little-endian systems, and as ARGB on big-endian systems
    int num_bytes = av_image_get_buffer_size(AV_PIX_FMT_YUV444P, width_, height_, 1); // YUV444P format
    // int num_bytes = av_image_get_buffer_size(av_get_pix_fmt(pixel_format_name), width_, height_, 1); // original format
    buffer_.resize(num_bytes);
    // av_image_fill_arrays(rgba_frame_->data, rgba_frame_->linesize, buffer_.data(), AV_PIX_FMT_RGB32, width_, height_, 1);
    av_image_fill_arrays(rgba_frame_->data, rgba_frame_->linesize, buffer_.data(), AV_PIX_FMT_YUV444P, width_, height_, 1);
    // av_image_fill_arrays(rgba_frame_->data, rgba_frame_->linesize, buffer_.data(), av_get_pix_fmt(pixel_format_name), width_, height_, 1);

    sws_ctx_ = sws_getContext(
        width_, height_, codec_ctx_->pix_fmt,
        width_, height_, 
        AV_PIX_FMT_YUV444P,
        // AV_PIX_FMT_RGB32,
        // av_get_pix_fmt(pixel_format_name),
        SWS_BILINEAR, nullptr, nullptr, nullptr
    );
}

VideoReaderFFMPEG::~VideoReaderFFMPEG() {
    av_packet_free(&packet_);
    av_frame_free(&frame_);
    av_frame_free(&rgba_frame_);
    sws_freeContext(sws_ctx_);
    avcodec_free_context(&codec_ctx_);
    avformat_close_input(&format_ctx_);
}

bool VideoReaderFFMPEG::read_next_frame(std::vector<uint8_t>& output_buffer) {
    while (av_read_frame(format_ctx_, packet_) >= 0) {
        if (packet_->stream_index == video_stream_index_) {
            if (avcodec_send_packet(codec_ctx_, packet_) == 0) {
                while (avcodec_receive_frame(codec_ctx_, frame_) == 0) {
                    sws_scale(
                        sws_ctx_,
                        frame_->data, frame_->linesize,
                        0, height_,
                        rgba_frame_->data, rgba_frame_->linesize
                    );
                    output_buffer.assign(buffer_.begin(), buffer_.end());
                    av_packet_unref(packet_);
                    current_frame_++;
                    std::cout << "[LOG] Reading frame " << current_frame_ << " of " << frame_count_ << "\n";
                    return true;
                }
            }
        }
        av_packet_unref(packet_);
    }
    return false;
}

int VideoReaderFFMPEG::get_width() const {
    return width_;
}

int VideoReaderFFMPEG::get_height() const {
    return height_;
}

int64_t VideoReaderFFMPEG::get_frame_count() const {
    return frame_count_;
}

int64_t VideoReaderFFMPEG::get_expected_frame_count() const {
    return expected_frame_count_;
}

int64_t VideoReaderFFMPEG::get_current_frame() const {
    return current_frame_;
}

int VideoReaderFFMPEG::get_fps() const {
    return fps_;
}

int64_t VideoReaderFFMPEG::get_duration() const {
    return duration_;
}

SwsContext* VideoReaderFFMPEG::get_sws_context() const {
    return sws_ctx_;
}