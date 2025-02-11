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

//2번째 조명 파라미터 추가
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

		// file에서 소스를 읽어 컴파일하여 ID3dBlob 바이너리로 만든다.
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


	// 여기서부터 normal map을 위한 새로운 blob, pixel shader를 만든다.
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


	// Layout은 dx9의 fvf와 마찬가지로, 데이터 스트림의 구조를 표시함
	/*
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	// SemanticName은 shader 프로그램의 VertexInputType와 대응됨
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;  // 참고: D3D11_INPUT_PER_INSTANCE_DATA는 instancing을 위한 듯
	polygonLayout[0].InstanceDataStepRate = 0; // instancing을 위한 값. 그릴 instance의 수

	polygonLayout[1].SemanticName = "TEXCOORD";  // 0은 아랫줄 SemanticIndex가 만들어 주는 듯
	polygonLayout[1].SemanticIndex = 0; // color가 여러개 사용되면 숫자가 생김
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;  // 이전값[0]으로부터 자동으로 바이트 길이가 누적하여 계산됨
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

	// vs, ps buffer는 layout도 만들었고, shader도 만들었으니 더이상 필요 없다.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = NULL;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = NULL;

	// 상수 버퍼를 전달
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType); // 우리가 정의한 model,view,projection matrix의 구조체
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

	// sampler 설명 정보를 생성
	D3D11_SAMPLER_DESC samplerDesc;// = {};
	samplerDesc.Filter = D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT; // !! D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.MipLODBias = 0.0f; // mip map에서 level을 +를 해준다.
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS; // depth, stencil에서 사용
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 1; // 가장 선명한 texture
	samplerDesc.MaxLOD = 1; // lod 안쓰려고 함. D3D11_FLOAT32_MAX 까지 가능
	samplerDesc.MaxAnisotropy = 1; // 사용하지 않더라도 1도 지정하라


	// 위의 설명 정보를 이용하여 sampler인 ID3D11SamplerState 만든다.
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
	ofstream fout; // 에러 메세지를 담고 있는 문자열 버퍼의 포인터를 가져옵니다. 
	compileErrors = (char*)(errorMsg->GetBufferPointer()); // 메세지의 길이를 가져옵니다. 
	bufferSize = errorMsg->GetBufferSize(); // 파일을 열고 안에 메세지를 기록합니다. 
	fout.open("shader-error.txt"); // 에러 메세지를 씁니다. 
	for (i = 0; i < bufferSize; i++) { fout << compileErrors[i]; }
	// 파일을 닫습니다. 
	fout.close(); // 에러 메세지를 반환합니다. 
	errorMsg->Release();
	errorMsg = 0;

	// 컴파일 에러가 있음을 팝업 메세지로 알려줍니다. 
	MessageBox(hwnd, L"Error compiling shader. Check shader-error.txt for message.", shaderFilename, MB_OK);
}

// using GPT
XMFLOAT3 CalculateViewingDirection(const XMMATRIX& viewMatrix) {
	// 뷰 행렬의 3번째 행(row 2)을 추출
	XMVECTOR forwardVector = XMVectorSet(
		viewMatrix.r[2].m128_f32[0], // _13
		viewMatrix.r[2].m128_f32[1], // _23
		viewMatrix.r[2].m128_f32[2], // _33
		0.0f
	);

	// 관찰 방향은 Z축의 반대 방향이므로, -forwardVector를 계산
	XMVECTOR viewingDir = XMVectorNegate(forwardVector);

	// 결과를 XMFLOAT3로 변환
	XMFLOAT3 result;
	XMStoreFloat3(&result, XMVector3Normalize(viewingDir)); // 정규화
	return result;
}

//조명2 파라미터 추가
bool PNTShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView* texture, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor, XMFLOAT3 lightDirection2, XMFLOAT4 diffuseColor2, int shadeMode) {

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr;

	// 전역 좌표에서 카메라 방향을 얻는다.
	XMFLOAT3 viewDir = CalculateViewingDirection(viewMatrix);

	// gpu 버퍼를 접근할 수 있는 포인터를 얻어옴. dx9의 lock()
	// 아직까지는 상수 버퍼 용도인지 알 수 없음
	hr = deviceContext->Map(_matrixBuffer,
		0, // subresource는 보조 자원으로, 예를 들어 buffer에 LOD 두단계 추가되면 2가 됨
		D3D11_MAP_WRITE_DISCARD, // 계속 이전내용을 무시하고 덮어쓸 수 있음. cpu write + gpu read가 반복되는 경우 사용
		0, // 참고: D3D11_MAP_FLAG_DO_NOT_WAIT 는 cpu 가 쓰려고 할때 GPU에 의해 block되면 DXGI_ERROR_WAS_STILL_DRAWING을 리턴함
		&mappedResource);
	if (FAILED(hr))
		return false;

	// 얻어온 버퍼 포인터에 값을 저장한다.
	// cpu 데이터를 만든다음 memcpy 해도 된다.

	// dx와 shader는 행/열 순서가 다름
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	deviceContext->Unmap(_matrixBuffer, 0);

	unsigned int bufferNumber = 0;

	// 생성된 gpu 버퍼를 셰이더의 상수 버퍼로 사용
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &_matrixBuffer);
	// 참고: 다음과 같이 여러 버퍼를 한번에 set가능
	// bufferNumber는 buffer배열의 시작 위치
	// ID3D11Buffer* bufs[3];
	// VSSetConstantBuffers(1, 2, bufs); // 1번,2번 버퍼만 set가능


	deviceContext->PSSetShaderResources(0, 1, &texture);
	// 참고: 위의 vs와 마찬가지로 배열의 시작위치, 사용할 갯수, 배열을 입력
	// 지금은 한개라서 포인터 변수의 주소를 넘김

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
	// 삼각형이면 indexCount=3
	deviceContext->DrawIndexed(indexCount, 0, 0);
}

