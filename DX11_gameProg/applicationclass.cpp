////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "applicationclass.h"

// global clock
int g_clock = 0;

ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_PNTShader = 0;
	m_Light = 0;
	m_Light2 = 0;  //2번째 조명 초기화
	m_RenderTexture = 0;
	m_RenderTexture2 = 0;  //2번째 RenderTexture 초기화
	m_TextureShader = 0;
	m_DisplayPlane = 0;

	m_catRotation = 0;
	m_rotation = 0;
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	//const WCHAR* textureFilename = L"lake.jpg";;
	bool result;

	// Create and initialize the Direct3D object.
	m_Direct3D = new D3DClass;

	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// Create and initialize the camera object.
	m_Camera = new CameraClass;
	m_Camera->Render();

	m_Light = new LightClass;

	//조명1색상, 방향 설정
	m_Light->SetDiffuseColor(1, 1, 1, 1.0f);  // 기본 흰색 조명
	m_Light->SetDirection(0.0f, 1.0f, 0.0f);  // 조명의 방향 설정

	//조명2 색상, 방향 설정
	m_Light2 = new LightClass;

	m_Light2->SetDiffuseColor(1.0f, 0.0f, 0.0f, 1.0f); // 빨간색 조명
	m_Light2->SetDirection(1.0f, 1.0f, -1.0f);  // 조명의 방향 설정

	//normal map 파일 추가
	const WCHAR* textureFilenamef = L"data/stone01.tga";        // 기본 텍스처
	const WCHAR* normalMapFilename = L"data/normal01.tga"; // 노말맵 텍스처

	//바닥 텍스쳐
	const WCHAR* textureFilenamef2 = L"data/floorTexture.tga";       
	const WCHAR* normalMapFilename2 = L"data/floorNormal.tga"; 

	//벽 텍스쳐
	const WCHAR* textureFilenamef3 = L"data/wallTexture.tga";       
	const WCHAR* normalMapFilename3 = L"data/wallNormal.tga"; 

	//의자 텍스쳐
	const WCHAR* textureFilenamef4 = L"data/chairTexture.tga";        
	const WCHAR* normalMapFilename4 = L"data/chairNormal.tga"; 

	//책상 텍스쳐
	const WCHAR* textureFilenamef5 = L"data/deskTexture.tga";       
	const WCHAR* normalMapFilename5 = L"data/deskNormal.tga"; 

	//의자 텍스쳐2
	const WCHAR* textureFilenamef6 = L"data/chairTexture2.tga";        
	const WCHAR* normalMapFilename6 = L"data/chairNormal2.tga"; 



	std::string str; // 물체 이름 저장
	MyMesh* pModel = NULL;  // 물체 포인터
	XMMATRIX mtxModel = XMMatrixIdentity();  // 물체의 변환 행렬, 초기값은 단위 행렬


	if (true) {
		// 1번째 물체 : 바닥
		pModel = new MyMesh;
		str = "floor";
		mtxModel = XMMatrixTranslation(0, 0, 0);   // 원점에 위치
		result = pModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), NULL, normalMapFilename2, str, mtxModel);
		if (!result)
		{
			std::wstring stemp = std::wstring(str.begin(), str.end());
			LPCWSTR sw = stemp.c_str();
			MessageBox(hwnd, L"Could not initialize the model object.", sw, MB_OK);
			return false;
		}
		m_models.push_back(pModel);

		// 2번째 물체 : 테이블(obj)
		pModel = new MyMesh;
		str = "table";
		mtxModel = XMMatrixTranslation(3, 0, 2); // y는 높이니까 공중에 뜨지 않게, x,z를 움직여라.
		result = pModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), NULL, textureFilenamef5, str, mtxModel);
		if (!result)
		{
			std::wstring stemp = std::wstring(str.begin(), str.end());
			LPCWSTR sw = stemp.c_str();
			MessageBox(hwnd, L"Could not initialize the model object.", sw, MB_OK);
			return false;
		}
		m_models.push_back(pModel);
		

		// 3번째 물체 : 의자1
		pModel = new MyMesh;
		str = "chair";
		mtxModel = XMMatrixTranslation(-3, 0, -5); // 
		result = pModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), NULL, textureFilenamef4, str, mtxModel);
		if (!result)
		{
			std::wstring stemp = std::wstring(str.begin(), str.end());
			LPCWSTR sw = stemp.c_str();
			MessageBox(hwnd, L"Could not initialize the model object.", sw, MB_OK);
			return false;
		}
		m_models.push_back(pModel);
		

		//4번째 물체 : 의자2
		pModel = new MyMesh;
		str = "chair";
		mtxModel = XMMatrixTranslation(3, 0, -5); // y는 높이니까 공중에 뜨지 않게, x,z를 움직여라.
		result = pModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), NULL, textureFilenamef4, str, mtxModel);
		if (!result)
		{
			std::wstring stemp = std::wstring(str.begin(), str.end());
			LPCWSTR sw = stemp.c_str();
			MessageBox(hwnd, L"Could not initialize the model object.", sw, MB_OK);
			return false;
		}
		m_models.push_back(pModel);


		// 5번째 물체: 공
		pModel = new MyMesh;
		str = "sphere";
		mtxModel = XMMatrixTranslation(2, 0, 2); // y는 높이니까 공중에 뜨지 않게, x,z를 움직여라.
		result = pModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), NULL, NULL, str, mtxModel);
		if (!result)
		{
			std::wstring stemp = std::wstring(str.begin(), str.end());
			LPCWSTR sw = stemp.c_str();
			MessageBox(hwnd, L"Could not initialize the model object.", sw, MB_OK);
			return false;
		}
		m_models.push_back(pModel);

		
	

		// 6번째 물체 : 토끼(obj)
		pModel = new MyMesh;
		str = "bunny";
		mtxModel = XMMatrixTranslation(3.2, 2.3, -4.8); // y는 높이니까 공중에 뜨지 않게, x,z를 움직여라.
		result = pModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), NULL, NULL, str, mtxModel);
		if (!result)
		{
			std::wstring stemp = std::wstring(str.begin(), str.end());
			LPCWSTR sw = stemp.c_str();
			MessageBox(hwnd, L"Could not initialize the model object.", sw, MB_OK);
			return false;
		}
		m_models.push_back(pModel);

		

		// 7번째 물체 : 고양이(obj)
		pModel = new MyMesh;
		str = "cat";
		mtxModel = XMMatrixTranslation(0, 0, 0); // y는 높이니까 공중에 뜨지 않게, x,z를 움직여라.
		result = pModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), NULL, NULL, str, mtxModel);
		if (!result)
		{
			std::wstring stemp = std::wstring(str.begin(), str.end());
			LPCWSTR sw = stemp.c_str();
			MessageBox(hwnd, L"Could not initialize the model object.", sw, MB_OK);
			return false;
		}
		m_models.push_back(pModel);


		// 8번째 물체 : 고양이 의자(obj)
		pModel = new MyMesh;
		str = "catChair";
		mtxModel = XMMatrixTranslation(-8, 0, 8); // y는 높이니까 공중에 뜨지 않게, x,z를 움직여라.
		result = pModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), NULL, textureFilenamef5, str, mtxModel);
		if (!result)
		{
			std::wstring stemp = std::wstring(str.begin(), str.end());
			LPCWSTR sw = stemp.c_str();
			MessageBox(hwnd, L"Could not initialize the model object.", sw, MB_OK);
			return false;
		}
		m_models.push_back(pModel);

	}

	
	//result = m_Model->Initialize(m_Direct3D->GetDevice(), NULL, textureFilename);

	m_PNTShader = new PNTShaderClass;

	result = m_PNTShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		return false;
	}

	// Create and initialize the render to texture object.
	m_RenderTexture = new RenderTextureClass;
	
	//고양이 시선 화면
	result = m_RenderTexture->Initialize(m_Direct3D->GetDevice(), 512, 512, SCREEN_DEPTH, SCREEN_NEAR, 1);
	if (!result)
	{
		return false;
	}

	//토끼 시선 화면
	m_RenderTexture2 = new RenderTextureClass;
	result = m_RenderTexture2->Initialize(m_Direct3D->GetDevice(), 512, 512, SCREEN_DEPTH, SCREEN_NEAR, 1);
	if (!result)
	{
		return false;
	}

	m_TextureShader = new TextureShaderClass;

	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		return false;
	}

	// Create and initialize the display plane object.
	m_DisplayPlane = new DisplayPlaneClass;

	result = m_DisplayPlane->Initialize(m_Direct3D->GetDevice(), 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}

	return true;
}


void ApplicationClass::Shutdown()
{
	// Release the display plane object.
	if (m_DisplayPlane)
	{
		m_DisplayPlane->Shutdown();
		delete m_DisplayPlane;
		m_DisplayPlane = 0;
	}

	// Release the texture shader.
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// Release the render to texture object.
	if (m_RenderTexture)
	{
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}
	//m_RenderTexture2의 Shutdown
	if (m_RenderTexture2)
	{
		m_RenderTexture2->Shutdown();
		delete m_RenderTexture2;
		m_RenderTexture2 = 0;
	}

	if (m_PNTShader)
	{
		m_PNTShader->Shutdown();
		delete m_PNTShader;
		m_PNTShader = 0;
	}
	// 반복하면서 지움
	int s = m_models.size();
	for (int i = 0; i<s; i++) {
		m_models[i]->Shutdown();
		delete m_models[i];
		m_models[i] = 0;
	}
	// Release the model object.
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the Direct3D object.
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	if (m_Light) {
		delete m_Light;
		m_Light = 0;
	}

	return;
}


bool ApplicationClass::Frame(InputClass* Input)
{
	g_clock++;
	static float positionX = 0.0f;  
	bool result;


	const float Radian2Degree = 0.0174532925f;  // 라디안 변환을 위한 상수

	// Check if the user pressed escape and wants to exit the application.
	if (Input->IsEscapePressed())
	{
		return false;
	}
	const float param = 5.0;
	if		(Input->IsLeftArrowPressed()) {
		m_rotation += Radian2Degree * param;
		positionX -= 0.1f;
		m_Light->SetDiffuseColor(0.6f, 1.0f, 0.6f, 1.0f);  // 초록색 조명
		m_Light2->SetDiffuseColor(1.0f, 0.0f, 0.0f, 1.0f); // 빨간색 조명
	}

	if		(Input->IsRightArrowPressed()) {
		m_rotation -= Radian2Degree * param;
		positionX += 0.1f; // Move the object to the right
		m_Light->SetDiffuseColor(1.0f, 0.8f, 0.8f, 1.0f);  // 빨간색 조명
		m_Light2->SetDiffuseColor(0.0f, 0.5f, 0.0f, 1.0f); // 초록색 조명
	}
	if (Input->IsKeyPressed('Q') || Input->IsKeyPressed('E')) { // 조명 방향 변경
		// using GPT
		XMFLOAT3 l = m_Light->GetDirection(); 
		XMVECTOR light = XMLoadFloat3(&l);

		XMFLOAT3 rotaxis(1, 1, 1);					// 회전축
		XMVECTOR axis = XMLoadFloat3(&rotaxis);
		float theta = Radian2Degree;				// 회전 각도
		if (Input->IsKeyPressed('E'))
			theta = -theta;
		XMVECTOR res = XMVector3TransformNormal(light, XMMatrixRotationAxis(axis, theta));	// 회전행렬을 적용
		res = XMVector3Normalize(res);	// 단위벡터로 만들고 저장
		XMFLOAT3 result;
		XMStoreFloat3(&result, res);
		m_Light->SetDirection(result.x, result.y, result.z);
	}
	//고양이 모델 회전
	if (Input->IsUpArrowPressed()) {
		m_catRotation += 1.0;
	}

	if (Input->IsDownArrowPressed()) {
		m_catRotation -= 1.0;
	}


	// 이동 제한: 화면 밖으로 나가지 않도록 
	if (positionX > 4.0f) positionX = 4.0f;
	if (positionX < -4.0f) positionX = -4.0f;

	// 고양이 카메라의 위치와 회전 설정
	XMFLOAT3 catCamPos(-8.0f, 1.0f + 3.0f, 8.0f);  // 고양이 카메라의 위치
	XMFLOAT3 catCamRot(0, 135+ m_catRotation, 0);  // 고양이 카메라의 회전 (y축 기준 회전)
	result = RenderSceneToTexture(m_RenderTexture, catCamPos, catCamRot);  // 씬을 텍스처에 렌더링
	if (!result)
	{
		return false;
	}

	static float tt = 0;
	tt -= 0.1f;

	// 두 번째 도화지에 그림 그리기 (토끼 카메라)
	XMFLOAT3 bunnyCamPos(3.2, 4.3, -4.8);  // 토끼 카메라의 위치
	XMFLOAT3 bunnyCamRot(0, -40, 0);   // 토끼 카메라의 회전 (y축 기준 회전)
	result = RenderSceneToTexture(m_RenderTexture2, bunnyCamPos, bunnyCamRot);  // 씬을 텍스처에 렌더링
	if (!result)
	{
		return false;
	}

	

	result = Render(positionX);
	if (!result)
	{
		return false;
	}

	return true;
}

bool ApplicationClass::RenderSceneToTexture(RenderTextureClass* rtt, XMFLOAT3 camPos, XMFLOAT3 camRot)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// Set the render target to be the render texture and clear it.
	rtt->SetRenderTarget(m_Direct3D->GetDeviceContext());
	rtt->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 1.0f, 1.0f, 1.0f, 1.0f);

	// Set the position of the camera for viewing the cube.
	m_Camera->SetPosition(camPos.x, camPos.y, camPos.z);
	m_Camera->SetRotation(camRot.x, camRot.y, camRot.z);
	m_Camera->Render();

	// Get the matrices.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	rtt->GetProjectionMatrix(projectionMatrix);


	// 여러개의 물체 그리기
	result = RenderModels(viewMatrix, projectionMatrix);
	if (!result) {
		return false;
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.  And reset the viewport back to the original.
	m_Direct3D->SetBackBufferRenderTarget();
	m_Direct3D->ResetViewport();

	// Render the final graphics scene.
	return true;
}
bool ApplicationClass::RenderModels(XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	for (int i = 0; i < m_models.size(); i++) {
		XMMATRIX worldMatrix = XMMatrixIdentity();  // 초기 worldMatrix를 항등 행렬로 설정
		
		m_models[i]->Render(m_Direct3D->GetDeviceContext());
		//모델의 이름이 "cat"인 경우, m_catRotation을 기준으로 y축 회전을 추가
		if (m_models[i]->GetName() == string("cat")) {
			worldMatrix *= XMMatrixRotationY(m_catRotation / 180.0 * 3.141592f);
		}
		worldMatrix *= m_models[i]->GetModelMatrix();  //모델의 고유 변환 행렬 적용
		int shadeMode = m_models[i]->GetShadeMode();   // 모델의 쉐이딩 모드 가져오기
		bool result = m_PNTShader->Render(m_Direct3D->GetDeviceContext(), m_models[i]->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, 
			m_models[i]->GetTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Light2->GetDirection(), m_Light2->GetDiffuseColor(), shadeMode);
		if (!result) {
			return false;
		}
	}
	return true;
}
bool ApplicationClass::Render(float positionX)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	  
	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(0.6f, 0.6f, 0.6f, 1.0f);

	// Set the position of the camera for viewing the display planes with the render textures on them.
	m_Camera->SetPosition(0.0f, 10.0f, -30.0f);
	m_Camera->SetRotation(10, m_rotation, 0);
	
	
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);


	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 여러개의 물체 그리기
	result = RenderModels(viewMatrix, projectionMatrix);

	worldMatrix = XMMatrixScaling(2.5, 2, 0.5);
	worldMatrix *= XMMatrixTranslation(-4, 10.0f, -10.0f);

	// Render the display plane using the texture shader and the render texture resource.
	m_DisplayPlane->Render(m_Direct3D->GetDeviceContext());

	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_DisplayPlane->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_RenderTexture->GetShaderResourceView());
	if (!result)
	{
		return false;
	}

	//m_RenderTexture2의 m_DisplayPlane 생성
	worldMatrix = XMMatrixScaling(2.5, 2, 0.5);
	worldMatrix *= XMMatrixTranslation(4.0f, 10.0f, -10.0f);
	//
	m_DisplayPlane->Render(m_Direct3D->GetDeviceContext());
	//
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_DisplayPlane->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_RenderTexture2->GetShaderResourceView());
	if (!result)
	{
		return false;
	}

	m_Direct3D->EndScene();

	return true;
}