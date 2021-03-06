#include "MyFitsImage.h"

#include <fitsio.h>
#include <cmath>
#include <algorithm>

#include <QtDebug>

// opencv
#include <opencv2/world.hpp>


using namespace cv;

MyFitsImage::MyFitsImage(QString filePath) :
hduType(0), naxis1(0), naxis2(0), nPixels(0), nKeys(0), bscale(1), bzero(0), expTime(0), bayer(false), bitpix(0)
,image1D_ushort(NULL), image1D_float(NULL), image1D_shortint(NULL)
{

    fitsfile *fptr;
    int morekeys=0, nfound, anynul;
    long naxes[2], firstPixel, ii;
    double nullval;
    char comment[FLEN_COMMENT], keyString[FLEN_VALUE], card[FLEN_CARD];
    char keyword[FLEN_KEYWORD], keyValue[FLEN_VALUE];
    int status = 0;

	std::string filePathStr(filePath.toStdString());

    //qDebug() << "fits_is_reentrant =" << fits_is_reentrant();

//	if (fits_open_data(&fptr, filePathStr.c_str(), READONLY, status))
//	{
//        std::cout << "MyFitsImage:: Error opening FITS file" << std::endl;
//		printerror(*status);
//	}

    if (fits_open_file(&fptr, filePathStr.c_str(), READONLY, &status))
    {
        std::cout << "MyFitsImage:: Error opening FITS file" << std::endl;
        printerror(status);
    }
		
    status = 0;
    if (fits_get_hdu_type(fptr, &hduType, &status))
    {
        qDebug() << "Error at fits_get_hdu_type";
        printerror(status);
    }
    status = 0;

    printHDUType(hduType);
    int nhdus;
    if(fits_get_num_hdus(fptr, &nhdus, &status))
    {
        qDebug() << "Error at fits_get_num_hdus";
        printerror(status);
    }



    std::cout << "nhdus = " << nhdus << std::endl;
    if (nhdus > 1)
    {
        fits_movabs_hdu(fptr, 2, &hduType, &status);
    }

    int isCompressed = 0;
    isCompressed = fits_is_compressed_image(fptr, &status);

    fitsfile *outfptr;


    if (status)
    {
        qDebug() << "Error at fits_movabs_hdu ";
        printerror(status);
    }

	// If ZCMPTYPE does not exist, data are uncompressed -> Read NAXIS
	// else, data are compressed, read ZNAXIS. 
    status = 0;
    if (fits_read_key(fptr, TSTRING, "ZCMPTYPE", keyString, NULL, &status))
	{   
        status = 0;
		/* read the NAXIS1 and NAXIS2 keyword to get image size */
        if (fits_read_keys_lng(fptr, "NAXIS", 1, 2, naxes, &nfound, &status))
		{
            qDebug() << "Error reading NAXIS 1 , 2";
            printerror(status);
		}

        fits_read_key(fptr, TINT, "NAXIS1", &naxis1, NULL, &status);
        fits_read_key(fptr, TINT, "NAXIS2", &naxis2, NULL, &status);

        if (fits_read_key(fptr, TINT, "BITPIX", &bitpix, NULL, &status))
        {
            qDebug() << "Error reading BITPIX";
            printerror(status);
        }

	}
	else
	{
        /* read the ZNAXIS1 and ZNAXIS2 keyword to get image size */
        if (fits_read_keys_lng(fptr, "ZNAXIS", 1, 2, naxes, &nfound, &status))
		{
            qDebug() << "Error reading ZNAXIS";
            printerror(status);
		}
        naxis1 = naxes[0];
        naxis2 = naxes[1];
        if (fits_read_key(fptr, TINT, "ZBITPIX", &bitpix, NULL, &status))
        {
            qDebug() << "Error reading ZBITPIX";
            printerror(status);
        }
	}

    if (fits_read_key(fptr, TFLOAT, "BSCALE", &bscale, NULL, &status)) { status = 0; }
    if (fits_read_key(fptr, TINT, "BZERO", &bzero, NULL, &status)) { status = 0; }
    if (fits_read_key(fptr, TLOGICAL, "BAYER", &bayer, NULL, &status)) { status = 0; }
    if (fits_read_key(fptr, TFLOAT, "EXPTIME", &expTime, NULL, &status)) { status = 0; }

	nPixels = naxis1 * naxis2; // Total number of pixels in the image

    fits_get_hdrspace(fptr, &nKeys, &morekeys, &status);

     for (ii=1; ii<=nKeys; ii++)
     {
         fits_read_keyn(fptr, ii, keyword, keyValue, comment, &status);
         //qDebug() << "keyn Keyword"<<ii<<":" << keyword << keyValue << comment;
         keyNames<< QString(keyword);
         QString tempValue(keyValue);
         tempValue = tempValue.simplified();
         tempValue.replace(" ", "");
         tempValue.replace("'", "");
         keyValues<< tempValue;
         keyComments<< QString(comment);
     }


    // anynul is set to 1 if there any undefined pixel value.
    anynul = 0;
    // undefined (e.g., blank) pixels are set to NaN.
    nullval = NAN;

    firstPixel	= 1;

    // Use C++ array initialization (https://stackoverflow.com/questions/2204176/how-to-initialise-memory-with-new-operator-in-c)
    if (bitpix == USHORT_IMG)
    {
        image1D_ushort = new ushort[nPixels]();
        if (fits_read_img(fptr, TUSHORT, firstPixel, nPixels, &nullval, image1D_ushort, &anynul, &status))
            printerror(status);

        matFits = Mat(naxis2, naxis1, CV_16U, image1D_ushort);
        qDebug("MyFitsImage:: USHORT_IMG CV_16U");

    }
    else if (bitpix == SHORT_IMG && bzero == 0)
    { /// e.g. USET's Retiga Cameras
        image1D_shortint = new short int[nPixels]();

        if (fits_read_img(fptr, TSHORT, firstPixel, nPixels, &nullval, image1D_shortint, &anynul, &status))
            printerror(status);

        matFits = Mat(naxis2, naxis1, CV_16S, image1D_shortint);

    }
    else if (bitpix == SHORT_IMG && bzero == 32768)
    {
        image1D_ushort = new ushort[nPixels]();

        if (fits_read_img(fptr, TUSHORT, firstPixel, nPixels, &nullval, image1D_ushort, &anynul, &status))
            printerror(status);


        matFits = Mat(naxis2, naxis1, CV_16U, image1D_ushort);
        qDebug("MyFitsImage:: SHORT_IMG CV_16U");

    }
    else if (bitpix == SHORT_IMG  || bitpix == FLOAT_IMG || bitpix == LONG_IMG || bitpix == DOUBLE_IMG)
    {
        image1D_float = new float[nPixels]();
        if (fits_read_img(fptr, TFLOAT, firstPixel, nPixels, &nullval, image1D_float, &anynul, &status))
            printerror(status);
        matFits = Mat(naxis2, naxis1, CV_32F, image1D_float);

        qDebug("MyFitsImage:: 4 SHORT_IMG CV_32F");

    }
    else
    {
        if (fits_close_file(fptr, &status))
            printerror(status);
        qDebug("MyFitsImage:: data type not supported, returning.");
        return;
    }

    if (fits_close_file(fptr, &status))
        printerror(status);

    if (matFits.type() != CV_32F)
    {
        matFits.convertTo(matFits, CV_16U);
    }

//    float *image = new float[nPixels]();
//    for (uint i=0; i < nPixels; i++)
//    {
//        image[i] = (float) image1D_float[i];
//    }

//    int nPts = 2;
//    float *xout = new float[nPts]{10.3, 10.2};
//    float *yout = new float[nPts]{20.5, 15.4};
//    float *values = new float[nPts]();

//    bilin_interp(values, image, xout, yout, naxis1, nPts);

//    std::cout<< "naxis1 = " << naxis1 << std::endl;
//    std::cout<< "values[0] = " << values[0] << std::endl;
//    std::cout<< "values[1] = " << values[1] << std::endl;

//    delete[] image;
}

MyFitsImage::~MyFitsImage()
{
    delete[] image1D_ushort;
    delete[] image1D_shortint;
    delete[] image1D_float;
    //delete[] image1D_char;
}

// getters

qint32 MyFitsImage::getNaxis1() const
{
	return naxis1;
}


qint32 MyFitsImage::getNaxis2() const
{
	return naxis2;
}

bool MyFitsImage::isBayer() const
{
    return bayer;
}

float MyFitsImage::getBscale() const
{
    return bscale;
}

float MyFitsImage::getExpTime() const
{
    return expTime;
}

int MyFitsImage::getBzero() const
{
    return bzero;
}

cv::Mat MyFitsImage::getMatFits() const
{
    return matFits;
}

void MyFitsImage::printHDUType(int hduType)
{
	switch (hduType)
	{
    case IMAGE_HDU: std::cout << "HDUType = Primary Array or IMAGE HDU" << std::endl;
		break;
	case ASCII_TBL: std::cout << "HDUType = ASCII table HDU" << std::endl;
		break;
	case BINARY_TBL: std::cout << "HDUType = Binary table HDU" << std::endl;
		break;
	case ANY_HDU: std::cout << "HDUType = matches any type of HDU" << std::endl;
		break;
	default:
		break;
	}
	return;
}

void MyFitsImage::printerror(int status)
{
	if (status)
	{
        fits_report_error(stderr, status); /* print error report */

		exit(status);    /* terminate the program, returning error status */
	}
	return;
}


int MyFitsImage::getNKeys() const
{
    return nKeys;
}

QVector<QString> MyFitsImage::getKeyNames() const
{
    return keyNames;
}

QVector<QString> MyFitsImage::getKeyValues() const
{
    return keyValues;
}

QVector<QString> MyFitsImage::getKeyComments() const
{
    return keyComments;
}

void MyFitsImage::bilin_interp(float *values, float *image, float *xout, float *yout, const int NX, const int npts)
{
        for (int k=0; k<npts; k++){

            const int   x0 = floor(xout[k]);
            const int   y0 = floor(yout[k]);
            // Get the 4 nearest neighbours
            float h00, h01, h10, h11;
            // Left
            h00 = image[y0*NX + x0];
            // Right
            h10 = image[y0*NX + x0 + 1];
            // Bottom?
            h01 = image[(y0+1)*NX + x0];
            // Top?
            h11 = image[(y0+1)*NX + x0 + 1];

            printf("image[%i, %i] = %f\n", x0, y0, h00);

            // Calculate the weights for each pixel

            float fx = xout[k] - x0;
            float fy = yout[k] - y0;
            float fx1 = 1.0f - fx;
            float fy1 = 1.0f - fy;

            float w1 = fx1 * fy1;
            float w2 = fx * fy1;
            float w3 = fx1 * fy;
            float w4 = fx * fy;

            // Calculate the weighted sum of pixels
            values[k] = h00 * w1 + h10 * w2 + h01 * w3 + h11 * w4;
        }

        return;
}
