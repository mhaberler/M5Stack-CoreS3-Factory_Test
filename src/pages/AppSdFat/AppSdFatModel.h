#ifndef __AppSdFat_MODEL_H
#define __AppSdFat_MODEL_H

#include "lvgl.h"
#include "M5Unified.h"
#include "config.h"

// TF Detect AW9523 P0.4

namespace Page {

class AppSdFatModel {
   public:
   bool SDInit();
   void SDDeinit();
   bool GetInitFlag();
   void ClearInitFlag();
   bool IsSDCardExist();
   private:
      bool init_flag = false;
};

}  // namespace Page

#endif
