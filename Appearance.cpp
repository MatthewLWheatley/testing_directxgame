#include "Appearance.h"

Appearance::Appearance()
{

    // Initialize any necessary default values or resources.
}

Appearance::Appearance(std::shared_ptr<Geometry> geometry, std::shared_ptr<Material> material, ID3D11ShaderResourceView* textureRV)
    : _geometry(geometry), _material(material), _textureRV(textureRV)
{

}

Appearance::~Appearance()
{

    // Release resources if necessary.
    //if (_textureRV)
    //{
    //    _textureRV->Release();
    //    _textureRV = nullptr;
    //}

}

void Appearance::Draw(ID3D11DeviceContext* pImmediateContext)
{

    pImmediateContext->IASetVertexBuffers(0, 1, &_geometry->vertexBuffer, &_geometry->vertexBufferStride, &_geometry->vertexBufferOffset);
    pImmediateContext->IASetIndexBuffer(_geometry->indexBuffer, DXGI_FORMAT_R16_UINT, 0);

    pImmediateContext->DrawIndexed(_geometry->numberOfIndices, 0, 0);
}

