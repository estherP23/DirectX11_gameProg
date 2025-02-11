#include "MyMesh.h"
#include "Bezier.h"
#include <string>
MyMesh::MyMesh() {
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_texture = 0;
	m_modelMatrix = XMMatrixIdentity(); // 초기화
	m_name = std::string();
	m_shadeMode = 0;
}

MyMesh::MyMesh(const MyMesh&) {

}
MyMesh::~MyMesh() {

}

namespace {
	// Helper for flipping winding of geometric primitives for LH vs. RH coords
	inline void ReverseWinding(IndexCollection& indices, VertexCollection& vertices)
	{
		assert((indices.size() % 3) == 0);
		for (auto it = indices.begin(); it != indices.end(); it += 3)
		{
			std::swap(*it, *(it + 2));
		}

		for (auto it = vertices.begin(); it != vertices.end(); ++it)
		{
			it->textureCoordinate.x = (1.f - it->textureCoordinate.x);
		}
	}
#include "TeapotData.inc"

	void TessellatePatch(VertexCollection& vertices, IndexCollection& indices, TeapotPatch const& patch, size_t tessellation, FXMVECTOR scale, bool isMirrored)
	{
		// Look up the 16 control points for this patch.
		XMVECTOR controlPoints[16];

		for (int i = 0; i < 16; i++)
		{
			controlPoints[i] = XMVectorMultiply(TeapotControlPoints[patch.indices[i]], scale);
		}

		// Create the index data.
		size_t vbase = vertices.size();
		Bezier::CreatePatchIndices(tessellation, isMirrored, [&](size_t index)
			{
				indices.push_back(static_cast<uint16_t>(vbase + index));
			});

		// Create the vertex data.
		Bezier::CreatePatchVertices(controlPoints, tessellation, isMirrored, [&](FXMVECTOR position, FXMVECTOR normal, FXMVECTOR textureCoordinate)
			{
				//vertices.push_back(VertexPositionNormalTexture(position, normal, textureCoordinate));
				vertices.push_back(VertexPositionNormalTexture(position, normal, textureCoordinate));
			});
	}
}

/*bool MyMesh::Initialize(ID3D11Device* device, const WCHAR* modelFilename, const WCHAR* texFilename)
{
	VertexCollection vertices;
	IndexCollection indices;
	if (!modelFilename) {
		float size = 5;
		size_t tessellation = 10;
		bool rhcoords = false;
		ComputeTeapot(vertices, indices, size, tessellation, rhcoords);
		//ComputeSphere(vertices, indices, size, tessellation*2, rhcoords, false);
		m_indexCount = (unsigned int)indices.size();
	}
	else {
		// 나중에 load file
		//ComputeTriangle(vertices, indices);
		LoadOBJ(vertices, indices, modelFilename);
		m_indexCount = (unsigned int)indices.size();
	}


	bool result = InitializeBuffers(device, vertices, indices);
	if (!result) return false;

	result = LoadTexture(device, texFilename);
	if (!result) return false;




	return true;
}*/
#include <string>
bool MyMesh::Initialize(ID3D11Device* device, ID3D11DeviceContext* context, const WCHAR* modelFilename, const WCHAR* texFilename, std::string option, XMMATRIX model)
{
	// 물체의 이름 설정
	m_name = option;
	// 기본 조명 모드 설정 (0: 기본값)
	m_shadeMode = 0;
	// 옵션 문자열이 비어 있는 경우 기본 초기화 함수 호출
	if (option.length() == 0) {
		return Initialize(device, context, modelFilename, texFilename);
	}

	// 초기 변환 행렬 설정
	m_modelMatrix = model;

	// 정점 및 인덱스 컬렉션 생성
	VertexCollection vertices;
	IndexCollection indices;

	if (option == "floor") {
		// 박스 생성
		bool rhcoords = true;     // 오른손 좌표계 여부
		DirectX::XMFLOAT3 size(20, 0.1, 20);
		ComputeBox(vertices, indices, size, rhcoords, false);
		m_indexCount = (unsigned int)indices.size();
		m_shadeMode = 1; // normal map 적용
	}
	else if (option == "sphere") {
		//구 생성
		m_modelMatrix = XMMatrixTranslation(0, 1.5, 0);
		m_modelMatrix *= model; // 외부에서 움직이기를 바라는 정도
		bool rhcoords = false; 
		// 오른손 좌표계 사용 여부
		float outerRadius = 3.0f;  
		float innerRadius = 1.0f;  
		size_t tessellation = 20;  
		ComputeSphere(vertices, indices, outerRadius, 20, rhcoords, false);
		m_indexCount = (unsigned int)indices.size();
		m_shadeMode = 2; // phong 조명 모델 적용
	}
	else if (option == "chair") {
		//원기둥 생성
		m_modelMatrix = XMMatrixTranslation(0, 1, 0); // 화면 중심에 있으며, 바닥 위에 위치해있음
		m_modelMatrix *=  model; 

		bool rhcoords = true;  
		bool invertn = false;

		// 원기둥의 크기 설정
		float height = 3.0f;      // 높이
		float radius = 1.0f;      // 반지름
		size_t tessellation = 50;
		ComputeCylinder(vertices, indices, height, radius, tessellation, rhcoords, invertn);
		m_indexCount = (unsigned int)indices.size();
	}
	
	else if (option == "table") {
		//테이블 obj 생성
		// 물체의 기본 matrix
		m_modelMatrix = XMMatrixRotationX(XMConvertToRadians(-90.0f));  //X축으로 -90도 회전
		m_modelMatrix *= XMMatrixScaling(0.3, 0.3, 0.3);  //크기 조정
		m_modelMatrix *= XMMatrixTranslation(-3, 0, -3);  //위치 지정

		m_modelMatrix *= model;

		bool rhcoords = true;    
		std::wstring modelFilename = L"table.obj"; // 유니코드 문자열로 변환
		LoadOBJ(vertices, indices, modelFilename.c_str()); // WCHAR*로 전달
		XMMATRIX rotationMatrix = XMMatrixRotationX(XMConvertToRadians(-90.0f));
		m_indexCount = (unsigned int)indices.size();

		}

	else if (option == "cat") {
		//고양이 obj 생성
		// 물체의 기본 matrix
		m_modelMatrix = XMMatrixRotationY(XMConvertToRadians(45.0f));
		m_modelMatrix *= XMMatrixScaling(0.008, 0.008, 0.008);
		m_modelMatrix *= XMMatrixTranslation(-8, 1, 8);

		m_modelMatrix *= model; 

		bool rhcoords = true;    
		std::wstring modelFilename = L"cat.obj"; // 유니코드 문자열로 변환
		LoadOBJ(vertices, indices, modelFilename.c_str()); // WCHAR*로 전달
		m_indexCount = (unsigned int)indices.size();
		m_shadeMode = 3; // Blink(깜빡임) 효과 적용

		}


	else if (option == "bunny") {
			//토끼 obj 생성
			// 물체의 기본 matrix
			m_modelMatrix = XMMatrixRotationY(XMConvertToRadians(0));
			m_modelMatrix *= XMMatrixScaling(1.000, 1.000, 1.000);
			m_modelMatrix *= XMMatrixTranslation(0, 0, 0);

			m_modelMatrix *= model; 

			bool rhcoords = true;     
			std::wstring modelFilename = L"bunny.obj"; // 유니코드 문자열로 변환
			LoadOBJ(vertices, indices, modelFilename.c_str()); // WCHAR*로 전달
			m_indexCount = (unsigned int)indices.size();
			m_shadeMode = 2; //phong 조명 모델 적용

			}

	else if (option == "catChair") {
		//고양이 의자 obj 생성
		// 물체의 기본 matrix.
		m_modelMatrix = XMMatrixRotationY(XMConvertToRadians(180));
		m_modelMatrix *= XMMatrixRotationX(XMConvertToRadians(90));
		m_modelMatrix *= XMMatrixScaling(0.800, 0.2, 0.800);
		m_modelMatrix *= XMMatrixTranslation(0, 0, 0);

		m_modelMatrix *= model; 

		bool rhcoords = true;    
		std::wstring modelFilename = L"chair.obj"; // 유니코드 문자열로 변환
		LoadOBJ(vertices, indices, modelFilename.c_str()); // WCHAR*로 전달
		m_indexCount = (unsigned int)indices.size();
		m_shadeMode = 0; //텍스쳐 적용 

		}

	

	bool result = InitializeBuffers(device, vertices, indices);
	if (!result) return false;

	if (texFilename)
		result = LoadTexture(device, context, texFilename);
	if (!result) return false;

	return true;

}
bool MyMesh::Initialize(ID3D11Device* device, ID3D11DeviceContext* context, const WCHAR* modelFilename, const WCHAR* texFilename)
{
	VertexCollection vertices;
	IndexCollection indices;

	if (!modelFilename) {
		
		float outerRadius = 3.0f;  
		float innerRadius = 1.0f;  
		size_t tessellation = 20;  
		bool rhcoords = false;     
		ComputeTorus(vertices, indices, outerRadius, innerRadius, tessellation*2, rhcoords);
		m_indexCount = (unsigned int)indices.size();
	}
	else {
		// 나중에 OBJ 파일 로드
		LoadOBJ(vertices, indices, modelFilename);
		m_indexCount = (unsigned int)indices.size();
	}

	bool result = InitializeBuffers(device, vertices, indices);
	if (!result) return false;

	result = LoadTexture(device, context, texFilename);
	if (!result) return false;

	return true;
}

void MyMesh::Shutdown() {
	ReleaseTexture();
	ShutdownBuffers();
}

void MyMesh::Render(ID3D11DeviceContext* deviceContext) {
	RenderBuffers(deviceContext);
}

int MyMesh::GetIndexCount() {
	return m_indexCount;
}

ID3D11ShaderResourceView* MyMesh::GetTexture() {
	if (m_texture)
		return m_texture->GetTexture();
	else
		return NULL;
}
// vertexbuffer와 indexbuffer를 만든다.
bool MyMesh::InitializeBuffers(ID3D11Device* device, const VertexCollection& vertices, const IndexCollection& indices) {

	// gpu memory
	// 데이터를 해석하는 방법
	D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * (unsigned int)vertices.size();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	// 실제 데이터, 메모리 덩어리 정보
	D3D11_SUBRESOURCE_DATA vertexData = { 0 };
	vertexData.pSysMem = vertices.data();

	// _vertexBuffer 에 생성됨
	HRESULT result = device->CreateBuffer(&vertexBufferDesc,
		&vertexData, &m_vertexBuffer);

	// index buffer도 마찬가지
	D3D11_BUFFER_DESC indexBufferDesc = { 0 };
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * (unsigned int)indices.size();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA indexData = { 0 };
	indexData.pSysMem = indices.data();

	result = device->CreateBuffer(&indexBufferDesc,
		&indexData, &m_indexBuffer);

	return true;
}
void MyMesh::ShutdownBuffers() {
	if (m_indexBuffer) {
		m_indexBuffer->Release();
		m_indexBuffer = NULL;
	}
	if (m_vertexBuffer) {
		m_vertexBuffer->Release();
		m_vertexBuffer = NULL;
	}
}
void MyMesh::RenderBuffers(ID3D11DeviceContext* deviceContext) {
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// IA: Input Assembler
	// 여러 vertex buffer의 배열을 한번에 set할 수 있다.
	// 0은 배열의 시작위치, 1은 사용할 갯수
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// indexbuffer는 한개씩 set하는 것으로 보임
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool MyMesh::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* context, const WCHAR* filename) {

	m_texture = new TextureClass;
	if (!m_texture)
		return false;

	char pStr[1000];
	int size = WideCharToMultiByte(CP_ACP, 0, filename, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, filename, -1, pStr, size, 0, 0);

	bool result = m_texture->Initialize(device, context, pStr);
	if (!result)
		return false;

	return true;
}

void MyMesh::ReleaseTexture() {
	if (m_texture) {
		m_texture->Shutdown();
		delete m_texture;
		m_texture = 0;
	}
}

void MyMesh::ComputeTriangle(VertexCollection& vertices, IndexCollection& indices)
{
	vertices.clear();
	vertices.resize(3);

	vertices[0].position = XMFLOAT3(-1, -1, 0);
	vertices[0].normal = XMFLOAT3(0, 0, 1);
	vertices[0].textureCoordinate = XMFLOAT2(0, 1);
	vertices[1].position = XMFLOAT3(0, 1, 0);
	vertices[1].normal = XMFLOAT3(0, 0, 1);
	vertices[1].textureCoordinate = XMFLOAT2(0.5, 0);
	vertices[2].position = XMFLOAT3(1, -1, 0);
	vertices[2].normal = XMFLOAT3(0, 0, 1);
	vertices[2].textureCoordinate = XMFLOAT2(1, 1);

	indices.clear();
	indices.resize(3);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
}
/*void MyMesh::ComputeTeapot(VertexCollection& vertices, IndexCollection& indices, float size, size_t tessellation, bool rhcoords)
{
	vertices.clear();
	indices.clear();

	//if (tessellation < 1)
	//	throw std::out_of_range("tesselation parameter out of range");

	XMVECTOR scaleVector = XMVectorReplicate(size);

	XMVECTOR scaleNegateX = XMVectorMultiply(scaleVector, g_XMNegateX);
	XMVECTOR scaleNegateZ = XMVectorMultiply(scaleVector, g_XMNegateZ);
	XMVECTOR scaleNegateXZ = XMVectorMultiply(scaleVector, XMVectorMultiply(g_XMNegateX, g_XMNegateZ));

	for (size_t i = 0; i < _countof(TeapotPatches); i++)
	{
		TeapotPatch const& patch = TeapotPatches[i];

		// Because the teapot is symmetrical from left to right, we only store
		// data for one side, then tessellate each patch twice, mirroring in X.
		TessellatePatch(vertices, indices, patch, tessellation, scaleVector, false);
		TessellatePatch(vertices, indices, patch, tessellation, scaleNegateX, true);

		if (patch.mirrorZ)
		{
			// Some parts of the teapot (the body, lid, and rim, but not the
			// handle or spout) are also symmetrical from front to back, so
			// we tessellate them four times, mirroring in Z as well as X.
			TessellatePatch(vertices, indices, patch, tessellation, scaleNegateZ, true);
			TessellatePatch(vertices, indices, patch, tessellation, scaleNegateXZ, false);
		}
	}

	// Built RH above
	if (!rhcoords)
		ReverseWinding(indices, vertices);
}*/


void MyMesh::ComputeTorus(VertexCollection& vertices, IndexCollection& indices, float outerRadius, float innerRadius, size_t tessellation, bool rhcoords)
{
	vertices.clear();
	indices.clear();

	// 도넛 크기 반으로 줄이기 (outerRadius, innerRadius 값을 작게 설정)
	outerRadius /= 2.0f;  // 기존 크기에서 반으로 줄이기
	innerRadius /= 2.0f;

	// 더 세밀하게 만들기 위해 tessellation 값을 증가
	tessellation = tessellation * 2;  // 세밀한 삼각형으로 만들기 위해 tessellation 값을 두 배로 증가

	// 정점 계산 (테셀레이션에 따라 계산)
	for (size_t i = 0; i <= tessellation; i++) {
		float u = (float)i / tessellation * XM_2PI;  // 외부 원을 따라 반복
		float cosu = cosf(u);
		float sinu = sinf(u);

		for (size_t j = 0; j <= tessellation; j++) {
			float v = (float)j / tessellation * XM_2PI;  // 내부 원을 따라 반복
			float cosv = cosf(v);
			float sinv = sinf(v);

			// 도넛의 정점 계산 (이 부분이 핵심)
			float x = (outerRadius + innerRadius * cosv) * cosu;
			float y = (outerRadius + innerRadius * cosv) * sinu;
			float z = innerRadius * sinv;

			// 법선 벡터 계산 (도넛 표면에서 수직인 벡터)
			XMVECTOR normal = XMVectorSet(cosv * cosu, cosv * sinu, sinv, 0);  // 법선 계산
			normal = XMVector3Normalize(normal);  // 정규화

			// 텍스처 좌표 (UV 맵핑)
			float uCoord = (float)i / tessellation;
			float vCoord = (float)j / tessellation;

			// 정점 추가
			vertices.push_back(VertexPositionNormalTexture(XMFLOAT3(x, y, z), XMFLOAT3(XMVectorGetX(normal), XMVectorGetY(normal), XMVectorGetZ(normal)), XMFLOAT2(uCoord, vCoord)));
		}
	}

	// 인덱스 계산 (도넛은 두 삼각형씩 정의됨)
	for (size_t i = 0; i < tessellation; i++) {
		for (size_t j = 0; j < tessellation; j++) {
			size_t nextI = (i + 1) % tessellation;
			size_t nextJ = (j + 1) % tessellation;

			// 삼각형 (두 개의 삼각형으로 면을 구성)
			indices.push_back(i * (tessellation + 1) + j);
			indices.push_back(nextI * (tessellation + 1) + j);
			indices.push_back(i * (tessellation + 1) + nextJ);

			indices.push_back(i * (tessellation + 1) + nextJ);
			indices.push_back(nextI * (tessellation + 1) + j);
			indices.push_back(nextI * (tessellation + 1) + nextJ);
		}
	}

	// 좌표계 변경이 필요하면 (좌표계 뒤집기)
	if (!rhcoords) {
		ReverseWinding(indices, vertices);  // 좌표계 뒤집기 (Right-Handed 좌표계로)
	}
}


void MyMesh::ComputeBox(VertexCollection& vertices, IndexCollection& indices, const XMFLOAT3& size, bool rhcoords, bool invertn)
{
	vertices.clear();
	indices.clear();

	// A box has six faces, each one pointing in a different direction.
	const int FaceCount = 6;

	static const XMVECTORF32 faceNormals[FaceCount] =
	{
		{ { {  0,  0,  1, 0 } } },
		{ { {  0,  0, -1, 0 } } },
		{ { {  1,  0,  0, 0 } } },
		{ { { -1,  0,  0, 0 } } },
		{ { {  0,  1,  0, 0 } } },
		{ { {  0, -1,  0, 0 } } },
	};

	static const XMVECTORF32 textureCoordinates[4] =
	{
		{ { { 1, 0, 0, 0 } } },
		{ { { 1, 1, 0, 0 } } },
		{ { { 0, 1, 0, 0 } } },
		{ { { 0, 0, 0, 0 } } },
	};

	XMVECTOR tsize = XMLoadFloat3(&size);
	tsize = XMVectorDivide(tsize, g_XMTwo);

	//추가
	// Add an offset to shift the box down by half its height
	XMVECTOR yOffset = XMVectorSet(0, size.y / 2.0f, 0, 0);

	// Create each face in turn.
	for (int i = 0; i < FaceCount; i++)
	{
		XMVECTOR normal = faceNormals[i];

		// Get two vectors perpendicular both to the face normal and to each other.
		XMVECTOR basis = (i >= 4) ? g_XMIdentityR2 : g_XMIdentityR1;

		XMVECTOR side1 = XMVector3Cross(normal, basis);
		XMVECTOR side2 = XMVector3Cross(normal, side1);

		// Six indices (two triangles) per face.
		size_t vbase = vertices.size();
		indices.push_back((unsigned int)vbase + 0);
		indices.push_back((unsigned int)vbase + 1);
		indices.push_back((unsigned int)vbase + 2);

		indices.push_back((unsigned int)vbase + 0);
		indices.push_back((unsigned int)vbase + 2);
		indices.push_back((unsigned int)vbase + 3);

		// Four vertices per face.
		// (normal - side1 - side2) * tsize // normal // t0
		vertices.push_back(VertexPositionNormalTexture(XMVectorMultiply(XMVectorSubtract(XMVectorSubtract(normal, side1), side2), tsize), normal, textureCoordinates[0]));

		// (normal - side1 + side2) * tsize // normal // t1
		vertices.push_back(VertexPositionNormalTexture(XMVectorMultiply(XMVectorAdd(XMVectorSubtract(normal, side1), side2), tsize), normal, textureCoordinates[1]));

		// (normal + side1 + side2) * tsize // normal // t2
		vertices.push_back(VertexPositionNormalTexture(XMVectorMultiply(XMVectorAdd(normal, XMVectorAdd(side1, side2)), tsize), normal, textureCoordinates[2]));

		// (normal + side1 - side2) * tsize // normal // t3
		vertices.push_back(VertexPositionNormalTexture(XMVectorMultiply(XMVectorSubtract(XMVectorAdd(normal, side1), side2), tsize), normal, textureCoordinates[3]));
	}

	// Build RH above
	if (!rhcoords)
		ReverseWinding(indices, vertices);

	//if (invertn)
	//	InvertNormals(vertices);
}

void MyMesh::ComputeSphere(VertexCollection& vertices, IndexCollection& indices, float diameter, size_t tessellation, bool rhcoords, bool invertn)
{
	vertices.clear();
	indices.clear();

	//if (tessellation < 3)
	//	throw std::out_of_range("tesselation parameter out of range");

	size_t verticalSegments = tessellation;
	size_t horizontalSegments = tessellation * 2;

	float radius = diameter / 2;

	// Create rings of vertices at progressively higher latitudes.
	for (size_t i = 0; i <= verticalSegments; i++)
	{
		float v = 1 - float(i) / verticalSegments;

		float latitude = (i * XM_PI / verticalSegments) - XM_PIDIV2;
		float dy, dxz;

		XMScalarSinCos(&dy, &dxz, latitude);

		// Create a single ring of vertices at this latitude.
		for (size_t j = 0; j <= horizontalSegments; j++)
		{
			float u = float(j) / horizontalSegments;

			float longitude = j * XM_2PI / horizontalSegments;
			float dx, dz;

			XMScalarSinCos(&dx, &dz, longitude);

			dx *= dxz;
			dz *= dxz;

			XMVECTOR normal = XMVectorSet(dx, dy, dz, 0);
			XMVECTOR textureCoordinate = XMVectorSet(u, v, 0, 0);

			vertices.push_back(VertexPositionNormalTexture(XMVectorScale(normal, radius), normal, textureCoordinate));
		}
	}

	// Fill the index buffer with triangles joining each pair of latitude rings.
	size_t stride = horizontalSegments + 1;

	for (size_t i = 0; i < verticalSegments; i++)
	{
		for (size_t j = 0; j <= horizontalSegments; j++)
		{
			size_t nextI = i + 1;
			size_t nextJ = (j + 1) % stride;

			indices.push_back((unsigned int)(i * stride + j));
			indices.push_back((unsigned int)(nextI * stride + j));
			indices.push_back((unsigned int)(i * stride + nextJ));

			indices.push_back((unsigned int)(i * stride + nextJ));
			indices.push_back((unsigned int)(nextI * stride + j));
			indices.push_back((unsigned int)(nextI * stride + nextJ));
		}
	}

	// Build RH above
	if (!rhcoords)
		ReverseWinding(indices, vertices);

	//if (invertn)
	//	InvertNormals(vertices);
}

//추가
void MyMesh::ComputeCylinder(VertexCollection& vertices, IndexCollection& indices, float height, float radius, size_t tessellation, bool rhcoords, bool invertn)
{
	vertices.clear();
	indices.clear();

	// 원기둥의 높이를 기준으로 중심이 정해진다
	float halfHeight = height / 2.0f;

	// 원기둥의 원 둘레를 구성할 세그먼트 수
	size_t segmentCount = tessellation;

	// 밑면과 윗면을 위한 정점 계산
	for (size_t i = 0; i < segmentCount; i++)
	{
		float angle = XM_2PI * i / segmentCount;
		float x = radius * cos(angle);
		float z = radius * sin(angle);

		// 밑면 정점
		vertices.push_back(VertexPositionNormalTexture(XMVectorSet(x, -halfHeight, z, 1.0f), XMVectorSet(0, -1, 0, 0), XMVectorSet(i / float(segmentCount), 1.0f, 0, 0)));

		// 윗면 정점
		vertices.push_back(VertexPositionNormalTexture(XMVectorSet(x, halfHeight, z, 1.0f), XMVectorSet(0, 1, 0, 0), XMVectorSet(i / float(segmentCount), 0, 0, 0)));
	}

	// 측면을 위한 정점 계산
	for (size_t i = 0; i < segmentCount; i++)
	{
		float angle = XM_2PI * i / segmentCount;
		float x = radius * cos(angle);
		float z = radius * sin(angle);

		// 하단 측면 정점
		vertices.push_back(VertexPositionNormalTexture(XMVectorSet(x, -halfHeight, z, 1.0f), XMVectorSet(x, 0, z, 0), XMVectorSet(i / float(segmentCount), 1.0f, 0, 0)));

		// 상단 측면 정점
		vertices.push_back(VertexPositionNormalTexture(XMVectorSet(x, halfHeight, z, 1.0f), XMVectorSet(x, 0, z, 0), XMVectorSet(i / float(segmentCount), 0, 0, 0)));
	}

	// 밑면과 윗면 인덱스 정의
	size_t baseIndex = 0;
	size_t topIndex = segmentCount;

	// 밑면 삼각형들
	for (size_t i = 0; i < segmentCount; i++)
	{
		size_t next = (i + 1) % segmentCount;
		indices.push_back(baseIndex + i);
		indices.push_back(baseIndex + next);
		indices.push_back(baseIndex + segmentCount);
	}

	// 윗면 삼각형들
	for (size_t i = 0; i < segmentCount; i++)
	{
		size_t next = (i + 1) % segmentCount;
		indices.push_back(topIndex + i);
		indices.push_back(topIndex + next);
		indices.push_back(topIndex + segmentCount);
	}

	// 측면 삼각형들
	size_t sideStart = segmentCount * 2;

	for (size_t i = 0; i < segmentCount; i++)
	{
		size_t next = (i + 1) % segmentCount;

		// 하단 측면
		indices.push_back(sideStart + i * 2);
		indices.push_back(sideStart + next * 2);
		indices.push_back(sideStart + i * 2 + 1);

		// 상단 측면
		indices.push_back(sideStart + next * 2);
		indices.push_back(sideStart + next * 2 + 1);
		indices.push_back(sideStart + i * 2 + 1);
	}

	// RH 좌표계가 아닌 경우 인덱스 순서 반전
	if (!rhcoords)
	{
		ReverseWinding(indices, vertices);
	}

	// 법선 반전이 필요할 경우 처리
	// if (invertn)
	// {
	//     InvertNormals(vertices);
	// }
}




#include "OBJ_Loader.h"

unsigned int MyMesh::LoadOBJ(VertexCollection& vertices, IndexCollection& indices, const WCHAR* modelFilename) {

	vertices.clear();
	indices.clear();

	// Initialize Loader
	objl::Loader Loader;

	// 유니코드 경로를 를 멀티바이트로 캐스팅
	std::wstring message_w = modelFilename;
	std::string path;
	path.assign(message_w.begin(), message_w.end());

	// Load .obj File very very slow because of file io
	bool loadout = Loader.LoadFile(path);
	if (!loadout)
		return 0; // error

	// 여러개의 mesh로 구성된 경우
	int nTris = 0;
	for (int i = 0; i < Loader.LoadedMeshes.size(); i++) {

		// Copy one of the loaded meshes to be our current mesh
		objl::Mesh curMesh = Loader.LoadedMeshes[i];
		// 정점 개수만큼 loop
		for (int j = 0; j < curMesh.Vertices.size(); j++) {

			// memcpy(v, curMesh.Vertices[j], sizeof(VertexPositionNormalTexture)); // 이것도 가능
			XMFLOAT3 p(&curMesh.Vertices[j].Position.X);
			XMFLOAT3 normal(&curMesh.Vertices[j].Normal.X);
			XMFLOAT2 texC(&curMesh.Vertices[j].TextureCoordinate.X);
			VertexPositionNormalTexture v(p, normal, texC);

			vertices.push_back(v);
		}

		for (int j = 0; j < curMesh.Indices.size(); j++) {
			indices.push_back(curMesh.Indices[j]);
		}
		nTris = curMesh.Indices.size() / 3;
		// curMesh.MeshMaterial //으로 사용 가능
	}

	return nTris;
}