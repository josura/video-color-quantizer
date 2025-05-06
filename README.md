# video-color-quantizer
[![Docs](https://img.shields.io/badge/docs-latest-blue)](https://josura.github.io/video-color-quantizer/)
[![License](https://img.shields.io/badge/License-GPLv3-blue.svg)](LICENSE)
]

Color quantization for videos

## Requirements
- CMake
- FFMpeg libraries
- OpenCL

## Build
```bash
cmake -B build
cmake --build build
```

## Usage
To know the available options, run the tool with the `--help` or `-h` flag. Example usage could be:
```bash
./video-color-quantizer --input <input_video> --output <output_video> --levels <levels_of_quantization>
```

To use a different OpenCL platform, you can specify the device like this:
```bash
OCL_PLATFORM=<numberOfThePlatform> ./video-color-quantizer --input <input_video> --output <output_video> --levels <levels_of_quantization>
```
To find the available OpenCL platforms, you can run the following command(clinfo must be installed):
```bash
clinfo
```