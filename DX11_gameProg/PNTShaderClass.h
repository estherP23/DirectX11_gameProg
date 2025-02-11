#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>

using namespace DirectX;
using namespace std;

class PNTShaderClass {
private:
	struct MatrixBufferType {
		XMMATRIX world, view, projection;
	};

	struct LightBufferType
	{
		//phong ���� ���� ����?
		XMFLOAT4 diffuseColor;
		int shadeMode;
		XMFLOAT3 lightDirection;
		//����2 �߰�
		XMFLOAT4 diffuseColor2;

		float frameCount; 
		XMFLOAT3 lightDirection2;

		XMFLOAT3 viewDir;
		float padding;// Added extra padding so structure is a multiple of 16 for CreateBuffer function requirements.
	};

public:
	PNTShaderClass();
	PNTShaderClass(const PNTShaderClass&);
	~PNTShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor, XMFLOAT3 lightDirection2, XMFLOAT4 diffuseColor2, int);  //�ι�° ���� �߰�

private:
	// D3DCompileFromFile���� �䱸�ϹǷ� �� WCHAR��
	bool InitializeShader(ID3D11Device*, HWND, const WCHAR*, const WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3DBlob*, HWND, const WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor, XMFLOAT3 lightDirection2, XMFLOAT4 diffuseColor2, int); //�ι�° ���� �߰�
	void RenderShader(ID3D11DeviceContext*, int, int);

private:
	ID3D11VertexShader* _vertexShader;
	ID3D11PixelShader* _pixelShader;
	ID3D11PixelShader* _pixelNormalMapShader;
	ID3D11InputLayout* _layout;
	ID3D11Buffer* _matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
	ID3D11SamplerState* _sampleState;
};
