#include "image.hpp"
#include "log.hpp"

#include <fstream>
#include <functional>


Image::Image( unsigned int const width, unsigned int const height )
    : m_pixels { std::make_unique<unsigned char[]>( width * height * 4 ) },
      m_width { width }, m_height { height } {}

std::pair<unsigned int, unsigned int> read_header( std::istream & stream, bool const read_max_value ) {
    unsigned int width, height;
    stream >> width >> height;

    if ( read_max_value ) {
        unsigned int max_value;
        stream >> max_value;
        if ( max_value != 255 )
            Log::warning( "PNM files are only supported with a max value of 255, not ", max_value,
                          "; using 255 anyway." );
    }
    return { width, height };
}


void read_ascii_bit( std::istream & stream,
                     unsigned char * pixels,
                     unsigned int const width,
                     unsigned int const height ) {
    char c;
    for ( unsigned int i { 0 }; i < width * height and stream.good(); ++i ) {
        stream >> c;
        if ( c == '0' )
            pixels[0] = 255;
        else if ( c == '1' )
            pixels[0] = 0;
        else {
            Log::error( "Unexpected character '", c, "' in image data." );
            return;
        }
        pixels[1] = pixels[0];
        pixels[2] = pixels[0];
        pixels[3] = 255;
        pixels += 4;
    }
}

void read_ascii_gray( std::istream & stream,
                      unsigned char * pixels,
                      unsigned int const width,
                      unsigned int const height ) {
    unsigned int value;
    for ( unsigned int i { 0 }; i < width * height and stream.good(); ++i ) {
        stream >> value;
        pixels[0] = value;
        pixels[1] = value;
        pixels[2] = value;
        pixels[3] = 255;
        pixels += 4;
    }
}

void read_ascii_pix( std::istream & stream,
                     unsigned char * pixels,
                     unsigned int const width,
                     unsigned int const height ) {
    unsigned int r, g, b;
    for ( unsigned int i { 0 }; i < width * height and stream.good(); ++i ) {
        stream >> r >> g >> b;
        pixels[0] = r;
        pixels[1] = g;
        pixels[2] = b;
        pixels[3] = 255;
        pixels += 4;
    }
}

void read_binary_bit( std::istream & stream,
                      unsigned char * pixels,
                      unsigned int const width,
                      unsigned int const height ) {
    unsigned char mini_buffer;
    for ( unsigned int row { 0 }; row < height; ++row ) {
        for ( unsigned int col { 0 }; col < width; ++col ) {
            if ( col % 8 == 0 )
                stream.get( reinterpret_cast<char &>(mini_buffer) );

            auto const value { mini_buffer & 0x80 ? 0u : 255u };
            mini_buffer <<= 1;
            pixels[0] = value;
            pixels[1] = value;
            pixels[2] = value;
            pixels[3] = 255;
            pixels += 4;
        }
    }
}

void read_binary_gray( std::istream & stream,
                       unsigned char * pixels,
                       unsigned int const width,
                       unsigned int const height ) {
    for ( auto const end { pixels + 4 * width * height }; pixels != end and stream.good(); pixels += 4 ) {
        stream.get( reinterpret_cast<char &>(pixels[0]) );
        pixels[1] = pixels[0];
        pixels[2] = pixels[0];
        pixels[3] = 255;
    }
}

void read_binary_pix( std::istream & stream,
                      unsigned char * pixels,
                      unsigned int const width,
                      unsigned int const height ) {
    for ( auto const end { pixels + 4 * width * height }; pixels != end and stream.good(); pixels += 4 ) {
        stream.read( reinterpret_cast<char *>(pixels), 3 );
        pixels[3] = 255;
    }
}

void read_arbitrary( std::istream & stream,
                     unsigned char * pixels,
                     unsigned int const width,
                     unsigned int const height ) {
    for ( auto const end { pixels + 4 * width * height }; pixels != end and stream.good(); pixels += 4 )
        stream.read( reinterpret_cast<char *>(pixels), 4 );
}

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
    if ( file_type != Arbitrary ) {
        file >> width;
        file >> height;

        if ( file_type != AsciiBit and file_type != BinaryBit ) {
            unsigned int max_value;
            file >> max_value;
            if ( max_value != 255 )
                Log::warning( "Maximum values in PNM files other than 255 are not supported, falling back to 255." );
        }
        if ( file.get() != '\n' ) {
            Log::error( "PNM file '", filename, "' has some inconsistencies at the end of its header." );
            return nullptr;
        }
    } else {
        std::string line_type;
        unsigned int temp;
        while ( file >> line_type ) {
            if ( line_type == "WIDTH" )
                file >> width;
            else if ( line_type == "HEIGHT" )
                file >> height;
            else if ( line_type == "DEPTH" ) {
                file >> temp;
                if ( temp != 4 )
                    Log::warning( "PAM file '", filename, "' has a depth of ", temp, ", which is not supported;"
                                  "using 4 instead." );
            } else if ( line_type == "MAXVAL" ) {
                file >> temp;
                if ( temp != 255 )
                    Log::warning( "PAM file '", filename, "' has a max value of ", temp, ", which is not supported;"
                                  "using 255 instead." );
            } else if ( line_type == "TUPLTYPE" ) {
                std::string tuple_type;
                file >> tuple_type;
                if ( tuple_type != "RGB_ALPHA" )
                    Log::warning( "PAM file '", filename, "' has a tuple type of '", tuple_type, "', which is not"
                                  "supported; using RGB_ALPHA instead." );
            } else if ( line_type == "ENDHDR" )
                break;
            else {
                Log::error( "Unknown header field detected in PAM file '", filename, "'." );
                return nullptr;
            }

        }
    }
    auto image { std::make_unique<Image>( width, height ) };

    std::function<void ( std::istream &, unsigned char *, unsigned int, unsigned int )> const readers[] {
        read_ascii_bit,
        read_ascii_gray,
        read_ascii_pix,
        read_binary_bit,
        read_binary_gray,
        read_binary_pix,
        read_arbitrary
    };

    auto const & reader { readers[file_type - '1'] };
    if ( not reader ) {
        Log::error( "Not implemented yet." );
        return nullptr;
    }
    reader( file, image->m_pixels.get(), width, height );

    if ( file.bad() ) {
        Log::error( "Something went wrong while reading file '", filename, "'." );
        return nullptr;
    }
    return image;
}

void write_ascii_bit( std::ostream & stream,
                      unsigned char const * pixels,
                      unsigned int const width,
                      unsigned int const height ) {
    for ( unsigned int row { 0 }; row < height; ++row ) {
        for ( unsigned int col { 0 }; col < width; ++col ) {
            unsigned int const average { (pixels[0] + pixels[1] + pixels[2]) / 3u };
            stream << (average < 0x10);
            pixels += 4;
        }
        stream << '\n';
    }
}

void write_ascii_gray( std::ostream & stream,
                       unsigned char const * pixels,
                       unsigned int const width,
                       unsigned int const height ) {
    for ( unsigned int row { 0 }; row < height; ++row ) {
        for ( unsigned int col { 0 }; col < width; ++col ) {
            stream << (pixels[0] + pixels[1] + pixels[2]) / 3u << (col == width - 1 ? '\n' : ' ');
            pixels += 4;
        }
    }
}

void write_ascii_pix( std::ostream & stream,
                      unsigned char const * pixels,
                      unsigned int const width,
                      unsigned int const height ) {
    for ( unsigned int row { 0 }; row < height; ++row ) {
        for ( unsigned int col { 0 }; col < width; ++col ) {
            stream << static_cast<unsigned int>(pixels[0]) << ' ';
            stream << static_cast<unsigned int>(pixels[1]) << ' ';
            stream << static_cast<unsigned int>(pixels[2]) << '\n';
            pixels += 4;
        }
        stream << '\n';
    }
}

void write_binary_bit( std::ostream & stream,
                       unsigned char const * pixels,
                       unsigned int const width,
                       unsigned int const height ) {
    for ( unsigned int row { 0 }; row < height; ++row ) {
        unsigned char mini_buffer { 0 };

        for ( unsigned int col { 0 }; col < width; ++col ) {
            unsigned int const average { (pixels[0] + pixels[1] + pixels[2]) / 3u };
            pixels += 4;

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
                        unsigned char const * pixels,
                        unsigned int const width,
                        unsigned int const height ) {
    for ( auto const end { pixels + 4 * width * height }; pixels != end; pixels += 4 )
        stream << static_cast<unsigned char>((pixels[0] + pixels[1] + pixels[2]) / 3u);
}

void write_binary_pix( std::ostream & stream,
                       unsigned char const * pixels,
                       unsigned int const width,
                       unsigned int const height ) {
    for ( auto const end { pixels + 4 * width * height }; pixels != end; pixels += 4 )
        stream.write( reinterpret_cast<char const *>(pixels), 3 );
}

void write_arbitrary( std::ostream & stream,
                      unsigned char const * pixels,
                      unsigned int const width,
                      unsigned int const height ) {
    for ( unsigned int i { 0 }; i < width * height; ++i ) {
        stream.write( reinterpret_cast<char const *>(pixels), 4 );
        pixels += 4;
    }
}

bool Image::save( std::filesystem::path const & filename, FileType const type ) const {
    std::ofstream file { filename };
    if ( not file.is_open() ) {
        Log::error( "Failed to open file '", filename, "'." );
        return false;
    }

    if ( type == Arbitrary )
        file << "P7\n"
            << "WIDTH " << m_width << "\nHEIGHT " << m_height
            << "\nDEPTH 4\nMAXVAL 255\nTUPLTYPE RGB_ALPHA\nENDHDR \n";
    else {
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
        write_arbitrary
    };

    writers[type - '1']( file, m_pixels.get(), m_width, m_height );
    return true;
}

unsigned char * Image::get( unsigned int const row, unsigned int const col ) {
    return m_pixels.get() + (row * m_width + col) * 4;
}
