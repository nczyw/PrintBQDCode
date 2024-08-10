#include "creatbqdcode.h"
#include <QApplication>

/*
#include <QPainter>
#include <QPrinter>
#include <QTextDocument>
#include <QPrintDialog>
*/
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
#if 1
    CreatBQDCode w;
    w.printBQDCode("test.pdf",QPrinter::PdfFormat,QPageSize::A4,QMarginsF(1,1,1,1));
    w.printBQDCode("Microsoft Print to PDF",QPrinter::NativeFormat,QPageSize::A4,QMarginsF(1,1,1,1));

#else
    // 创建一个 QPrinter 对象
    QPrinter printer(QPrinter::HighResolution);

    // 弹出打印对话框，让用户选择打印机
    QPrintDialog printDialog(&printer);
    if (printDialog.exec() == QDialog::Accepted) {
        // 创建一个 QPainter 对象用于绘制内容到打印机
        QPainter painter;
        if (painter.begin(&printer)) {
            // 创建一个 QTextDocument 对象来设置要打印的文本内容
            QTextDocument doc;
            doc.setPlainText("Hello World");

            // 打印文本内容
            doc.drawContents(&painter);
            painter.end();
        } else {
            // 如果 QPainter 对象无法开始绘制，打印错误信息
            qWarning("Failed to start painting on printer.");
        }
    } else {
        qWarning("Print dialog was cancelled.");
    }
#endif
    return 0;
}
