struct ImageHeader {
   uint32_t byteLength;
   uint32_t pixelSize;
   uint32_t pixelLength;
   uint32_t width;
   uint32_t height;
   void print() {
      Serial.print("Image(byteLength, pixelSize, pixelLength, width, height): Image(");
      Serial.print(byteLength);
      Serial.print(",");
      Serial.print(pixelSize);
      Serial.print(",");
      Serial.print(pixelLength);
      Serial.print(",");
      Serial.print(width);
      Serial.print(",");
      Serial.print(height);
      Serial.println(")");
   }
};
const uint32_t chunkSize = 1 << 4;
const uint32_t wireClockSpeed = 1000000;