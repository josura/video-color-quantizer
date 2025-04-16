/**
 * @file VideoWriterFFMPEG.cpp
 * @brief Implementation of the VideoWriterFFMPEG class using FFmpeg.
 */
#include "VideoWriterFFMPEG.hpp"
#include <stdexcept>
#include <iostream>

VideoWriterFFMPEG::VideoWriterFFMPEG(const std::string& filename, int width, int height, int fps)
    : filename_(filename), width_(width), height_(height), fps_(fps), frame_index_(0),
    format_ctx_(nullptr), video_stream_(nullptr), codec_ctx_(nullptr), codec_(nullptr),
    frame_(nullptr), pkt_(nullptr), sws_ctx_(nullptr) {

    avformat_alloc_output_context2(&format_ctx_, nullptr, nullptr, filename.c_str());
    if (!format_ctx_) {
        throw std::runtime_error("Could not allocate output format context");
    }

    codec_ = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec_) {
        throw std::runtime_error("H.264 encoder not found");
    }

    video_stream_ = avformat_new_stream(format_ctx_, nullptr);
    if (!video_stream_) {
        throw std::runtime_error("Could not create new stream");
    }

    codec_ctx_ = avcodec_alloc_context3(codec_);
    if (!codec_ctx_) {
        throw std::runtime_error("Could not allocate codec context");
    }

    codec_ctx_->codec_id = AV_CODEC_ID_H264;
    codec_ctx_->codec_type = AVMEDIA_TYPE_VIDEO;
    codec_ctx_->width = width_;
    codec_ctx_->height = height_;
    codec_ctx_->time_base = AVRational{1, fps_};
    codec_ctx_->framerate = AVRational{fps_, 1};
    codec_ctx_->gop_size = 12;
    codec_ctx_->pix_fmt = AV_PIX_FMT_YUV420P;
    codec_ctx_->max_b_frames = 2;

    if (format_ctx_->oformat->flags & AVFMT_GLOBALHEADER) {
        codec_ctx_->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    if (avcodec_open2(codec_ctx_, codec_, nullptr) < 0) {
        throw std::runtime_error("Could not open codec");
    }

    if (avcodec_parameters_from_context(video_stream_->codecpar, codec_ctx_) < 0) {
        throw std::runtime_error("Could not copy codec parameters");
    }

    video_stream_->time_base = codec_ctx_->time_base;

    if (!(format_ctx_->oformat->flags & AVFMT_NOFILE)) {
        if (avio_open(&format_ctx_->pb, filename.c_str(), AVIO_FLAG_WRITE) < 0) {
            throw std::runtime_error("Could not open output file");
        }
    }

    if (avformat_write_header(format_ctx_, nullptr) < 0) {
        throw std::runtime_error("Error occurred when writing header");
    }

    frame_ = av_frame_alloc();
    if (!frame_) {
        throw std::runtime_error("Could not allocate frame");
    }
    frame_->format = codec_ctx_->pix_fmt;
    frame_->width = codec_ctx_->width;
    frame_->height = codec_ctx_->height;

    if (av_frame_get_buffer(frame_, 32) < 0) {
        throw std::runtime_error("Could not allocate frame data");
    }

    pkt_ = av_packet_alloc();
    if (!pkt_) {
        throw std::runtime_error("Could not allocate packet");
    }

    sws_ctx_ = sws_getContext(
        width_, height_, AV_PIX_FMT_RGBA,
        width_, height_, codec_ctx_->pix_fmt,
        SWS_BILINEAR, nullptr, nullptr, nullptr
    );
    if (!sws_ctx_) {
        throw std::runtime_error("Could not initialize sws context");
    }
}

VideoWriterFFMPEG::~VideoWriterFFMPEG() {
    if (codec_ctx_) {
        avcodec_send_frame(codec_ctx_, nullptr);
        while (avcodec_receive_packet(codec_ctx_, pkt_) == 0) {
            av_interleaved_write_frame(format_ctx_, pkt_);
            av_packet_unref(pkt_);
        }
    }

    av_write_trailer(format_ctx_);

    if (pkt_) {
        av_packet_free(&pkt_);
    }
    if (frame_) {
        av_frame_free(&frame_);
    }
    if (sws_ctx_) {
        sws_freeContext(sws_ctx_);
    }
    if (codec_ctx_) {
        avcodec_free_context(&codec_ctx_);
    }
    if (!(format_ctx_->oformat->flags & AVFMT_NOFILE)) {
        avio_closep(&format_ctx_->pb);
    }
    if (format_ctx_) {
        avformat_free_context(format_ctx_);
    }
}

void VideoWriterFFMPEG::write_frame(const uint8_t* rgba_data) {
    if (av_frame_make_writable(frame_) < 0) {
        throw std::runtime_error("Frame not writable");
    }

    const uint8_t* in_data[1] = { rgba_data };
    int in_linesize[1] = { 4 * width_ };

    sws_scale(sws_ctx_, in_data, in_linesize, 0, height_, frame_->data, frame_->linesize);

    frame_->pts = frame_index_++;

    if (avcodec_send_frame(codec_ctx_, frame_) < 0) {
        throw std::runtime_error("Error sending frame to encoder");
    }

    while (avcodec_receive_packet(codec_ctx_, pkt_) == 0) {
        av_packet_rescale_ts(pkt_, codec_ctx_->time_base, video_stream_->time_base);
        pkt_->stream_index = video_stream_->index;

        if (av_interleaved_write_frame(format_ctx_, pkt_) < 0) {
            throw std::runtime_error("Error writing packet");
        }
        av_packet_unref(pkt_);
    }
}
