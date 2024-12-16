#version 430 core

in vec3 normal;
in vec3 FragPos;
in vec2 texCoords;
in vec3 tangents;
in vec3 bitangents;

out vec4 FragColor;

layout (location = 3) uniform vec3 camPos;
layout (location = 4) uniform uint type;
layout (location = 5) uniform mat4 lightSpaceMatrix;

layout (binding = 0) uniform sampler2D texSampler;
layout (binding = 1) uniform sampler2D normSampler;
layout (binding = 2) uniform sampler2D specSampler;
layout (binding = 3) uniform sampler2D shadowSampler;

vec3 lightPos = vec3(0.0, 10.0, 20.0);

// �p�⳱�v���
float getShadow(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w; // ��v�Ŷ����y��
    projCoords = projCoords * 0.5 + 0.5; // �ഫ�� 0~1 
    float currentDepth = projCoords.z;

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowSampler, 0); // ����K�Ϫ��j�p

    // �ϥ� 3x3 PCF (Percentage Closer Filtering) �i�業�Ƴ��v�p��
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowSampler, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0; // �������v��
    
    // �p�G Z �W�X�d��A�h���⳱�v
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

// ²���H���ݰʨ�ơA�Ω��ı�u��
float rand(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453);
}

float dither(vec2 uv) {
    return (rand(uv)*2.0-1.0) / 256.0;
}

void main()
{
    // �N FragPos �ഫ��������Ŷ�
    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

    // �]�m�������C��M�j��
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    float ambientStrength = 0.4; // ���ҥ��j��
    vec3 ambient = ambientStrength * lightColor;

    // L (�����I��) �p��Ѽ�
    float l_a = 0.001;
    float l_b = 0.01;
    float l_c = 0.001;
    float d = abs(distance(lightPos, FragPos)); // �����칳�����Z��
    float L = 1 / (l_a + d * l_b + d * d * l_c); // �p��I��]�l

    // �k�V�q�B�z
    vec3 norm = normalize(normal);
    if(type == 5) { // �p�G�O�a���k�u�K�Ϫ�����
        vec3 tangents = normalize(tangents);
        vec3 bitangents = normalize(bitangents);

        // TBN �x�}�B�z�k�u�K��
        mat3 TBN = mat3(tangents, bitangents, norm);
        norm = TBN * ((2 * vec3(texture(normSampler, texCoords))) - 1);
        norm = normalize(norm);
    }

    // �p�⺩�Ϯg��
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0) * L;
    vec3 diffuse = diff * lightColor * 0.5;

    // �p�Ⱚ���Ϯg
    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 viewDir = normalize(camPos - FragPos);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32) * L;

    // �p�⳱�v
    float shadow = getShadow(fragPosLightSpace);

    if(type == 5) { // �a���z������
        vec4 tex = texture(texSampler, texCoords);
        vec3 specular = spec * vec3(texture(specSampler, texCoords)) * 0.5;
        vec3 result = (ambient + (diffuse + specular) * (1.0 - shadow)) * vec3(tex);
        FragColor = vec4(result, tex.w);
    } 
    else { // �¥մѽL�檫�� (�Ҧp�a�O)
        vec3 objectColor = vec3(1.0, 1.0, 1.0); // �w�]�զ�
        // �ѽL���޿�G�ھ� FragPos �� X �M Z �y�ШM�w�C��
        if ((int(floor(FragPos.x * 2.0) + floor(FragPos.z * 2.0)) & 1) == 0) {
            objectColor = vec3(0.4, 0.4, 0.4); // �¦��l
        }
        vec3 specular = spec * lightColor * 0.5;
        vec3 result = (ambient + (diffuse + specular) * (1.0 - shadow)) * objectColor + dither(vec2(FragPos.x, FragPos.z));
        FragColor = vec4(result, 1.0);
    }
}
