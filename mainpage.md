/**
 * @mainpage Video Quantization with OpenCL and FFmpeg
 *
 * @section intro_sec Introduction
 *
 * This project performs uniform color quantization on each frame of a video
 * using GPU acceleration (OpenCL). It utilizes FFmpeg to decode input videos
 * into raw frames, applies quantization kernels on the GPU, and encodes the
 * processed frames back into a final video output.
 *
 * @section features_sec Features
 * - 🔍 Efficient video decoding and encoding using FFmpeg
 * - ⚡ Parallel color quantization using OpenCL (e.g., uniform quantization)
 * - 🧰 Modular and documented C++ utility wrappers
 * - 🛠 CMake-based build system
 * - 📚 Full Doxygen documentation support
 *
 * @section usage_sec Usage
 *
 * Build with:
 * ```
 * mkdir build && cd build
 * cmake ..
 * make
 * ```
 *
 * Run the program with:
 * ```
 * ./video_quantizer -i input.mp4 -o output.mp4
 * ```
 *
 * @section structure_sec Project Structure
 *
 * ```
 * .
 * ├── CMakeLists.txt
 * ├── docs/
 * │   └── mainpage.md          # This file
 * ├── src/
 * │   ├── main.cpp             # Entry point
 * │   ├── ocl_utility.hpp      # OpenCL helper utilities
 * │   ├── VideoReaderFFMPEG.*  # Video decoding class
 * │   ├── VideoWriterFFMPEG.*  # Video encoding class
 * │   └── kernels/
 * │       └── uniformQuantization.cl  # OpenCL kernel
 * ```
 *
 * @section license_sec License
 * This project is licensed under the MIT License. See `LICENSE` for details.
 */
