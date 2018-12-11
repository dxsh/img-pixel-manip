#include "ImageDisplay.h"

ImageDisplay::ImageDisplay(const char* window_title, int width, int height, int channels) {
    m_window_title = window_title;
    m_width = width;
    m_height = height;
    m_channels = channels;
    m_pitch = width * 4;

    /* Initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        throw std::runtime_error("SDL_Init failed to initialize");
    }

    m_window = SDL_CreateWindow(m_window_title,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            m_width, m_height,
            SDL_WINDOW_SHOWN);

    m_screen = SDL_GetWindowSurface(m_window);
}

/*
 * A truecolor PNG image uses three seperate values for each
 * pixel (R, G, B) each being 8 bits. Each of these values belong to a
 * channel. Thus a png24 image has three channels (24 / 8), and a greyscale
 * png8 (8 / 8) has one channel. A special channel exists for Alpha, however
 * this channel does not provide any color but rather decides the opacity
 * for each pixel.
 *
 * To seamlessly use SDL_CreateRGBSurfaceFrom() we pack the bits by shifting
 * them to ensure a valid 0xAABBGGRR hex. (0xRRGGBBAA is reversed due to
 * endianness.)
 *
 * Ref: Compressed Image File Formats: JPEG, PNG, GIF, XBM, BMP, John Miano
 *      https://books.google.se/books/about/Compressed_Image_File_Formats.html?id=_nJLvY757dQC
 *
 */
void ImageDisplay::convert_to_rgba(unsigned char** data) {
    m_pixels = new unsigned[m_height * m_width];

    switch (m_channels) {
        case 1: {   /* png8 */
                    for (int y = 0; y < m_height; y++)
                        for (int x = 0; x < m_width; x++) {
                            unsigned L = data[y][x];
                            m_pixels[y * m_width + x] = 0xff000000 | L << 16 | L << 8 | L;
                        }
                    break;
                }
        case 3: {   /* png24 */
                    for (int y = 0; y < m_height; y++)
                        for (int x = 0; x < m_width; x++) {
                            unsigned R = data[y][x * 3 + 0];
                            unsigned G = data[y][x * 3 + 1];
                            unsigned B = data[y][x * 3 + 2];
                            m_pixels[y * m_width + x] = 0xff000000 | B << 16 | G << 8 | R;
                        }
                    break;
                }
        case 4: {   /* png32 */
                    for (int i = 0; i < m_height; i++)
                        memcpy(m_pixels + i * m_width, data[i], m_pitch);
                    break;
                }
    }
}

}

ImageDisplay::~ImageDisplay() {
    delete[] m_pixels;
}
/* vim: set ts=4 sw=4 tw=79 ft=cpp et :*/