#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

// --- ZMIENNE Z C++ ---
uniform vec3 viewPos; // Pozycja naszej kamery (potrzebna do refleksów)
uniform vec3 objectColor; // Kolor pianina (zastąpimy teksturą na Lab 11)

// Światło 1: Kierunkowe (np. chłodne światło zza okna)
uniform vec3 dirLightDir;
uniform vec3 dirLightColor;

// Światło 2: Punktowe (np. ciepła lampka na pianinie)
uniform vec3 pointLightPos;
uniform vec3 pointLightColor;

void main() {
    // 1. Przygotowanie wektorów (muszą mieć długość 1)
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // ==========================================
    // ŚWIATŁO 1: KIERUNKOWE (Directional Light)
    // ==========================================
    vec3 lightDir1 = normalize(-dirLightDir);
    
    // Ambient (Otoczenie)
    float ambientStrength = 0.15;
    vec3 ambient1 = ambientStrength * dirLightColor;
    
    // Diffuse (Rozproszenie)
    float diff1 = max(dot(norm, lightDir1), 0.0);
    vec3 diffuse1 = diff1 * dirLightColor;
    
    // Specular (Odbicie - model Blinna-Phonga)
    vec3 halfwayDir1 = normalize(lightDir1 + viewDir);
    float spec1 = pow(max(dot(norm, halfwayDir1), 0.0), 32.0); // 32.0 to "połysk" materiału
    vec3 specular1 = 0.3 * spec1 * dirLightColor;

    vec3 resultDirLight = ambient1 + diffuse1 + specular1;

    // ==========================================
    // ŚWIATŁO 2: PUNKTOWE (Point Light)
    // ==========================================
    vec3 lightDir2 = normalize(pointLightPos - FragPos);
    
    // Diffuse
    float diff2 = max(dot(norm, lightDir2), 0.0);
    vec3 diffuse2 = diff2 * pointLightColor;
    
    // Specular
    vec3 halfwayDir2 = normalize(lightDir2 + viewDir);
    float spec2 = pow(max(dot(norm, halfwayDir2), 0.0), 32.0);
    vec3 specular2 = 0.8 * spec2 * pointLightColor;
    
    // Tłumienie (Attenuation) - światło punktowe słabnie z kwadratem odległości!
    float distance = length(pointLightPos - FragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));
    
    vec3 resultPointLight = (diffuse2 + specular2) * attenuation;

    // ==========================================
    // WYNIK KOŃCOWY
    // ==========================================
    // Sumujemy światła i mnożymy przez bazowy kolor obiektu
    vec3 finalResult = (resultDirLight + resultPointLight) * objectColor;
    
    FragColor = vec4(finalResult, 1.0);
}