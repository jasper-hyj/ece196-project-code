#include "src/web_server/ESPWebController.h"

ESPWebController controller;

void setup() {
  	Serial.begin(115200);
  	controller.begin();
}

void loop() {
    controller.update();
    const auto& directions = controller.getDirectionMap();

    Serial.print("Direction states: ");
    for (const auto& pair : directions) {
      Serial.printf("%s=%s ", toString(pair.first), pair.second ? "true" : "false");
    }
    Serial.println(); // Finish the line
}
