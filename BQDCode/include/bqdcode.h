#ifndef BQDCODE_H
#define BQDCODE_H
#include "barcode.h"
#include "qrencode.h"
#include "dmtx.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)
    #define BQDCODE_EXPORT __declspec(dllexport)
#elif defined(__GNUC__)
    #define BQDCODE_EXPORT __attribute__ ((visibility("default")))
#elif defined(__clang__)
    #define BQDCODE_EXPORT __attribute__ ((visibility("default")))
//#elif defined(__INTEL_COMPILER)
    // ICC 编译器特定的代码
#else
    #error "Unsupported system"
#endif
/*
 * Create and destroy barcode structures
 */
BQDCODE_EXPORT struct Barcode_Item *BQDBarcode_Create(char *text);
BQDCODE_EXPORT int                  BQDBarcode_Delete(struct Barcode_Item *bc);

/*
 * Encode and print
 */
BQDCODE_EXPORT int BQDBarcode_Encode(struct Barcode_Item *bc, int flags);
BQDCODE_EXPORT int BQDBarcode_Print(struct Barcode_Item *bc, FILE *f, int flags);

/*
 * Choose the position
 */
BQDCODE_EXPORT int BQDBarcode_Position(struct Barcode_Item *bc, int wid, int hei,
			    int xoff, int yoff, double scalef);

/*
 * Do it all in one step
 */
BQDCODE_EXPORT int BQDBarcode_Encode_and_Print(char *text, FILE *f, int wid, int hei,
				    int xoff, int yoff, int flags);

/*
 * Return current version (integer and string)
 */
BQDCODE_EXPORT int BQDBarcode_Version(char *versionname);



/*
 * dmtx functions
*/

/* dmtxtime.c */
BQDCODE_EXPORT DmtxTime BQDdmtxTimeNow(void);
BQDCODE_EXPORT DmtxTime BQDdmtxTimeAdd(DmtxTime t, long msec);
BQDCODE_EXPORT int BQDdmtxTimeExceeded(DmtxTime timeout);

/* dmtxencode.c */
BQDCODE_EXPORT DmtxEncode *BQDdmtxEncodeCreate(void);
BQDCODE_EXPORT DmtxPassFail BQDdmtxEncodeDestroy(DmtxEncode **enc);
BQDCODE_EXPORT DmtxPassFail BQDdmtxEncodeSetProp(DmtxEncode *enc, int prop, int value);
BQDCODE_EXPORT int BQDdmtxEncodeGetProp(DmtxEncode *enc, int prop);
BQDCODE_EXPORT DmtxPassFail BQDdmtxEncodeDataMatrix(DmtxEncode *enc, int n, unsigned char *s);
BQDCODE_EXPORT DmtxPassFail BQDdmtxEncodeDataMosaic(DmtxEncode *enc, int n, unsigned char *s);

/* dmtxdecode.c */
BQDCODE_EXPORT DmtxDecode *BQDdmtxDecodeCreate(DmtxImage *img, int scale);
BQDCODE_EXPORT DmtxPassFail BQDdmtxDecodeDestroy(DmtxDecode **dec);
BQDCODE_EXPORT DmtxPassFail BQDdmtxDecodeSetProp(DmtxDecode *dec, int prop, int value);
BQDCODE_EXPORT int BQDdmtxDecodeGetProp(DmtxDecode *dec, int prop);
BQDCODE_EXPORT /*@exposed@*/ unsigned char *BQDdmtxDecodeGetCache(DmtxDecode *dec, int x, int y);
BQDCODE_EXPORT DmtxPassFail BQDdmtxDecodeGetPixelValue(DmtxDecode *dec, int x, int y, int channel, /*@out@*/ int *value);
BQDCODE_EXPORT DmtxMessage *BQDdmtxDecodeMatrixRegion(DmtxDecode *dec, DmtxRegion *reg, int fix);
BQDCODE_EXPORT DmtxMessage *BQDdmtxDecodePopulatedArray(int sizeIdx, DmtxMessage *msg, int fix);
BQDCODE_EXPORT DmtxMessage *BQDdmtxDecodeMosaicRegion(DmtxDecode *dec, DmtxRegion *reg, int fix);
BQDCODE_EXPORT unsigned char *BQDdmtxDecodeCreateDiagnostic(DmtxDecode *dec, /*@out@*/ int *totalBytes, /*@out@*/ int *headerBytes, int style);

/* dmtxregion.c */
BQDCODE_EXPORT DmtxRegion *BQDdmtxRegionCreate(DmtxRegion *reg);
BQDCODE_EXPORT DmtxPassFail BQDdmtxRegionDestroy(DmtxRegion **reg);
BQDCODE_EXPORT DmtxRegion *BQDdmtxRegionFindNext(DmtxDecode *dec, DmtxTime *timeout);
BQDCODE_EXPORT DmtxRegion *BQDdmtxRegionScanPixel(DmtxDecode *dec, int x, int y);
BQDCODE_EXPORT DmtxPassFail BQDdmtxRegionUpdateCorners(DmtxDecode *dec, DmtxRegion *reg, DmtxVector2 p00,
      DmtxVector2 p10, DmtxVector2 p11, DmtxVector2 p01);
BQDCODE_EXPORT DmtxPassFail BQDdmtxRegionUpdateXfrms(DmtxDecode *dec, DmtxRegion *reg);

/* dmtxmessage.c */
BQDCODE_EXPORT DmtxMessage *BQDdmtxMessageCreate(int sizeIdx, int symbolFormat);
BQDCODE_EXPORT DmtxPassFail BQDdmtxMessageDestroy(DmtxMessage **msg);

/* dmtximage.c */
BQDCODE_EXPORT DmtxImage *BQDdmtxImageCreate(unsigned char *pxl, int width, int height, int pack);
BQDCODE_EXPORT DmtxPassFail BQDdmtxImageDestroy(DmtxImage **img);
BQDCODE_EXPORT DmtxPassFail BQDdmtxImageSetChannel(DmtxImage *img, int channelStart, int bitsPerChannel);
BQDCODE_EXPORT DmtxPassFail BQDdmtxImageSetProp(DmtxImage *img, int prop, int value);
BQDCODE_EXPORT int BQDdmtxImageGetProp(DmtxImage *img, int prop);
BQDCODE_EXPORT int BQDdmtxImageGetByteOffset(DmtxImage *img, int x, int y);
BQDCODE_EXPORT DmtxPassFail BQDdmtxImageGetPixelValue(DmtxImage *img, int x, int y, int channel, /*@out@*/ int *value);
BQDCODE_EXPORT DmtxPassFail BQDdmtxImageSetPixelValue(DmtxImage *img, int x, int y, int channel, int value);
BQDCODE_EXPORT DmtxBoolean BQDdmtxImageContainsInt(DmtxImage *img, int margin, int x, int y);
BQDCODE_EXPORT DmtxBoolean BQDdmtxImageContainsFloat(DmtxImage *img, double x, double y);

/* dmtxvector2.c */
BQDCODE_EXPORT DmtxVector2 *BQDdmtxVector2AddTo(DmtxVector2 *v1, const DmtxVector2 *v2);
BQDCODE_EXPORT DmtxVector2 *BQDdmtxVector2Add(/*@out@*/ DmtxVector2 *vOut, const DmtxVector2 *v1, const DmtxVector2 *v2);
BQDCODE_EXPORT DmtxVector2 *BQDdmtxVector2SubFrom(DmtxVector2 *v1, const DmtxVector2 *v2);
BQDCODE_EXPORT DmtxVector2 *BQDdmtxVector2Sub(/*@out@*/ DmtxVector2 *vOut, const DmtxVector2 *v1, const DmtxVector2 *v2);
BQDCODE_EXPORT DmtxVector2 *BQDdmtxVector2ScaleBy(DmtxVector2 *v, double s);
BQDCODE_EXPORT DmtxVector2 *BQDdmtxVector2Scale(/*@out@*/ DmtxVector2 *vOut, const DmtxVector2 *v, double s);
BQDCODE_EXPORT double BQDdmtxVector2Cross(const DmtxVector2 *v1, const DmtxVector2 *v2);
BQDCODE_EXPORT double BQDdmtxVector2Norm(DmtxVector2 *v);
BQDCODE_EXPORT double BQDdmtxVector2Dot(const DmtxVector2 *v1, const DmtxVector2 *v2);
BQDCODE_EXPORT double BQDdmtxVector2Mag(const DmtxVector2 *v);
BQDCODE_EXPORT double BQDdmtxDistanceFromRay2(const DmtxRay2 *r, const DmtxVector2 *q);
BQDCODE_EXPORT double BQDdmtxDistanceAlongRay2(const DmtxRay2 *r, const DmtxVector2 *q);
BQDCODE_EXPORT DmtxPassFail BQDdmtxRay2Intersect(/*@out@*/ DmtxVector2 *point, const DmtxRay2 *p0, const DmtxRay2 *p1);
BQDCODE_EXPORT DmtxPassFail BQDdmtxPointAlongRay2(/*@out@*/ DmtxVector2 *point, const DmtxRay2 *r, double t);

/* dmtxmatrix3.c */
BQDCODE_EXPORT void BQDdmtxMatrix3Copy(/*@out@*/ DmtxMatrix3 m0, DmtxMatrix3 m1);
BQDCODE_EXPORT void BQDdmtxMatrix3Identity(/*@out@*/ DmtxMatrix3 m);
BQDCODE_EXPORT void BQDdmtxMatrix3Translate(/*@out@*/ DmtxMatrix3 m, double tx, double ty);
BQDCODE_EXPORT void BQDdmtxMatrix3Rotate(/*@out@*/ DmtxMatrix3 m, double angle);
BQDCODE_EXPORT void BQDdmtxMatrix3Scale(/*@out@*/ DmtxMatrix3 m, double sx, double sy);
BQDCODE_EXPORT void BQDdmtxMatrix3Shear(/*@out@*/ DmtxMatrix3 m, double shx, double shy);
BQDCODE_EXPORT void BQDdmtxMatrix3LineSkewTop(/*@out@*/ DmtxMatrix3 m, double b0, double b1, double sz);
BQDCODE_EXPORT void BQDdmtxMatrix3LineSkewTopInv(/*@out@*/ DmtxMatrix3 m, double b0, double b1, double sz);
BQDCODE_EXPORT void BQDdmtxMatrix3LineSkewSide(/*@out@*/ DmtxMatrix3 m, double b0, double b1, double sz);
BQDCODE_EXPORT void BQDdmtxMatrix3LineSkewSideInv(/*@out@*/ DmtxMatrix3 m, double b0, double b1, double sz);
BQDCODE_EXPORT void BQDdmtxMatrix3Multiply(/*@out@*/ DmtxMatrix3 mOut, DmtxMatrix3 m0, DmtxMatrix3 m1);
BQDCODE_EXPORT void BQDdmtxMatrix3MultiplyBy(DmtxMatrix3 m0, DmtxMatrix3 m1);
BQDCODE_EXPORT int BQDdmtxMatrix3VMultiply(/*@out@*/ DmtxVector2 *vOut, DmtxVector2 *vIn, DmtxMatrix3 m);
BQDCODE_EXPORT int BQDdmtxMatrix3VMultiplyBy(DmtxVector2 *v, DmtxMatrix3 m);
BQDCODE_EXPORT void BQDdmtxMatrix3Print(DmtxMatrix3 m);

/* dmtxsymbol.c */
BQDCODE_EXPORT int BQDdmtxSymbolModuleStatus(DmtxMessage *mapping, int sizeIdx, int row, int col);
BQDCODE_EXPORT int BQDdmtxGetSymbolAttribute(int attribute, int sizeIdx);
BQDCODE_EXPORT int BQDdmtxGetBlockDataSize(int sizeIdx, int blockIdx);
BQDCODE_EXPORT int BQDgetSizeIdxFromSymbolDimension(int rows, int cols);

/* dmtxbytelist.c */
BQDCODE_EXPORT DmtxByteList BQDdmtxByteListBuild(DmtxByte *storage, int capacity);
BQDCODE_EXPORT void BQDdmtxByteListInit(DmtxByteList *list, int length, DmtxByte value, DmtxPassFail *passFail);
BQDCODE_EXPORT void BQDdmtxByteListClear(DmtxByteList *list);
BQDCODE_EXPORT DmtxBoolean BQDdmtxByteListHasCapacity(DmtxByteList *list);
BQDCODE_EXPORT void BQDdmtxByteListCopy(DmtxByteList *dst, const DmtxByteList *src, DmtxPassFail *passFail);
BQDCODE_EXPORT void BQDdmtxByteListPush(DmtxByteList *list, DmtxByte value, DmtxPassFail *passFail);
BQDCODE_EXPORT DmtxByte BQDdmtxByteListPop(DmtxByteList *list, DmtxPassFail *passFail);
BQDCODE_EXPORT void BQDdmtxByteListPrint(DmtxByteList *list, char *prefix);

BQDCODE_EXPORT char *BQDdmtxVersion(void);

/*
 * QREncode functions
*/

/**
 * Instantiate an input data object. The version is set to 0 (auto-select)
 * and the error correction level is set to QR_ECLEVEL_L.
 * @return an input object (initialized). On error, NULL is returned and errno
 *         is set to indicate the error.
 * @throw ENOMEM unable to allocate memory.
 */
BQDCODE_EXPORT QRinput *BQDQRinput_new(void);

/**
 * Instantiate an input data object.
 * @param version version number.
 * @param level Error correction level.
 * @return an input object (initialized). On error, NULL is returned and errno
 *         is set to indicate the error.
 * @throw ENOMEM unable to allocate memory for input objects.
 * @throw EINVAL invalid arguments.
 */
BQDCODE_EXPORT QRinput *BQDQRinput_new2(int version, QRecLevel level);

/**
 * Instantiate an input data object. Object's Micro QR Code flag is set.
 * Unlike with full-sized QR Code, version number must be specified (>0).
 * @param version version number (1--4).
 * @param level Error correction level.
 * @return an input object (initialized). On error, NULL is returned and errno
 *         is set to indicate the error.
 * @throw ENOMEM unable to allocate memory for input objects.
 * @throw EINVAL invalid arguments.
 */
BQDCODE_EXPORT QRinput *BQDQRinput_newMQR(int version, QRecLevel level);

/**
 * Append data to an input object.
 * The data is copied and appended to the input object.
 * @param input input object.
 * @param mode encoding mode.
 * @param size size of data (byte).
 * @param data a pointer to the memory area of the input data.
 * @retval 0 success.
 * @retval -1 an error occurred and errno is set to indeicate the error.
 *            See Execptions for the details.
 * @throw ENOMEM unable to allocate memory.
 * @throw EINVAL input data is invalid.
 *
 */
BQDCODE_EXPORT int BQDQRinput_append(QRinput *input, QRencodeMode mode, int size, const unsigned char *data);

/**
 * Append ECI header.
 * @param input input object.
 * @param ecinum ECI indicator number (0 - 999999)
 * @retval 0 success.
 * @retval -1 an error occurred and errno is set to indeicate the error.
 *            See Execptions for the details.
 * @throw ENOMEM unable to allocate memory.
 * @throw EINVAL input data is invalid.
 *
 */
BQDCODE_EXPORT int BQDQRinput_appendECIheader(QRinput *input, unsigned int ecinum);

/**
 * Get current version.
 * @param input input object.
 * @return current version.
 */
BQDCODE_EXPORT int BQDQRinput_getVersion(QRinput *input);

/**
 * Set version of the QR code that is to be encoded.
 * This function cannot be applied to Micro QR Code.
 * @param input input object.
 * @param version version number (0 = auto)
 * @retval 0 success.
 * @retval -1 invalid argument.
 */
BQDCODE_EXPORT int BQDQRinput_setVersion(QRinput *input, int version);

/**
 * Get current error correction level.
 * @param input input object.
 * @return Current error correcntion level.
 */
BQDCODE_EXPORT QRecLevel BQDQRinput_getErrorCorrectionLevel(QRinput *input);

/**
 * Set error correction level of the QR code that is to be encoded.
 * This function cannot be applied to Micro QR Code.
 * @param input input object.
 * @param level Error correction level.
 * @retval 0 success.
 * @retval -1 invalid argument.
 */
BQDCODE_EXPORT int BQDQRinput_setErrorCorrectionLevel(QRinput *input, QRecLevel level);

/**
 * Set version and error correction level of the QR code at once.
 * This function is recommened for Micro QR Code.
 * @param input input object.
 * @param version version number (0 = auto)
 * @param level Error correction level.
 * @retval 0 success.
 * @retval -1 invalid argument.
 */
BQDCODE_EXPORT int BQDQRinput_setVersionAndErrorCorrectionLevel(QRinput *input, int version, QRecLevel level);

/**
 * Free the input object.
 * All of data chunks in the input object are freed too.
 * @param input input object.
 */
BQDCODE_EXPORT void BQDQRinput_free(QRinput *input);

/**
 * Validate the input data.
 * @param mode encoding mode.
 * @param size size of data (byte).
 * @param data a pointer to the memory area of the input data.
 * @retval 0 success.
 * @retval -1 invalid arguments.
 */
BQDCODE_EXPORT int BQDQRinput_check(QRencodeMode mode, int size, const unsigned char *data);

/**
 * Set of QRinput for structured symbols.
 */
typedef struct _QRinput_Struct QRinput_Struct;

/**
 * Instantiate a set of input data object.
 * @return an instance of QRinput_Struct. On error, NULL is returned and errno
 *         is set to indicate the error.
 * @throw ENOMEM unable to allocate memory.
 */
BQDCODE_EXPORT QRinput_Struct *BQDQRinput_Struct_new(void);

/**
 * Set parity of structured symbols.
 * @param s structured input object.
 * @param parity parity of s.
 */
BQDCODE_EXPORT void BQDQRinput_Struct_setParity(QRinput_Struct *s, unsigned char parity);

/**
 * Append a QRinput object to the set. QRinput created by QRinput_newMQR()
 * will be rejected.
 * @warning never append the same QRinput object twice or more.
 * @param s structured input object.
 * @param input an input object.
 * @retval >0 number of input objects in the structure.
 * @retval -1 an error occurred. See Exceptions for the details.
 * @throw ENOMEM unable to allocate memory.
 * @throw EINVAL invalid arguments.
 */
BQDCODE_EXPORT int BQDQRinput_Struct_appendInput(QRinput_Struct *s, QRinput *input);

/**
 * Free all of QRinput in the set.
 * @param s a structured input object.
 */
BQDCODE_EXPORT void BQDQRinput_Struct_free(QRinput_Struct *s);

/**
 * Split a QRinput to QRinput_Struct. It calculates a parity, set it, then
 * insert structured-append headers. QRinput created by QRinput_newMQR() will
 * be rejected.
 * @param input input object. Version number and error correction level must be
 *        set.
 * @return a set of input data. On error, NULL is returned, and errno is set
 *         to indicate the error. See Exceptions for the details.
 * @throw ERANGE input data is too large.
 * @throw EINVAL invalid input data.
 * @throw ENOMEM unable to allocate memory.
 */
BQDCODE_EXPORT QRinput_Struct *BQDQRinput_splitQRinputToStruct(QRinput *input);

/**
 * Insert structured-append headers to the input structure. It calculates
 * a parity and set it if the parity is not set yet.
 * @param s input structure
 * @retval 0 success.
 * @retval -1 an error occurred and errno is set to indeicate the error.
 *            See Execptions for the details.
 * @throw EINVAL invalid input object.
 * @throw ENOMEM unable to allocate memory.
 */
BQDCODE_EXPORT int BQDQRinput_Struct_insertStructuredAppendHeaders(QRinput_Struct *s);

/**
 * Set FNC1-1st position flag.
 */
BQDCODE_EXPORT int BQDQRinput_setFNC1First(QRinput *input);

/**
 * Set FNC1-2nd position flag and application identifier.
 */
BQDCODE_EXPORT int BQDQRinput_setFNC1Second(QRinput *input, unsigned char appid);

/******************************************************************************
 * QRcode output (qrencode.c)
 *****************************************************************************/

/**
 * QRcode class.
 * Symbol data is represented as an array contains width*width uchars.
 * Each uchar represents a module (dot). If the less significant bit of
 * the uchar is 1, the corresponding module is black. The other bits are
 * meaningless for usual applications, but here its specification is described.
 *
 * @verbatim
   MSB 76543210 LSB
       |||||||`- 1=black/0=white
       ||||||`-- 1=ecc/0=data code area
       |||||`--- format information
       ||||`---- version information
       |||`----- timing pattern
       ||`------ alignment pattern
       |`------- finder pattern and separator
       `-------- non-data modules (format, timing, etc.)
   @endverbatim
 */

/*
typedef struct {
	int version;         ///< version of the symbol
	int width;           ///< width of the symbol
	unsigned char *data; ///< symbol data
} QRcode;
*/
/**
 * Singly-linked list of QRcode. Used to represent a structured symbols.
 * A list is terminated with NULL.
 */
/*
typedef struct _QRcode_List {
	QRcode *code;
	struct _QRcode_List *next;
} QRcode_List;
*/
/**
 * Create a symbol from the input data.
 * @warning This function is THREAD UNSAFE when pthread is disabled.
 * @param input input data.
 * @return an instance of QRcode class. The version of the result QRcode may
 *         be larger than the designated version. On error, NULL is returned,
 *         and errno is set to indicate the error. See Exceptions for the
 *         details.
 * @throw EINVAL invalid input object.
 * @throw ENOMEM unable to allocate memory for input objects.
 */
BQDCODE_EXPORT QRcode *BQDQRcode_encodeInput(QRinput *input);

/**
 * Create a symbol from the string. The library automatically parses the input
 * string and encodes in a QR Code symbol.
 * @warning This function is THREAD UNSAFE when pthread is disabled.
 * @param string input string. It must be NUL terminated.
 * @param version version of the symbol. If 0, the library chooses the minimum
 *                version for the given input data.
 * @param level error correction level.
 * @param hint tell the library how Japanese Kanji characters should be
 *             encoded. If QR_MODE_KANJI is given, the library assumes that the
 *             given string contains Shift-JIS characters and encodes them in
 *             Kanji-mode. If QR_MODE_8 is given, all of non-alphanumerical
 *             characters will be encoded as is. If you want to embed UTF-8
 *             string, choose this. Other mode will cause EINVAL error.
 * @param casesensitive case-sensitive(1) or not(0).
 * @return an instance of QRcode class. The version of the result QRcode may
 *         be larger than the designated version. On error, NULL is returned,
 *         and errno is set to indicate the error. See Exceptions for the
 *         details.
 * @throw EINVAL invalid input object.
 * @throw ENOMEM unable to allocate memory for input objects.
 * @throw ERANGE input data is too large.
 */
BQDCODE_EXPORT QRcode *BQDQRcode_encodeString(const char *string, int version, QRecLevel level, QRencodeMode hint, int casesensitive);

/**
 * Same to QRcode_encodeString(), but encode whole data in 8-bit mode.
 * @warning This function is THREAD UNSAFE when pthread is disabled.
 */
BQDCODE_EXPORT QRcode *BQDQRcode_encodeString8bit(const char *string, int version, QRecLevel level);

/**
 * Micro QR Code version of QRcode_encodeString().
 * @warning This function is THREAD UNSAFE when pthread is disabled.
 */
BQDCODE_EXPORT QRcode *BQDQRcode_encodeStringMQR(const char *string, int version, QRecLevel level, QRencodeMode hint, int casesensitive);

/**
 * Micro QR Code version of QRcode_encodeString8bit().
 * @warning This function is THREAD UNSAFE when pthread is disabled.
 */
BQDCODE_EXPORT QRcode *BQDQRcode_encodeString8bitMQR(const char *string, int version, QRecLevel level);

/**
 * Encode byte stream (may include '\0') in 8-bit mode.
 * @warning This function is THREAD UNSAFE when pthread is disabled.
 * @param size size of the input data.
 * @param data input data.
 * @param version version of the symbol. If 0, the library chooses the minimum
 *                version for the given input data.
 * @param level error correction level.
 * @throw EINVAL invalid input object.
 * @throw ENOMEM unable to allocate memory for input objects.
 * @throw ERANGE input data is too large.
 */
BQDCODE_EXPORT QRcode *BQDQRcode_encodeData(int size, const unsigned char *data, int version, QRecLevel level);

/**
 * Micro QR Code version of QRcode_encodeData().
 * @warning This function is THREAD UNSAFE when pthread is disabled.
 */
BQDCODE_EXPORT QRcode *BQDQRcode_encodeDataMQR(int size, const unsigned char *data, int version, QRecLevel level);

/**
 * Free the instance of QRcode class.
 * @param qrcode an instance of QRcode class.
 */
BQDCODE_EXPORT void BQDQRcode_free(QRcode *qrcode);

/**
 * Create structured symbols from the input data.
 * @warning This function is THREAD UNSAFE when pthread is disabled.
 * @param s input data, structured.
 * @return a singly-linked list of QRcode.
 */
BQDCODE_EXPORT QRcode_List *BQDQRcode_encodeInputStructured(QRinput_Struct *s);

/**
 * Create structured symbols from the string. The library automatically parses
 * the input string and encodes in a QR Code symbol.
 * @warning This function is THREAD UNSAFE when pthread is disabled.
 * @param string input string. It must be NUL terminated.
 * @param version version of the symbol.
 * @param level error correction level.
 * @param hint tell the library how Japanese Kanji characters should be
 *             encoded. If QR_MODE_KANJI is given, the library assumes that the
 *             given string contains Shift-JIS characters and encodes them in
 *             Kanji-mode. If QR_MODE_8 is given, all of non-alphanumerical
 *             characters will be encoded as is. If you want to embed UTF-8
 *             string, choose this. Other mode will cause EINVAL error.
 * @param casesensitive case-sensitive(1) or not(0).
 * @return a singly-linked list of QRcode. On error, NULL is returned, and
 *         errno is set to indicate the error. See Exceptions for the details.
 * @throw EINVAL invalid input object.
 * @throw ENOMEM unable to allocate memory for input objects.
 */
BQDCODE_EXPORT QRcode_List *BQDQRcode_encodeStringStructured(const char *string, int version, QRecLevel level, QRencodeMode hint, int casesensitive);

/**
 * Same to QRcode_encodeStringStructured(), but encode whole data in 8-bit mode.
 * @warning This function is THREAD UNSAFE when pthread is disabled.
 */
BQDCODE_EXPORT QRcode_List *BQDQRcode_encodeString8bitStructured(const char *string, int version, QRecLevel level);

/**
 * Create structured symbols from byte stream (may include '\0'). Wholde data
 * are encoded in 8-bit mode.
 * @warning This function is THREAD UNSAFE when pthread is disabled.
 * @param size size of the input data.
 * @param data input dat.
 * @param version version of the symbol.
 * @param level error correction level.
 * @return a singly-linked list of QRcode. On error, NULL is returned, and
 *         errno is set to indicate the error. See Exceptions for the details.
 * @throw EINVAL invalid input object.
 * @throw ENOMEM unable to allocate memory for input objects.
 */
BQDCODE_EXPORT QRcode_List *BQDQRcode_encodeDataStructured(int size, const unsigned char *data, int version, QRecLevel level);

/**
 * Return the number of symbols included in a QRcode_List.
 * @param qrlist a head entry of a QRcode_List.
 * @return number of symbols in the list.
 */
BQDCODE_EXPORT int BQDQRcode_List_size(QRcode_List *qrlist);

/**
 * Free the QRcode_List.
 * @param qrlist a head entry of a QRcode_List.
 */
BQDCODE_EXPORT void BQDQRcode_List_free(QRcode_List *qrlist);


/******************************************************************************
 * System utilities
 *****************************************************************************/

/**
 * Return a string that identifies the library version.
 * @param major_version major version number
 * @param minor_version minor version number
 * @param micro_version micro version number
 */
BQDCODE_EXPORT void BQDQRcode_APIVersion(int *major_version, int *minor_version, int *micro_version);

/**
 * Return a string that identifies the library version.
 * @return a string identifies the library version. The string is held by the
 * library. Do NOT free it.
 */
BQDCODE_EXPORT char *BQDQRcode_APIVersionString(void);

/**
 * @deprecated
 */
#ifndef _MSC_VER
BQDCODE_EXPORT void BQDQRcode_clearCache(void) __attribute__ ((deprecated));
#else
BQDCODE_EXPORT void BQDQRcode_clearCache(void);
#endif


#ifdef __cplusplus
}
#endif

#endif