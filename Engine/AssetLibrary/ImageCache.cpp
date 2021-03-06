#include "ImageCache.hpp"

#include "Image.hpp"


namespace inl::asset {


template <gxeng::ePixelChannelType Type>
static gxeng::IPixelReader& GetPixelReaderCh(int channelCount) {
	switch (channelCount) {
		case 1: return gxeng::Pixel<Type, 1, gxeng::ePixelClass::LINEAR>::Reader();
		case 2: return gxeng::Pixel<Type, 2, gxeng::ePixelClass::LINEAR>::Reader();
		case 3: return gxeng::Pixel<Type, 3, gxeng::ePixelClass::LINEAR>::Reader();
		case 4: return gxeng::Pixel<Type, 4, gxeng::ePixelClass::LINEAR>::Reader();
		default: throw InvalidArgumentException("Image must have 1-4 channels.");
	}
}


static gxeng::IPixelReader& GetPixelReader(eChannelType channelType, int channelCount) {
	switch (channelType) {
		case eChannelType::INT8: return GetPixelReaderCh<gxeng::ePixelChannelType::INT8_NORM>(channelCount);
		case eChannelType::INT16: return GetPixelReaderCh<gxeng::ePixelChannelType::INT16_NORM>(channelCount);
		case eChannelType::INT32: return GetPixelReaderCh<gxeng::ePixelChannelType::INT32>(channelCount);
		case eChannelType::FLOAT: return GetPixelReaderCh<gxeng::ePixelChannelType::FLOAT32>(channelCount);
		default: std::terminate();
	}
}


ImageCache::ImageCache(gxeng::IGraphicsEngine& engine)
	: m_engine(engine) {}


std::shared_ptr<gxeng::IImage> ImageCache::Create(const std::filesystem::path& path) {
	std::shared_ptr<gxeng::IImage> resource(m_engine.CreateImage());
	Reload(*resource, path);
	return resource;
}


void ImageCache::Reload(gxeng::IImage& asset, const std::filesystem::path& path) {
	Image image{ path.generic_u8string() };
	int channelCount = image.GetChannelCount();
	eChannelType channelType = image.GetType();

	gxeng::IPixelReader& reader = GetPixelReader(channelType, channelCount);

	gxeng::ePixelChannelType gxChannelType = [&] {
		switch (channelType) {
			case eChannelType::INT8: return gxeng::ePixelChannelType::INT8_NORM;
			case eChannelType::INT16: return gxeng::ePixelChannelType::INT16_NORM;
			case eChannelType::INT32: return gxeng::ePixelChannelType::INT32;
			case eChannelType::FLOAT: return gxeng::ePixelChannelType::FLOAT32;
			default: assert(false); return gxeng::ePixelChannelType::INT8_NORM;
		}
	}();

	asset.SetLayout(image.GetWidth(), (uint32_t)image.GetHeight(), gxChannelType, channelCount, gxeng::ePixelClass::LINEAR);
	asset.Update(0, 0, image.GetWidth(), (uint32_t)image.GetHeight(), 0, image.GetData(), reader);
}


} // namespace inl::asset
