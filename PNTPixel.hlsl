SamplerState SampleType;
Texture2D shaderTexture;


cbuffer LightBuffer
{
    float4 diffuseColor;  // ù ��° ������ Ȯ�� ����
    int shadeMode;  // ���̵� ��� (0: �ؽ�ó, 1: �븻 ��, 2: phong, 3: blink)
    float3 lightDirection;  // ù ��° ������ ����
    
    float4 diffuseColor2;  // �� ��° ������ Ȯ�� ����
    
    float  frameCount;
    float3 lightDirection2; // �� ��° ������ Ȯ�� ����
    
    float3 viewDir;  // �� ���� (ī�޶�)
    float padding;  // padding�� ���� 16����Ʈ ����
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
    textureColor = shaderTexture.Sample(SampleType, input.tex);  // // �ؽ�ó ���ø�
    //// �ؽ�ó�� ��� ������ ������ ��ȯ
    if (length(textureColor) == 0)
        return float4(1, 0, 0, 1);

    // ��� �ʿ��� ���� ���͸� �����ͼ� ����
    float3 normalFromMap = textureColor.xyz * 2.0f - 1.0f;
    float3 L = normalize(float3(1, 1, 1)); // ���� ����
    float NL = abs(dot(normalFromMap, L));  // ���� ����� ������ ����
    float i = 0.3 + 0.7 * NL; // ���� ���
    return float4(i, i, i, 1);  // ���� ���� ��ȯ
}

float4 ColorPixelShader(PixelInputType input) : SV_TARGET
{
	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
    float4 textureColor = shaderTexture.Sample(SampleType, input.tex);
   
    if (shadeMode == 0) // normal texture
        return textureColor;
    
    if (shadeMode == 1) // normal map
    {
        // ���� ���� ���ø��Ͽ� ���� ����
        float3 normalFromMap = textureColor.xyz * 2.0f - 1.0f;
        float3 N = normalize(normalFromMap);  // ���� ������ ��� �ʿ��� ���ø��� ���� �߰�
        float NL = clamp(dot(lightDirection, N), 0, 1);  // ������ ���� ������ ����
        float3 col = 0.3f + 0.6f * diffuseColor * NL + 0.2f;
        return float4(col, 1); // ���� ��ȯ

    }
    // �⺻ ���� ����
    float3 objColor = float3(212 / 255.0, 192 / 255.0, 214 / 255.0);
    if (shadeMode == 3) // blink
    {
        // `frameCount`�� ���� ������ ȿ��
        float weight = sin(frameCount*0.1) * 0.5 + 0.5;
        objColor = lerp(float3(1, 0, 0), float3(0, 1, 0), weight);  // �������� �ʷϻ� ���� ����
    }
    
    // phong ���� ���
    float NL = clamp(dot(input.normal, lightDirection), 0, 1);  // ������ ���� ������ ����
    float3 R = normalize(2.0 * dot(input.normal, lightDirection) * input.normal - lightDirection);  // �ݻ� ���� ���
    float RV = clamp(dot(R, viewDir), 0, 1);  // �� ����� �ݻ� ������ ����
    
    // ���� ���� ���
    float3 col = 0.2f * objColor + 0.7f * objColor * diffuseColor.xyz * NL + 0.5f * pow(RV, 16); //0.8f + 0.6f 
    col = clamp(col, 0, 1);
    return float4(col, 1);
   
    float3 lightDir;
    float3 lightDir2; //����2 ����
    float lightIntensity;
    float lightIntensity2; //����2 ���
    float4 color;

    //Phong ���� ���� ���� �߰���
    float3 viewDir; // �� ���� �߰�
    float3 reflectVec; // �ݻ� ���� �߰�
    float3 halfVec; // Blinn-Phong�� ���� ����
    float phongSpecular; // Phong ���� ����ŧ��
    float blinnSpecular; // Blinn-Phong ���� ����ŧ��
    
    return float4(1, 1, 0, 1);



	// Invert the light direction for calculations.
    lightDir = -lightDirection;
    lightDir = normalize(lightDir);

    //����2 ����
    lightDir2 = -lightDirection2;
    lightDir2 = normalize(lightDir2);

    // Phong������ ���� �� ���� ���
    viewDir = normalize(-input.position.xyz);

    // Diffuse ���
    lightIntensity = saturate(dot(input.normal, lightDir));
    lightIntensity2 = saturate(dot(input.normal, lightDir2));

    // Phong ����ŧ�� ���
    reflectVec = reflect(-lightDir, input.normal);
    phongSpecular = pow(saturate(dot(reflectVec, viewDir)), 32); // n �� ���� ����

    // Blinn-Phong ����ŧ�� ���
    halfVec = normalize(lightDir + viewDir);
    blinnSpecular = pow(saturate(dot(halfVec, input.normal)), 64); // n �� ���� ����

    // ���� ���� ���
    color = diffuseColor * (lightIntensity + lightIntensity2);
    color += diffuseColor * (phongSpecular + blinnSpecular);



    return color;
}