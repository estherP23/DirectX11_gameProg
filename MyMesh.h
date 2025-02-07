#pragma once

// 우선 VertexPositionNormalTexture 만 지원하도록 한다.
#include <vector>
#include <string>
#include <d3d11.h>
#include <DirectXMath.h>
#include "TextureClass.h"
#include "VertexTypes.h"

using namespace DirectX;

namespace DirectX {

	typedef std::vector<DirectX::VertexPositionNormalTexture> VertexCollection;
	typedef std::vector<uint32_t> IndexCollection;
	using VertexType = VertexPositionNormalTexture;
}

class MyMesh {
	//	`void ComputeTeapot(VertexCollection& vertices, IndexCollection& indices, float size, size_t tessellation, bool rhcoords);

public:
	MyMesh();
	MyMesh(const MyMesh&);
	~MyMesh();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* context, const WCHAR* modelFilename, const WCHAR* texFilename, std::string option, XMMATRIX model);
	bool Initialize(ID3D11Device*, ID3D11DeviceContext* context, const WCHAR* modelFilename, const WCHAR* texFilename);
	void Shutdown();
	void Render(ID3D11DeviceContext*);
	std::string GetName() {
		return m_name;
	}
	int GetShadeMode() {
		return m_shadeMode;
	}
	XMMATRIX GetModelMatrix() {
		return m_modelMatrix;
	}
	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*, const VertexCollection&, const IndexCollection&);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext* context, const WCHAR*);
	void ReleaseTexture();
	//void ComputeTeapot(VertexCollection& vertices, IndexCollection& indices, float size, size_t tessellation, bool rhcoords);
	// 도넛 모델 생성 함수 추가
	void ComputeTorus(VertexCollection& vertices, IndexCollection& indices, float outerRadius, float innerRadius, size_t tessellation, bool rhcoords);
	void ComputeTriangle(VertexCollection& vertices, IndexCollection& indices);
	void ComputeBox(VertexCollection& vertices, IndexCollection& indices, const XMFLOAT3& size, bool rhcoords, bool invertn);
	void ComputeSphere(VertexCollection& vertices, IndexCollection& indices, float diameter, size_t tessellation, bool rhcoords, bool invertn);
	void ComputeCylinder(VertexCollection& vertices, IndexCollection& indices, float height, float radius, size_t tessellation, bool rhcoords, bool invertn);
	unsigned int LoadOBJ(VertexCollection& vertices, IndexCollection& indices, const WCHAR* modelFilename);
private:
	XMMATRIX m_modelMatrix; // 물체마다 장면에 적용되는 행렬
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	TextureClass* m_texture;
	unsigned int m_indexCount; // _vertexCount, 
	std::string _stlInfo;
	std::string m_name;
	int m_shadeMode;
};

