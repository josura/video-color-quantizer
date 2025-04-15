/**
 * @file video_reader.cpp
 * @brief Implementation of FFmpeg-based video frame extraction utility.
 */

 extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libswscale/swscale.h>
    #include <libavutil/imgutils.h>
}
    
#include "video_reader.hpp"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstdint>
#include <string>

void extract_frames_from_video(
    const std::string& input_filename,
    std::vector<std::vector<uint8_t>>& frames,
    int& width,
    int& height
) {
    AVFormatContext* format_ctx = nullptr;
    if (avformat_open_input(&format_ctx, input_filename.c_str(), nullptr, nullptr) < 0) {
        throw std::runtime_error("Failed to open video file: " + input_filename);
    }

    if (avformat_find_stream_info(format_ctx, nullptr) < 0) {
        avformat_close_input(&format_ctx);
        throw std::runtime_error("Failed to find stream info");
    }

    int video_stream_index = -1;
    for (unsigned i = 0; i < format_ctx->nb_streams; i++) {
        if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            break;
        }
    }

    if (video_stream_index == -1) {
        avformat_close_input(&format_ctx);
        throw std::runtime_error("No video stream found");
    }

    AVCodecParameters* codecpar = format_ctx->streams[video_stream_index]->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codecpar->codec_id);
    if (!codec) {
        avformat_close_input(&format_ctx);
        throw std::runtime_error("Unsupported codec");
    }

    AVCodecContext* codec_ctx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codec_ctx, codecpar);
    avcodec_open2(codec_ctx, codec, nullptr);

    width = codec_ctx->width;
    height = codec_ctx->height;

    SwsContext* sws_ctx = sws_getContext(
        width, height, codec_ctx->pix_fmt,
        width, height, AV_PIX_FMT_RGB24,
        SWS_BILINEAR, nullptr, nullptr, nullptr
    );

    AVFrame* frame = av_frame_alloc();
    AVFrame* rgb_frame = av_frame_alloc();
    int num_bytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, width, height, 1);
    std::vector<uint8_t> buffer(num_bytes);
    av_image_fill_arrays(rgb_frame->data, rgb_frame->linesize, buffer.data(), AV_PIX_FMT_RGB24, width, height, 1);

    AVPacket* packet = av_packet_alloc();

    while (av_read_frame(format_ctx, packet) >= 0) {
        if (packet->stream_index == video_stream_index) {
            if (avcodec_send_packet(codec_ctx, packet) == 0) {
                while (avcodec_receive_frame(codec_ctx, frame) == 0) {
                    sws_scale(sws_ctx, frame->data, frame->linesize, 0, height, rgb_frame->data, rgb_frame->linesize);
                    frames.emplace_back(buffer.begin(), buffer.end());
                }
            }
        }
        av_packet_unref(packet);
    }

    av_packet_free(&packet);
    av_frame_free(&frame);
    av_frame_free(&rgb_frame);
    sws_freeContext(sws_ctx);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&format_ctx);
}
