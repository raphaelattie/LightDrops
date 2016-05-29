#include "MyFitsImage.h"

#include <cfitsio/fitsio.h>
#include <cmath>
#include <algorithm>

#include <QtDebug>

// opencv
#include <opencv2/world.hpp>


using namespace cv;

MyFitsImage::MyFitsImage(QString filePath) :
hduType(0), naxis1(0), naxis2(0), nPixels(0), nKeys(0), bscale(1), expTime(0), bzero(0), bayer(false), bitpix(0)
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
        printerror(status);
	
    //printHDUType(hduType);

	// If ZCMPTYPE does not exist, data are uncompressed -> Read NAXIS
	// else, data are compressed, read ZNAXIS. 
    status = 0;
    if (fits_read_key(fptr, TSTRING, "ZCMPTYPE", keyString, NULL, &status))
	{   
        status = 0;
		/* read the NAXIS1 and NAXIS2 keyword to get image size */
        if (fits_read_keys_lng(fptr, "NAXIS", 1, 2, naxes, &nfound, &status))
		{
            printerror(status);
		}

        fits_read_key(fptr, TINT, "NAXIS1", &naxis1, NULL, &status);
        fits_read_key(fptr, TINT, "NAXIS2", &naxis2, NULL, &status);

        if (fits_read_key(fptr, TINT, "BITPIX", &bitpix, NULL, &status))
        {
            printerror(status);
        }

	}
	else
	{
        /* read the NAXIS1 and NAXIS2 keyword to get image size */
        if (fits_read_keys_lng(fptr, "ZNAXIS", 1, 2, naxes, &nfound, &status))
		{
            printerror(status);
		}
        if (fits_read_key(fptr, TINT, "ZBITPIX", &bitpix, NULL, &status))
        {
            printerror(status);
        }
	}

    if (fits_read_key(fptr, TFLOAT, "BSCALE", &bscale, NULL, &status))
    {
        status = 0;
    }

    if (fits_read_key(fptr, TINT, "BZERO", &bzero, NULL, &status))
    {
        status = 0;
    }


    if (fits_read_key(fptr, TLOGICAL, "BAYER", &bayer, NULL, &status))
    {
        status = 0;
    }

    if (fits_read_key(fptr, TFLOAT, "EXPTIME", &expTime, NULL, &status))
    {
        status = 0;
    }


//    naxis1 = naxes[0];
//    naxis2 = naxes[1];
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

    if (bitpix == USHORT_IMG)
    {
        image1D_ushort = new ushort[nPixels]();
        if (fits_read_img(fptr, TUSHORT, firstPixel, nPixels, &nullval, image1D_ushort, &anynul, &status))
            printerror(status);

        matFits = Mat(naxis2, naxis1, CV_16U, image1D_ushort);

        //delete[] image1D;
    }
    else if (bitpix == SHORT_IMG && bzero == 0)
    {
        image1D_shortint = new short int[nPixels]();

        if (fits_read_img(fptr, TSHORT, firstPixel, nPixels, &nullval, image1D_shortint, &anynul, &status))
            printerror(status);

        matFits = Mat(naxis2, naxis1, CV_16S, image1D_shortint);
//        qDebug() << "matFits.at<short int>.at(0,0) = " << matFits.at<short int>(0,0);
//        qDebug() << "matFits.at<short int>.at(1,1) = " << matFits.at<short int>(1,1);

//        qDebug() << "matFits.at<short int>.at(2046,1000) = " << matFits.at<short int>(0,1000);
//        qDebug() << "matFits.at<short int>.at(2047,1000) = " << matFits.at<short int>(1,1000);

//        long startpix[2];
//        startpix[0] = 1;
//        startpix[1] = 1;
//        int* image1D_int; // Declare a pointer to an int pointer. In the header.
//        image1D_int = (int *) malloc(sizeof(int) * naxis1 * naxis2); // Use *data to derefence, to store the value (an adress) where this allocation to

//        if (fits_read_pix(fptr, TINT, firstPixels, naxis1 * naxis2, 0, *data, NULL, &status))
//            printerror(status);

//        status = 0;
//        if(fits_read_pix(fptr, TINT, startpix, naxis1 * naxis2, 0, image1D_int, NULL, &status))
//            printerror(status);

        //matFits = Mat(naxis2, naxis1, CV_16S, image1D_shortint);
        //matFits = Mat(naxis2, naxis1, CV_32S, *image1D_int);

        //delete[] image1D;
    }
    else if (bitpix == SHORT_IMG && bzero == 32768)
    {
        image1D_ushort = new ushort[nPixels]();
        //ushort image1D(nPixels);

        if (fits_read_img(fptr, TUSHORT, firstPixel, nPixels, &nullval, image1D_ushort, &anynul, &status))
            printerror(status);

        matFits = Mat(naxis2, naxis1, CV_16U, image1D_ushort);

        //delete[] image1D;
    }
    else if (bitpix == SHORT_IMG  || bitpix == FLOAT_IMG || bitpix == LONG_IMG)
    {
        image1D_float = new float[nPixels]();
        if (fits_read_img(fptr, TFLOAT, firstPixel, nPixels, &nullval, image1D_float, &anynul, &status))
            printerror(status);
        matFits = Mat(naxis2, naxis1, CV_32F, image1D_float);

        //delete[] image1D;
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

    //matFits.convertTo(matFits, CV_32F);

    if (matFits.type() != CV_32F)
    {
        matFits.convertTo(matFits, CV_16U);
    }

}

MyFitsImage::~MyFitsImage()
{
    delete[] image1D_ushort;
    delete[] image1D_shortint;
    delete[] image1D_float;
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
