__kernel void vector_addition(__global const float* a, 
                              __global const float* b, 
                              __global float* result, 
                              const int nels) {
    // Get the index of the current element
    int id = get_global_id(0);

    // Perform addition if within bounds
    if (id < nels) {
        result[id] = a[id] + b[id];
    }
}

__kernel void vector_initialization_once(__global float* a, 
                                  const int nels) {
    // Get the index of the current element
    int id = get_global_id(0);

    // Initialize the vector with the given value if within bounds
    if (id < nels) {
        a[id] = id;
    }
}

__kernel void vector_initialization_twice(__global float* a, 
                                          __global float* b,
                                        const int nels) {
    // Get the index of the current element
    int id = get_global_id(0);
    // Initialize the vectors with the given values if within bounds
    if (id < nels) {
        a[id] = id;
        b[id] = nels -id;
    }
}
