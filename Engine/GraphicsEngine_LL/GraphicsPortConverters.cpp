#include "GraphicsPortConverters.hpp"


namespace inl {

//------------------------------------------------------------------------------
// Generators
//------------------------------------------------------------------------------

template <>
std::vector<std::pair<gxapi::eFormat, std::string>> impl::ParseTableGenerator() {
	static std::vector<std::pair<gxapi::eFormat, std::string>> records = {
		{ gxapi::eFormat::UNKNOWN, "UNKNOWN" },

		{ gxapi::eFormat::R32G32B32A32_TYPELESS, "R32G32B32A32_TYPELESS" },
		{ gxapi::eFormat::R32G32B32A32_FLOAT, "R32G32B32A32_FLOAT" },
		{ gxapi::eFormat::R32G32B32A32_UINT, "R32G32B32A32_UINT" },
		{ gxapi::eFormat::R32G32B32A32_SINT, "R32G32B32A32_SINT" },

		{ gxapi::eFormat::R32G32B32_TYPELESS, "R32G32B32_TYPELESS" },
		{ gxapi::eFormat::R32G32B32_FLOAT, "R32G32B32_FLOAT" },
		{ gxapi::eFormat::R32G32B32_UINT, "R32G32B32_UINT" },
		{ gxapi::eFormat::R32G32B32_SINT, "R32G32B32_SINT" },

		{ gxapi::eFormat::R16G16B16A16_TYPELESS, "R16G16B16A16_TYPELESS" },
		{ gxapi::eFormat::R16G16B16A16_FLOAT, "R16G16B16A16_FLOAT" },
		{ gxapi::eFormat::R16G16B16A16_UNORM, "R16G16B16A16_UNORM" },
		{ gxapi::eFormat::R16G16B16A16_UINT, "R16G16B16A16_UINT" },
		{ gxapi::eFormat::R16G16B16A16_SNORM, "R16G16B16A16_SNORM" },
		{ gxapi::eFormat::R16G16B16A16_SINT, "R16G16B16A16_SINT" },

		{ gxapi::eFormat::R32G32_TYPELESS, "R32G32_TYPELESS" },
		{ gxapi::eFormat::R32G32_FLOAT, "R32G32_FLOAT" },
		{ gxapi::eFormat::R32G32_UINT, "R32G32_UINT" },
		{ gxapi::eFormat::R32G32_SINT, "R32G32_SINT" },

		{ gxapi::eFormat::R32G8X24_TYPELESS, "R32G8X24_TYPELESS" },
		{ gxapi::eFormat::D32_FLOAT_S8X24_UINT, "D32_FLOAT_S8X24_UINT" },
		{ gxapi::eFormat::R32_FLOAT_X8X24_TYPELESS, "R32_FLOAT_X8X24_TYPELESS" },
		{ gxapi::eFormat::X32_TYPELESS_G8X24_UINT, "X32_TYPELESS_G8X24_UINT" },

		{ gxapi::eFormat::R10G10B10A2_TYPELESS, "R10G10B10A2_TYPELESS" },
		{ gxapi::eFormat::R10G10B10A2_UNORM, "R10G10B10A2_UNORM" },
		{ gxapi::eFormat::R10G10B10A2_UINT, "R10G10B10A2_UINT" },
		{ gxapi::eFormat::R11G11B10_FLOAT, "R11G11B10_FLOAT" },

		{ gxapi::eFormat::R8G8B8A8_TYPELESS, "R8G8B8A8_TYPELESS" },
		{ gxapi::eFormat::R8G8B8A8_UNORM, "R8G8B8A8_UNORM" },
		{ gxapi::eFormat::R8G8B8A8_UNORM_SRGB, "R8G8B8A8_UNORM_SRGB" },
		{ gxapi::eFormat::R8G8B8A8_UINT, "R8G8B8A8_UINT" },
		{ gxapi::eFormat::R8G8B8A8_SNORM, "R8G8B8A8_SNORM" },
		{ gxapi::eFormat::R8G8B8A8_SINT, "R8G8B8A8_SINT" },

		{ gxapi::eFormat::R16G16_TYPELESS, "R16G16_TYPELESS" },
		{ gxapi::eFormat::R16G16_FLOAT, "R16G16_FLOAT" },
		{ gxapi::eFormat::R16G16_UNORM, "R16G16_UNORM" },
		{ gxapi::eFormat::R16G16_UINT, "R16G16_UINT" },
		{ gxapi::eFormat::R16G16_SNORM, "R16G16_SNORM" },
		{ gxapi::eFormat::R16G16_SINT, "R16G16_SINT" },

		{ gxapi::eFormat::R32_TYPELESS, "R32_TYPELESS" },
		{ gxapi::eFormat::D32_FLOAT, "D32_FLOAT" },
		{ gxapi::eFormat::R32_FLOAT, "R32_FLOAT" },
		{ gxapi::eFormat::R32_UINT, "R32_UINT" },
		{ gxapi::eFormat::R32_SINT, "R32_SINT" },

		{ gxapi::eFormat::R24G8_TYPELESS, "R24G8_TYPELESS" },
		{ gxapi::eFormat::D24_UNORM_S8_UINT, "D24_UNORM_S8_UINT" },
		{ gxapi::eFormat::R24_UNORM_X8_TYPELESS, "R24_UNORM_X8_TYPELESS" },
		{ gxapi::eFormat::X24_TYPELESS_G8_UINT, "X24_TYPELESS_G8_UINT" },

		{ gxapi::eFormat::R8G8_TYPELESS, "R8G8_TYPELESS" },
		{ gxapi::eFormat::R8G8_UNORM, "R8G8_UNORM" },
		{ gxapi::eFormat::R8G8_UINT, "R8G8_UINT" },
		{ gxapi::eFormat::R8G8_SNORM, "R8G8_SNORM" },
		{ gxapi::eFormat::R8G8_SINT, "R8G8_SINT" },

		{ gxapi::eFormat::R16_TYPELESS, "R16_TYPELESS" },
		{ gxapi::eFormat::R16_FLOAT, "R16_FLOAT" },
		{ gxapi::eFormat::D16_UNORM, "D16_UNORM" },
		{ gxapi::eFormat::R16_UNORM, "R16_UNORM" },
		{ gxapi::eFormat::R16_UINT, "R16_UINT" },
		{ gxapi::eFormat::R16_SNORM, "R16_SNORM" },
		{ gxapi::eFormat::R16_SINT, "R16_SINT" },

		{ gxapi::eFormat::R8_TYPELESS, "R8_TYPELESS" },
		{ gxapi::eFormat::R8_UNORM, "R8_UNORM" },
		{ gxapi::eFormat::R8_UINT, "R8_UINT" },
		{ gxapi::eFormat::R8_SNORM, "R8_SNORM" },
		{ gxapi::eFormat::R8_SINT, "R8_SINT" },
		{ gxapi::eFormat::A8_UNORM, "A8_UNORM" },
	};

	return records;
}


template <>
std::vector<std::pair<gxapi::eBlendOperation, std::string>> impl::ParseTableGenerator() {
	std::vector<std::pair<gxapi::eBlendOperation, std::string>> records = {
		{ gxapi::eBlendOperation::ADD, "ADD" },
		{ gxapi::eBlendOperation::SUBTRACT, "SUBTRACT" },
		{ gxapi::eBlendOperation::REVERSE_SUBTRACT, "REVERSE_SUBTRACT" },
		{ gxapi::eBlendOperation::MIN, "MIN" },
		{ gxapi::eBlendOperation::MAX, "MAX" },
	};
	return records;
}

template <>
std::vector<std::pair<gxapi::eBlendOperand, std::string>> impl::ParseTableGenerator() {
	std::vector<std::pair<gxapi::eBlendOperand, std::string>> records = {
		{ gxapi::eBlendOperand::ZERO, "ZERO" },
		{ gxapi::eBlendOperand::ONE, "ONE" },
		{ gxapi::eBlendOperand::SHADER_OUT, "SHADER_OUT" },
		{ gxapi::eBlendOperand::INV_SHADER_OUT, "INV_SHADER_OUT" },
		{ gxapi::eBlendOperand::SHADER_ALPHA, "SHADER_ALPHA" },
		{ gxapi::eBlendOperand::INV_SHADER_ALPHA, "INV_SHADER_ALPHA" },
		{ gxapi::eBlendOperand::TARGET_OUT, "TARGET_OUT" },
		{ gxapi::eBlendOperand::INV_TARGET_OUT, "INV_TARGET_OUT" },
		{ gxapi::eBlendOperand::TARGET_ALPHA, "TARGET_ALPHA" },
		{ gxapi::eBlendOperand::INV_TARGET_ALPHA, "INV_TARGET_ALPHA" },
		{ gxapi::eBlendOperand::SHADER_ALPHA_SAT, "SHADER_ALPHA_SAT" },
		{ gxapi::eBlendOperand::BLEND_FACTOR, "BLEND_FACTOR" },
		{ gxapi::eBlendOperand::INV_BLEND_FACTOR, "INV_BLEND_FACTOR" },
	};
	return records;
}

template <>
std::vector<std::pair<gxapi::eBlendLogicOperation, std::string>> impl::ParseTableGenerator() {
	std::vector<std::pair<gxapi::eBlendLogicOperation, std::string>> records = {
		{ gxapi::eBlendLogicOperation::CLEAR, "CLEAR" },
		{ gxapi::eBlendLogicOperation::SET, "SET" },
		{ gxapi::eBlendLogicOperation::COPY, "COPY" },
		{ gxapi::eBlendLogicOperation::COPY_INVERTED, "COPY_INVERTED" },
		{ gxapi::eBlendLogicOperation::NOOP, "NOOP" },
		{ gxapi::eBlendLogicOperation::INVERT, "INVERT" },
		{ gxapi::eBlendLogicOperation::AND, "AND" },
		{ gxapi::eBlendLogicOperation::NAND, "NAND" },
		{ gxapi::eBlendLogicOperation::OR, "OR" },
		{ gxapi::eBlendLogicOperation::NOR, "NOR" },
		{ gxapi::eBlendLogicOperation::XOR, "XOR" },
		{ gxapi::eBlendLogicOperation::EQUIV, "EQUIV" },
		{ gxapi::eBlendLogicOperation::AND_REVERSE, "AND_REVERSE" },
		{ gxapi::eBlendLogicOperation::AND_INVERTED, "AND_INVERTED" },
		{ gxapi::eBlendLogicOperation::OR_REVERSE, "OR_REVERSE" },
		{ gxapi::eBlendLogicOperation::OR_INVERTED, "OR_INVERTED" },
	};
	return records;
}



std::string PortConverter<gxapi::RenderTargetBlendState>::ToString(const gxapi::RenderTargetBlendState& arg) const {
	std::stringstream ss;

	auto ToString = [](auto arg) -> std::string {
		return EnumConverter<std::decay_t<decltype(arg)>, impl::ParseTableGenerator<std::decay_t<decltype(arg)>>>::ToString(arg);
	};
	auto ToStringMask = [](gxapi::eColorMask arg) {
		std::string str;
		if (arg & gxapi::eColorMask::RED) {
			str += (str.empty() ? "RED" : "|RED");
		}
		if (arg & gxapi::eColorMask::GREEN) {
			str += (str.empty() ? "GREEN" : "|GREEN");
		}
		if (arg & gxapi::eColorMask::BLUE) {
			str += (str.empty() ? "BLUE" : "|BLUE");
		}
		if (arg & gxapi::eColorMask::ALPHA) {
			str += (str.empty() ? "ALPHA" : "|ALPHA");
		}
		return str;
	};

	ss << "{"
	   << (arg.enableBlending ? "enabled" : "disabled") << ","
	   << (arg.enableLogicOp ? "enabled" : "disabled") << ","
	   << ToString(arg.shaderColorFactor) << ","
	   << ToString(arg.targetColorFactor) << ","
	   << ToString(arg.colorOperation) << ","
	   << ToString(arg.shaderAlphaFactor) << ","
	   << ToString(arg.targetAlphaFactor) << ","
	   << ToString(arg.alphaOperation) << ","
	   << ToStringMask(arg.mask) << ","
	   << ToString(arg.logicOperation)
	   << "}";

	return ss.str();
}
gxapi::RenderTargetBlendState PortConverter<gxapi::RenderTargetBlendState>::FromString(const std::string& arg) {
	// Remove spaces
	std::string str;
	for (const auto& c : arg) {
		if (!isspace(c))
			str += c;
	}

	// Remove curly braces
	if (str.size() < 3 || str.front() != '{' || str.back() != '}') {
		throw InvalidArgumentException("Object must be serialized as specified by ToString.");
	}
	str = str.substr(1, str.size() - 2);

	// Tokenize by commas
	std::vector<std::string> tokens;
	for (const auto& c : str) {
		if (c == ',' || tokens.empty())
			tokens.push_back({});
		if (c != ',')
			tokens.back() += c;
	}

	if (tokens.size() != 10) {
		throw InvalidArgumentException("Object must contain 10 comma-separated fields.");
	}

	// Parse helper
	auto FromString = [](std::string in, auto& out) -> void {
		out = EnumConverter<std::decay_t<decltype(out)>, impl::ParseTableGenerator<std::decay_t<decltype(out)>>>::FromString(in);
	};

	// Parse tokens one-by-one
	gxapi::RenderTargetBlendState obj;
	if (tokens[0] == "enabled")
		obj.enableBlending = true;
	else if (tokens[0] == "disabled")
		obj.enableBlending = false;
	else
		throw InvalidArgumentException("Field 0 must be 'enabled' or 'disabled'.");

	if (tokens[1] == "enabled")
		obj.enableLogicOp = true;
	else if (tokens[1] == "disabled")
		obj.enableLogicOp = false;
	else
		throw InvalidArgumentException("Field 1 must be 'enabled' or 'disabled'.");

	FromString(tokens[2], obj.shaderColorFactor);
	FromString(tokens[3], obj.targetColorFactor);
	FromString(tokens[4], obj.colorOperation);
	FromString(tokens[5], obj.shaderAlphaFactor);
	FromString(tokens[6], obj.targetAlphaFactor);
	FromString(tokens[7], obj.alphaOperation);

	std::vector<std::string> maskTokens;
	for (const auto& c : tokens[8]) {
		if (c == '|' || maskTokens.empty())
			maskTokens.push_back({});
		if (c != '|')
			maskTokens.back() += c;
	}
	for (const auto& mtoken : maskTokens) {
		if (mtoken == "RED")
			obj.mask += gxapi::eColorMask::RED;
		else if (mtoken == "GREEN")
			obj.mask += gxapi::eColorMask::GREEN;
		else if (mtoken == "BLUE")
			obj.mask += gxapi::eColorMask::BLUE;
		else if (mtoken == "ALPHA")
			obj.mask += gxapi::eColorMask::ALPHA;
		else
			throw InvalidArgumentException("Colormask (param 9) must be RED, GREEN, BLUE, ALPHA, or a | combination of them.");
	}

	FromString(tokens[9], obj.logicOperation);


	return obj;
}



std::string PortConverter<gxeng::TextureUsage>::ToString(const gxeng::TextureUsage& arg) const {
	std::string str;
	if (arg.depthStencil) {
		str += (str.empty() ? "DS" : "|DS");
	}
	if (arg.randomAccess) {
		str += (str.empty() ? "RW" : "|RW");
	}
	if (arg.renderTarget) {
		str += (str.empty() ? "RT" : "|RT");
	}
	if (arg.shaderResource) {
		str += (str.empty() ? "SR" : "|SR");
	}
	return str;
}
gxeng::TextureUsage PortConverter<gxeng::TextureUsage>::FromString(const std::string& arg) {
	gxeng::TextureUsage obj{ false, false, false, false };

	std::vector<std::string> tokens;
	for (const auto& c : arg) {
		if (c == '|' || tokens.empty())
			tokens.push_back({});
		if (c != '|')
			tokens.back() += c;
	}
	for (const auto& tk : tokens) {
		if (tk == "DS")
			obj.depthStencil = true;
		else if (tk == "RW")
			obj.randomAccess = true;
		else if (tk == "RT")
			obj.renderTarget = true;
		else if (tk == "SR")
			obj.shaderResource = true;
		else
			throw InvalidArgumentException("Usage must be DS, RW, RT, SR, or a | combination of them.");
	}

	return obj;
}



}; // namespace inl