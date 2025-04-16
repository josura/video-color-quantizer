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

    int num_bytes = av_image_get_buffer_size(AV_PIX_FMT_RGB32, width_, height_, 1); // RGBA format, will be stored as BGRA on little-endian systems, and as ARGB on big-endian systems
    buffer_.resize(num_bytes);
    av_image_fill_arrays(rgba_frame_->data, rgba_frame_->linesize, buffer_.data(), AV_PIX_FMT_RGB32, width_, height_, 1);

    sws_ctx_ = sws_getContext(
        width_, height_, codec_ctx_->pix_fmt,
        width_, height_, AV_PIX_FMT_RGB32,
        SWS_BILINEAR, nullptr, nullptr, nullptr
    );
    // read fps and duration
    fps_ = av_q2d(format_ctx_->streams[video_stream_index_]->avg_frame_rate);
    duration_ = format_ctx_->duration;
    std::cout << "[LOG] Video opened: " << filename_ << "\n";
    std::cout << "[LOG] Video stream index: " << video_stream_index_ << "\n";
    std::cout << "[LOG] Video width: " << width_ << "\n";
    std::cout << "[LOG] Video height: " << height_ << "\n";
    std::cout << "[LOG] Video frame count: " << frame_count_ << "\n";
    std::cout << "[LOG] Video fps: " << fps_ << "\n";
    std::cout << "[LOG] Video duration: " << duration_ / AV_TIME_BASE << " seconds\n";
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
                    return true;
                }
            }
        }
        av_packet_unref(packet_);
        current_frame_++;
        std::cout << "[LOG] Reading frame " << current_frame_ << " of " << frame_count_ << "\n";
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
