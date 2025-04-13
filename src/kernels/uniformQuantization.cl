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
