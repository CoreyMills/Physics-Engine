#include "Appearance.h"

Appearance::Appearance(Geometry* geometry, Material* material): _geometry(geometry), _material(material)
{
	_textureRV = nullptr;
	_isTransparent = false;
}

Appearance::~Appearance()
{
	_geometry = nullptr;
	delete _geometry;

	_material = nullptr;
	delete _material;

	_textureRV = nullptr;
	delete _textureRV;
}