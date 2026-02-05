#include "image.hpp"
#include "log.hpp"

#include <fstream>
#include <functional>


Image::Image( unsigned int const width, unsigned int const height )
    : m_pixels { std::make_unique<unsigned char[]>( width * height * 4 ) },
      m_width { width }, m_height { height } {}

std::unique_ptr<Image> Image::load( std::filesystem::path const & filename ) {
    std::ifstream file { filename };
    if ( not file.is_open() ) {
        Log::error( "Failed to open file '", filename, "'." );
        return nullptr;
    }

    char file_identifier, file_type;
    file.get( file_identifier );
    file.get( file_type );
    if ( file_identifier != 'P' or file_type < '1' or file_type > '7' ) {
        Log::error( "File '", filename, "' is not a valid Netpbm file." );
        return nullptr;
    }

    unsigned int width, height;

    if ( file_type != '7' ) {
        file >> width;
        file >> height;
    } else {
        Log::error( "Not implemented yet." );
        return nullptr;
    }

    auto image { std::make_unique<Image>( width, height ) };
    switch ( file_type ) {
    case AsciiBit:
    case AsciiGray:
    case AsciiPix:
    case BinaryBit:
    case BinaryGray:
    case BinaryPix:
    case Arbitrary:
        Log::error( "Not implemented yet." );
        return nullptr;
    default:
        Log::error( "This should not be reachable; got '", file_type, ", but this should be in ['1', '7']." );
        return nullptr;
    }
    return image;
}

void write_ascii_bit( std::ostream & stream,
                      unsigned char const * const pixels,
                      unsigned int const width,
                      unsigned int const height ) {
    unsigned char const * current { pixels };
    for ( unsigned int row { 0 }; row < height; ++row ) {
        for ( unsigned int col { 0 }; col < width; ++col ) {
            unsigned int const average { (*current + *(current + 1) + *(current + 2)) / 3u };
            stream << (average < 0x10);
            current += 4;
        }
        stream << '\n';
    }
}

void write_ascii_gray( std::ostream & stream,
                       unsigned char const * const pixels,
                       unsigned int const width,
                       unsigned int const height ) {
    unsigned char const * current { pixels };
    for ( unsigned int row { 0 }; row < height; ++row ) {
        for ( unsigned int col { 0 }; col < width; ++col ) {
            unsigned int const average { (*current + *(current + 1) + *(current + 2)) / 3u };
            stream << average << (col == width - 1 ? '\n' : ' ');
            current += 4;
        }
    }
}

void write_ascii_pix( std::ostream & stream,
                      unsigned char const * const pixels,
                      unsigned int const width,
                      unsigned int const height ) {
    unsigned char const * current { pixels };
    for ( unsigned int row { 0 }; row < height; ++row ) {
        for ( unsigned int col { 0 }; col < width; ++col ) {
            stream << static_cast<unsigned int>(*current) << ' ';
            stream << static_cast<unsigned int>(*++current) << ' ';
            stream << static_cast<unsigned int>(*++current) << '\n';
            current += 2;
        }
        stream << '\n';
    }
}

void write_binary_bit( std::ostream & stream,
                       unsigned char const * const pixels,
                       unsigned int const width,
                       unsigned int const height ) {
    unsigned char const * current { pixels };
    for ( unsigned int row { 0 }; row < height; ++row ) {

        unsigned char mini_buffer { 0 };

        for ( unsigned int col { 0 }; col < width; ++col ) {
            unsigned int const average { (*current + *(current + 1) + *(current + 2)) / 3u };
            current += 4;

            unsigned int const bit_index { 7 - col % 8 };
            mini_buffer |= (average < 0x10) << bit_index;
            if ( col % 8 == 7 ) {
                stream << mini_buffer;
                mini_buffer = 0;
            }
        }
        // Write any leftover bits padded with 0s until the end of the byte
        if ( width % 8 )
            stream << mini_buffer;
    }
}

void write_binary_gray( std::ostream & stream,
                        unsigned char const * const pixels,
                        unsigned int const width,
                        unsigned int const height ) {
    unsigned char const * current { pixels };
    for ( unsigned int i { 0 }; i < width * height; ++i ) {
        stream << static_cast<unsigned char>((*current + *(current + 1) + *(current + 2)) / 3u);
        current += 4;
    }
}

void write_binary_pix( std::ostream & stream,
                       unsigned char const * const pixels,
                       unsigned int const width,
                       unsigned int const height ) {
    unsigned char const * current { pixels };
    for ( unsigned int i { 0 }; i < width * height; ++i ) {
        stream << *current << *(current + 1) << *(current + 2);
        current += 4;
    }
}

bool Image::save( std::filesystem::path const & filename, FileType const type ) const {
    std::ofstream file { filename };
    if ( not file.is_open() ) {
        Log::error( "Failed to open file '", filename, "'." );
        return false;
    }

    if ( type != Arbitrary ) {
        file << 'P' << type << '\n' << m_width << ' ' << m_height << '\n';
        if ( type != AsciiBit and type != BinaryBit )
            file << "255\n";
    }

    std::function<void ( std::ostream &, unsigned char const *, unsigned int, unsigned int )> writers[] {
        write_ascii_bit,
        write_ascii_gray,
        write_ascii_pix,
        write_binary_bit,
        write_binary_gray,
        write_binary_pix,
        nullptr
    };

    auto const & writer { writers[type - '1'] };
    if ( not writer ) {
        Log::error( "Not implemented yet..." );
        return false;
    }
    writer( file, m_pixels.get(), m_width, m_height );
    return true;
}

unsigned char * Image::get( unsigned int const row, unsigned int const col ) {
    return m_pixels.get() + (row * m_width + col) * 4;
}
