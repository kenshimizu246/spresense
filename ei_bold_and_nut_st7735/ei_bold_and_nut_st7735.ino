

#include <Camera.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

#include <bolt_and_nut_inferencing.h>
#include "edge-impulse-sdk/dsp/image/image.hpp"

#define EI_CAMERA_RAW_FRAME_BUFFER_COLS           CAM_IMGSIZE_QQVGA_H
#define EI_CAMERA_RAW_FRAME_BUFFER_ROWS           CAM_IMGSIZE_QQVGA_V
#define EI_CAMERA_RAW_FRAME_BYTE_SIZE             2

#define TFT_CS        24
#define TFT_RST       18 //27 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC        26 //18

// #define TFT_CS        24
// #define TFT_RST       27 // Or set to -1 and connect to Arduino RESET pin
// #define TFT_DC        18

const int imgBufferSize = (320 * 240 * 2) + 4;

static unsigned char imgBuffer[imgBufferSize + 1];

static bool debug_nn = false; // Set this to true to see e.g. features generated from the raw signal
uint8_t *snapshot_buf; //points to the output of the capture

Adafruit_ST7735 tft = Adafruit_ST7735(&SPI5, TFT_CS, TFT_DC, TFT_RST);

void CamCB(CamImage img) {
  int8_t sndid = 100;

  // MPLog("CamCB: Start\n");
  if (img.isAvailable()) {
    //MPLog("CamCB: Available\n");
    CamImage imgTmp(img), imgSend;
    CamErr e;

    img.convertPixFormat(CAM_IMAGE_PIX_FMT_RGB565);

    e = imgTmp.convertPixFormat(CAM_IMAGE_PIX_FMT_RGB565);
    if(e != CAM_ERR_SUCCESS){
        //MPLog("CamErr: convertPixFormat CAM_IMAGE_PIX_FMT_YUV422 %d\n", e);
    }

  } else {
    //MPLog("Image is not available!\n");
  }
}

static int ei_camera_get_data(size_t offset, size_t length, float *out_ptr)
{
    // we already have a RGB888 buffer, so recalculate offset into pixel index
    size_t pixel_ix = offset * 3;
    size_t pixels_left = length;
    size_t out_ptr_ix = 0;

//     while (pixels_left != 0) {
//         out_ptr[out_ptr_ix] = (snapshot_buf[pixel_ix] << 16) + (snapshot_buf[pixel_ix + 1] << 8) + snapshot_buf[pixel_ix + 2];

//         // go to the next pixel
//         out_ptr_ix++;
//         pixel_ix+=3;
//         pixels_left--;
//     }
    // and done!
    return 0;
}

void inferencing(){
  ei::signal_t signal;
  signal.total_length = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
  signal.get_data = &ei_camera_get_data;

//   ei_impulse_result_t result = { 0 };

//   EI_IMPULSE_ERROR err = run_classifier(&signal, &result, debug_nn);
//   if (err != EI_IMPULSE_OK) {
//       ei_printf("ERR: Failed to run classifier (%d)\n", err);
//       return;
//   }
  
//   ei_printf("Predictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms.): \n",
//             result.timing.dsp, result.timing.classification, result.timing.anomaly);

//   uint8_t tline = 0;
//   bool bb_found = result.bounding_boxes[0].value > 0;
//   for (size_t ix = 0; ix < result.bounding_boxes_count; ix++) {
//       auto bb = result.bounding_boxes[ix];
//       if (bb.value == 0) {
//           continue;
//       }
//       ei_printf("    %s (%f) [ x: %u, y: %u, width: %u, height: %u ]\n", bb.label, bb.value, bb.x, bb.y, bb.width, bb.height);
//       char text[24];
//       sprintf(text, "%s %.02f", bb.label, bb.value);
//       tft.setCursor(96,tline);
//       tft.print(text);
//       tft.drawCircle(bb.x, bb.y, 5, ST77XX_YELLOW);
//       tline += 10;
//   }
//   if (!bb_found) {
//       ei_printf("    No objects found\n");
//   }

  Serial.println("Photo saved to file");
}

void setup(void) {
  Serial.begin(115200);

  tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab
  tft.setRotation(1);

  Serial.println(F("Initialized"));
;
  uint16_t time = millis();
  tft.fillScreen(ST77XX_BLACK);
  time = millis() - time;

  Serial.println(time, DEC);
  delay(1000);

  // large block of text
  tft.fillScreen(ST77XX_BLACK);
  //testdrawtext("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", ST77XX_WHITE);
  delay(1000);

  theCamera.begin();
  theCamera.setStillPictureImageFormat(
     CAM_IMGSIZE_QQVGA_H,
     CAM_IMGSIZE_QQVGA_V,
     CAM_IMAGE_PIX_FMT_RGB565);

  Serial.println("done");
  delay(1000);
}

void loop() {
  Serial.println(F("Start loop..."));
  delay(3000);

  CamImage img = theCamera.takePicture();
  Serial.println(F("Took picture..."));

  CamImage imgTmp(img);
  CamErr e;

  imgTmp.convertPixFormat(CAM_IMAGE_PIX_FMT_RGB565);

  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);
  tft.drawRGBBitmap(0, 0, (uint16_t *)img.getImgBuff(), CAM_IMGSIZE_QQVGA_H, CAM_IMGSIZE_QQVGA_V);

  // snapshot_buf = (uint8_t*)malloc(EI_CAMERA_RAW_FRAME_BUFFER_COLS * EI_CAMERA_RAW_FRAME_BUFFER_ROWS * EI_CAMERA_RAW_FRAME_BYTE_SIZE);
  // if(snapshot_buf == nullptr) {
  //   ei_printf("ERR: Failed to allocate snapshot buffer!\n");
  //   return;
  // }

  // //bool converted = fmt2rgb888(img.getImgBuff(), img.getImgBuffSize(), PIXFORMAT_JPEG, snapshot_buf);


  // free(snapshot_buf);

  Serial.println(F("Displayed picture..."));
}

