#include <d3dcompiler.h>
#include "PNTShaderClass.h"
#include "VertexTypes.h"

// global clock
extern int g_clock;

PNTShaderClass::PNTShaderClass() {
	_vertexShader = 0;
	_pixelShader = 0;
	_pixelNormalMapShader = 0;
	_layout = 0;
	_matrixBuffer = 0;
	m_lightBuffer = 0;
	_sampleState = 0;
}
PNTShaderClass::PNTShaderClass(const PNTShaderClass&) {

}
PNTShaderClass::~PNTShaderClass() {

}

bool PNTShaderClass::Initialize(ID3D11Device* device, HWND hwnd) {

	bool result = InitializeShader(device, hwnd, L"PNTVertex.hlsl", L"PNTPixel.hlsl");
	if (!result)
		return false;

	return true;
}

void PNTShaderClass::Shutdown() {
	ShutdownShader();
}

//2��° ���� �Ķ���� �߰�
bool PNTShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount
	, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView* texture, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor, XMFLOAT3 lightDirection2, XMFLOAT4 diffuseColor2, int shadeMode) {
	
	bool result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, diffuseColor, lightDirection2, diffuseColor2, shadeMode);
	if (!result)
		return false;

	RenderShader(deviceContext, indexCount, shadeMode);
	return true;
}


bool PNTShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* vsFilename, const WCHAR* psFilename) {

	//	"FX/color.fx", 0, 0, 0, "fx_5_0", shaderFlags,
	//		0, &compiledShader, &compilationMsgs)

		// file���� �ҽ��� �о� �������Ͽ� ID3dBlob ���̳ʸ��� �����.
	ID3DBlob* vertexShaderBuffer, * pixelShaderBuffer, * errorMessage;
	HRESULT hr = D3DCompileFromFile(vsFilename, 0, 0, "ColorVertexShader",
		"vs_5_0", D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_IEEE_STRICTNESS,
		0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(hr)) {
		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		else {
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	hr = D3DCompileFromFile(psFilename, 0, 0, "ColorPixelShader",
		"ps_5_0", D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_IEEE_STRICTNESS,
		0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(hr)) {
		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		else {
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	hr = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), NULL, &_vertexShader);
	if (FAILED(hr))
		return false;
	hr = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(), NULL, &_pixelShader);
	if (FAILED(hr))
		return false;


	// ���⼭���� normal map�� ���� ���ο� blob, pixel shader�� �����.
	ID3D10Blob* pixelNormalMapShaderBuffer;
	hr = D3DCompileFromFile(psFilename, 0, 0, "NomalMapPixelShader",
		"ps_5_0", D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_IEEE_STRICTNESS,
		0, &pixelNormalMapShaderBuffer, &errorMessage);

	if (FAILED(hr))
	{
		return false;
	}
	hr = device->CreatePixelShader(pixelNormalMapShaderBuffer->GetBufferPointer(), pixelNormalMapShaderBuffer->GetBufferSize(), NULL, &_pixelNormalMapShader);
	if (FAILED(hr))
	{
		return false;
	}


	// Layout�� dx9�� fvf�� ����������, ������ ��Ʈ���� ������ ǥ����
	/*
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	// SemanticName�� shader ���α׷��� VertexInputType�� ������
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;  // ����: D3D11_INPUT_PER_INSTANCE_DATA�� instancing�� ���� ��
	polygonLayout[0].InstanceDataStepRate = 0; // instancing�� ���� ��. �׸� instance�� ��

	polygonLayout[1].SemanticName = "TEXCOORD";  // 0�� �Ʒ��� SemanticIndex�� ����� �ִ� ��
	polygonLayout[1].SemanticIndex = 0; // color�� ������ ���Ǹ� ���ڰ� ����
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;  // ������[0]���κ��� �ڵ����� ����Ʈ ���̰� �����Ͽ� ����
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	int numElement = 2; // sizeof(polygonLayout) / sizeof(polygonLayout[0])

	hr = device->CreateInputLayout(polygonLayout, numElement,
		vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &_layout);
	*/
	device->CreateInputLayout(
		VertexPositionNormalTexture::InputElements,
		VertexPositionNormalTexture::InputElementCount,
		vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
		&_layout);

	if (FAILED(hr))
		return false;

	// vs, ps buffer�� layout�� �������, shader�� ��������� ���̻� �ʿ� ����.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = NULL;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = NULL;

	// ��� ���۸� ����
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType); // �츮�� ������ model,view,projection matrix�� ����ü
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	hr = device->CreateBuffer(&matrixBufferDesc, NULL, &_matrixBuffer);
	if (FAILED(hr))
		return false;

	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	hr = device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	// sampler ���� ������ ����
	D3D11_SAMPLER_DESC samplerDesc;// = {};
	samplerDesc.Filter = D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT; // !! D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.MipLODBias = 0.0f; // mip map���� level�� +�� ���ش�.
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS; // depth, stencil���� ���
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 1; // ���� ������ texture
	samplerDesc.MaxLOD = 1; // lod �Ⱦ����� ��. D3D11_FLOAT32_MAX ���� ����
	samplerDesc.MaxAnisotropy = 1; // ������� �ʴ��� 1�� �����϶�


	// ���� ���� ������ �̿��Ͽ� sampler�� ID3D11SamplerState �����.
	hr = device->CreateSamplerState(&samplerDesc, &_sampleState);
	return true;
}


void PNTShaderClass::ShutdownShader() {
	// Release the light constant buffer.
	if (m_lightBuffer)
	{
		m_lightBuffer->Release();
		m_lightBuffer = 0;
	}

	if (_sampleState) {
		_sampleState->Release();
		_sampleState = 0;
	}
	if (_matrixBuffer) {
		_matrixBuffer->Release();
		_matrixBuffer = 0;
	}
	if (_layout) {
		_layout->Release();
		_layout = 0;
	}
	if (_pixelNormalMapShader)
	{
		_pixelNormalMapShader->Release();
		_pixelNormalMapShader = 0;
	}
	if (_pixelShader) {
		_pixelShader->Release();
		_pixelShader = 0;
	}
	if (_vertexShader) {
		_vertexShader->Release();
		_vertexShader = 0;
	}

}
void PNTShaderClass::OutputShaderErrorMessage(ID3DBlob* errorMsg, HWND hwnd, const WCHAR* shaderFilename) {
	char* compileErrors;
	SIZE_T bufferSize, i;
	ofstream fout; // ���� �޼����� ��� �ִ� ���ڿ� ������ �����͸� �����ɴϴ�. 
	compileErrors = (char*)(errorMsg->GetBufferPointer()); // �޼����� ���̸� �����ɴϴ�. 
	bufferSize = errorMsg->GetBufferSize(); // ������ ���� �ȿ� �޼����� ����մϴ�. 
	fout.open("shader-error.txt"); // ���� �޼����� ���ϴ�. 
	for (i = 0; i < bufferSize; i++) { fout << compileErrors[i]; }
	// ������ �ݽ��ϴ�. 
	fout.close(); // ���� �޼����� ��ȯ�մϴ�. 
	errorMsg->Release();
	errorMsg = 0;

	// ������ ������ ������ �˾� �޼����� �˷��ݴϴ�. 
	MessageBox(hwnd, L"Error compiling shader. Check shader-error.txt for message.", shaderFilename, MB_OK);
}

// using GPT
XMFLOAT3 CalculateViewingDirection(const XMMATRIX& viewMatrix) {
	// �� ����� 3��° ��(row 2)�� ����
	XMVECTOR forwardVector = XMVectorSet(
		viewMatrix.r[2].m128_f32[0], // _13
		viewMatrix.r[2].m128_f32[1], // _23
		viewMatrix.r[2].m128_f32[2], // _33
		0.0f
	);

	// ���� ������ Z���� �ݴ� �����̹Ƿ�, -forwardVector�� ���
	XMVECTOR viewingDir = XMVectorNegate(forwardVector);

	// ����� XMFLOAT3�� ��ȯ
	XMFLOAT3 result;
	XMStoreFloat3(&result, XMVector3Normalize(viewingDir)); // ����ȭ
	return result;
}

//����2 �Ķ���� �߰�
bool PNTShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView* texture, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor, XMFLOAT3 lightDirection2, XMFLOAT4 diffuseColor2, int shadeMode) {

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr;

	// ���� ��ǥ���� ī�޶� ������ ��´�.
	XMFLOAT3 viewDir = CalculateViewingDirection(viewMatrix);

	// gpu ���۸� ������ �� �ִ� �����͸� ����. dx9�� lock()
	// ���������� ��� ���� �뵵���� �� �� ����
	hr = deviceContext->Map(_matrixBuffer,
		0, // subresource�� ���� �ڿ�����, ���� ��� buffer�� LOD �δܰ� �߰��Ǹ� 2�� ��
		D3D11_MAP_WRITE_DISCARD, // ��� ���������� �����ϰ� ��� �� ����. cpu write + gpu read�� �ݺ��Ǵ� ��� ���
		0, // ����: D3D11_MAP_FLAG_DO_NOT_WAIT �� cpu �� ������ �Ҷ� GPU�� ���� block�Ǹ� DXGI_ERROR_WAS_STILL_DRAWING�� ������
		&mappedResource);
	if (FAILED(hr))
		return false;

	// ���� ���� �����Ϳ� ���� �����Ѵ�.
	// cpu �����͸� ������� memcpy �ص� �ȴ�.

	// dx�� shader�� ��/�� ������ �ٸ�
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	deviceContext->Unmap(_matrixBuffer, 0);

	unsigned int bufferNumber = 0;

	// ������ gpu ���۸� ���̴��� ��� ���۷� ���
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &_matrixBuffer);
	// ����: ������ ���� ���� ���۸� �ѹ��� set����
	// bufferNumber�� buffer�迭�� ���� ��ġ
	// ID3D11Buffer* bufs[3];
	// VSSetConstantBuffers(1, 2, bufs); // 1��,2�� ���۸� set����


	deviceContext->PSSetShaderResources(0, 1, &texture);
	// ����: ���� vs�� ���������� �迭�� ������ġ, ����� ����, �迭�� �Է�
	// ������ �Ѱ��� ������ ������ �ּҸ� �ѱ�

	// Lock the light constant buffer so it can be written to.
	hr = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
	{
		return false;
	}
	LightBufferType* dataPtr2 = (LightBufferType*)mappedResource.pData;
	// Get a pointer to the data in the constant buffer.
	dataPtr2 = (LightBufferType*)mappedResource.pData;

	// Copy the lighting variables into the constant buffer.
	dataPtr2->diffuseColor = diffuseColor;
	dataPtr2->lightDirection = lightDirection;
	dataPtr2->diffuseColor2 = diffuseColor;
	dataPtr2->lightDirection2 = lightDirection;
	dataPtr2->shadeMode = shadeMode;
	dataPtr2->padding = 1.0f;

	dataPtr2->viewDir = viewDir;
	dataPtr2->frameCount = (float)g_clock;
	// Unlock the constant buffer.
	deviceContext->Unmap(m_lightBuffer, 0);

	// Set the position of the light constant buffer in the pixel shader.
	bufferNumber = 0;

	// Finally set the light constant buffer in the pixel shader with the updated values.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);
	return true;
}
void PNTShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount, int shademode) {
	deviceContext->IASetInputLayout(_layout);
	deviceContext->VSSetShader(_vertexShader, NULL, 0);
	//if (useNormalMap)
	//	deviceContext->PSSetShader(_pixelNormalMapShader, NULL, 0);
	//else
		deviceContext->PSSetShader(_pixelShader, NULL, 0);

	deviceContext->PSSetSamplers(0, 1, &_sampleState);
	// �ﰢ���̸� indexCount=3
	deviceContext->DrawIndexed(indexCount, 0, 0);
}

