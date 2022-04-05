#pragma once
#include "Application/Layers/PostProcessingLayer.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/Textures/Texture3D.h"
#include "Graphics/Framebuffer.h"

//Code is based on the 5x5 Box Filter code from the tutorials

class Bloom : public PostProcessingLayer::Effect {
public:
	MAKE_PTRS(Bloom);
	float Filter[25];

	Bloom();
	virtual ~Bloom();

	virtual void Apply(const Framebuffer::Sptr& gBuffer) override;
	virtual void RenderImGui() override;

	// Inherited from IResource
	
	Bloom::Sptr FromJson(const nlohmann::json& data);
	virtual nlohmann::json ToJson() const override;
	
protected:
	ShaderProgram::Sptr _shader;
};