# video-color-quantizer
Color quantization for videos

## Requirements
- CMake
- FFMpeg libraries
- OpenCL Devices

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