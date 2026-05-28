#include <emscripten.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

// AES S-box lookup table for substitution step
static const uint8_t sbox[256] = {
    0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76,
    0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0,
    0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15,
    0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75,
    0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84,
    0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf,
    0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8,
    0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2,
    0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73,
    0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb,
    0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79,
    0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08,
    0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a,
    0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e,
    0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf,
    0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16
};

// AES encryption benchmark (developer workload)
// Simulates symmetric encryption throughput using S-box substitution
EMSCRIPTEN_KEEPALIVE
uint32_t bench_aes(uint32_t iterations) {
    uint8_t block[16] = {0x32,0x43,0xf6,0xa8,0x88,0x5a,0x30,0x8d,0x31,0x31,0x98,0xa2,0xe0,0x37,0x07,0x34};
    uint32_t result = 0;
    for (uint32_t i = 0; i < iterations; i++) {
        for (int j = 0; j < 16; j++)
            block[j] = sbox[block[j] ^ (uint8_t)(i >> (j % 4) * 8)];
        result += block[0];
    }
    return result;
}

// Quicksort implementation for sorting benchmark (developer workload)
// Tests branch prediction and cache performance
static void qsort_inner(uint32_t* arr, int lo, int hi) {
    if (lo >= hi) return;
    uint32_t pivot = arr[hi];
    int i = lo - 1;
    for (int j = lo; j < hi; j++) {
        if (arr[j] <= pivot) { i++; uint32_t t=arr[i]; arr[i]=arr[j]; arr[j]=t; }
    }
    uint32_t t=arr[i+1]; arr[i+1]=arr[hi]; arr[hi]=t;
    qsort_inner(arr, lo, i);
    qsort_inner(arr, i+2, hi);
}

// Sorting benchmark — shuffles and sorts 1024 elements per iteration
EMSCRIPTEN_KEEPALIVE
uint32_t bench_sort(uint32_t iterations) {
    static uint32_t arr[1024];
    uint32_t result = 0;
    for (uint32_t i = 0; i < iterations; i++) {
        // Fill with deterministic pseudo-random values
        for (int j = 0; j < 1024; j++) arr[j] = (uint32_t)(j * 2654435761u ^ i);
        qsort_inner(arr, 0, 1023);
        result += arr[0];
    }
    return result;
}

// 4x4 matrix multiplication benchmark (developer / creative workload)
// Simulates linear algebra operations common in graphics and ML
EMSCRIPTEN_KEEPALIVE
uint32_t bench_matrix(uint32_t iterations) {
    float a[16], b[16], c[16];
    for (int i = 0; i < 16; i++) { a[i] = (float)(i+1); b[i] = (float)(16-i); }
    uint32_t result = 0;
    for (uint32_t n = 0; n < iterations; n++) {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++) {
                c[i*4+j] = 0;
                for (int k = 0; k < 4; k++) c[i*4+j] += a[i*4+k] * b[k*4+j];
            }
        a[0] = c[0] * 0.0001f + 1.0f; // Prevent dead code elimination
        result += (uint32_t)c[0];
    }
    return result;
}

// Ray-sphere intersection benchmark (gaming workload)
// Simulates the core math of a ray tracer — tests FPU throughput
EMSCRIPTEN_KEEPALIVE
uint32_t bench_raycast(uint32_t iterations) {
    uint32_t hits = 0;
    for (uint32_t i = 0; i < iterations; i++) {
        // Ray origin varies per iteration
        float ox = (float)(i % 100) * 0.01f - 0.5f;
        float oy = (float)((i/100) % 100) * 0.01f - 0.5f;
        float dx = 0.0f, dy = 0.0f, dz = 1.0f; // Ray direction
        // Sphere at (0, 0, 3) with radius 1
        float cx = 0.0f, cy = 0.0f, cz = 3.0f, r = 1.0f;
        float ocx = ox-cx, ocy = oy-cy, ocz = -cz;
        float b2 = dx*ocx + dy*ocy + dz*ocz;
        float c2 = ocx*ocx + ocy*ocy + ocz*ocz - r*r;
        float disc = b2*b2 - c2; // Discriminant
        if (disc >= 0) hits++;
    }
    return hits;
}

// JSON parsing simulation benchmark (general workload)
// Walks a JSON string counting structural tokens — tests string throughput
EMSCRIPTEN_KEEPALIVE
uint32_t bench_json(uint32_t iterations) {
    static const char json[] = "{\"cpu\":\"Core Ultra 7 155H\",\"score\":12847,\"oem\":\"ASUS\",\"single\":{\"dev\":1240,\"creative\":980}}";
    uint32_t result = 0;
    int len = (int)strlen(json);
    for (uint32_t i = 0; i < iterations; i++) {
        int depth = 0;
        for (int j = 0; j < len; j++) {
            if (json[j] == '{') depth++;
            else if (json[j] == '}') depth--;
            else if (json[j] == ':') result++; // Count key-value pairs
        }
    }
    return result;
}

// Gaussian blur benchmark (creative workload)
// Applies a 3x3 weighted kernel on a 64x64 image — tests memory bandwidth
EMSCRIPTEN_KEEPALIVE
uint32_t bench_blur(uint32_t iterations) {
    static uint8_t img[64*64];
    static uint8_t out[64*64];
    // Initialize image with gradient pattern
    for (int i = 0; i < 64*64; i++) img[i] = (uint8_t)(i * 37);
    uint32_t result = 0;
    for (uint32_t n = 0; n < iterations; n++) {
        // 3x3 Gaussian kernel: [1,2,1 / 2,4,2 / 1,2,1] / 16
        for (int y = 1; y < 63; y++)
            for (int x = 1; x < 63; x++) {
                uint32_t sum =
                    img[(y-1)*64+(x-1)] + img[(y-1)*64+x]*2 + img[(y-1)*64+(x+1)] +
                    img[y*64+(x-1)]*2   + img[y*64+x]*4     + img[y*64+(x+1)]*2 +
                    img[(y+1)*64+(x-1)] + img[(y+1)*64+x]*2 + img[(y+1)*64+(x+1)];
                out[y*64+x] = (uint8_t)(sum >> 4);
            }
        result += out[32*64+32];
        img[0] = out[0]; // Prevent dead code elimination
    }
    return result;
}

// Perlin-style noise benchmark (gaming workload)
// Tests smooth interpolation math common in procedural generation
EMSCRIPTEN_KEEPALIVE
uint32_t bench_noise(uint32_t iterations) {
    uint32_t result = 0;
    for (uint32_t i = 0; i < iterations; i++) {
        float x = (float)(i % 256) * 0.1f;
        float y = (float)((i / 256) % 256) * 0.1f;
        int xi = (int)x & 255, yi = (int)y & 255;
        float xf = x - (int)x, yf = y - (int)y;
        // Smoothstep interpolation
        float u = xf*xf*(3-2*xf), v = yf*yf*(3-2*yf);
        // Hash-based gradient
        float n = u * v * (float)((xi * 1664525 + yi * 1013904223) & 0xFFFFFF);
        result += (uint32_t)fabsf(n);
    }
    return result;
}

// 2D physics simulation benchmark (gaming workload)
// 32 circles with AABB bounds + O(n^2) collision detection
EMSCRIPTEN_KEEPALIVE
uint32_t bench_physics(uint32_t iterations) {
    float px[32], py[32], vx[32], vy[32], r[32];
    // Initialize particles with deterministic positions and velocities
    for (int i = 0; i < 32; i++) {
        px[i] = (float)(i * 17 % 100); py[i] = (float)(i * 13 % 100);
        vx[i] = (float)(i % 5) - 2.0f; vy[i] = (float)(i % 3) - 1.0f;
        r[i] = 2.0f;
    }
    uint32_t collisions = 0;
    for (uint32_t n = 0; n < iterations; n++) {
        // Integrate positions and bounce off walls
        for (int i = 0; i < 32; i++) {
            px[i] += vx[i]; py[i] += vy[i];
            if (px[i] < 0 || px[i] > 100) vx[i] = -vx[i];
            if (py[i] < 0 || py[i] > 100) vy[i] = -vy[i];
        }
        // Brute-force circle collision detection
        for (int i = 0; i < 32; i++)
            for (int j = i+1; j < 32; j++) {
                float dx = px[i]-px[j], dy = py[i]-py[j];
                if (dx*dx+dy*dy < (r[i]+r[j])*(r[i]+r[j])) collisions++;
            }
    }
    return collisions;
}
