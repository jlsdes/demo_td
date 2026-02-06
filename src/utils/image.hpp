#ifndef DEMO_TD_IMAGE_HPP
#define DEMO_TD_IMAGE_HPP

#include <filesystem>
#include <memory>


/** A basic image class with some readers and writers to/from PNM formats. */
class Image {
public:
    Image( unsigned int width, unsigned int height );

    /// The Netpbm file types, with their corresponding identifiers as they appear in the files.
    enum FileType : char {
        AsciiBit = '1',
        AsciiGray = '2',
        AsciiPix = '3',
        BinaryBit = '4',
        BinaryGray = '5',
        BinaryPix = '6',
        Arbitrary = '7'
    };

    static std::unique_ptr<Image> load( std::filesystem::path const & filename );
    bool save( std::filesystem::path const & filename, FileType type = Arbitrary ) const;

    unsigned char * get( unsigned int row, unsigned int col );

private:
    std::unique_ptr<unsigned char[]> m_pixels;
    unsigned int m_width;
    unsigned int m_height;
};


#endif //DEMO_TD_IMAGE_HPP
