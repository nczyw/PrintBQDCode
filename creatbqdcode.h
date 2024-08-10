#ifndef CREATBQDCODE_H
#define CREATBQDCODE_H

#include <QObject>
#include <QColor>
#include <QFont>
#include <QCoreApplication>
#include <QPointF>
#include <QRect>
#include <QPainter>
#include <QPrinter>
#include <QGuiApplication>
#include <QScreen>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

#include <bqdcode.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/daily_file_sink.h>

class CreatBQDCode : public QObject
{
    Q_OBJECT
public:
    //条码类型
    typedef struct typeBarCode{
        int             layer               = 0 ;                           //所在图层
        QByteArray      str                 = "BQDCode";                    //条码要显示的内容
        bool            var                 = false ;                       //是否为变量
        QByteArray      varName             = "";                           //变量名字
        QPointF         postion             = QPointF(0,0);                 //条码位置
        QColor          bColor              = QColor(255,255,255,0);        //条码背景色默认透明
        QColor          fColor              = QColor(0,0,0,255);            //条码前景角默认为黑色
        QFont           font                = QFont("Arial",12);            //条码默认文本字体
        int             type                = BARCODE_128;                  //条码默认类型
        bool            disPlayText         = true;                         //条码默认显示文本
        double          argin               = 0;                            //条码默认边缘不留空
        double          widthScalef         = 25;                            //条码宽度默认比例
        double          height              = 80;                           //条码默认高度
        double          angle               = 0 ;                           //条码默认不旋转
    }typeBarCode;

    //DataMatrix类型
    typedef struct typeDataMatrix{
        int             layer               = 0 ;                           //所在图层
        QByteArray      str                 = "BQDCode";                    //条码要显示的内容
        bool            var                 = false ;                       //是否为变量
        QByteArray      varName             = "";                           //变量名字
        QPointF         postion             = QPointF(0,0);                 //条码位置
        QColor          bColor              = QColor(255,255,255,0);        //条码背景色默认透明
        QColor          fColor              = QColor(0,0,0,255);            //条码前景角默认为黑色
        double          argin               = 0;                            //条码默认边缘不留空
        double          scalef              = 10;                           //条码宽度默认比例
        double          angle               = 0 ;                           //条码默认不旋转
    }typeDataMatrix;

    typedef struct typeQRCode{
        int             layer               = 0 ;                           //所在图层
        QByteArray      str                 = "BQDCode";                    //要显示的内容
        bool            var                 = false ;                       //是否为变量
        QByteArray      varName             = "";                           //变量名字
        QPointF         postion             = QPointF(0,0);                 //条码位置
        QColor          bColor              = QColor(255,255,255,0) ;       //背景色
        QColor          fColor              = QColor(0,0,0,255) ;           //前景色
        int             QRVersion           = 2 ;                           //符号版本
        QRecLevel       QRlevel             = QR_ECLEVEL_Q ;                //纠错等级
        QRencodeMode    QRhint              = QR_MODE_8 ;                   //编码模式
        bool            CaseSensitive       = true ;                        //是否区分大小写
        double             scalef              = 10;                           //QR长宽度默认比例
        double          angle               = 0 ;                           //QR默认不旋转
    }typeQRCode;


    //字符类型
    typedef struct typeStringText{
        int             layer               = 0 ;                           //所在图层
        QByteArray      str                 = "String";                    //条码要显示的内容
        bool            var                 = false ;                       //是否为变量
        QByteArray      varName             = "";                           //变量名字
        QPointF         postion             = QPointF(0,0);                 //字符位置
        QColor          bColor              = QColor(255,255,255,0);        //条码背景色默认透明
        QColor          fColor              = QColor(0,0,0,255);            //条码前景角默认为黑色
        QFont           font                = QFont("Arial",12);            //条码默认文本字体
        double          angle               = 0 ;                           //条码默认不旋转
    }typeStringText;


    //矩形类型
    typedef struct typeRectangle{
        int             layer               = 0 ;                           //所在图层
        QPointF         postion             = QPointF(0,0);                 //坐标
        QRectF          rect                = QRectF(0,0,1,1);              //矩形参数
        QColor          bColor              = QColor(255,255,255,0);        //背景色
        QColor          fColor              = QColor(0,0,0,255);            //前景角默认为黑色
        double          penWidth            = 0.1;                          //矩形画笔宽度
        double          angle               = 0 ;
    }typeRectangle;

    //矩形类型
    typedef struct typeRoundedRect{
        int             layer               = 0 ;                           //所在图层
        QPointF         postion             = QPointF(0,0);                 //坐标
        QRectF          rect                = QRectF(0,0,1,1);              //矩形参数
        double          xRadius             = 0 ;                           //X圆角半径
        double          yRadius             = 0 ;                           //Y圆角半径
        QColor          bColor              = QColor(255,255,255,0);        //背景色
        QColor          fColor              = QColor(0,0,0,255);            //前景角默认为黑色
        double          penWidth            = 0.1;                          //矩形画笔宽度
        double          angle               = 0 ;
    }typeRoundedRect;

    //圆类型
    typedef struct typeEllipse{
        int             layer               = 0 ;                           //所在图层
        QPointF         postion             = QPointF(0,0);                 //坐标
        double          width               = 0 ;                           //宽
        double          height              = 0 ;                           //高
        QColor          bColor              = QColor(255,255,255,0);        //背景色
        QColor          fColor              = QColor(0,0,0,255);            //前景角默认为黑色
        double          penWidth            = 0.1;                          //矩形画笔宽度
        double          angle               = 0 ;
    }typeEllipse;

    //线条类型
    typedef struct typeLine{
        int             layer               = 0 ;                           //所在图层
        QPointF         spostion            = QPointF(0,0);                 //开始坐标
        QPointF         epostion            = QPointF(0,0);                 //结束坐标
        QColor          fColor              = QColor(0,0,0,255);            //前景角默认为黑色
        double          penWidth            = 0.1;                          //画笔宽度
        double          angle               = 0 ;
    }typeLine;

    //创建PDF文件
    explicit CreatBQDCode(QObject *parent = nullptr);
    ///
    /// \brief printBQDCode     打印机
    /// \param name             文件名或者打印机名字
    /// \param opf              输出的格式，打印机，或者PDF
    /// \param margins          页边距
    /// \param pageSize         纸张大小
    /// \return
    ///
    bool printBQDCode(const QString &name , const QPrinter::OutputFormat &opf , const QPageSize &pageSize,  const QMarginsF &margins);        //如果传的是名字，那么就表示要生成PDF
private:
    QString appDirPath = QCoreApplication::applicationDirPath();

private:
    //spdlog 初始化
    std::shared_ptr<spdlog::logger> badcodelogger;  //日志保存
    void splogInit(void);                           //日志初始化
private:
    //sqlite数据库处理
    QSqlDatabase BQDCodeDb ;       //存放BQD数据
    bool sqliteInit(void);  //BQD数据初始化


//打印机相关参数
private:
    typedef struct typeDPI{
        // 获取打印机DPI
        qreal printerdpiX ;         //打印机X
        qreal printerdpiY ;         //打印机Y

        qreal screendpiX ;          //主屏幕X
        qreal screendpiY ;          //主屏幕Y

    }typeDPI;



// 条码
private:
    QVector <typeBarCode> BarCode;        //存放条码数据
    int miniwidthBarCode;                 //条码最小宽度
    int miniheightBarCode;                //条码最小高度
    int currentwidthBarCode;              //条码实际宽度
    int currentheightBarCode;             //条码实际高度
    bool createBarCode(QPainter &painter, typeDPI &dpi);             //创建一个条码
    bool updatesizeBarCode(typeBarCode barcode ,struct Barcode_Item * bc);   //更新条码尺寸
    bool renderBarCode(typeBarCode barcode , QPainter &painter , typeDPI &dpi, struct Barcode_Item * bc);  //绘制整个条码
    bool drawBarBarCode(typeBarCode barcode ,QPainter &painter , struct Barcode_Item * bc);  //绘制条码部分
    bool drawtextBarCode(typeBarCode barcode , QPainter &painter, typeDPI &dpi);    //绘制字符部分
    bool readBarcode(int page, typeDPI &dpi);                     //读取barcode数据

//DataMatrix
private:
    QVector<typeDataMatrix> datamatrix;     //DataMatrix码数据
    int widthDataMatrix;                    //二维码宽度
    int heightDataMatrix;                   //二维码高度

    bool createDataMatrix(QPainter &painter, typeDPI &dpi);   //创建一个二维码
    bool readDataMatrix(int page, typeDPI &dpi);          //读取指定页数据



//QRCode
private:
    QVector<typeQRCode> qrcode;             //QR数据
    int widthQRCode;                        //QR宽度
    int heightQRCode;                       //QR高度

    bool createQRCode(QPainter &painter , typeDPI &dpi);   //创建一个QR
    bool readQRCode(int page, typeDPI &dpi);              //读取指定页数据

//StringText
private:
    QVector<typeStringText> stringText;             //字符数据存放
    bool createStringText(QPainter &painter, typeDPI &dpi);       //创建一个字符
    bool readStringText(int page, typeDPI &dpi);                  //读取指定页的数据

//Rectangle
private:
    QVector<typeRectangle> rectangle;               //矩形数据存放
    bool createRectangle(QPainter &painter);        //创建一个矩形
    bool readRectangle(int page, typeDPI &dpi);                   //读取指定页的数据

//RoundedRect
private:
    QVector<typeRoundedRect> roundedrect;           //圆角矩形
    bool createRoundedRect(QPainter &painter);      //创建一个圆角矩形
    bool readRoundedRect(int page, typeDPI &dpi);                 //读取指定页的数据

//Line
private:
    QVector<typeLine> line;                         //线条数据存放
    bool createLine(QPainter &painter);             //创建一个线条
    bool readLine(int page, typeDPI &dpi);                        //读取指定页的数据

//Ellipse
    QVector<typeEllipse> ellipse;               //圆形数据
    bool createEllipse(QPainter &painter);      //创建一个圆
    bool readEllipse(int page,typeDPI &dpi);    //读取指定页的数据
signals:
};

#endif // CREATBQDCODE_H
