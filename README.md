# 🏠 DirectX11_3D Room Simulation with Cat CCTV  

## 📌 프로젝트 개요  
**이 프로젝트는 방 안에 3D 오브젝트를 배치하고, 고양이 CCTV를 통해 360도 시점을 조작할 수 있는 DirectX 기반의 그래픽스 렌더링 실습입니다.**  
고양이 카메라를 활용하여 방 내부를 자유롭게 둘러볼 수 있으며, 다양한 조명 및 텍스처 효과를 적용하여 몰입감을 높였습니다.

<img width="323" alt="image" src="https://github.com/user-attachments/assets/b33f59fc-5b52-4466-8007-fa5cb56407ea" /> <img width="323" alt="image" src="https://github.com/user-attachments/assets/5046c407-7cbf-465d-a531-163547f04b4f" />



## 🎮 주요 기능  
- 🏠 **360도 시점 이동:** 기본 카메라 및 고양이 카메라를 이용해 방 내부를 자유롭게 관찰  
- 🎨 **조명 효과 적용:** 다중 조명을 활용하여 물체의 광원 반응을 사실적으로 구현  
- 🎭 **텍스처 및 노멀 맵 적용:** 표면 질감을 세밀하게 표현하여 현실감 극대화  
- 🎥 **실시간 카메라 뷰:** 고양이의 시점을 통해 방을 바라볼 수 있도록 구현  

## 🕹️ 조작법  
| 키 입력  | 기능 설명 |
|----------|---------|
| ← → | 기본 관찰 카메라 좌/우 회전 |
| ↑ ↓ | 고양이 카메라 좌/우 회전 |
| Q, E  | 조명 방향 회전 |

## 🛠️ 사용 기술  
- **프로그래밍 언어:** C++  
- **그래픽 API:** DirectX 11  
- **셰이더 프로그래밍:** HLSL (Phong 조명 모델, Blinn-Phong 반사)  
- **3D 그래픽 처리:** 텍스처, 노멀 맵, 광원 효과 적용  

## 📂 코드 구조  
### 📌 `applicationClass.cpp`  
- **다중 조명 및 텍스처 초기화**  
- **카메라 이동 및 회전 처리**  
- **렌더링 설정 및 프레임 업데이트**  

### 📌 `MyMesh.cpp`  
- **3D 오브젝트 초기화 및 설정**  
- **셰이딩 모드 (텍스처, 노멀맵, Phong, Blink) 적용**  

### 📌 `PNTPixel.hlsl`  
- **조명과 노멀 맵을 활용한 표면 반사 처리**  
- **Phong 및 Blinn-Phong 조명 모델을 이용한 광원 효과 구현**  

## 📸 시연 영상  
🔗 https://youtu.be/94i0vJFU47c
## 🚀 실행 방법  
1. **프로젝트 클론:**  
   ```sh
   git clone https://github.com/your-repo/3D-Room-Simulation.git
