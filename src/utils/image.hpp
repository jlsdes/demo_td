#ifndef DEMO_TD_IMAGE_HPP
#define DEMO_TD_IMAGE_HPP

#include <filesystem>
#include <memory>


/// The PNM file types, with their corresponding identifiers as they appear in the files.
enum PNMFileType : char {
    AsciiBit = '1',
    AsciiGrey = '2',
    AsciiPix = '3',
    BinaryBit = '4',
    BinaryGrey = '5',
    BinaryPix = '6',
    Arbitrary = '7'
};


struct Pixel {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};


struct Image {
    unsigned int width;
    unsigned int height;

    /// The pixels in row-major order, with every pixel containing 4 integer values with value in [0, 255]. Thus, pixel
    /// locations can be calculated as '(row * width + col) * 4'.
    std::unique_ptr<Pixel[]> pixels;

    Image( unsigned int width, unsigned int height, std::unique_ptr<Pixel[]> && pixels = nullptr );
};


/** Abstract base class for image readers and writers. */
class ImageIO {
public:
    virtual ~ImageIO() = default;

    /// Abstract functions to be implemented by derived classes, reading/writing image data.
    [[nodiscard]] virtual Image load( std::istream & stream ) const = 0;
    virtual void save( Image const & image, std::ostream & stream ) const = 0;

    /// Utility functions for reading/writing image data to files.
    [[nodiscard]] Image load_file( std::filesystem::path const & filename ) const;
    void save_file( Image const & image, std::filesystem::path const & filename ) const;

    /// Utility functions that set the desired output type. This doesn't work for every derived class.
    void set_ascii();
    void set_binary();

protected:
    bool m_ascii_output { false };
};


/** Handles image data to/from the PBM (Portable BitMap) format. */
class PBMImageIO : public ImageIO {
public:
    ~PBMImageIO() override = default;

    [[nodiscard]] Image load( std::istream & stream ) const override;
    void save( Image const & image, std::ostream & stream ) const override;
};


/** Handles image data to/from the PGM (Portable GreyMap) format. */
class PGMImageIO : public ImageIO {
public:
    ~PGMImageIO() override = default;

    [[nodiscard]] Image load( std::istream & stream ) const override;
    void save( Image const & image, std::ostream & stream ) const override;
};


/** Handles image data to/from the PPM (Portable PixMap) format. */
class PPMImageIO : public ImageIO {
public:
    ~PPMImageIO() override = default;

    [[nodiscard]] Image load( std::istream & stream ) const override;
    void save( Image const & image, std::ostream & stream ) const override;
};


/** Handles image data to/from the PAM (Portable ArbitraryMap) format. This implementation only supports the RGB_ALPHA
 *  tuple type, as other use cases can be covered by it or one of the other PNM file types. */
class PAMImageIO : public ImageIO {
public:
    ~PAMImageIO() override = default;

    [[nodiscard]] Image load( std::istream & stream ) const override;
    void save( Image const & image, std::ostream & stream ) const override;
};


/** Handles image data to/from a PNM (Portable AnyMap) format. This class will select one of the other IO classes above
 *  to handle any of the tasks it gets. */
class PNMImageIO : public ImageIO {
public:
    ~PNMImageIO() override = default;

    [[nodiscard]] Image load( std::istream & stream ) const override;
    void save( Image const & image, std::ostream & stream ) const override;
};


#endif //DEMO_TD_IMAGE_HPP
