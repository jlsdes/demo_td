#ifndef DEMO_TD_IMAGE_HPP
#define DEMO_TD_IMAGE_HPP

#include <filesystem>
#include <istream>
#include <memory>
#include <ostream>


/** A basic image class with some readers and writers to/from Netpbm formats. */
class Image {
public:
    Image( unsigned int width, unsigned int height, unsigned int nr_channels = 4 );

    static std::unique_ptr<Image> load( std::filesystem::path const & filename );
    bool save( std::filesystem::path const & filename ) const;

private:
    std::unique_ptr<unsigned char[]> m_pixels;
    unsigned int m_width;
    unsigned int m_height;
    unsigned int m_nr_channels;
};


#endif //DEMO_TD_IMAGE_HPP
