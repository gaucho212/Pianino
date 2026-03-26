#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords; // NOWE: Odbieramy UV

uniform vec3 viewPos;
uniform sampler2D tex; // NOWE: Zmienna reprezentująca nasz obrazek

uniform vec3 dirLightDir;
uniform vec3 dirLightColor;
uniform vec3 pointLightPos;
uniform vec3 pointLightColor;

void main() {
    // Odczytujemy kolor bazowy piksela prosto z obrazka png!
    vec3 texColor = texture(tex, TexCoords).rgb;

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // --- Światło Kierunkowe ---
    vec3 lightDir1 = normalize(-dirLightDir);
    float ambientStrength = 0.15;
    vec3 ambient1 = ambientStrength * dirLightColor;
    
    float diff1 = max(dot(norm, lightDir1), 0.0);
    vec3 diffuse1 = diff1 * dirLightColor;
    
    vec3 halfwayDir1 = normalize(lightDir1 + viewDir);
    float spec1 = pow(max(dot(norm, halfwayDir1), 0.0), 32.0);
    vec3 specular1 = 0.3 * spec1 * dirLightColor;

    vec3 resultDirLight = ambient1 + diffuse1 + specular1;

    // --- Światło Punktowe ---
    vec3 lightDir2 = normalize(pointLightPos - FragPos);
    float diff2 = max(dot(norm, lightDir2), 0.0);
    vec3 diffuse2 = diff2 * pointLightColor;
    
    vec3 halfwayDir2 = normalize(lightDir2 + viewDir);
    float spec2 = pow(max(dot(norm, halfwayDir2), 0.0), 32.0);
    vec3 specular2 = 0.8 * spec2 * pointLightColor;
    
    float distance = length(pointLightPos - FragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));
    vec3 resultPointLight = (diffuse2 + specular2) * attenuation;

    // --- FINALNY WYNIK ---
    // Mnożymy sumę światła przez kolor wyciągnięty z TEKSTURY, a nie stały wektor!
    vec3 finalResult = (resultDirLight + resultPointLight) * texColor;
    
    FragColor = vec4(finalResult, 1.0);
}