// Vertex shader
float4 vShader(float3 pos : KL_Position) : SV_Position {
    return float4(pos, 1.0f);
}

// Pixel shader
cbuffer PS_CB : register(b0) {
    float2 frameSize;
    float2 zoom;
    float2 position;
    float2 startPosition;
}

float2 ComplexSqr(float2 a) {
    return float2(a.x * a.x - a.y * a.y, 2.0f * a.x * a.y);
}

float4 pShader(float4 screen : SV_Position) : SV_Target {
    float4 pixel = float4(1.0f, 1.0f, 1.0f, 1.0f);
	
    float2 uv = (screen.xy / frameSize * 2.0f - 1.0f) * float2(frameSize.x / frameSize.y, 1.0f);
    uv /= zoom.x;
    uv += position;
	
    float n = 64.0f;
    float2 num = startPosition.x + uv;
    for (int i = 0; i < n; i++) {
        if (length(num) > 2.0f) {
            pixel *= (i / n);
            break;
        }
		
        num = ComplexSqr(num) + uv;
    }

    return pixel;
}
