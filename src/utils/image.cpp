#include "image.hpp"
#include "log.hpp"

#include <fstream>


Image::Image( unsigned int const width, unsigned int const height, std::unique_ptr<Pixel[]> && pixels )
    : width { width }, height { height },
      pixels { pixels ? std::move( pixels ) : std::make_unique<Pixel[]>( width * height ) } {}

Image ImageIO::load_file( std::filesystem::path const & filename ) const {
    std::ifstream file { filename };
    if ( not file.is_open() ) {
        Log::error( "Failed to open file '", filename, "'." );
        return { 0, 0, nullptr };
    }
    return load( file );
}

void ImageIO::save_file( Image const & image, std::filesystem::path const & filename ) const {
    std::ofstream file { filename };
    if ( not file.is_open() ) {
        Log::error( "Failed to open file '", filename, "'." );
        return;
    }
    save( image, file );
}

void ImageIO::set_ascii() {
    m_ascii_output = true;
}

void ImageIO::set_binary() {
    m_ascii_output = false;
}

struct HeaderInfo {
    unsigned int width;
    unsigned int height;
    unsigned int max_value;
    char type;
};

std::pair<bool, HeaderInfo> read_header( std::istream & stream ) {
    HeaderInfo info {};

    char file_id;
    stream.get( file_id );
    if ( file_id != 'P' ) {
        Log::error( "Invalid header for a PNM file; expected 'P', but got '", file_id, "'." );
        return { false, {} };
    }

    // Ideally lines should be checked for comments (starting with #), but at the moment they're not supported.
    stream >> info.type >> info.width >> info.height;

    if ( info.type != AsciiBit and info.type != BinaryBit ) {
        stream >> info.max_value;
        if ( info.max_value != 255 )
            Log::warning( "PNM file has a max value '", info.max_value, "', which is currently not supported;"
                          "using 255 as the max value instead." );
    }

    char c;
    stream.get( c );
    if ( c != '\n' ) {
        Log::warning( "Shouldn't there be a new line here?" );
        stream.putback( c );
    }

    return { true, info };
}

Image PBMImageIO::load( std::istream & stream ) const {
    auto const [success, header] { read_header( stream ) };

    if ( not success )
        return { 0, 0, nullptr };
    if ( header.type != AsciiBit and header.type != BinaryBit ) {
        Log::error( "Invalid header for a PBM file; expected '1' or '4', but got '", header.type, "'." );
        return { 0, 0, nullptr };
    }

    Image image { header.width, header.height };
    Pixel * pixel { image.pixels.get() };
    char mini_buffer;

    Pixel constexpr black { 0, 0, 0, 255 };
    Pixel constexpr white { 255, 255, 255, 255 };

    if ( header.type == AsciiBit ) {
        // Ascii PBM files use '0' to represent white and '1' for black. Any other character is invalid.
        for ( unsigned int i { 0 }; i < header.width * header.height; ++i ) {
            stream >> mini_buffer;
            switch ( mini_buffer ) {
            case '0':
                *pixel = white;
                break;
            case '1':
                *pixel = black;
                break;
            default:
                Log::error( "Unexpected character '", mini_buffer, "' in image data." );
                return { 0, 0, nullptr };
            }
            ++pixel;
        }
    } else /* header.type == BinaryBit */ {
        // Binary PBM files use a single bit per pixel, where 0 indicates white (no ink) and 1 indicates black (ink).
        for ( unsigned int row { 0 }; row < header.height; ++row ) {
            for ( unsigned int col { 0 }; col < header.width; ++col ) {
                if ( col % 8 == 0 )
                    stream.get( mini_buffer );
                *pixel = mini_buffer & 0x80 ? black : white;
                ++pixel;
                mini_buffer <<= 1;
            }
        }
    }
    return image;
}

void PBMImageIO::save( Image const & image, std::ostream & stream ) const {
    Pixel const * pixel { image.pixels.get() };

    if ( m_ascii_output ) {
        // Ascii PBM files contain '0' and '1' characters depending on whether the pixel is white or black respectively.
        // Contrary to most image files, '0' is in fact a white pixel; these value represent boolean values indicating
        // ink (black) or no ink (white).
        stream << "P1\n" << image.width << ' ' << image.height << '\n';
        for ( unsigned int row { 0 }; row < image.height; ++row ) {
            for ( unsigned int col { 0 }; col < image.width; ++col ) {
                unsigned int const average { (pixel->r + pixel->g + pixel->b) / 3u };
                stream << (average & 0x80 ? '0' : '1');
                ++pixel;
            }
            stream << '\n';
        }
    } else /* not m_ascii_output */ {
        // In binary BPM files every pixel is represented by a single bit, where 0 indicates white (no ink), and 1
        // represents black (ink).
        stream << "P4\n" << image.width << ' ' << image.height << '\n';
        for ( unsigned int row { 0 }; row < image.height; ++row ) {
            unsigned char mini_buffer { 0 };
            for ( unsigned int col { 0 }; col < image.width; ++col ) {
                // Compute the average of the red, green, and blue components to determine whether the pixel should be
                // black or white.
                unsigned int const average { (pixel->r + pixel->g + pixel->b) / 3u };
                unsigned int const bit_index { 7 - col % 8 }; // Index of the relevant bit within the current byte
                mini_buffer |= (average & 0x80 ? 0 : 1) << bit_index;
                if ( col % 8 == 7 ) {
                    stream << mini_buffer;
                    mini_buffer = 0;
                }
                ++pixel;
            }
            // Any leftover bits should be padded with zeroes until it's a full byte, and then written to the stream.
            // Here, the 'mini_buffer' is always reset to 0, so padding isn't necessary anymore.
            if ( image.width % 8 )
                stream << mini_buffer;
        }
    }
}

Image PGMImageIO::load( std::istream & stream ) const {
    auto const [success, header] { read_header( stream ) };

    if ( not success )
        return { 0, 0, nullptr };
    if ( header.type != AsciiGrey and header.type != BinaryGrey ) {
        Log::error( "Invalid header for a PGM file; expected '2' or '5', but got '", header.type, "'." );
        return { 0, 0, nullptr };
    }

    Image image { header.width, header.height };
    Pixel * pixel { image.pixels.get() };

    if ( header.type == AsciiGrey ) {
        // Ascii PGM files contain values ranging from 0 to 255 (or at least the supported files use this range), where
        // values represent the brightness of a pixel. Here, 0 indicates black, 255 indicates white, and all other
        // values indicate a shade of grey.
        for ( Pixel const * const end { pixel + header.width * header.height }; pixel != end; ++pixel ) {
            unsigned int value;
            stream >> value;
            unsigned char const same_value { static_cast<unsigned char>(value) };
            *pixel = { same_value, same_value, same_value, 255 };
        }
    } else /* header.type == BinaryGrey */ {
        // Binary PGM files contain the same values, but as a single byte instead of a character string.
        for ( Pixel const * const end { pixel + header.width * header.height }; pixel != end; ++pixel ) {
            unsigned char value;
            stream.get( reinterpret_cast<char &>(value) );
            *pixel = { value, value, value, 255 };
        }
    }
    return image;
}

void PGMImageIO::save( Image const & image, std::ostream & stream ) const {
    Pixel const * pixel { image.pixels.get() };

    stream << 'P' << (m_ascii_output ? '2' : '5') << '\n' << image.width << ' ' << image.height << "\n255\n";
    for ( unsigned int i { 0 }; i < image.width * image.height; ++i ) {
        unsigned int const average { (pixel->r + pixel->g + pixel->b) / 3u };
        if ( m_ascii_output )
            stream << average << (i % image.width == image.width - 1 ? '\n' : ' ');
        else
            stream << static_cast<char>(average);
        ++pixel;
    }
}

Image PPMImageIO::load( std::istream & stream ) const {
    auto const [success, header] { read_header( stream ) };

    if ( not success )
        return { 0, 0, nullptr };
    if ( header.type != AsciiPix and header.type != BinaryPix ) {
        Log::error( "Invalid header for a PPM file; expected '3' or '6', but got '", header.type, "'." );
        return { 0, 0, nullptr };
    }

    Image image { header.width, header.height };
    Pixel * pixel { image.pixels.get() };

    if ( header.type == AsciiPix ) {
        // Ascii PPM files contain 3 values per pixel, one for each of the red, green, and blue components.
        for ( Pixel const * const end { pixel + header.width * header.height }; pixel != end; ++pixel ) {
            stream >> reinterpret_cast<unsigned int &>(pixel->r);
            stream >> reinterpret_cast<unsigned int &>(pixel->g);
            stream >> reinterpret_cast<unsigned int &>(pixel->b);
            pixel->a = 255;
        }
    } else /* header.type == BinaryPix */ {
        // Binary PPM files contain 3 bytes per pixel, with each byte representing one of the main colour components.
        for ( Pixel const * const end { pixel + header.width * header.height }; pixel != end; ++pixel ) {
            stream.get( reinterpret_cast<char &>(pixel->r) );
            stream.get( reinterpret_cast<char &>(pixel->g) );
            stream.get( reinterpret_cast<char &>(pixel->b) );
            pixel->a = 255;
        }
    }
    return image;
}

void PPMImageIO::save( Image const & image, std::ostream & stream ) const {
    Pixel const * pixel { image.pixels.get() };

    stream << 'P' << (m_ascii_output ? '3' : '6') << '\n' << image.width << ' ' << image.height << "\n255\n";
    for ( unsigned int i { 0 }; i < image.width * image.height; ++i ) {
        if ( m_ascii_output ) {
            stream << static_cast<unsigned int>(pixel->r) << ' ';
            stream << static_cast<unsigned int>(pixel->g) << ' ';
            stream << static_cast<unsigned int>(pixel->b) << ((i + 1) % image.width ? "\n" : "\n\n");
        } else
            stream.write( reinterpret_cast<char const *>(pixel), 3 );
        ++pixel;
    }
}

Image PAMImageIO::load( std::istream & stream ) const {
    char file_id[2];
    stream.read( file_id, 2 );
    if ( file_id[0] != 'P' or file_id[1] != '7' ) {
        Log::error( R"(Invalid header for a PNM file; expected "P7", but got ")", file_id, "\"." );
        return { 0, 0, nullptr };
    }

    // The PAM header is different from the other headers
    std::string line_type;
    unsigned int width, height, temp;
    while ( stream >> line_type ) {
        if ( line_type == "WIDTH" )
            stream >> width;
        else if ( line_type == "HEIGHT" )
            stream >> height;
        else if ( line_type == "DEPTH" ) {
            stream >> temp;
            if ( temp != 4 )
                Log::warning( "PAM file has a depth of ", temp, ", which is not supported; using 4 instead." );
        } else if ( line_type == "MAXVAL" ) {
            stream >> temp;
            if ( temp != 255 )
                Log::warning( "PAM file has a max value of ", temp, ", which is not supported; using 255 instead." );
        } else if ( line_type == "TUPLTYPE" ) {
            std::string tuple_type;
            stream >> tuple_type;
            if ( tuple_type != "RGB_ALPHA" )
                Log::warning( "PAM file has a tuple type of '", tuple_type, "', which is not supported;"
                              "using RGB_ALPHA instead." );
        } else if ( line_type == "ENDHDR" )
            break;
        else {
            Log::error( "Unknown header field detected in PAM file." );
            return { 0, 0, nullptr };
        }
    }
    char c;
    stream.get( c );
    if ( c != '\n' ) {
        Log::warning( "Shouldn't there be a new line here?" );
        stream.putback( c );
    }

    Image image { width, height };
    stream.read( reinterpret_cast<char *>(image.pixels.get()), width * height * 4 );
    return image;
}

void PAMImageIO::save( Image const & image, std::ostream & stream ) const {
    Pixel * pixel { image.pixels.get() };

    // PAM files do not have an Ascii version, so that setting can simply be ignored. Because only RGB_ALPHA is
    // supported as a tuple type, the pixels can simply be written to the file as they are.
    stream << "P7\n" << "WIDTH " << image.width << "\nHEIGHT " << image.height
        << "\nDEPTH 4\nMAXVAL 255\nTUPLTYPE RGB_ALPHA\nENDHDR\n";
    for ( Pixel const * const end { pixel + image.width * image.height }; pixel != end; ++pixel )
        stream.write( reinterpret_cast<char *>(pixel), 4 );
}

Image PNMImageIO::load( std::istream & stream ) const {
    char file_id[2];
    stream.read( file_id, 2 );
    if ( file_id[0] != 'P' or file_id[1] < '1' or file_id[1] > '7' ) {
        Log::error( "Invalid header for a PNM file; expected 'P', but got '", file_id[0], "'." );
        return { 0, 0, nullptr };
    }

    // Put the 2 extracted characters back because the other load() functions expect them to be there.
    stream.putback( file_id[1] );
    stream.putback( file_id[0] );

    switch ( file_id[1] ) {
    case AsciiBit:
    case BinaryBit:
        return PBMImageIO().load( stream );
    case AsciiGrey:
    case BinaryGrey:
        return PGMImageIO().load( stream );
    case AsciiPix:
    case BinaryPix:
        return PPMImageIO().load( stream );
    case Arbitrary:
        return PAMImageIO().load( stream );
    default:
        Log::error( "Invalid header for a PNM file; expected ['1'...'7'], but got '", file_id[1], "'." );
        return { 0, 0, nullptr };
    }
}

void PNMImageIO::save( Image const & image, std::ostream & stream ) const {
    Pixel const * const begin { image.pixels.get() };
    Pixel const * const end { begin + image.width * image.height };

    // If any of the pixels are transparent, then PAM is required
    for ( Pixel const * pixel { begin }; pixel != end; ++pixel )
        if ( pixel->a != 255 )
            return PAMImageIO().save( image, stream );

    // If any of the pixels are not grey, then PPM is required
    for ( Pixel const * pixel { begin }; pixel != end; ++pixel )
        if ( pixel->r != pixel->g or pixel->r != pixel->b or pixel->g != pixel->b )
            return PPMImageIO().save( image, stream );

    // If any of the pixels are not black or white, then PGM is required
    for ( Pixel const * pixel { begin }; pixel != end; ++pixel )
        if ( (pixel->r and !~pixel->r) or (pixel->g and !~pixel->g) or (pixel->b and !~pixel->b) )
            return PGMImageIO().save( image, stream );

    // If none of the above were true, then PBM is an option
    return PBMImageIO().save( image, stream );
}
