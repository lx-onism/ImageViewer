#include "pch.h"
#include "gli_interface.h"
#include <gli/gli.hpp>
#include <gli/gl.hpp>
#include "compress_interface.h"
#include "ktx_interface.h"
#include "GliImage.h"


std::unique_ptr<image::IImage> gli_load(const char* filename)
{
	auto res = std::make_unique<GliImage>(gli::load(filename));

	if (image::isSupported(res->getFormat())) return res;

	return res->convert(image::getSupportedFormat(res->getFormat()), 100);
}

std::vector<uint32_t> dds_get_export_formats()
{
	// note: some bgra formats are disabled because im not sure if the default dds loader or gli stores them incorrectly
	// those formats are commented out with: // gli swizzling

	return std::vector<uint32_t>{

	// uniform
	// gli swizzling gli::format::FORMAT_BGRA4_UNORM_PACK16,
	// gli swizzling gli::format::FORMAT_B5G6R5_UNORM_PACK16,
	// gli swizzling gli::format::FORMAT_BGR5A1_UNORM_PACK16,
	gli::format::FORMAT_R8_UNORM_PACK8,
	gli::format::FORMAT_R8_SNORM_PACK8,
	gli::format::FORMAT_RG8_UNORM_PACK8,
	gli::format::FORMAT_RG8_SNORM_PACK8,
	gli::format::FORMAT_RGBA8_UNORM_PACK8,
	gli::format::FORMAT_RGBA8_SNORM_PACK8,
	gli::format::FORMAT_RGBA8_SRGB_PACK8,
	// gli swizzling gli::format::FORMAT_BGRA8_UNORM_PACK8,
	// gli swizzling gli::format::FORMAT_BGRA8_SRGB_PACK8,
	gli::format::FORMAT_RGB10A2_UNORM_PACK32,
	// gli swizzling gli::format::FORMAT_BGR8_UNORM_PACK32,
	// gli swizzling gli::format::FORMAT_BGR8_SRGB_PACK32,

	// float formats
	gli::format::FORMAT_R16_UNORM_PACK16,
	gli::format::FORMAT_R16_SNORM_PACK16,
	gli::format::FORMAT_R16_SFLOAT_PACK16,
	gli::format::FORMAT_RG16_UNORM_PACK16,
	gli::format::FORMAT_RG16_SNORM_PACK16,
	gli::format::FORMAT_RG16_SFLOAT_PACK16,
	gli::format::FORMAT_RGBA16_UNORM_PACK16,
	gli::format::FORMAT_RGBA16_SNORM_PACK16,
	gli::format::FORMAT_RGBA16_SFLOAT_PACK16,
	gli::format::FORMAT_R32_SFLOAT_PACK32,
	gli::format::FORMAT_RG32_SFLOAT_PACK32,
	gli::format::FORMAT_RGB32_SFLOAT_PACK32,
	gli::format::FORMAT_RGBA32_SFLOAT_PACK32,
	gli::format::FORMAT_RG11B10_UFLOAT_PACK32,
	gli::format::FORMAT_RGB9E5_UFLOAT_PACK32,

	// dds compressed
	// DXT
	gli::format::FORMAT_RGBA_DXT1_UNORM_BLOCK8,
	gli::format::FORMAT_RGBA_DXT1_SRGB_BLOCK8,
	gli::format::FORMAT_RGBA_DXT3_UNORM_BLOCK16,
	gli::format::FORMAT_RGBA_DXT3_SRGB_BLOCK16,
	gli::format::FORMAT_RGBA_DXT5_SRGB_BLOCK16,
	gli::format::FORMAT_RGBA_DXT5_UNORM_BLOCK16,
	gli::format::FORMAT_R_ATI1N_UNORM_BLOCK8,
	gli::format::FORMAT_R_ATI1N_SNORM_BLOCK8,
	gli::format::FORMAT_RG_ATI2N_UNORM_BLOCK16,
	gli::format::FORMAT_RG_ATI2N_SNORM_BLOCK16,
	gli::format::FORMAT_RGB_BP_UFLOAT_BLOCK16,
	gli::format::FORMAT_RGB_BP_SFLOAT_BLOCK16,
	gli::format::FORMAT_RGBA_BP_UNORM_BLOCK16,
	gli::format::FORMAT_RGBA_BP_SRGB_BLOCK16,
	/*gli::format::FORMAT_RGB_ETC2_UNORM_BLOCK8,
	gli::format::FORMAT_RGB_ETC2_SRGB_BLOCK8,
	gli::format::FORMAT_RGBA_ETC2_UNORM_BLOCK8,
	gli::format::FORMAT_RGBA_ETC2_SRGB_BLOCK8,
	gli::format::FORMAT_RGBA_ETC2_UNORM_BLOCK16,
	gli::format::FORMAT_RGBA_ETC2_SRGB_BLOCK16,
	gli::format::FORMAT_R_EAC_UNORM_BLOCK8,
	gli::format::FORMAT_R_EAC_SNORM_BLOCK8,
	gli::format::FORMAT_RG_EAC_UNORM_BLOCK16,
	gli::format::FORMAT_RG_EAC_SNORM_BLOCK16,
	gli::format::FORMAT_RGBA_ASTC_4X4_UNORM_BLOCK16,
	gli::format::FORMAT_RGBA_ASTC_4X4_SRGB_BLOCK16,
	gli::format::FORMAT_RGBA_ASTC_5X4_UNORM_BLOCK16,
	gli::format::FORMAT_RGBA_ASTC_5X4_SRGB_BLOCK16,
	gli::format::FORMAT_RGBA_ASTC_5X5_UNORM_BLOCK16,
	gli::format::FORMAT_RGBA_ASTC_5X5_SRGB_BLOCK16,
	gli::format::FORMAT_RGBA_ASTC_6X5_UNORM_BLOCK16,
	gli::format::FORMAT_RGBA_ASTC_6X5_SRGB_BLOCK16,
	gli::format::FORMAT_RGBA_ASTC_6X6_UNORM_BLOCK16,
	gli::format::FORMAT_RGBA_ASTC_6X6_SRGB_BLOCK16,
	gli::format::FORMAT_RGBA_ASTC_8X5_UNORM_BLOCK16,
	gli::format::FORMAT_RGBA_ASTC_8X5_SRGB_BLOCK16,
	gli::format::FORMAT_RGBA_ASTC_8X6_UNORM_BLOCK16,
	gli::format::FORMAT_RGBA_ASTC_8X6_SRGB_BLOCK16,
	gli::format::FORMAT_RGBA_ASTC_8X8_UNORM_BLOCK16,
	gli::format::FORMAT_RGBA_ASTC_8X8_SRGB_BLOCK16,
	gli::format::FORMAT_RGBA_ASTC_10X5_UNORM_BLOCK16,
	gli::format::FORMAT_RGBA_ASTC_10X5_SRGB_BLOCK16,
	gli::format::FORMAT_RGBA_ASTC_10X6_UNORM_BLOCK16,
	gli::format::FORMAT_RGBA_ASTC_10X6_SRGB_BLOCK16,
	gli::format::FORMAT_RGBA_ASTC_10X8_UNORM_BLOCK16,
	gli::format::FORMAT_RGBA_ASTC_10X8_SRGB_BLOCK16,
	gli::format::FORMAT_RGBA_ASTC_10X10_UNORM_BLOCK16,
	gli::format::FORMAT_RGBA_ASTC_10X10_SRGB_BLOCK16,
	gli::format::FORMAT_RGBA_ASTC_12X10_UNORM_BLOCK16,
	gli::format::FORMAT_RGBA_ASTC_12X10_SRGB_BLOCK16,
	gli::format::FORMAT_RGBA_ASTC_12X12_UNORM_BLOCK16,
	gli::format::FORMAT_RGBA_ASTC_12X12_SRGB_BLOCK16,
	gli::format::FORMAT_RGB_PVRTC1_8X8_UNORM_BLOCK32,
	gli::format::FORMAT_RGB_PVRTC1_8X8_SRGB_BLOCK32,
	gli::format::FORMAT_RGB_PVRTC1_16X8_UNORM_BLOCK32,
	gli::format::FORMAT_RGB_PVRTC1_16X8_SRGB_BLOCK32,
	gli::format::FORMAT_RGBA_PVRTC1_8X8_UNORM_BLOCK32,
	gli::format::FORMAT_RGBA_PVRTC1_8X8_SRGB_BLOCK32,
	gli::format::FORMAT_RGBA_PVRTC1_16X8_UNORM_BLOCK32,
	gli::format::FORMAT_RGBA_PVRTC1_16X8_SRGB_BLOCK32,
	gli::format::FORMAT_RGBA_PVRTC2_4X4_UNORM_BLOCK8,
	gli::format::FORMAT_RGBA_PVRTC2_4X4_SRGB_BLOCK8,
	gli::format::FORMAT_RGBA_PVRTC2_8X4_UNORM_BLOCK8,
	gli::format::FORMAT_RGBA_PVRTC2_8X4_SRGB_BLOCK8,
	gli::format::FORMAT_RGB_ETC_UNORM_BLOCK8,
	gli::format::FORMAT_RGB_ATC_UNORM_BLOCK8,
	gli::format::FORMAT_RGBA_ATCA_UNORM_BLOCK16,
	gli::format::FORMAT_RGBA_ATCI_UNORM_BLOCK16,
	gli::format::FORMAT_L8_UNORM_PACK8,
	
	gli::format::FORMAT_LA8_UNORM_PACK8,
	gli::format::FORMAT_L16_UNORM_PACK16,
	gli::format::FORMAT_LA16_UNORM_PACK16,
	*/
	};
}

std::vector<uint32_t> ktx_get_export_formats()
{
	return std::vector<uint32_t>{

		// uniform
		gli::format::FORMAT_RG3B2_UNORM_PACK8,
			gli::format::FORMAT_RGBA4_UNORM_PACK16,
			gli::format::FORMAT_BGRA4_UNORM_PACK16,
			gli::format::FORMAT_R5G6B5_UNORM_PACK16,
			gli::format::FORMAT_B5G6R5_UNORM_PACK16,
			gli::format::FORMAT_RGB5A1_UNORM_PACK16,
			gli::format::FORMAT_BGR5A1_UNORM_PACK16,
			gli::format::FORMAT_R8_UNORM_PACK8,
			gli::format::FORMAT_R8_SNORM_PACK8,
			gli::format::FORMAT_RG8_UNORM_PACK8,
			gli::format::FORMAT_RG8_SNORM_PACK8,
			gli::format::FORMAT_RGB8_UNORM_PACK8,
			gli::format::FORMAT_RGB8_SNORM_PACK8,
			gli::format::FORMAT_RGB8_SRGB_PACK8,
			gli::format::FORMAT_BGR8_UNORM_PACK8,
			gli::format::FORMAT_BGR8_SNORM_PACK8,
			gli::format::FORMAT_BGR8_SRGB_PACK8,
			gli::format::FORMAT_RGBA8_UNORM_PACK8,
			gli::format::FORMAT_RGBA8_SNORM_PACK8,
			gli::format::FORMAT_RGBA8_SRGB_PACK8,
			gli::format::FORMAT_BGRA8_UNORM_PACK8,
			gli::format::FORMAT_BGRA8_SNORM_PACK8,
			gli::format::FORMAT_BGRA8_SRGB_PACK8,
			gli::format::FORMAT_RGBA8_UNORM_PACK32,
			gli::format::FORMAT_RGBA8_SNORM_PACK32,
			gli::format::FORMAT_RGBA8_SRGB_PACK32,
			gli::format::FORMAT_RGB10A2_UNORM_PACK32,
			gli::format::FORMAT_BGR10A2_UNORM_PACK32,
			gli::format::FORMAT_BGR10A2_SNORM_PACK32,
			gli::format::FORMAT_A8_UNORM_PACK8,
			gli::format::FORMAT_A16_UNORM_PACK16,
			gli::format::FORMAT_BGR8_UNORM_PACK32,
			gli::format::FORMAT_BGR8_SRGB_PACK32,

			// float formats
			gli::format::FORMAT_R16_UNORM_PACK16,
			gli::format::FORMAT_R16_SNORM_PACK16,
			gli::format::FORMAT_R16_SFLOAT_PACK16,
			gli::format::FORMAT_RG16_UNORM_PACK16,
			gli::format::FORMAT_RG16_SNORM_PACK16,
			gli::format::FORMAT_RG16_SFLOAT_PACK16,
			gli::format::FORMAT_RGB16_UNORM_PACK16,
			gli::format::FORMAT_RGB16_SNORM_PACK16,
			gli::format::FORMAT_RGB16_SFLOAT_PACK16,
			gli::format::FORMAT_RGBA16_UNORM_PACK16,
			gli::format::FORMAT_RGBA16_SNORM_PACK16,
			gli::format::FORMAT_RGBA16_SFLOAT_PACK16,
			gli::format::FORMAT_R32_SFLOAT_PACK32,
			gli::format::FORMAT_RG32_SFLOAT_PACK32,
			gli::format::FORMAT_RGB32_SFLOAT_PACK32,
			gli::format::FORMAT_RGBA32_SFLOAT_PACK32,
			gli::format::FORMAT_RG11B10_UFLOAT_PACK32,
			gli::format::FORMAT_RGB9E5_UFLOAT_PACK32,

			// dds compressed
			// DXT
			gli::format::FORMAT_RGB_DXT1_UNORM_BLOCK8,
			gli::format::FORMAT_RGB_DXT1_SRGB_BLOCK8,
			gli::format::FORMAT_RGBA_DXT1_UNORM_BLOCK8,
			gli::format::FORMAT_RGBA_DXT1_SRGB_BLOCK8,
			gli::format::FORMAT_RGBA_DXT3_UNORM_BLOCK16,
			gli::format::FORMAT_RGBA_DXT3_SRGB_BLOCK16,
			gli::format::FORMAT_RGBA_DXT5_SRGB_BLOCK16,
			gli::format::FORMAT_RGBA_DXT5_UNORM_BLOCK16,
			gli::format::FORMAT_R_ATI1N_UNORM_BLOCK8,
			gli::format::FORMAT_R_ATI1N_SNORM_BLOCK8,
			gli::format::FORMAT_RG_ATI2N_UNORM_BLOCK16,
			gli::format::FORMAT_RG_ATI2N_SNORM_BLOCK16,
			gli::format::FORMAT_RGB_BP_UFLOAT_BLOCK16,
			gli::format::FORMAT_RGB_BP_SFLOAT_BLOCK16,
			gli::format::FORMAT_RGBA_BP_UNORM_BLOCK16,
			gli::format::FORMAT_RGBA_BP_SRGB_BLOCK16,
			/*gli::format::FORMAT_RGB_ETC2_UNORM_BLOCK8,
			gli::format::FORMAT_RGB_ETC2_SRGB_BLOCK8,
			gli::format::FORMAT_RGBA_ETC2_UNORM_BLOCK8,
			gli::format::FORMAT_RGBA_ETC2_SRGB_BLOCK8,
			gli::format::FORMAT_RGBA_ETC2_UNORM_BLOCK16,
			gli::format::FORMAT_RGBA_ETC2_SRGB_BLOCK16,
			gli::format::FORMAT_R_EAC_UNORM_BLOCK8,
			gli::format::FORMAT_R_EAC_SNORM_BLOCK8,
			gli::format::FORMAT_RG_EAC_UNORM_BLOCK16,
			gli::format::FORMAT_RG_EAC_SNORM_BLOCK16,
			gli::format::FORMAT_RGBA_ASTC_4X4_UNORM_BLOCK16,
			gli::format::FORMAT_RGBA_ASTC_4X4_SRGB_BLOCK16,
			gli::format::FORMAT_RGBA_ASTC_5X4_UNORM_BLOCK16,
			gli::format::FORMAT_RGBA_ASTC_5X4_SRGB_BLOCK16,
			gli::format::FORMAT_RGBA_ASTC_5X5_UNORM_BLOCK16,
			gli::format::FORMAT_RGBA_ASTC_5X5_SRGB_BLOCK16,
			gli::format::FORMAT_RGBA_ASTC_6X5_UNORM_BLOCK16,
			gli::format::FORMAT_RGBA_ASTC_6X5_SRGB_BLOCK16,
			gli::format::FORMAT_RGBA_ASTC_6X6_UNORM_BLOCK16,
			gli::format::FORMAT_RGBA_ASTC_6X6_SRGB_BLOCK16,
			gli::format::FORMAT_RGBA_ASTC_8X5_UNORM_BLOCK16,
			gli::format::FORMAT_RGBA_ASTC_8X5_SRGB_BLOCK16,
			gli::format::FORMAT_RGBA_ASTC_8X6_UNORM_BLOCK16,
			gli::format::FORMAT_RGBA_ASTC_8X6_SRGB_BLOCK16,
			gli::format::FORMAT_RGBA_ASTC_8X8_UNORM_BLOCK16,
			gli::format::FORMAT_RGBA_ASTC_8X8_SRGB_BLOCK16,
			gli::format::FORMAT_RGBA_ASTC_10X5_UNORM_BLOCK16,
			gli::format::FORMAT_RGBA_ASTC_10X5_SRGB_BLOCK16,
			gli::format::FORMAT_RGBA_ASTC_10X6_UNORM_BLOCK16,
			gli::format::FORMAT_RGBA_ASTC_10X6_SRGB_BLOCK16,
			gli::format::FORMAT_RGBA_ASTC_10X8_UNORM_BLOCK16,
			gli::format::FORMAT_RGBA_ASTC_10X8_SRGB_BLOCK16,
			gli::format::FORMAT_RGBA_ASTC_10X10_UNORM_BLOCK16,
			gli::format::FORMAT_RGBA_ASTC_10X10_SRGB_BLOCK16,
			gli::format::FORMAT_RGBA_ASTC_12X10_UNORM_BLOCK16,
			gli::format::FORMAT_RGBA_ASTC_12X10_SRGB_BLOCK16,
			gli::format::FORMAT_RGBA_ASTC_12X12_UNORM_BLOCK16,
			gli::format::FORMAT_RGBA_ASTC_12X12_SRGB_BLOCK16,
			gli::format::FORMAT_RGB_PVRTC1_8X8_UNORM_BLOCK32,
			gli::format::FORMAT_RGB_PVRTC1_8X8_SRGB_BLOCK32,
			gli::format::FORMAT_RGB_PVRTC1_16X8_UNORM_BLOCK32,
			gli::format::FORMAT_RGB_PVRTC1_16X8_SRGB_BLOCK32,
			gli::format::FORMAT_RGBA_PVRTC1_8X8_UNORM_BLOCK32,
			gli::format::FORMAT_RGBA_PVRTC1_8X8_SRGB_BLOCK32,
			gli::format::FORMAT_RGBA_PVRTC1_16X8_UNORM_BLOCK32,
			gli::format::FORMAT_RGBA_PVRTC1_16X8_SRGB_BLOCK32,
			gli::format::FORMAT_RGBA_PVRTC2_4X4_UNORM_BLOCK8,
			gli::format::FORMAT_RGBA_PVRTC2_4X4_SRGB_BLOCK8,
			gli::format::FORMAT_RGBA_PVRTC2_8X4_UNORM_BLOCK8,
			gli::format::FORMAT_RGBA_PVRTC2_8X4_SRGB_BLOCK8,
			gli::format::FORMAT_RGB_ETC_UNORM_BLOCK8,
			gli::format::FORMAT_RGB_ATC_UNORM_BLOCK8,
			gli::format::FORMAT_RGBA_ATCA_UNORM_BLOCK16,
			gli::format::FORMAT_RGBA_ATCI_UNORM_BLOCK16,
			gli::format::FORMAT_L8_UNORM_PACK8,

			gli::format::FORMAT_LA8_UNORM_PACK8,
			gli::format::FORMAT_L16_UNORM_PACK16,
			gli::format::FORMAT_LA16_UNORM_PACK16,
			*/
	};
}

void gli_save_image(const char* filename, GliImage& image, gli::format format, bool ktx, int quality)
{

	if(image.getFormat() == format)
	{
		if (ktx) image.saveKtx(filename);
		else image.saveDds(filename);
		return;
	}

	auto res = image.convert(format, quality);
	if (ktx) res->saveKtx(filename);
	else res->saveDds(filename);
}