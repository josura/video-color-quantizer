/**
 * @file uniformQuantization.cl
 * @brief OpenCL kernels for uniform quantization of images.
 * @details This file contains OpenCL kernels for uniform quantization of images.
 * The quantization is performed in the RGB color space, and the alpha channel is preserved.
 * There could be problem based on the colorspase encoding depending on where the channel are stored, so that needs to be taken into account.
 * Since the quantization is done equally across the channels(other than the alpha channel), there is no difference if the colorspace changes the order of RGB.
 * Problems arise if we need to work on the single channels, if the colorspace is not RGB, and the order of the transparency channel is not the last one.
 */
__kernel void uniform_quantize_lower_bound(
    __global const uchar4* input_image,
    __global uchar4* output_image,
    const int width,
    const int height,
    const int levels
) {
    int x = get_global_id(0);
    int y = get_global_id(1);
    int idx = y * width + x;

    if (x >= width || y >= height)
        return;

    uchar4 pixel = input_image[idx];

    int step = 256 / levels;

    uchar4 result;
    // floor(pixel.x / step) * step gives the quantized value (lower bound of each interval)
    result.x = (uchar)((pixel.x / step) * step); // R
    result.y = (uchar)((pixel.y / step) * step); // G
    result.z = (uchar)((pixel.z / step) * step); // B
    result.w = pixel.w; // Preserve alpha

    output_image[idx] = result;
}

kernel void uniform_quantize_upper_bound(
    __global const uchar4* input_image,
    __global uchar4* output_image,
    const int width,
    const int height,
    const int levels
) {
    int x = get_global_id(0);
    int y = get_global_id(1);
    int idx = y * width + x;

    if (x >= width || y >= height)
        return;

    uchar4 pixel = input_image[idx];

    int step = 256 / levels;

    uchar4 result;
    // ceil(pixel.x / step) * step gives the quantized value (upper bound of each interval)
    result.x = (uchar)(((pixel.x + step - 1) / step) * step); // R
    result.y = (uchar)(((pixel.y + step - 1) / step) * step); // G
    result.z = (uchar)(((pixel.z + step - 1) / step) * step); // B
    result.w = pixel.w; // Preserve alpha

    output_image[idx] = result;
}

kernel void uniform_quantize_nearest(
    __global const uchar4* input_image,
    __global uchar4* output_image,
    const int width,
    const int height,
    const int levels
) {
    int x = get_global_id(0);
    int y = get_global_id(1);
    int idx = y * width + x;

    if (x >= width || y >= height)
        return;

    uchar4 pixel = input_image[idx];

    int step = 256 / levels;

    uchar4 result;
    // round(pixel.x / step) * step gives the quantized value (nearest value)
    result.x = (uchar)(((pixel.x + step / 2) / step) * step); // R
    result.y = (uchar)(((pixel.y + step / 2) / step) * step); // G
    result.z = (uchar)(((pixel.z + step / 2) / step) * step); // B
    result.w = pixel.w; // Preserve alpha

    output_image[idx] = result;
}

// 2 level quantization, meaning values are either 0 or 255, with thresholding
kernel void uniform_quantize_binary_threshold(
    __global const uchar4* input_image,
    __global uchar4* output_image,
    const int width,
    const int height
) {
    int x = get_global_id(0);
    int y = get_global_id(1);
    int idx = y * width + x;

    if (x >= width || y >= height)
        return;

    uchar4 pixel = input_image[idx];

    uchar4 result;
    // If the pixel value is greater than 127, set it to 255, otherwise set it to 0, could be done with bitwise operations
    // This is a simple thresholding operation
    result.x = (pixel.x > 127) ? 255 : 0; // R
    result.y = (pixel.y > 127) ? 255 : 0; // G
    result.z = (pixel.z > 127) ? 255 : 0; // B
    result.w = pixel.w; // Preserve alpha

    output_image[idx] = result;
} 

// 2 level quantization, meaning values are either 0 or 255, with bitshifting
kernel void uniform_quantize_binary_bitshift(
    __global const uchar4* input_image,
    __global uchar4* output_image,
    const int width,
    const int height
) {
    int x = get_global_id(0);
    int y = get_global_id(1);
    int idx = y * width + x;

    if (x >= width || y >= height)
        return;

    uchar4 pixel = input_image[idx];

    uchar4 result;
    // pixel.x >> 7 gives either 0 or 1(if value is greater than 127), then multiply by 255 to get either 0 or 255, this logic can be used for power of 2 quantization
    // bitwise operation
    result.x = (pixel.x >> 7) * 255; // R
    result.y = (pixel.y >> 7) * 255; // G
    result.z = (pixel.z >> 7) * 255; // B
    result.w = pixel.w; // Preserve alpha

    output_image[idx] = result;
}