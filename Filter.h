#ifndef FILTER_H
#define FILTER_H
#include <algorithm>

class Filter {
 protected:
     unsigned char* get_pixel(const int pos, unsigned char** pixels) const;
     unsigned char* get_pixel(const int x, const int y, unsigned char** pixels) const;
     void set_rgba(const int pos, unsigned char new_value, unsigned char* pixel) const;
     void replace_pixels(unsigned char** old_pixels, unsigned char** new_pixels, const int size) const;
     int clamp(const int rgba_value) const;

 private:
     int min_rgba_value;
     int max_rgba_value;
 
 public:
     Filter(int min_rgba_value, int max_rgba_value)
         : min_rgba_value{min_rgba_value}
         , max_rgba_value{max_rgba_value}
       { }
     virtual ~Filter();
     
     virtual void apply_filter(int width, int height, int channels, unsigned char** pixels) = 0;

};

#endif

/* vim: set ts=4 sw=4 tw=79 ft=cpp et :*/
