/**
 * @file video_reader.hpp
 * @brief Header for FFmpeg-based video frame extraction utility.
 */

 #ifndef VIDEO_READER_HPP
 #define VIDEO_READER_HPP
 
 #include <string>
 #include <vector>
 #include <cstdint>
 
 /**
  * @brief Extracts all RGB frames from a video file using FFmpeg.
  *
  * This function opens a video file and decodes it frame-by-frame.
  * Each frame is converted to RGB format using libswscale and returned
  * as a flat buffer.
  *
  * @param input_filename The path to the input video file.
  * @param frames A vector that will contain raw RGB data for each frame.
  * @param width Output parameter for the video frame width.
  * @param height Output parameter for the video frame height.
  */
 void extract_frames_from_video(
     const std::string& input_filename,
     std::vector<std::vector<uint8_t>>& frames,
     int& width,
     int& height
 );
 
 #endif // VIDEO_READER_HPP