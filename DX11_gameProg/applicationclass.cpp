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
	m_Light2 = 0;  //2��° ���� �ʱ�ȭ
	m_RenderTexture = 0;
	m_RenderTexture2 = 0;  //2��° RenderTexture �ʱ�ȭ
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

	//����1����, ���� ����
	m_Light->SetDiffuseColor(1, 1, 1, 1.0f);  // �⺻ ��� ����
	m_Light->SetDirection(0.0f, 1.0f, 0.0f);  // ������ ���� ����

	//����2 ����, ���� ����
	m_Light2 = new LightClass;

	m_Light2->SetDiffuseColor(1.0f, 0.0f, 0.0f, 1.0f); // ������ ����
	m_Light2->SetDirection(1.0f, 1.0f, -1.0f);  // ������ ���� ����

	//normal map ���� �߰�
	const WCHAR* textureFilenamef = L"data/stone01.tga";        // �⺻ �ؽ�ó
	const WCHAR* normalMapFilename = L"data/normal01.tga"; // �븻�� �ؽ�ó

	//�ٴ� �ؽ���
	const WCHAR* textureFilenamef2 = L"data/floorTexture.tga";       
	const WCHAR* normalMapFilename2 = L"data/floorNormal.tga"; 

	//�� �ؽ���
	const WCHAR* textureFilenamef3 = L"data/wallTexture.tga";       
	const WCHAR* normalMapFilename3 = L"data/wallNormal.tga"; 

	//���� �ؽ���
	const WCHAR* textureFilenamef4 = L"data/chairTexture.tga";        
	const WCHAR* normalMapFilename4 = L"data/chairNormal.tga"; 

	//å�� �ؽ���
	const WCHAR* textureFilenamef5 = L"data/deskTexture.tga";       
	const WCHAR* normalMapFilename5 = L"data/deskNormal.tga"; 

	//���� �ؽ���2
	const WCHAR* textureFilenamef6 = L"data/chairTexture2.tga";        
	const WCHAR* normalMapFilename6 = L"data/chairNormal2.tga"; 



	std::string str; // ��ü �̸� ����
	MyMesh* pModel = NULL;  // ��ü ������
	XMMATRIX mtxModel = XMMatrixIdentity();  // ��ü�� ��ȯ ���, �ʱⰪ�� ���� ���


	if (true) {
		// 1��° ��ü : �ٴ�
		pModel = new MyMesh;
		str = "floor";
		mtxModel = XMMatrixTranslation(0, 0, 0);   // ������ ��ġ
		result = pModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), NULL, normalMapFilename2, str, mtxModel);
		if (!result)
		{
			std::wstring stemp = std::wstring(str.begin(), str.end());
			LPCWSTR sw = stemp.c_str();
			MessageBox(hwnd, L"Could not initialize the model object.", sw, MB_OK);
			return false;
		}
		m_models.push_back(pModel);

		// 2��° ��ü : ���̺�(obj)
		pModel = new MyMesh;
		str = "table";
		mtxModel = XMMatrixTranslation(3, 0, 2); // y�� ���̴ϱ� ���߿� ���� �ʰ�, x,z�� ��������.
		result = pModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), NULL, textureFilenamef5, str, mtxModel);
		if (!result)
		{
			std::wstring stemp = std::wstring(str.begin(), str.end());
			LPCWSTR sw = stemp.c_str();
			MessageBox(hwnd, L"Could not initialize the model object.", sw, MB_OK);
			return false;
		}
		m_models.push_back(pModel);
		

		// 3��° ��ü : ����1
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
		

		//4��° ��ü : ����2
		pModel = new MyMesh;
		str = "chair";
		mtxModel = XMMatrixTranslation(3, 0, -5); // y�� ���̴ϱ� ���߿� ���� �ʰ�, x,z�� ��������.
		result = pModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), NULL, textureFilenamef4, str, mtxModel);
		if (!result)
		{
			std::wstring stemp = std::wstring(str.begin(), str.end());
			LPCWSTR sw = stemp.c_str();
			MessageBox(hwnd, L"Could not initialize the model object.", sw, MB_OK);
			return false;
		}
		m_models.push_back(pModel);


		// 5��° ��ü: ��
		pModel = new MyMesh;
		str = "sphere";
		mtxModel = XMMatrixTranslation(2, 0, 2); // y�� ���̴ϱ� ���߿� ���� �ʰ�, x,z�� ��������.
		result = pModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), NULL, NULL, str, mtxModel);
		if (!result)
		{
			std::wstring stemp = std::wstring(str.begin(), str.end());
			LPCWSTR sw = stemp.c_str();
			MessageBox(hwnd, L"Could not initialize the model object.", sw, MB_OK);
			return false;
		}
		m_models.push_back(pModel);

		
	

		// 6��° ��ü : �䳢(obj)
		pModel = new MyMesh;
		str = "bunny";
		mtxModel = XMMatrixTranslation(3.2, 2.3, -4.8); // y�� ���̴ϱ� ���߿� ���� �ʰ�, x,z�� ��������.
		result = pModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), NULL, NULL, str, mtxModel);
		if (!result)
		{
			std::wstring stemp = std::wstring(str.begin(), str.end());
			LPCWSTR sw = stemp.c_str();
			MessageBox(hwnd, L"Could not initialize the model object.", sw, MB_OK);
			return false;
		}
		m_models.push_back(pModel);

		

		// 7��° ��ü : �����(obj)
		pModel = new MyMesh;
		str = "cat";
		mtxModel = XMMatrixTranslation(0, 0, 0); // y�� ���̴ϱ� ���߿� ���� �ʰ�, x,z�� ��������.
		result = pModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), NULL, NULL, str, mtxModel);
		if (!result)
		{
			std::wstring stemp = std::wstring(str.begin(), str.end());
			LPCWSTR sw = stemp.c_str();
			MessageBox(hwnd, L"Could not initialize the model object.", sw, MB_OK);
			return false;
		}
		m_models.push_back(pModel);


		// 8��° ��ü : ����� ����(obj)
		pModel = new MyMesh;
		str = "catChair";
		mtxModel = XMMatrixTranslation(-8, 0, 8); // y�� ���̴ϱ� ���߿� ���� �ʰ�, x,z�� ��������.
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
	
	//����� �ü� ȭ��
	result = m_RenderTexture->Initialize(m_Direct3D->GetDevice(), 512, 512, SCREEN_DEPTH, SCREEN_NEAR, 1);
	if (!result)
	{
		return false;
	}

	//�䳢 �ü� ȭ��
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
	//m_RenderTexture2�� Shutdown
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
	// �ݺ��ϸ鼭 ����
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


	const float Radian2Degree = 0.0174532925f;  // ���� ��ȯ�� ���� ���

	// Check if the user pressed escape and wants to exit the application.
	if (Input->IsEscapePressed())
	{
		return false;
	}
	const float param = 5.0;
	if		(Input->IsLeftArrowPressed()) {
		m_rotation += Radian2Degree * param;
		positionX -= 0.1f;
		m_Light->SetDiffuseColor(0.6f, 1.0f, 0.6f, 1.0f);  // �ʷϻ� ����
		m_Light2->SetDiffuseColor(1.0f, 0.0f, 0.0f, 1.0f); // ������ ����
	}

	if		(Input->IsRightArrowPressed()) {
		m_rotation -= Radian2Degree * param;
		positionX += 0.1f; // Move the object to the right
		m_Light->SetDiffuseColor(1.0f, 0.8f, 0.8f, 1.0f);  // ������ ����
		m_Light2->SetDiffuseColor(0.0f, 0.5f, 0.0f, 1.0f); // �ʷϻ� ����
	}
	if (Input->IsKeyPressed('Q') || Input->IsKeyPressed('E')) { // ���� ���� ����
		// using GPT
		XMFLOAT3 l = m_Light->GetDirection(); 
		XMVECTOR light = XMLoadFloat3(&l);

		XMFLOAT3 rotaxis(1, 1, 1);					// ȸ����
		XMVECTOR axis = XMLoadFloat3(&rotaxis);
		float theta = Radian2Degree;				// ȸ�� ����
		if (Input->IsKeyPressed('E'))
			theta = -theta;
		XMVECTOR res = XMVector3TransformNormal(light, XMMatrixRotationAxis(axis, theta));	// ȸ������� ����
		res = XMVector3Normalize(res);	// �������ͷ� ����� ����
		XMFLOAT3 result;
		XMStoreFloat3(&result, res);
		m_Light->SetDirection(result.x, result.y, result.z);
	}
	//����� �� ȸ��
	if (Input->IsUpArrowPressed()) {
		m_catRotation += 1.0;
	}

	if (Input->IsDownArrowPressed()) {
		m_catRotation -= 1.0;
	}


	// �̵� ����: ȭ�� ������ ������ �ʵ��� 
	if (positionX > 4.0f) positionX = 4.0f;
	if (positionX < -4.0f) positionX = -4.0f;

	// ����� ī�޶��� ��ġ�� ȸ�� ����
	XMFLOAT3 catCamPos(-8.0f, 1.0f + 3.0f, 8.0f);  // ����� ī�޶��� ��ġ
	XMFLOAT3 catCamRot(0, 135+ m_catRotation, 0);  // ����� ī�޶��� ȸ�� (y�� ���� ȸ��)
	result = RenderSceneToTexture(m_RenderTexture, catCamPos, catCamRot);  // ���� �ؽ�ó�� ������
	if (!result)
	{
		return false;
	}

	static float tt = 0;
	tt -= 0.1f;

	// �� ��° ��ȭ���� �׸� �׸��� (�䳢 ī�޶�)
	XMFLOAT3 bunnyCamPos(3.2, 4.3, -4.8);  // �䳢 ī�޶��� ��ġ
	XMFLOAT3 bunnyCamRot(0, -40, 0);   // �䳢 ī�޶��� ȸ�� (y�� ���� ȸ��)
	result = RenderSceneToTexture(m_RenderTexture2, bunnyCamPos, bunnyCamRot);  // ���� �ؽ�ó�� ������
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


	// �������� ��ü �׸���
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
		XMMATRIX worldMatrix = XMMatrixIdentity();  // �ʱ� worldMatrix�� �׵� ��ķ� ����
		
		m_models[i]->Render(m_Direct3D->GetDeviceContext());
		//���� �̸��� "cat"�� ���, m_catRotation�� �������� y�� ȸ���� �߰�
		if (m_models[i]->GetName() == string("cat")) {
			worldMatrix *= XMMatrixRotationY(m_catRotation / 180.0 * 3.141592f);
		}
		worldMatrix *= m_models[i]->GetModelMatrix();  //���� ���� ��ȯ ��� ����
		int shadeMode = m_models[i]->GetShadeMode();   // ���� ���̵� ��� ��������
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

	// �������� ��ü �׸���
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

	//m_RenderTexture2�� m_DisplayPlane ����
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