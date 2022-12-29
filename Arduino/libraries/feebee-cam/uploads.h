#ifndef FEEBEE_CAM__HANDLE_UPLOADS
#define FEEBEE_CAM__HANdLE_UPLOADS

namespace FeebeeCam {

   bool initializeTimers();
   bool handleUploads(bool updateStatus = true);
   extern bool uploadingReports;

}

#endif