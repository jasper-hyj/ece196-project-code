#include "ESPWebController.h"

ESPWebController controller;

void setup() {
  Serial.begin(115200);
  controller.begin();
}

void loop() {
  controller.update();
  const auto& directions = controller.getDirectionMap();
}
