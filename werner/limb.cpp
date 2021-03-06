#include "limb.h"


int getMaxIncl(int *array, int length){
   /* searches for pixel with maximum increase in brightness */
   int i , ii, iii, sum;
   int sl=6;
   float *array2 = new float[length]; // R: illegal in proper C++
   float *array3 = new float[length-1];
   float *array4 = new float[length-1];
   for (i=0;i<length;i++){ /*smooth (sl*2+1 px)*/
      sum = 0;
      iii = 0;
      for (ii=-sl;ii<=sl;ii++){
         if ((i+ii>=0)&&(i+ii<length)){
            sum=sum+array[i+ii];
            iii++;}}
      array2[i] = sum/iii;}
   /*derivative*/   
   array3[0] = (-3*array2[0]+4*array2[1]-array2[2])/2.0;
   for (i=1;i<length-2;i++){ 
      array3[i] = (array2[i+1]-array2[i-1])/2.0;}
   array3[length-1] = (3*array2[length-1]-4*array2[length-2]+array2[length-3])/2.0;
   /*derivative*/
   for (i=0;i<length-1;i++){ /*smooth (sl*2+1 px)*/
      sum = 0;
      iii = 0;
      for (ii=-sl;ii<=sl;ii++){
         if ((i+ii>=0)&&(i+ii<length-1)){
            sum=sum+array3[i+ii];
            iii++;}}
      array4[i] = sum/iii;}
   sum = 0;
   ii = 0;
   for (i=0;i<length-1;i++){ /*find max*/
      if (array4[i]>=0){
         if (array4[i]>sum){
            sum = array4[i];
            ii = i;}}
      else {
        if (array4[i]*-1>sum){
            sum = array4[i]*-1;
            ii = i+1;}}}
   delete[] array2;
   delete[] array3;
   delete[] array4;
   return ii;
}

int getMaxIncl2(ushort *array, int length){
   /* searches for pixel with maximum increase in brightness */
   int i , ii, iii, sum;
   int sl=6;
   float *array2 = new float[length]; // R: illegal in proper C++
   float *array3 = new float[length-1];
   float *array4 = new float[length-1];
   for (i=0;i<length;i++){
      sum = 0;
      iii = 0;
      // boxcar average over 2*sl+1 px, from -sl to sl incl.
      for (ii=-sl;ii<=sl;ii++)
      {
         if ((i+ii>=0)&&(i+ii<length)){
            sum = sum + array[i+ii];
            iii++;}
      }
      array2[i] = sum/iii;}
   /*derivative*/
   array3[0] = (-3*array2[0]+4*array2[1]-array2[2])/2.0;
   for (i=1;i<length-2;i++){
      array3[i] = (array2[i+1]-array2[i-1])/2.0;}
   array3[length-1] = (3*array2[length-1]-4*array2[length-2]+array2[length-3])/2.0;
   /*derivative*/
   for (i=0;i<length-1;i++){ /*smooth (sl*2+1 px)*/
      sum = 0;
      iii = 0;
      for (ii=-sl;ii<=sl;ii++){
         if ((i+ii>=0)&&(i+ii<length-1)){
            sum=sum+array3[i+ii];
            iii++;}}
      array4[i] = sum/iii;}
   sum = 0;
   ii = 0;
   for (i=0;i<length-1;i++){ /*find max*/
      if (array4[i]>=0){
         if (array4[i]>sum){
            sum = array4[i];
            ii = i;}}
      else {
        if (array4[i]*-1>sum){
            sum = array4[i]*-1;
            ii = i+1;}}}
   delete[] array2;
   delete[] array3;
   delete[] array4;
   return ii;
}

void FindLimb(int *data, Data *dat, int naxis1, int naxis2, int numDots) {
   /* get data for function getSun

    data is of type int and comes from fits file

    */
   int i, ii;

   int out[numDots*4];
   int outX[numDots*4];
   int outY[numDots*4];
   //Array(1D) which contains part of a row or column
   int *line = new int[naxis1/4]() ;

   /// In (horizontal) rows from 1/4 to 3/4 of y-axis (naxis2),
   /// process x-axis from 0 to 1/4 of naxis1
   for (ii=0; ii<numDots; ii++){
      memcpy(line, &data[naxis1*(naxis2/4+ii*naxis2/(2*numDots))], naxis1/4*sizeof(int));
      out[ii] = getMaxIncl(line, naxis1/4);
      outY[ii] = naxis2/4+ii*naxis2/(2*numDots);
      outX[ii] = out[ii];}
   /// In (horizontal) rows from 1/4 to 3/4 of y-axis (naxis2),
   /// process x-axis from 3/4 to end of naxis1
   for (ii=0; ii<numDots; ii++){
      memcpy(line, &data[naxis1*(naxis2/4+ii*naxis2/(2*numDots)) + 3*naxis1/4], naxis1/4*sizeof(int));
      out[ii+numDots] = getMaxIncl(line, naxis1/4);
      outY[ii+numDots] = naxis2/4+ii*naxis2/(2*numDots);
      outX[ii+numDots] = out[ii+numDots] + 3*naxis1/4;}
   /// In (vertical) columns (ii) from 1/4 to 3/4 of x-axis (naxis1),
   /// process y-axis (i) from 0 to 1/4 of naxis2
   for (ii=0; ii<numDots; ii++){
      for (i=0; i<naxis2/4; i++){
         //memcpy(&line[i], &data[i*naxis1+ii*naxis1/(2*numDots)+naxis1/4], sizeof(int)); // Werner. memcpy on a single value (overkill?)
          line[i] = data[i*naxis1+ii*naxis1/(2*numDots) + naxis1/4];
       }
      out[ii+numDots*2] = getMaxIncl(line,naxis2/4);
      outY[ii+numDots*2] = out[ii+numDots*2];
      outX[ii+numDots*2] = naxis2/4+ii*naxis2/(2*numDots);}
   /// In (vertical) columns (ii) from 1/4 to 3/4 of x-axis (naxis1),
   /// process y-axis (i) from 3/4 to end of naxis2
   for (ii=0; ii<numDots; ii++){
      for (i=0; i<naxis2/4; i++){
         //memcpy(&line[i], &data[naxis1*(i+3*naxis2/4)+ii*naxis1/(2*numDots) + naxis1/4], sizeof(int));
         line[i] = data[naxis1*(i+3*naxis2/4)+ii*naxis1/(2*numDots) + naxis1/4];
      }
      out[ii+numDots*3] = getMaxIncl(line,naxis2/4);
      outY[ii+numDots*3] = out[ii+numDots*3]+3*naxis1/4;
      outX[ii+numDots*3] = naxis2/4+ii*naxis2/(2*numDots);}

   for (i=0;i<numDots*4;i++)
   {
      dat->X[i] = outX[i];
      dat->Y[i] = outY[i];
      cv::Point point(dat->X[i], dat->Y[i]);
      std::cout << "Point = " << point << std::endl;
   }
   //free(line);
   delete[] line;
}

void FindLimb2(ushort *data, Data *dat, int naxis1, int naxis2, int numDots) {
   /* get data for function getSun

    data is of type int and comes from fits file

    */
   int i, ii;
   int* out = new int[numDots*4];
   int* outX = new int[numDots*4];
   int* outY = new int[numDots*4];
   // Array(1D) which contains part of a row or column
   ushort *line = new ushort[naxis1/4];

   for (ii=0;ii<numDots;ii++)
   {
       /// finding limb-x with y = from naxis1/4 to 3/4 naxis1 (shouldn't there be naxis2 instead?)
      memcpy(line, &data[naxis1*(naxis2/4+ii*naxis2/(2*numDots))], naxis1/4*sizeof(ushort));
      //data[(naxis1/4+ii*naxis1/(2*numDots))*naxis1] = 4095;

      out[ii] = getMaxIncl2(line, naxis2/4); // Scan through 1/4th of the axis.
      outY[ii] = naxis2/4 + ii*naxis2/(2*numDots);
      outX[ii] = out[ii];
      std::cout << "Point 1 = [ " << outX[ii] << ", " << outY[ii] << "]" << std::endl;
   }
//   for (ii=0;ii<numDots;ii++)
//   {
//       /// find limb-x with y = from naxis1/4 to 3/4 naxis1
//      memcpy(line, &data[(naxis1/4+ii*naxis1/(2*numDots))*naxis1+3*naxis2/4],naxis2/4*sizeof(ushort));
//      data[(naxis1/4+ii*naxis1/(2*numDots))*naxis1+3*naxis2/4] = 4095;

//      out[ii+numDots] = getMaxIncl2(line, naxis2/4);
//      outY[ii+numDots] = 0;
//      outX[ii+numDots] = 0;
//   }
//   for (ii=0;ii<numDots;ii++)
//   {
//      for (i=0;i<naxis2/4;i++)
//      {
//         memcpy(&line[i], &data[i*naxis1+ii*naxis2/(2*numDots)+naxis2/4],sizeof(ushort));
//      }
//      out[ii+numDots*2] = getMaxIncl2(line, naxis2/4);
//      outY[ii+numDots*2] = 0;
//      outX[ii+numDots*2] = 0;
//   }
//   for (ii=0;ii<numDots;ii++)
//   {
//      for (i=0;i<naxis2/4;i++)
//      {
//         memcpy(&line[i], &data[(i+3*naxis1/4)*naxis1+ii*naxis2/(2*numDots)+naxis2/4],sizeof(ushort));
//      }
//      out[ii+numDots*3] = getMaxIncl2(line, naxis2/4);
//      outY[ii+numDots*3] = 0;
//      outX[ii+numDots*3] = 0;
//   }

   for (i=0;i<numDots;i++)
   {
      dat->X[i] = (reals) outX[i];
      dat->Y[i] = (reals) outY[i];
      cv::Point point(dat->X[i], dat->Y[i]);
      std::cout << "Point 2 = " << point << std::endl;
   }
   //free(line);
   delete[] out;
   delete[] outX;
   delete[] outY;
   delete[] line;

}


void FindLimb3(cv::Mat matImage, Data *dat, int numDots)
{

    /// Need to find out what is the resampling needed based on numDots
    /// We have to extract numDots points within half the original size
    /// i.e, naxis1/2 or naxis2/2 pixels.
    /// So the reduction factor = numDots / (naxis1/2)

//    cv::Mat blurMat;
//    cv::blur(matImage, blurMat, cv::Size(7, 7));
//    cv::Mat gradX, gradY, absGradX, absGradY;

//    //Gradient X
//    int scale = 1;
//    int delta = 0;
//    cv::Sobel(blurMat, gradX, CV_16S, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT);

//    int i, ii;
//    int* out = new int[numDots];
//    int* outX = new int[numDots];
//    int* outY = new int[numDots];

//    int naxis1 = matImage.cols;
//    int naxis2 = matImage.rows;

//    for (ii = 0; ii < numDots; ii++)
//    {
//        /// Extract the row at given Y
//        int Y = naxis2/4 + ii*naxis2/(2*numDots);
//        //cv::Mat matRow = blurMat.row(rowY);
//        /// Extract the first part of the row, over naxis1/4 x-elements.
//        //cv::Mat matRow2 = matRow.colRange(0, naxis1/4 -1);
//        /// Although we can do this with cv::Rect in one line or with range or slice?
//        //cv::Mat matSlice(blurMat, cv::Rect(0, Y, naxis1/4, 1));
//        cv::Mat matSlice = blurMat(cv::Range(Y,Y), cv::Range(0, naxis1/4));
//        // Now we only need to get the maximum

//    }





}


Circle getSun(int *data,int naxis1,int naxis2, int numDots){
   /* calculate sun center, sun radius and RMSE of the radius 
      returns: sun.a (X-pos), sun.b (Y-pos), sun.r (radius), sun.s (RMSE) 
      iterate in order to remove bad pixels (spots, clouds)

      data is of type int and comes from fits file

      */
   Data dat = Data(numDots*4);
   Data dat2 = Data(numDots*4);
   Circle sun;
   int i, ii,n;
   float f;
   n = numDots;
   //FindLimb(data,&dat,naxis1,naxis2);
   FindLimb(data,&dat,naxis1,naxis2, n);
   sun = CircleFitByTaubin(dat);
   if ((sun.s > 2.0) && !isnan(sun.s) && (n > 5)) { /* if error too high, points which don't fit are removed */
      i = 0;
      for (ii=0;ii<dat.n;ii++){
         f = sqrt(powf(dat.X[ii]-sun.a,2)+powf(dat.Y[ii]-sun.b,2));
         if (f<(sun.r+30.0) && f>(sun.r-30.0)) {
            dat2.X[ii-i] = dat.X[ii];
            dat2.Y[ii-i] = dat.Y[ii];
            }
         else {i++;}}
      dat2.n = dat2.n - i;
      dat.n = dat.n - i;
      n = dat.n;
      if (n > 5)
         {
         sun = CircleFitByTaubin(dat2);}
      }
   if ((sun.s > 2.0) && !isnan(sun.s) && (n > 5)) { /* if error too high, points which don't fit are removed */
      i = 0;
      for (ii=0;ii<dat2.n;ii++){
         f = sqrt(powf(dat2.X[ii]-sun.a,2)+powf(dat2.Y[ii]-sun.b,2));
         if (f<sun.r+10.0 && f>sun.r-10.0) {
            dat.X[ii-i] = dat2.X[ii];
            dat.Y[ii-i] = dat2.Y[ii];}
         else {i++;}}
      dat2.n = dat2.n - i;
      dat.n = dat.n - i;
      n = dat.n;
      if (n > 5)
         {sun = CircleFitByTaubin(dat);}
      }
   if ((sun.s > 2.0) && !isnan(sun.s) && (n > 5)) { /* if error too high, points which don't fit are removed */
      i = 0;
      for (ii=0;ii<dat.n;ii++){
         f = sqrt(powf(dat.X[ii]-sun.a,2)+powf(dat.Y[ii]-sun.b,2));
         if (f<sun.r+5.0 && f>sun.r-5.0) {
            dat2.X[ii-i] = dat.X[ii];
            dat2.Y[ii-i] = dat.Y[ii];}
         else {i++;}}
      dat2.n = dat2.n - i;
      dat.n = dat.n - i;
      n = dat.n;
      if (n > 5)
         {sun = CircleFitByTaubin(dat2);}
      }
   if ((sun.s > 2.0) && !isnan(sun.s) && (n > 15)) { /* if error too high, points which don't fit are removed */
      i = 0;
      for (ii=0;ii<dat2.n;ii++){
         f = sqrt(powf(dat2.X[ii]-sun.a,2)+powf(dat2.Y[ii]-sun.b,2));
         if (f<sun.r+2.0 && f>sun.r-2.0) {
            dat.X[ii-i] = dat2.X[ii];
            dat.Y[ii-i] = dat2.Y[ii];}
         else {i++;}}
      dat2.n = dat2.n - i;
      dat.n = dat.n - i;
      n = dat.n;
      if (n > 5)
         {sun = CircleFitByTaubin(dat);}
      }
   sun.a +=1;
   sun.b +=1;
   sun.r +=1;
   if (isnan(sun.s)){sun.s= 9.99;sun.a= 0;sun.b=0;sun.r=0;}
   return sun;
}



