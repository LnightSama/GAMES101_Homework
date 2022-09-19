#pragma once
#include <cstdint>
#include <fstream>
#include <vector>

#pragma pack(push,1)
struct TGAHeader {
    std::uint8_t  idlength{};
    std::uint8_t  colormaptype{};
    std::uint8_t  datatypecode{};
    std::uint16_t colormaporigin{};
    std::uint16_t colormaplength{};
    std::uint8_t  colormapdepth{};
    std::uint16_t x_origin{};
    std::uint16_t y_origin{};
    std::uint16_t width{};
    std::uint16_t height{};
    std::uint8_t  bitsperpixel{};
    std::uint8_t  imagedescriptor{};
};
#pragma pack(pop)

struct TGAColor {
    std::uint8_t bgra[4] = {0,0,0,0};
    std::uint8_t bytespp = {0};

    TGAColor() = default;
    TGAColor(const std::uint8_t R, const std::uint8_t G, const std::uint8_t B, const std::uint8_t A=255) : bgra{B,G,R,A}, bytespp(4) { }
    TGAColor(const std::uint8_t *p, const std::uint8_t bpp) : bytespp(bpp) {
        for (int i=bpp; i--; bgra[i] = p[i]);
    }
	//TGAColor(const vec3 v, const std::uint8_t A = 255) : bytespp(4) {
	//	for (int i = 3; i--; bgra[i] = v[3 - i]);
 //       bgra[3] = A;
	//}

    std::uint8_t& operator[](const int i) { return bgra[i]; }

    TGAColor operator+(TGAColor color) {
        return TGAColor(
            static_cast<unsigned int>(bgra[2]) + static_cast<unsigned int>(color.bgra[2]),
            static_cast<unsigned int>(bgra[1]) + static_cast<unsigned int>(color.bgra[1]),
            static_cast<unsigned int>(bgra[0]) + static_cast<unsigned int>(color.bgra[0]),
            255
        );
    }

	TGAColor operator*(double a) {
		return TGAColor(
			static_cast<unsigned int>(bgra[2]) * a,
			static_cast<unsigned int>(bgra[1]) * a,
			static_cast<unsigned int>(bgra[0]) * a,
			255
		);
	}

	TGAColor operator/(double a) {
		return TGAColor(
			static_cast<unsigned int>(bgra[2]) / a,
			static_cast<unsigned int>(bgra[1]) / a,
			static_cast<unsigned int>(bgra[0]) / a,
			255
		);
	}

};

struct TGAImage {
    enum Format { GRAYSCALE=1, RGB=3, RGBA=4 };

    TGAImage() = default;
    TGAImage(const int w, const int h, const int bpp);
    bool read_jpg_file(const std::string filename);
    bool read_tga_file(const std::string filename);
    bool write_tga_file(const std::string filename, const bool vflip=true, const bool rle=true) const;
    void flip_horizontally(); //水平翻转
    void flip_vertically(); //垂直翻转
    void flip_clockwise90(); //顺时针旋转90°
    void flip_antiClockwise90(); //逆时针旋转90°
    TGAColor get(const int x, const int y) const;
    void set(const int x, const int y, const TGAColor &c);
    int width()  const;
    int height() const;
private:
    bool   load_rle_data(std::ifstream &in);
    bool unload_rle_data(std::ofstream &out) const;

    int w   = 0;
    int h   = 0;
    int bpp = 0;
    std::vector<std::uint8_t> data = {};
};

