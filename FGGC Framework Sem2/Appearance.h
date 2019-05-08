#pragma once
#include "Common.h"

class Appearance
{
public:
	Appearance(Geometry* geometry, Material* material);
	~Appearance();

	Geometry* GetGeometryData() const { return _geometry; }
	void SetGeometryData(Geometry* geometry) { _geometry = geometry; }

	Material* GetMaterial() const { return _material; }
	void SetMaterial(Material* material) { _material = material; }

	void SetTextureRV(ID3D11ShaderResourceView * textureRV) { _textureRV = textureRV; }
	ID3D11ShaderResourceView * GetTextureRV() const { return _textureRV; }
	bool HasTexture() const { return _textureRV ? true : false; }

	void SetBlendState(ID3D11BlendState* transparency) { _transparency = transparency; }
	ID3D11BlendState* GetBlendState() { return _transparency; }

	void SetBlendFactor(Vector3 factor) { _blendFactor = factor; }
	Vector3 GetBlendFactor() { return _blendFactor; }

	void SetTransparency(bool val) { _isTransparent = val; }
	bool IsTransparent() { return _isTransparent; }

private:
	Geometry* _geometry;
	Material* _material;
	ID3D11ShaderResourceView* _textureRV;

	ID3D11BlendState* _transparency;
	Vector3 _blendFactor;
	bool _isTransparent;
};