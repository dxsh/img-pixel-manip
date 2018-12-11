#include "Image.h"

/*
 * Callback function passed to png_set_read_fn()
 */
void read_callback(png_structp png_ptr, png_bytep data, png_size_t len) {
    png_voidp io_ptr = png_get_io_ptr(png_ptr);
    reinterpret_cast<std::ifstream*>(io_ptr)->read(reinterpret_cast<char*>(data), len);
}

/*
 * Callback function passed to png_set_write_fn()
 */
void write_callback(png_structp png_ptr, png_bytep data, png_size_t len) {
    png_voidp io_ptr = png_get_io_ptr(png_ptr);
    reinterpret_cast<std::ofstream*>(io_ptr)->write(reinterpret_cast<char*>(data), len);
}

/*
 * Initialize libpng I/O.
 *
 * Allocate and initialize structs png_struct and png_info, both of which are
 * crucial to working with libpng.
 *
 * struct png_struct is used internally by libpng and must be the first
 * variable to every libpng function call.
 *
 * struct png_info contains information about the PNG file, such as
 * IHDR chunk, image palette information and text comments.
 *
 * Neither of these structs are directly accessible. Use interface functions
 * png_get_*() to access struct data.
 *
 * Ref: http://refspecs.linuxfoundation.org/LSB_3.1.1/LSB-Desktop-generic/LSB-Desktop-generic/toclibpng.html
 */
void Image::init_png_io() {
    m_read = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    m_info = png_create_info_struct(m_read);

    /* Define our own input function for reading PNG files, as opposed to using
     * the standard C I/O.
     */
    png_set_read_fn(m_read, reinterpret_cast<png_voidp>(&m_file), read_callback);

    png_read_info(m_read, m_info);
}

Image::Image(const std::string fn) {
    m_fn = fn;
    m_file.open(m_fn);

    if(!is_png()) {
        throw std::runtime_error("Invalid filetype; unable to ensure PNG.");
    }

    init_png_io();

    m_width = png_get_image_width(m_read, m_info);
    m_height = png_get_image_height(m_read, m_info);
    m_bitdepth = png_get_bit_depth(m_read, m_info);
    m_rowbytes = png_get_rowbytes(m_read, m_info);
    m_channels = png_get_channels(m_read, m_info);
    m_color_type = png_get_color_type(m_read, m_info);
    m_pixels = new unsigned char*[m_height];

    for (int i = 0; i < m_height; i++) {
        m_pixels[i] = new unsigned char[m_rowbytes];
    }

    png_read_update_info(m_read, m_info);
}

/*
 * Ensure the first eight bytes of supplied PNG file match the PNG
 * signature. The first eight bytes of a PNG file always contain the same
 * values. Return zero if the signature does not match, respectively, one
 * if the signature does match. The following bytes are used as a signature to
 * verify PNG files:
 *
 *      (decimal)              137  80  78  71  13  10  26  10
 *      (hex)                   89  50  4e  47  0d  0a  1a  0a
 *      (ASCII C)             \211   P   N   G  \r  \n \032 \n
 *
 * Ref: http://www.libpng.org/pub/png/book/chapter08.html#png.ch08.tbl.1
 *
 */
bool Image::is_png() const {
    const unsigned int signature_bytes = 8;
    std::ifstream file(m_fn, std::ios::binary);
    png_byte header[signature_bytes];

    if (!file.good())
        return false;

    file.read(reinterpret_cast<char *>(header), signature_bytes);

    return (png_sig_cmp(header, 0, signature_bytes) == 0);
}

Image::~Image() {
    if (m_read && m_info)
        png_destroy_read_struct(&m_read, &m_info, NULL);

    for (int i = 0; i < m_height; i++) {
        delete[] m_pixels[i];
    }

    delete[] m_pixels;
}

/* vim: set ts=4 sw=4 tw=79 ft=cpp et :*/