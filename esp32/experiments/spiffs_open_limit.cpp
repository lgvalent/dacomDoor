#include <Arduino.h>
#include <SPIFFS.h>

void setup() {
  Serial.begin(115200);
  if (!SPIFFS.begin(true)) {
    Serial.println("Erro ao montar SPIFFS");
    return;
  }

  File files[10];
  int openCount = 0;
  for (int i = 0; i < 10; ++i) {
    String fname = "/testfile_" + String(i) + ".txt";
    files[i] = SPIFFS.open(fname, FILE_WRITE);
    if (!files[i]) {
      Serial.printf("Falha ao abrir arquivo %d. Limite atingido?\n", i);
      break;
    } else {
      Serial.printf("Arquivo %d aberto com sucesso.\n", i);
      openCount++;
    }
  }
  Serial.printf("Total de arquivos abertos simultaneamente: %d\n", openCount);

  // Fechar todos
  for (int i = 0; i < openCount; ++i) {
    files[i].close();
  }
}

void loop() {
  // nada
}
