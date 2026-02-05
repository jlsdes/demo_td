#include "image.hpp"
#include "utils/log.hpp"

#include <fstream>


Image::Image( unsigned int const width, unsigned int const height, unsigned int const nr_channels )
    : m_pixels { std::make_unique<unsigned char[]>( width * height * nr_channels ) },
      m_width { width }, m_height { height }, m_nr_channels { nr_channels } {}

void load_ascii_bitmap( std::istream & stream, unsigned char const * const pixels ) {
    // ASCII pbm consists entirely of zeroes and ones

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

    unsigned int width, height, nr_channels;

    if ( file_type != '7' ) {
        file >> width;
        file >> height;
        unsigned int constexpr type_channels[] { 1, 1, 3, 1, 1, 3 };
        nr_channels = file_type < 7 ? type_channels[file_type] : 4;
    } else {
        Log::error( "Not implemented yet." );
        return nullptr;
    }

    auto image { std::make_unique<Image>( width, height, nr_channels ) };
    switch (file_type) {
    case '1':
        load_ascii_bitmap( file, image->m_pixels.get() );
        break;
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
        Log::error( "Not implemented yet." );
        return nullptr;
    default:
        Log::error( "This should not be reachable; get '", file_type, ", but this should be in ['1', '7']." );
        return nullptr;
    }

    return image;
}

bool Image::save( std::filesystem::path const & filename ) const {
    Log::error( "Not implemented" );
    return false;
}
