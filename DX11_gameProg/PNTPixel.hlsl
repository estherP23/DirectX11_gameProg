SamplerState SampleType;
Texture2D shaderTexture;


cbuffer LightBuffer
{
    float4 diffuseColor;  // 첫 번째 조명의 확산 색상
    int shadeMode;  // 쉐이딩 모드 (0: 텍스처, 1: 노말 맵, 2: phong, 3: blink)
    float3 lightDirection;  // 첫 번째 조명의 방향
    
    float4 diffuseColor2;  // 두 번째 조명의 확산 색상
    
    float  frameCount;
    float3 lightDirection2; // 두 번째 조명의 확산 색상
    
    float3 viewDir;  // 뷰 방향 (카메라)
    float padding;  // padding을 통해 16바이트 정렬
};

struct PixelInputType
{
	// SV: system value
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 tex : TEXCOORD0;
    float3 worldPos : TEXCOORD1;
};

float4 NomalMapPixelShader(PixelInputType input) : SV_TARGET
{
    float4 textureColor;
    textureColor = shaderTexture.Sample(SampleType, input.tex);  // // 텍스처 샘플링
    //// 텍스처가 비어 있으면 빨간색 반환
    if (length(textureColor) == 0)
        return float4(1, 0, 0, 1);

    // 노멀 맵에서 법선 벡터를 가져와서 수정
    float3 normalFromMap = textureColor.xyz * 2.0f - 1.0f;
    float3 L = normalize(float3(1, 1, 1)); // 조명 방향
    float NL = abs(dot(normalFromMap, L));  // 조명 방향과 법선의 내적
    float i = 0.3 + 0.7 * NL; // 조명 계산
    return float4(i, i, i, 1);  // 최종 색상 반환
}

float4 ColorPixelShader(PixelInputType input) : SV_TARGET
{
	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
    float4 textureColor = shaderTexture.Sample(SampleType, input.tex);
   
    if (shadeMode == 0) // normal texture
        return textureColor;
    
    if (shadeMode == 1) // normal map
    {
        // 법선 맵을 샘플링하여 법선 수정
        float3 normalFromMap = textureColor.xyz * 2.0f - 1.0f;
        float3 N = normalize(normalFromMap);  // 원래 법선에 노멀 맵에서 샘플링된 법선 추가
        float NL = clamp(dot(lightDirection, N), 0, 1);  // 법선과 조명 방향의 내적
        float3 col = 0.3f + 0.6f * diffuseColor * NL + 0.2f;
        return float4(col, 1); // 색상 반환

    }
    // 기본 색상 설정
    float3 objColor = float3(212 / 255.0, 192 / 255.0, 214 / 255.0);
    if (shadeMode == 3) // blink
    {
        // `frameCount`에 따른 깜박임 효과
        float weight = sin(frameCount*0.1) * 0.5 + 0.5;
        objColor = lerp(float3(1, 0, 0), float3(0, 1, 0), weight);  // 빨간색과 초록색 간의 보간
    }
    
    // phong 조명 계산
    float NL = clamp(dot(input.normal, lightDirection), 0, 1);  // 법선과 조명 방향의 내적
    float3 R = normalize(2.0 * dot(input.normal, lightDirection) * input.normal - lightDirection);  // 반사 벡터 계산
    float RV = clamp(dot(R, viewDir), 0, 1);  // 뷰 방향과 반사 벡터의 내적
    
    // 최종 색상 계산
    float3 col = 0.2f * objColor + 0.7f * objColor * diffuseColor.xyz * NL + 0.5f * pow(RV, 16); //0.8f + 0.6f 
    col = clamp(col, 0, 1);
    return float4(col, 1);
   
    float3 lightDir;
    float3 lightDir2; //조명2 방향
    float lightIntensity;
    float lightIntensity2; //조명2 밝기
    float4 color;

    //Phong 조명 모델을 위한 추가들
    float3 viewDir; // 뷰 방향 추가
    float3 reflectVec; // 반사 벡터 추가
    float3 halfVec; // Blinn-Phong의 하프 벡터
    float phongSpecular; // Phong 모델의 스페큘러
    float blinnSpecular; // Blinn-Phong 모델의 스페큘러
    
    return float4(1, 1, 0, 1);



	// Invert the light direction for calculations.
    lightDir = -lightDirection;
    lightDir = normalize(lightDir);

    //조명2 방향
    lightDir2 = -lightDirection2;
    lightDir2 = normalize(lightDir2);

    // Phong조명을 위한 뷰 벡터 계산
    viewDir = normalize(-input.position.xyz);

    // Diffuse 계산
    lightIntensity = saturate(dot(input.normal, lightDir));
    lightIntensity2 = saturate(dot(input.normal, lightDir2));

    // Phong 스페큘러 계산
    reflectVec = reflect(-lightDir, input.normal);
    phongSpecular = pow(saturate(dot(reflectVec, viewDir)), 32); // n 값 조정 가능

    // Blinn-Phong 스페큘러 계산
    halfVec = normalize(lightDir + viewDir);
    blinnSpecular = pow(saturate(dot(halfVec, input.normal)), 64); // n 값 조정 가능

    // 최종 색상 계산
    color = diffuseColor * (lightIntensity + lightIntensity2);
    color += diffuseColor * (phongSpecular + blinnSpecular);



    return color;
}