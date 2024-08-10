#include "creatbqdcode.h"

#include <QFileInfo>
#include <QDir>

CreatBQDCode::CreatBQDCode(QObject *parent)
    : QObject{parent}
{
    splogInit();
    sqliteInit();
}

bool CreatBQDCode::printBQDCode(const QString &name ,const QPrinter::OutputFormat &opf , const QPageSize &pageSize,  const QMarginsF &margins)
{
    //打印机部分
    QPrinter printer(QPrinter::HighResolution);                   //打印机
    printer.setOutputFormat(opf);   //设置打印机格式
    printer.setPageMargins(margins); //设置页边距
    printer.setPageSize(pageSize);  //设置纸张大小
    if(opf == QPrinter::PdfFormat){     //如果是输出PDF
        QFileInfo fileinfo(name);       //检查输出路径是否正确
        QString filepath = fileinfo.path();
        QDir dir(filepath);
        if(!dir.exists()){
            if(!dir.mkpath(filepath)){
                badcodelogger->error(tr("PDF输出目录创建失败:").toStdString() + "{}",filepath.toStdString());
                return false;
            }
        }
        printer.setOutputFileName(name); //设置PDF打印路径
    }else {          //调用标准打印机
        printer.setPrinterName(name);
    }

    typeDPI dpi;

    //下面获取打印机相关参数
    //获取打印机
    dpi.printerdpiX = printer.logicalDpiX();
    dpi.printerdpiY = printer.logicalDpiY();

    // 获取屏幕
    auto screen = QGuiApplication::primaryScreen();   //获取主屏幕
    // 获取屏幕的 DPI
    dpi.screendpiX = screen->logicalDotsPerInchX();
    dpi.screendpiY = screen->logicalDotsPerInchY();

    int page = 1 ;
    QPainter painter;       //初始化画笔工具
    if(!painter.begin(&printer)){
        qDebug() << "无法打印";
        return false;
    }
    while(page){


        bool recterr = false , lineerr = false , texterr = false , barcodeerr = false , roundedrecterr = false , datamatrixerr = false;
        bool qrcodeerr = false , ellipseerr = false;
        if(readRectangle(page,dpi)){        //如果对应页有数据就要画
            recterr = false;
        }else{
            recterr = true;
        }

        if(readLine(page,dpi)){
            lineerr = false;
        }else{
            lineerr = true;
        }

        if(readStringText(page,dpi)){
            texterr = false;
        }else{
            texterr = true;
        }

        if(readBarcode(page,dpi)){
            barcodeerr = false;   
        }else{
            barcodeerr = true;
        }

        if(readRoundedRect(page,dpi)){
            roundedrecterr = false ;
        }else{
            roundedrecterr = true;
        }

        if(readDataMatrix(page,dpi)){
            datamatrixerr = false ;
        }else{
            datamatrixerr = true;
        }

        if(readQRCode(page,dpi)){
            qrcodeerr = false;
        }else{
            qrcodeerr = true;
        }

        if(readEllipse(page,dpi)){
            ellipseerr = false;
        }else{
            ellipseerr = true;
        }

        if(BQDCodeDb.isOpen()) BQDCodeDb.close();   //数据库读取完成了，关闭数据库

        if(recterr && lineerr && texterr && barcodeerr && roundedrecterr && datamatrixerr && qrcodeerr && ellipseerr){
            break;
        }else{
            if(page > 1){
                printer.newPage();
            }
            createRectangle(painter);   //画矩形
            createRoundedRect(painter); //画圆角矩形
            createEllipse(painter);     //画圆
            createLine(painter);        //画线
            createStringText(painter,dpi);  //写字
            createBarCode(painter,dpi);     //画条码
            createDataMatrix(painter,dpi);  //画DataMatrix
            createQRCode(painter,dpi);      //画QR
        }

        ++page;
    }
    painter.end();
    return true;
}

void CreatBQDCode::splogInit()
{
    QString formattedDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    badcodelogger = spdlog::daily_logger_format_mt("BQDCode","BQDCode/logs/" + formattedDateTime.toStdString() + ".txt",0,0);
}

bool CreatBQDCode::sqliteInit()
{
    if(QSqlDatabase::contains("BQDCodeDb")){
        BQDCodeDb = QSqlDatabase::database("BQDCodeDb");
    }else{
        BQDCodeDb = QSqlDatabase::addDatabase("QSQLITE","BQDCodeDb");
    }
    QString BQDDirPath = appDirPath + "/BQDCode/";
    QDir BQDdir(BQDDirPath);
    if(!BQDdir.exists()){   //如果目录不存在
        badcodelogger->info("BQDCode目录不存在，新建目录");
        if (BQDdir.mkpath(BQDDirPath)) {
            qDebug() << "Directory created successfully.";
            badcodelogger->info("BQDCode目录新建成功");
        } else {
            qDebug() << "Failed to create directory.";
            badcodelogger->info("BQDCode目录新建失败");
        }
    }
    BQDCodeDb.setDatabaseName( BQDDirPath + "BQDCode.db");
    if(!BQDCodeDb.open()){  //如果打开失败了
        badcodelogger->error("BQDCode数据库打开失败，失败原因:{}",BQDCodeDb.lastError().text().toStdString());
        qDebug() << BQDCodeDb.lastError().text();
        return false;
    }
    qDebug() << "BQD数据库打开成功";
    badcodelogger->info("BQD数据库打开成功");

    //数据库打开成功了，要检查表格
    QSqlQuery query(BQDCodeDb);
    //尝试创建BarCode表格
    QString cmd = R"(
        create table if not exists [BarCode] (
            page  interger not null ,
            layer integer not null ,
            str text not null,
            var bool not null ,
            varName text ,
            posX double not null ,
            posY double not null ,
            bColor text not null ,
            fColor text not null ,
            fontName text not null ,
            fontSize integer not null ,
            fontWeight integer not null ,
            fontItalic bool not null ,
            type integer not null ,
            disPlayText bool not null ,
            argin integer not null ,
            widthScalef double not null ,
            height double not null ,
            angle double not null ,
            primary key(layer)
        );
    )";

    if(!query.exec(cmd)){
        badcodelogger->info(tr("条码表格创建失败,原因为{}").toStdString(),query.lastError().text().toStdString());
        return false ;
    }

    //尝试创建StringText表格
    cmd = R"(
        create table if not exists [StringText] (
            page  interger not null ,
            layer integer not null ,
            str text not null,
            var bool not null ,
            varName text ,
            posX double not null ,
            posY double not null ,
            bColor text not null ,
            fColor text not null ,
            fontName text not null ,
            fontSize integer not null ,
            fontWeight integer not null ,
            fontItalic bool not null ,
            angle double not null ,
            primary key(layer)
        );
    )";

    if(!query.exec(cmd)){
        badcodelogger->info(tr("字符表格创建失败,原因为{}").toStdString(),query.lastError().text().toStdString());
        return false ;
    }



    //尝试创建Rectangle表格
    cmd = R"(
        create table if not exists [Rectangle] (
            page  interger not null ,
            layer integer not null ,
            posX double not null ,
            posY double not null ,
            posWidth double not null ,
            posHeight double not null ,
            bColor text not null ,
            fColor text not null ,
            penWidth  double not null ,
            angle double not null ,
            primary key(layer)
        );
    )";


    if(!query.exec(cmd)){
        badcodelogger->info(tr("矩形表格创建失败,原因为{}").toStdString(),query.lastError().text().toStdString());
        return false ;
    }


    //尝试创建线条
    cmd = R"(
        create table if not exists [Line] (
            page  interger not null ,
            layer integer not null ,
            posX double not null ,
            posY double not null ,
            iX double not null ,
            iY double not null ,
            fColor text not null ,
            penWidth  double not null ,
            angle double not null ,
            primary key(layer)
        );
    )";


    if(!query.exec(cmd)){
        badcodelogger->info(tr("线条表格创建失败,原因为{}").toStdString(),query.lastError().text().toStdString());
        return false ;
    }


    //尝试创建圆角矩形表格
    cmd = R"(
        create table if not exists [RoundedRect] (
            page  interger not null ,
            layer integer not null ,
            posX double not null ,
            posY double not null ,
            posWidth double not null ,
            posHeight double not null ,
            xRadius double not null ,
            yRadius double not null ,
            bColor text not null ,
            fColor text not null ,
            penWidth  double not null ,
            angle double not null ,
            primary key(layer)
        );
    )";


    if(!query.exec(cmd)){
        badcodelogger->info(tr("圆角矩形表格创建失败,原因为{}").toStdString(),query.lastError().text().toStdString());
        return false ;
    }


    //尝试创建DataMatrix表格
    cmd = R"(
        create table if not exists [DataMatrix] (
            page  interger not null ,
            layer integer not null ,
            str text not null,
            var bool not null ,
            varName text ,
            posX double not null ,
            posY double not null ,
            bColor text not null ,
            fColor text not null ,
            argin integer not null ,
            scalef double not null ,
            angle double not null ,
            primary key(layer)
        );
    )";


    if(!query.exec(cmd)){
        badcodelogger->info(tr("DataMatrix表格创建失败,原因为{}").toStdString(),query.lastError().text().toStdString());
        return false ;
    }

    //尝试创建QRCode表格
    cmd = R"(
        create table if not exists [QRCode] (
            page  interger not null ,
            layer integer not null ,
            str text not null,
            var bool not null ,
            varName text ,
            posX double not null ,
            posY double not null ,
            bColor text not null ,
            fColor text not null ,
            QRVersion integer not null ,
            QRlevel integer not null ,
            QRhint integer not null ,
            CaseSensitive bool not null ,
            scalef double not null ,
            angle double not null ,
            primary key(layer)
        );
    )";


    if(!query.exec(cmd)){
        badcodelogger->info(tr("QRCode表格创建失败,原因为{}").toStdString(),query.lastError().text().toStdString());
        return false ;
    }

    //尝试创建Ellipse表格
    cmd = R"(
        create table if not exists [Ellipse] (
            page  interger not null ,
            layer integer not null ,
            posX double not null ,
            posY double not null ,
            width double not null ,
            height double not null ,
            bColor text not null ,
            fColor text not null ,
            penWidth double not null ,
            angle double not null ,
            primary key(layer)
        );
    )";


    if(!query.exec(cmd)){
        badcodelogger->info(tr("QRCode表格创建失败,原因为{}").toStdString(),query.lastError().text().toStdString());
        return false ;
    }



    return true;
}


bool CreatBQDCode::createBarCode(QPainter &painter , typeDPI &dpi)
{
    foreach (auto const _barcode, BarCode) {
        struct Barcode_Item * bc = nullptr;
        bc = BQDBarcode_Create(const_cast<char *>(_barcode.str.data()));  //创建条码
        if( !bc ) return false;
        int flags = 0 ;
        if(_barcode.disPlayText){
            flags |= _barcode.type;
            flags |= BARCODE_NO_ASCII ;
        }else{
            flags |= _barcode.type;
        }

        bc->flags = flags;
        bc->height = _barcode.height;
        bc->margin = _barcode.argin;                //设置边缘留空

        BQDBarcode_Encode(bc,flags);            //生成数据
        if(updatesizeBarCode(_barcode,bc)){

            qreal width_scale = currentwidthBarCode / bc->width ;      //计算宽度比例

            currentwidthBarCode = bc->width * width_scale ;             //重新校准宽度
            qDebug() << "条码:" << _barcode.str  << ",实际宽度为:" << currentwidthBarCode / dpi.printerdpiX * 25.4 << "MM";

            bc->width = bc->width * width_scale;
            bc->scalef = width_scale;
            bc->height = _barcode.height;

            if(renderBarCode(_barcode,painter,dpi,bc)){
            }
        }
        BQDBarcode_Delete(bc);
    }
    return true;
}

bool CreatBQDCode::updatesizeBarCode(typeBarCode barcode, Barcode_Item *bc)
{
    if( !bc ) return false;

    if ( !bc->partial || !bc->textinfo )
    {
        bc->error = EINVAL;
        return false;
    }
    /* First, calculate barlen */
    int barlen = bc->partial[0] - '0';
    for (char * ptr = bc->partial + 1; *ptr; ptr++)
    {
        if ( isdigit(*ptr) )
        {
            barlen += (*ptr - '0');
        }
        else if ( islower(*ptr) )
        {
            barlen += (*ptr - 'a' + 1);
        }
    }


    /* The scale factor depends on bar length */
    if ( (fabs(bc->scalef) < 1e-6) )
    {
        if ( !bc->width )
        {
            bc->width = barlen  ; /* default */
        }
        bc->scalef = (double) bc->width / (double)barlen;

        //qDebug() << "fabs(bc->scalef) < 1e-6 , bc->width = " << bc->width << "bc->scalef = " << bc->scalef;
    }

    /* The width defaults to "just enough" */
    if ( !bc->width )
    {
        bc->width = barlen  * bc->scalef + 1;
    }

    /* But it can be too small, in this case enlarge and center the area */
    if (bc->width < barlen  * bc->scalef)
    {
        int wid = barlen  * bc->scalef + 1;
        bc->xoff -= (wid - bc->width) / 2 ;
        bc->width = wid;
        /* Can't extend too far on the left */
        if (bc->xoff < 0)
        {
            bc->width += -bc->xoff;
            bc->xoff = 0;
        }
    }

    /* The height defaults to 80 points (rescaled) */
    if ( !bc->height )
    {
        bc->height = 80 * bc->scalef;
    }

    /* If too small (5 + text), reduce the scale factor and center */
    int i = 5 + 10 * (( bc->flags & BARCODE_NO_ASCII) == 0 );
    if (bc->height < i * bc->scalef )
    {
        double scaleg = ((double) bc->height) / i;
        int wid = bc->width * scaleg / bc->scalef;
        bc->xoff += ( bc->width - wid ) / 2;
        bc->width = wid;
        bc->scalef = scaleg;
    }
   // int  m_margin = bc->margin;
   // qDebug() << "margin:" << m_margin;
  //  qDebug() << "bcwidht:" << bc->width;

    miniwidthBarCode  = bc->xoff + bc->width  + 2 * bc->margin;
    miniheightBarCode = bc->yoff + bc->height + 2 * bc->margin;
    //  qDebug() << "width:" << miniwidthBarCode << ",height:" << miniheightBarCode;

//    currentwidthBarCode = barcode.widthScalef  * miniwidthBarCode;
    currentwidthBarCode = barcode.widthScalef;      //当前宽度要求等于设置宽度
    currentheightBarCode = miniheightBarCode ;

  //  qDebug() << "width:" << currentwidthBarCode << ",height:" << currentheightBarCode;

    return true;
}

bool CreatBQDCode::renderBarCode(typeBarCode barcode, QPainter &painter, typeDPI & dpi , Barcode_Item *bc)
{
    if( !bc ) return false;

    painter.translate(barcode.postion);     //设置原点
    painter.rotate(barcode.angle);          //设置角度

    int w = (bc->width + 2 * bc->margin) ;
    int h = (bc->height + 2 * bc->margin);
    painter.setBrush(barcode.bColor);
    painter.setPen(Qt::NoPen);
    painter.drawRect(QRect(0, 0, w, h));
    if ( !bc->partial || !bc->textinfo )
    {
        bc->error = EINVAL;
        return false;
    }
    if(!drawBarBarCode(barcode,painter,bc)){
        return false;
    }

    //    qDebug() << "bc->flags & BARCODE_NO_ASCII = " <<( bc->flags & BARCODE_NO_ASCII);
    if((bc->flags & BARCODE_NO_ASCII) )
    {
        drawtextBarCode(barcode,painter,dpi);
    }

    painter.translate(QPointF(0,0) - barcode.postion);     //回到画图之前的原点
    painter.rotate( 0);          //设置角度
    return true;
}

bool CreatBQDCode::drawBarBarCode(typeBarCode barcode, QPainter &painter, Barcode_Item *bc)
{
    int mode = '-';
    int i; /* text below bars */
    char * ptr;
    double xpos = bc->margin + (bc->partial[0] - '0') * bc->scalef;
    for (ptr = bc->partial + 1, i = 1; *ptr; ptr++, i++)
    {
        /* special cases: '+' and '-' */
        if (*ptr == '+' || *ptr == '-')
        {
            mode = *ptr; /* don't count it */
            i++;
            continue;
        }
        /* j is the width of this bar/space */
        int j;
        if (isdigit (*ptr))   j = *ptr - '0';
        else                  j = *ptr - 'a' + 1;

        double x0, y0, yr;
        if (i % 2) /* bar */
        {

            //qDebug() << "bc->xoff = " << bc->xoff << ", xpos = " << xpos;
            x0 = bc->xoff + xpos;// + (j * scalef) / 2;
            y0 = bc->yoff + bc->margin;
            yr = bc->height;
            if ((bc->flags & BARCODE_NO_ASCII) )
            { /* leave space for text */
                if (mode == '-')
                {
                    /* text below bars: 10 points or five points */
                    //y0 += (isdigit(*ptr) ? 10 : 5) * scalef;
                    QFontMetrics metrics(barcode.font);    //获取符宽度，用于设置坐标
                    int mH = metrics.height();      //获取字符高度
                    yr -= mH - 2 ;
                    // yr -= (isdigit(*ptr) ? 10 : 5) * bc->scalef;
                }
                else
                { /* '+' */
                    /* text above bars: 10 or 0 from bottom, and 10 from top */
                    y0 += (isdigit(*ptr) ? 10 : 0) * bc->scalef;
                    yr -= (isdigit(*ptr) ? 20 : 10) * bc->scalef;
                }
            }
            painter.fillRect(QRect(x0, y0, (j * bc->scalef), yr ), barcode.fColor);
        //    qDebug() << "fillRect: " <<  QRect(x0, y0, (j * bc->scalef), yr );
        }
        xpos += j * bc->scalef;
    }
    return true;
}

bool CreatBQDCode::drawtextBarCode(typeBarCode barcode, QPainter &painter , typeDPI &dpi)
{
    painter.save();

    QFontMetrics metrics(barcode.font);    //获取符宽度，用于设置坐标
    int mW = metrics.horizontalAdvance(barcode.str) * (dpi.printerdpiX / dpi.screendpiX);      //获取字符高度
    int mH = metrics.capHeight() * (dpi.printerdpiY / dpi.screendpiY);

    painter.setBrush(barcode.bColor);
    painter.setPen(Qt::NoPen);
    painter.drawRect(QRect((currentwidthBarCode - mW)/2, currentheightBarCode, mW, mH));

    painter.setPen(barcode.fColor);
    painter.setFont(barcode.font);
    painter.drawText((currentwidthBarCode - mW)/2  ,currentheightBarCode + mH,barcode.str);
    painter.restore();
    return true;
}

bool CreatBQDCode::readBarcode(int page,typeDPI &dpi)
{
    if(!BQDCodeDb.isOpen()) {       //如果数据库没有被打开
        if(!BQDCodeDb.open()){       //就要打开数据库
            QString inf = QString(tr("BQDCode.db数据打开失败，原因为:") + "%1").arg(BQDCodeDb.lastError().text());
            badcodelogger->error(inf.toStdString());
            qDebug() << inf;
            return false;
        }
    }
    //开始读取条码数据

    QSqlQuery query(BQDCodeDb);
    QString cmd = QString("select layer,str,var,varName,posX,posY,bColor,fColor,fontName,fontSize,fontWeight,fontItalic,type,disPlayText,argin,widthScalef,height,angle "
                          "from BarCode "
                          "where page = %1 "
                          "order by layer ASC;"
                          ).arg(page);

    if(!query.exec(cmd)){
        badcodelogger->info(tr("条码查询失败,原因为{}").toStdString(),query.lastError().text().toStdString());
        return false ;
    }else{
        BarCode.clear();        //清除条码
        int error = 0 ;
        while (query.next()) {
            ++error;
            QStringList bbColor = query.value(6).toString().split(",");
            QStringList ffColor = query.value(7).toString().split(",");

            if(bbColor.count() != 4){
                bbColor.resize(4);
                bbColor[0] = "255";bbColor[1] = "255";bbColor[2] = "255";bbColor[3] = "0";
            }
            if(ffColor.count() != 4){
                ffColor.resize(4);
                ffColor[0] = "0";ffColor[1] = "0";ffColor[2] = "0";ffColor[3] = "255";
            }

            typeBarCode tmp{
                .layer = query.value(0).toInt(),
                .str = query.value(1).toString().toUtf8(),
                .var = query.value(2).toBool(),
                .varName = query.value(3).toString().toUtf8(),
                .postion = QPointF(query.value(4).toDouble() * dpi.printerdpiX / 25.4 ,query.value(5).toDouble()* dpi.printerdpiY / 25.4),
                .bColor  = QColor(bbColor.at(0).toInt(),bbColor.at(1).toInt(),bbColor.at(2).toInt(),bbColor.at(3).toInt()),
                .fColor = QColor(ffColor.at(0).toInt(),ffColor.at(1).toInt(),ffColor.at(2).toInt(),ffColor.at(3).toInt()),
                .font = QFont(query.value(8).toString(),query.value(9).toInt(),query.value(10).toInt(),query.value(11).toBool()),
                .type = query.value(12).toInt(),
                .disPlayText = query.value(13).toBool(),
                .argin = (dpi.printerdpiX > dpi.printerdpiY) ? query.value(14).toDouble() * dpi.printerdpiX / 25.4 : query.value(14).toDouble() * dpi.printerdpiY / 25.4,
                .widthScalef = query.value(15).toDouble() * dpi.printerdpiX / 25.4,
                .height = query.value(16).toDouble() * dpi.printerdpiY / 25.4,
                .angle = query.value(17).toDouble()
            };
            BarCode.append(tmp);
        //    qDebug() << tmp.layer << QString(tmp.str) << tmp.var << QString(tmp.varName) << tmp.postion << tmp.bColor << tmp.fColor
        //             << tmp.font << tmp.type << tmp.disPlayText
        //             << tmp.argin << tmp.widthScalef << tmp.height << tmp.angle;
        }
        if(error == 0){
            return false;
        }
    }

    return true;
}

bool CreatBQDCode::createDataMatrix(QPainter &painter , typeDPI & dpi)
{
    foreach (auto const _datamatrix, datamatrix) {
        DmtxEncode * m_enc = nullptr;
        m_enc = BQDdmtxEncodeCreate();
        assert(m_enc != NULL);



        m_enc->sizeIdxRequest = DmtxSymbolSquareAuto;   //默认设置
        m_enc->moduleSize = 1;
        m_enc->marginSize = _datamatrix.argin;

        BQDdmtxEncodeSetProp(m_enc, DmtxPropBytesPerPixel, 1);
        int ret = BQDdmtxEncodeDataMatrix(m_enc,_datamatrix.str.length(),(unsigned char *)(_datamatrix.str.data()));
        assert(ret == 1);


        widthDataMatrix = BQDdmtxImageGetProp(m_enc->image, DmtxPropWidth);      //二维码宽度
        heightDataMatrix = BQDdmtxImageGetProp(m_enc->image, DmtxPropHeight);    //二维码高度

        //计算比例
        qreal scale_x = _datamatrix.scalef / widthDataMatrix;      //宽度比例
        qreal scale_y = _datamatrix.scalef / heightDataMatrix;      //高度比例

        painter.translate(_datamatrix.postion);

        painter.setBrush(_datamatrix.bColor); //设置背景色
        painter.setPen(Qt::NoPen);
        painter.drawRect(0, 0, widthDataMatrix,heightDataMatrix);   //画背景色
        painter.setBrush(_datamatrix.fColor);        //设置前景色


        const double offset_x = 0;
        const double offset_y = 0;

        int rowSizeBytes = m_enc->image->rowSizeBytes;
        int bytesPerPixel = m_enc->image->bytesPerPixel;
        for( int y = 0; y < heightDataMatrix; y ++)
        {
            for(int x = 0; x < widthDataMatrix; x ++)
            {
                unsigned char b = m_enc->image->pxl[y * rowSizeBytes  + x * bytesPerPixel];
                if(b == 0)
                {
                    QRectF r(offset_x + x * scale_x, offset_y + y * scale_y, scale_x, scale_y);
                    painter.drawRect(r);
                }
            }
        }
        BQDdmtxEncodeDestroy(&m_enc);
        painter.translate(QPointF(0,0) - _datamatrix.postion);
    }
    return true;
}

bool CreatBQDCode::readDataMatrix(int page ,typeDPI &dpi)
{
    if(!BQDCodeDb.isOpen()) {       //如果数据库没有被打开
        if(!BQDCodeDb.open()){       //就要打开数据库
            QString inf = QString(tr("BQDCode.db数据打开失败，原因为:") + "%1").arg(BQDCodeDb.lastError().text());
            badcodelogger->error(inf.toStdString());
            qDebug() << inf;
            return false;
        }
    }
    //开始读取DataMatrix数据

    QSqlQuery query(BQDCodeDb);
    QString cmd = QString("select layer,str,var,varName,posX,posY,bColor,fColor,argin,scalef,angle "
                          "from DataMatrix "
                          "where page = %1 "
                          "order by layer ASC;"
                          ).arg(page);

    if(!query.exec(cmd)){
        badcodelogger->info(tr("DataMatrix查询失败,原因为{}").toStdString(),query.lastError().text().toStdString());
        return false ;
    }else{
        datamatrix.clear();        //清除条码
        int error = 0 ;
        while (query.next()) {
            ++error;
            QStringList bbColor = query.value(6).toString().split(",");
            QStringList ffColor = query.value(7).toString().split(",");

            if(bbColor.count() != 4){
                bbColor.resize(4);
                bbColor[0] = "255";bbColor[1] = "255";bbColor[2] = "255";bbColor[3] = "0";
            }
            if(ffColor.count() != 4){
                ffColor.resize(4);
                ffColor[0] = "0";ffColor[1] = "0";ffColor[2] = "0";ffColor[3] = "255";
            }

            typeDataMatrix tmp{
                .layer = query.value(0).toInt(),
                .str = query.value(1).toString().toUtf8(),
                .var = query.value(2).toBool(),
                .varName = query.value(3).toString().toUtf8(),
                .postion = QPointF(query.value(4).toDouble() * dpi.printerdpiX / 25.4 ,query.value(5).toDouble()* dpi.printerdpiY / 25.4),
                .bColor  = QColor(bbColor.at(0).toInt(),bbColor.at(1).toInt(),bbColor.at(2).toInt(),bbColor.at(3).toInt()),
                .fColor = QColor(ffColor.at(0).toInt(),ffColor.at(1).toInt(),ffColor.at(2).toInt(),ffColor.at(3).toInt()),
                .argin = (dpi.printerdpiX > dpi.printerdpiY) ? query.value(8).toDouble() * dpi.printerdpiX / 25.4 : query.value(8).toDouble() * dpi.printerdpiY / 25.4,
                .scalef = (dpi.printerdpiX > dpi.printerdpiY) ? query.value(9).toDouble() * dpi.printerdpiX / 25.4 : query.value(9).toDouble() * dpi.printerdpiY / 25.4,
                .angle = query.value(10).toDouble()
            };
            datamatrix.append(tmp);

        }
        if(error == 0){
            return false;
        }
    }

    return true;
}

bool CreatBQDCode::createQRCode(QPainter &painter, typeDPI &dpi)
{
    foreach (auto const _qrcode, qrcode) {
        QRcode * qrcodedata = nullptr;

        qrcodedata = BQDQRcode_encodeString(_qrcode.str.data(), _qrcode.QRVersion, _qrcode.QRlevel, _qrcode.QRhint, _qrcode.CaseSensitive);

        qint32 qrcode_width = qrcodedata->width > 0 ? qrcodedata->width : 1;
    //    double scale_x = (double)width2D / (double)qrcode_width; //二维码图片的缩放比例
    //    double scale_y =(double) height2D /(double) qrcode_width;
        double scale_x = _qrcode.scalef / qrcode_width;
        double scale_y = _qrcode.scalef / qrcode_width;

        painter.translate(_qrcode.postion);

        painter.setBrush(_qrcode.bColor);         //设置背景色
        painter.setPen(Qt::NoPen);
        painter.drawRect(0,0,scale_x * qrcode_width,scale_y * qrcode_width); //画一个背景色
        painter.setBrush(_qrcode.fColor);

        unsigned char * pqrdata = qrcodedata->data;

        for(qint32 y = 0; y < qrcode_width; ++y) {
            for(qint32 x = 0; x < qrcode_width; ++x) {
                if((*pqrdata++) &0x01) {
                    QRectF r(x*scale_x, y*scale_y, scale_x, scale_y);
                    painter.drawRects(&r, 1);
                }
            }
        }

        BQDQRcode_free(qrcodedata);
        painter.translate(QPointF(0,0) - _qrcode.postion);
    }
    return true;
}

bool CreatBQDCode::readQRCode(int page,typeDPI &dpi)
{
    if(!BQDCodeDb.isOpen()) {       //如果数据库没有被打开
        if(!BQDCodeDb.open()){       //就要打开数据库
            QString inf = QString(tr("BQDCode.db数据打开失败，原因为:") + "%1").arg(BQDCodeDb.lastError().text());
            badcodelogger->error(inf.toStdString());
            qDebug() << inf;
            return false;
        }
    }
    //开始读取条码数据

    QSqlQuery query(BQDCodeDb);
    QString cmd = QString("select layer,str,var,varName,posX,posY,bColor,fColor,QRVersion,QRlevel,QRhint,CaseSensitive,scalef,angle "
                          "from QRCode "
                          "where page = %1 "
                          "order by layer ASC;"
                          ).arg(page);

    if(!query.exec(cmd)){
        badcodelogger->info(tr("QR查询失败,原因为{}").toStdString(),query.lastError().text().toStdString());
        return false ;
    }else{
        qrcode.clear();        //清除条码
        int error = 0 ;
        while (query.next()) {
            ++error;
            QStringList bbColor = query.value(6).toString().split(",");
            QStringList ffColor = query.value(7).toString().split(",");

            if(bbColor.count() != 4){
                bbColor.resize(4);
                bbColor[0] = "255";bbColor[1] = "255";bbColor[2] = "255";bbColor[3] = "0";
            }
            if(ffColor.count() != 4){
                ffColor.resize(4);
                ffColor[0] = "0";ffColor[1] = "0";ffColor[2] = "0";ffColor[3] = "255";
            }

            typeQRCode tmp{
                .layer = query.value(0).toInt(),
                .str = query.value(1).toString().toUtf8(),
                .var = query.value(2).toBool(),
                .varName = query.value(3).toString().toUtf8(),
                .postion = QPointF(query.value(4).toDouble() * dpi.printerdpiX / 25.4 ,query.value(5).toDouble()* dpi.printerdpiY / 25.4),
                .bColor  = QColor(bbColor.at(0).toInt(),bbColor.at(1).toInt(),bbColor.at(2).toInt(),bbColor.at(3).toInt()),
                .fColor = QColor(ffColor.at(0).toInt(),ffColor.at(1).toInt(),ffColor.at(2).toInt(),ffColor.at(3).toInt()),
                .QRVersion = query.value(8).toInt(),
                .QRlevel = static_cast<QRecLevel>(query.value(9).toInt()),
                .QRhint = static_cast<QRencodeMode>(query.value(10).toInt()),
                .CaseSensitive = query.value(11).toBool(),
                .scalef = (dpi.printerdpiX > dpi.printerdpiY) ? query.value(12).toDouble() * dpi.printerdpiX / 25.4 : query.value(12).toDouble() * dpi.printerdpiY / 25.4,
                .angle = query.value(13).toDouble()
            };
            qrcode.append(tmp);
        }
        if(error == 0){
            return false;
        }
    }
    return true;
}

bool CreatBQDCode::readStringText(int page , typeDPI &dpi)
{
    if(!BQDCodeDb.isOpen()) {       //如果数据库没有被打开
        if(!BQDCodeDb.open()){       //就要打开数据库
            QString inf = QString(tr("BQDCode.db数据打开失败，原因为:") + "%1").arg(BQDCodeDb.lastError().text());
            badcodelogger->error(inf.toStdString());
            qDebug() << inf;
            return false;
        }
    }
    QString cmd = QString("select layer,str,var,varName,posX,posY,bColor,fColor,fontName,fontSize,fontWeight,fontItalic,angle "
                          "from StringText "
                          "where page = %1 "
                          "order by layer ASC;"
                          ).arg(page);
    QSqlQuery query(BQDCodeDb);
    if(!query.exec(cmd)){
        badcodelogger->info(tr("字符查询失败,原因为{}").toStdString(),query.lastError().text().toStdString());
        return false ;
    }else{
        stringText.clear();
        int error = 0 ;
        while (query.next()) {
            ++error;
            QStringList bbColor = query.value(6).toString().split(",");
            QStringList ffColor = query.value(7).toString().split(",");

            if(bbColor.count() != 4){
                bbColor.resize(4);
                bbColor[0] = "255";bbColor[1] = "255";bbColor[2] = "255";bbColor[3] = "0";
            }
            if(ffColor.count() != 4){
                ffColor.resize(4);
                ffColor[0] = "0";ffColor[1] = "0";ffColor[2] = "0";ffColor[3] = "255";
            }

            typeStringText tmp{
                .layer      = query.value(0).toInt(),
                .str        = query.value(1).toString().toUtf8(),
                .var        = query.value(2).toBool(),
                .varName    = query.value(3).toString().toUtf8(),
                .postion    = QPointF(query.value(4).toDouble() * dpi.printerdpiX / 25.4 ,query.value(5).toDouble()* dpi.printerdpiY / 25.4),
                .bColor     = QColor(bbColor.at(0).toInt(),bbColor.at(1).toInt(),bbColor.at(2).toInt(),bbColor.at(3).toInt()),
                .fColor     = QColor(ffColor.at(0).toInt(),ffColor.at(1).toInt(),ffColor.at(2).toInt(),ffColor.at(3).toInt()),
                .font       = QFont(query.value(8).toString(),query.value(9).toInt(),query.value(10).toInt(),query.value(11).toBool()),
                .angle      = query.value(12).toDouble()
            };
            stringText.append(tmp);
        //    qDebug() << tmp.layer << tmp.str << tmp.var << tmp.varName << tmp.postion << tmp.bColor << tmp.fColor << tmp.font << tmp.angle;
        }
        if(error == 0){
            return false;
        }
    }
    return true;
}

bool CreatBQDCode::createStringText(QPainter &painter , typeDPI & dpi)
{
    foreach (auto const _stringtext, stringText) {
      painter.translate(_stringtext.postion);     //设置原点
      painter.rotate(_stringtext.angle);          //设置角度
      painter.save();


      QFontMetrics metrics(_stringtext.font);    //获取符宽度，用于设置坐标
      //填充背景色
      int mW = metrics.horizontalAdvance(_stringtext.str)  * (dpi.printerdpiX / dpi.screendpiX);      //宽度
      int mH = metrics.capHeight() * (dpi.printerdpiY / dpi.screendpiY);     //高度

      QBrush brush(_stringtext.bColor);

      painter.setBrush(brush);
      painter.setPen(Qt::NoPen);
      painter.drawRect(0,0,mW ,mH);         //画上背景色

      painter.setPen(_stringtext.fColor);
      painter.setFont(_stringtext.font);
      painter.drawText(0,mH ,_stringtext.str);



      painter.restore();
      painter.translate(QPointF(0,0) - _stringtext.postion);     //回到画图之前的原点
    }
    return true;
}

bool CreatBQDCode::createRectangle(QPainter &painter)
{
    foreach (auto const _rectangle, rectangle) {
        painter.translate(_rectangle.postion);
        painter.rotate(_rectangle.angle);
        painter.save();
        painter.setBrush(_rectangle.bColor);
        QPen pen(_rectangle.fColor);
        pen.setWidth(_rectangle.penWidth);
        painter.setPen(pen);
        painter.drawRect(_rectangle.rect);
        painter.restore();
        painter.translate(QPointF(0,0) - _rectangle.postion);     //回到画图之前的原点
    }
    return true;
}

bool CreatBQDCode::readRectangle(int page , typeDPI & dpi)
{
    if(!BQDCodeDb.isOpen()) {       //如果数据库没有被打开
        if(!BQDCodeDb.open()){       //就要打开数据库
            QString inf = QString(tr("BQDCode.db数据打开失败，原因为:") + "%1").arg(BQDCodeDb.lastError().text());
            badcodelogger->error(inf.toStdString());
            qDebug() << inf;
            return false;
        }
    }

    QString cmd = QString("select layer,posX,posY,posWidth,posHeight,bColor,fColor,penWidth,angle "
                          "from Rectangle "
                          "where page = %1 "
                          "order by layer ASC;"
                          ).arg(page);

    QSqlQuery query(BQDCodeDb);
    if(!query.exec(cmd)){
        badcodelogger->info(tr("矩形查询失败,原因为{}").toStdString(),query.lastError().text().toStdString());
        return false ;
    }else{
        rectangle.clear();
        int error = 0 ;
        while (query.next()) {
            ++error ;
            QStringList bbColor = query.value(5).toString().split(",");
            QStringList ffColor = query.value(6).toString().split(",");

            if(bbColor.count() != 4){
                bbColor.resize(4);
                bbColor[0] = "255";bbColor[1] = "255";bbColor[2] = "255";bbColor[3] = "0";
            }
            if(ffColor.count() != 4){
                ffColor.resize(4);
                ffColor[0] = "0";ffColor[1] = "0";ffColor[2] = "0";ffColor[3] = "255";
            }

            typeRectangle tmp{
                .layer = query.value(0).toInt(),
                .postion = QPointF(query.value(1).toDouble() * dpi.printerdpiX / 25.4 ,query.value(2).toDouble()* dpi.printerdpiY / 25.4),
                .rect = QRectF(0,0,query.value(3).toDouble() * dpi.printerdpiX / 25.4 ,query.value(4).toDouble()* dpi.printerdpiY / 25.4),
                .bColor = QColor(bbColor.at(0).toInt(),bbColor.at(1).toInt(),bbColor.at(2).toInt(),bbColor.at(3).toInt()),
                .fColor = QColor(ffColor.at(0).toInt(),ffColor.at(1).toInt(),ffColor.at(2).toInt(),ffColor.at(3).toInt()),
                .penWidth = query.value(7).toDouble() * dpi.printerdpiX / 25.4,
                .angle = query.value(8).toDouble(),
            };
            rectangle.append(tmp);
        }
        if(error == 0){
            return false;
        }
    }
    return true;
}

bool CreatBQDCode::createRoundedRect(QPainter &painter)
{
    foreach (auto const _roundedrect, roundedrect) {
        painter.translate(_roundedrect.postion);
        painter.rotate(_roundedrect.angle);
        painter.save();
        painter.setBrush(_roundedrect.bColor);
        QPen pen(_roundedrect.fColor);
        pen.setWidth(_roundedrect.penWidth);
        painter.setPen(pen);
        painter.drawRoundedRect(_roundedrect.rect,_roundedrect.xRadius,_roundedrect.yRadius);
        painter.restore();
        painter.translate(QPointF(0,0) - _roundedrect.postion);     //回到画图之前的原点
    }
    return true;
}

bool CreatBQDCode::readRoundedRect(int page , typeDPI & dpi)
{
    if(!BQDCodeDb.isOpen()) {       //如果数据库没有被打开
        if(!BQDCodeDb.open()){       //就要打开数据库
            QString inf = QString(tr("BQDCode.db数据打开失败，原因为:") + "%1").arg(BQDCodeDb.lastError().text());
            badcodelogger->error(inf.toStdString());
            qDebug() << inf;
            return false;
        }
    }

    QString cmd = QString("select layer,posX,posY,posWidth,posHeight,xRadius,yRadius,bColor,fColor,penWidth,angle "
                          "from RoundedRect "
                          "where page = %1 "
                          "order by layer ASC;"
                          ).arg(page);

    QSqlQuery query(BQDCodeDb);
    if(!query.exec(cmd)){
        badcodelogger->info(tr("圆角矩形查询失败,原因为{}").toStdString(),query.lastError().text().toStdString());
        return false ;
    }else{
        roundedrect.clear();
        int error = 0 ;
        while (query.next()) {
            ++error ;
            QStringList bbColor = query.value(7).toString().split(",");
            QStringList ffColor = query.value(8).toString().split(",");

            if(bbColor.count() != 4){
                bbColor.resize(4);
                bbColor[0] = "255";bbColor[1] = "255";bbColor[2] = "255";bbColor[3] = "0";
            }
            if(ffColor.count() != 4){
                ffColor.resize(4);
                ffColor[0] = "0";ffColor[1] = "0";ffColor[2] = "0";ffColor[3] = "255";
            }

            typeRoundedRect tmp{
                .layer = query.value(0).toInt(),
                .postion = QPointF(query.value(1).toDouble() * dpi.printerdpiX / 25.4 ,query.value(2).toDouble()* dpi.printerdpiY / 25.4),
                .rect = QRectF(0,0,query.value(3).toDouble() * dpi.printerdpiX / 25.4 ,query.value(4).toDouble()* dpi.printerdpiY / 25.4),
                .xRadius = query.value(5).toDouble() * dpi.printerdpiX / 25.4,
                .yRadius = query.value(6).toDouble() * dpi.printerdpiY / 25.4,
                .bColor = QColor(bbColor.at(0).toInt(),bbColor.at(1).toInt(),bbColor.at(2).toInt(),bbColor.at(3).toInt()),
                .fColor = QColor(ffColor.at(0).toInt(),ffColor.at(1).toInt(),ffColor.at(2).toInt(),ffColor.at(3).toInt()),
                .penWidth = query.value(9).toDouble() * dpi.printerdpiX / 25.4,
                .angle = query.value(10).toDouble(),
            };
            roundedrect.append(tmp);
        }
        if(error == 0){
            return false;
        }
    }
    return true;
}

bool CreatBQDCode::createLine(QPainter &painter)
{
    foreach (auto const _line, line) {
        painter.translate(_line.spostion);
        painter.rotate(_line.angle);
        painter.save();
        QPen pen(_line.fColor);
        pen.setWidth(_line.penWidth);
        painter.setPen(pen);
        painter.drawLine(QPointF(0,0),_line.epostion);
        painter.restore();
        painter.translate(QPointF(0,0) - _line.spostion);     //回到画图之前的原点
    }
    return true;
}

bool CreatBQDCode::readLine(int page , typeDPI & dpi)
{
    if(!BQDCodeDb.isOpen()) {       //如果数据库没有被打开
        if(!BQDCodeDb.open()){       //就要打开数据库
            QString inf = QString(tr("BQDCode.db数据打开失败，原因为:") + "%1").arg(BQDCodeDb.lastError().text());
            badcodelogger->error(inf.toStdString());
            qDebug() << inf;
            return false;
        }
    }
    QString cmd = QString("select layer,posX,posY,iX,iY,fColor,penWidth,angle "
                          "from Line "
                          "where page = %1 "
                          "order by layer ASC;"
                          ).arg(page);
    QSqlQuery query(BQDCodeDb);
    if(!query.exec(cmd)){
        badcodelogger->info(tr("线条查询失败,原因为{}").toStdString(),query.lastError().text().toStdString());
        return false ;
    }else{
        line.clear();
        int error = 0 ;
        while (query.next()) {
            ++error;
            QStringList ffColor = query.value(5).toString().split(",");

            if(ffColor.count() != 4){
                ffColor.resize(4);
                ffColor[0] = "0";ffColor[1] = "0";ffColor[2] = "0";ffColor[3] = "255";
            }

            typeLine tmp{
                .layer = query.value(0).toInt(),
                .spostion = QPointF(query.value(1).toDouble() * dpi.printerdpiX / 25.4 ,query.value(2).toDouble()* dpi.printerdpiY / 25.4),
                .epostion = QPointF(query.value(3).toDouble() * dpi.printerdpiX / 25.4 ,query.value(4).toDouble()* dpi.printerdpiY / 25.4),
                .fColor = QColor(ffColor.at(0).toInt(),ffColor.at(1).toInt(),ffColor.at(2).toInt(),ffColor.at(3).toInt()),
                .penWidth = query.value(6).toDouble() * dpi.printerdpiX / 25.4,
                .angle = query.value(7).toDouble(),
            };
            line.append(tmp);
        //    qDebug() << "线坐标:" << tmp.spostion;
        }
        if(error == 0){
            return false;
        }
    }
    return true;
}

bool CreatBQDCode::createEllipse(QPainter &painter)
{
    foreach (auto const _ellipse, ellipse) {
        painter.translate(_ellipse.postion);
        painter.rotate(_ellipse.angle);
        painter.save();
        painter.setBrush(_ellipse.bColor);
        QPen pen(_ellipse.fColor);
        pen.setWidth(_ellipse.penWidth);
        painter.setPen(pen);
        painter.drawEllipse(0,0,_ellipse.width,_ellipse.height);
        painter.restore();
        painter.translate(QPointF(0,0) - _ellipse.postion);     //回到画图之前的原点
    }
    return true;
}

bool CreatBQDCode::readEllipse(int page, typeDPI &dpi)
{
    if(!BQDCodeDb.isOpen()) {       //如果数据库没有被打开
        if(!BQDCodeDb.open()){       //就要打开数据库
            QString inf = QString(tr("BQDCode.db数据打开失败，原因为:") + "%1").arg(BQDCodeDb.lastError().text());
            badcodelogger->error(inf.toStdString());
            qDebug() << inf;
            return false;
        }
    }
    QString cmd = QString("select layer,posX,posY,width,height,bColor,fColor,penWidth,angle "
                          "from Ellipse "
                          "where page = %1 "
                          "order by layer ASC;"
                          ).arg(page);
    QSqlQuery query(BQDCodeDb);
    if(!query.exec(cmd)){
        badcodelogger->info(tr("圆查询失败,原因为{}").toStdString(),query.lastError().text().toStdString());
        return false ;
    }else{
        ellipse.clear();
        int error = 0 ;
        while (query.next()) {
            ++error;
            QStringList bbColor = query.value(5).toString().split(",");
            QStringList ffColor = query.value(6).toString().split(",");

            if(bbColor.count() != 4){
                bbColor.resize(4);
                bbColor[0] = "255";bbColor[1] = "255";bbColor[2] = "255";bbColor[3] = "0";
            }
            if(ffColor.count() != 4){
                ffColor.resize(4);
                ffColor[0] = "0";ffColor[1] = "0";ffColor[2] = "0";ffColor[3] = "255";
            }

            typeEllipse tmp{
                .layer = query.value(0).toInt(),
                .postion = QPointF(query.value(1).toDouble() * dpi.printerdpiX / 25.4 ,query.value(2).toDouble()* dpi.printerdpiY / 25.4),
                .width = query.value(3).toDouble()  * dpi.printerdpiX / 25.4 ,
                .height = query.value(4).toDouble()  * dpi.printerdpiY / 25.4 ,
                .bColor = QColor(bbColor.at(0).toInt(),bbColor.at(1).toInt(),bbColor.at(2).toInt(),bbColor.at(3).toInt()),
                .fColor = QColor(ffColor.at(0).toInt(),ffColor.at(1).toInt(),ffColor.at(2).toInt(),ffColor.at(3).toInt()),
                .penWidth = query.value(7).toDouble() * dpi.printerdpiX / 25.4,
                .angle = query.value(8).toDouble(),
            };
            ellipse.append(tmp);
        }
        if(error == 0){
            return false;
        }
    }
    return true;
}
