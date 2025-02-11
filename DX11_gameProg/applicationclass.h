////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "d3dclass.h"
#include "inputclass.h"
#include "cameraclass.h"
#include "PNTShaderClass.h"
#include "MyMesh.h"
#include "lightclass.h"
#include "rendertextureclass.h"
#include "textureshaderclass.h"
#include "displayplaneclass.h"


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;


////////////////////////////////////////////////////////////////////////////////
// Class name: ApplicationClass
////////////////////////////////////////////////////////////////////////////////
class ApplicationClass
{
public:
	ApplicationClass();
	ApplicationClass(const ApplicationClass&);
	~ApplicationClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(InputClass*);

private:
	bool Render(float);
	bool RenderSceneToTexture(RenderTextureClass* rtt, XMFLOAT3 camPos, XMFLOAT3 camDir);
	bool RenderModels(XMMATRIX, XMMATRIX);
private:
	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	PNTShaderClass* m_PNTShader;
	MyMesh* m_Model;
	std::vector<MyMesh*> m_models;
	LightClass* m_Light;
	LightClass* m_Light2;  //2번째 조명
	RenderTextureClass* m_RenderTexture;
	RenderTextureClass* m_RenderTexture2;  //2번째 RenderTexture 
	TextureShaderClass* m_TextureShader;
	DisplayPlaneClass* m_DisplayPlane;
	float m_catRotation; // cctv camera rotation
	float m_rotation;	// main camera rotation
};

#endif