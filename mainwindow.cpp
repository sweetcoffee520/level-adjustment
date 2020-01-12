#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QIODevice>
#include <QTextStream>
#include <QDebug>
#include <QRegExp>
#include "Matrix.h"
#include <cmath>

//点类
class point
{
public:
    point()
    {
        ptname="";
        Height=0;
        isknow=true;
    }
    point(QString name,double height)
    {
        ptname=name;
        Height=height;
        isknow=true;
    }
    //点名
    QString ptname;
    //点高程
    double Height;
    //点是否已知
    bool isknow;
};

//边类
class edge
{
public:
    edge()
    {
        ID="";
        startpt="";
        endpt="";
        h=0;
        s=0;
    }
    edge(QString id,QString start,QString end,double i,double j)
    {   ID=id;
        startpt=start;
        endpt=end;
        h=i;
        s=j;
    }
    //边的编号
    QString ID;
    //边的起始点
    QString startpt;
    //边的终点
    QString endpt;
    double h;//高差
    double s;//边长
};
//边个数，点个数，已知点个数
int hnum,ptnum,knum;
//未知点个数
int unum;
//单位权中误差
double sigma;
//point *ifopt=new point[ptnum];//存储点信息
QList <point> ifopt;
//edge *ifoed=new edge[hnum];//存储边信息
QList <edge> ifoed;
//建立B矩阵：
void getBL(Matrix&B,Matrix&L,Matrix &X0);
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

//读取文件
void MainWindow::on_actionopenfile_triggered()
{
    int i=0;
    QStringList content;
    QString showcontent="";
    this->path=QFileDialog::getOpenFileName(this,QStringLiteral("选择文件"),"","*.txt");
    QFile in(path);
    if(!in.open(QIODevice::ReadOnly|QIODevice::Text)){QMessageBox::warning(this,QStringLiteral("警告!"),QStringLiteral("文件未打开!"));
        return ;
    }
    else
    {
        QString array;
        while (!in.atEnd())//文件结尾
        {
            array=in.readLine().trimmed();
            showcontent+=array+'\n';
            content=array.split(QRegExp("\\s+"));
            hnum=content[0].toInt();
            ptnum=content[1].toInt();
            knum=content[2].toInt();
            sigma=content[3].toDouble();
            content.clear();
            array=in.readLine().trimmed();
            point temp;
            edge temp1;
            for(i=0;i<knum;i++)
            {
                array=in.readLine().trimmed();
                content=array.split(QRegExp("\\s+"));
                temp.ptname=content[0];
                temp.Height=content[1].toDouble();
                ifopt.append(temp);
                //ifopt[i].ptname="dfs";
                //ifopt[i].Height=content[1].toDouble();
                content.clear();
                showcontent+=array+'\n';
            }
            //未知点个数
            unum=ptnum-knum;
            for (i=0;i<unum;i++)
            {
                temp.ptname="";
                temp.Height=0;
                temp.isknow=0;
                ifopt.append(temp);
            }
            //////////////////////////////////////////
            array=in.readLine();//读空行
            showcontent+=array;
            for(i=0;i<hnum;i++)
            {
                array=in.readLine();
                content=array.split(QRegExp("\\s+"));
                temp1.ID=content[0];
                temp1.startpt=content[1];
                temp1.endpt=content[2];
                temp1.h=content[3].toDouble();
                temp1.s=content[4].toDouble();
                ifoed.append(temp1);
                //赋值完清空列表
                content.clear();
                showcontent+=array+'\n';
            }
            ui->textEdit->setPlainText(showcontent);
            in.close();
        }
    }
}
void MainWindow::on_pushButton_clicked()
{
    if(hnum==0)
    {QMessageBox::information(this,QStringLiteral("提示"),QStringLiteral("请选择数据文件"));}
    else
    {
        int i,j;
        Matrix temp(hnum,1);
        Matrix x,P(hnum,hnum),Nbb,W;
        Matrix v,L(hnum,1),X(unum,1),DXX;
        //初始化B,L矩阵
        Matrix B(hnum,unum),l(hnum,1),X0(unum);
        getBL(B,l,X0);
        //显示B矩阵结果
        QString showresult;
        //计算权阵
        for(i=0;i<hnum;i++)
        {
            temp.set(i,0,ifoed.at(i).h);
            P.set(i,i,1/ifoed.at(i).s);//定权
        }
        Nbb=B.Trans()*P*B;
        W=B.Trans()*P*l;
        x=Nbb.Inverse()*W;
        X=X0+x;
        v=B*x-l;
        L=temp+v;
        for(i=0;i<hnum;i++)
        {
            for(j=0;j<3;j++)
            {
                showresult+=QString::number(B.get(i,j))+" ";//显示
            }
            showresult+='\n';
        }
        ui->textEdit_2->setPlainText(showresult);
        showresult="";
        for(i=0;i<unum;i++)
        {
            showresult+=QString::number(X.get(i,0))+'\n';
        }
        ui->textEdit_3->setPlainText(showresult);
        showresult=QStringLiteral("平差结果:")+'\n';//显示
        for(i=0;i<hnum;i++)
        {
            showresult+=ifoed.at(i).startpt+" "+ifoed.at(i).endpt+" "+QString::number(L.get(i,0))+'\n';
        }
        ui->textEdit_4->setPlainText(showresult);
    }
}
void getBL(Matrix&B,Matrix&L,Matrix&X0)
{
    //保存起点点号
    int pointnum_start;
    //保存终点点号
    int pointnum_end;
    int i,j;
    //保存高差
    double tempheigh;
    point temp;
    for(i=0;i<hnum;i++)
    {
        //判断起点是否为已知点，终点是否为未知点
        if(ifoed.at(i).startpt.mid(0,1)!='p'&&ifoed.at(i).endpt.mid(0,1)=='p')
        {
            //未知点点号
            pointnum_end=ifoed.at(i).endpt.mid(1).toInt();
            //寻找已知点
            for(j=0;j<knum;j++)
            {
                if(ifoed.at(i).startpt==ifopt.at(j).ptname)
                {
                    //判断是否已经赋了估值
                    if(ifopt.at(knum+pointnum_end-1).Height==0)
                    {
                        //赋高程估值
                        temp.Height=ifopt.at(j).Height+ifoed.at(i).h;
                        ifopt[knum+pointnum_end-1]=temp;
                    }
                }
            }
        }
        //判断终点是否为未知点，终点是否为已知点
        else if(ifoed.at(i).startpt.mid(0,1)=='p'&&ifoed.at(i).endpt.mid(0,1)!='p')
        {
            //未知点点名
            temp.ptname=ifoed.at(i).startpt;
            temp.isknow=0;
            //未知点点号
            pointnum_start=ifoed.at(i).startpt.mid(1).toInt();
            //寻找已知点
            for(j=0;j<knum;j++)
            {
                if(ifoed.at(i).startpt==ifopt.at(j).ptname)
                {
                    //判断是否已经赋估值
                    if(ifopt.at(knum+pointnum_start-1).Height==0)
                    {
                        //高程估值
                        temp.Height=ifopt.at(j).Height-ifoed.at(i).h;
                        ifopt[knum+pointnum_start-1]=temp;
                    }
                }
            }
        }
        //都是未知点
        else if(ifoed.at(i).startpt.mid(0,1)=='p'&&ifoed.at(i).endpt.mid(0,1)=='p')
        {
            temp.ptname=ifoed.at(i).endpt;
            temp.isknow=0;
            //起始点点号
            pointnum_start=ifoed.at(i).startpt.mid(1).toInt();
            //终点点号
            pointnum_end=ifoed.at(i).endpt.mid(1).toInt();
            if(ifopt.at(knum+pointnum_end-1).Height==0)
            {
                //高程估值
                temp.Height=ifopt.at(knum+pointnum_start-1).Height+ifoed.at(i).h;
                ifopt[knum+pointnum_end-1]=temp;
            }
        }

    }
    for(i=0;i<hnum;i++)
    {
        //判断起点为已知点，终点为未知点
        if(ifoed.at(i).startpt.mid(0,1)!='p'&&ifoed.at(i).endpt.mid(0,1)=='p')
        {

            pointnum_end=ifoed.at(i).endpt.mid(1).toInt();
            //如果参数被减这位-1，否则为1
            B.set(i,pointnum_end-1,1);
            for(j=0;j<knum;j++)
            {
                if(ifoed.at(i).startpt==ifopt.at(j).ptname)
                {
                    //观测值减去估值
                    tempheigh=ifoed.at(i).h-(ifopt.at(knum+pointnum_end-1).Height-ifopt.at(j).Height);
                    L.set(i,0,tempheigh);
                }
            }
        }
        //判断终点为已知点，起点为未知点
        else if(ifoed.at(i).startpt.mid(0,1)=='p'&&ifoed.at(i).endpt.mid(0,1)!='p')
        {
            //未知点点号
            pointnum_start=ifoed.at(i).startpt.mid(1).toInt();
            B.set(i,pointnum_start-1,-1);
            //寻找已知点
            for(j=0;j<knum;j++)
            {
                if(ifoed.at(i).endpt==ifopt.at(j).ptname)
                {
                    //观测值减去估值
                    tempheigh=ifoed.at(i).h-(ifopt.at(j).Height-ifopt.at(knum+pointnum_start-1).Height);
                    L.set(i,0,tempheigh);
                }
            }
        }
        //都是未知点
        else if(ifoed.at(i).startpt.mid(0,1)=='p'&&ifoed.at(i).endpt.mid(0,1)=='p')
        {
            pointnum_start=ifoed.at(i).startpt.mid(1).toInt();
            pointnum_end=ifoed.at(i).endpt.mid(1).toInt();
            B.set(i,pointnum_start-1,-1);
            B.set(i,pointnum_end-1,1);
            //观测值减去估值
            tempheigh=ifoed.at(i).h-(ifopt.at(knum+pointnum_end-1).Height-ifopt.at(knum+pointnum_start-1).Height);
            L.set(i,0,tempheigh);
        }
    }
    for(i=0;i<(unum);i++)
    {
        X0.set(i,0,tempheigh=ifopt.at(knum+i).Height);
    }
}

void MainWindow::on_actionsave_triggered()
{
    QString saveFilename;
    QString result;
    result=ui->textEdit_4->toPlainText();
    saveFilename=QFileDialog::getSaveFileName(this,QStringLiteral("保存文件"),"",("*.txt"));
    QFile outfile(saveFilename);
    if(!outfile.open(QIODevice::WriteOnly|QIODevice::Text)){
        QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("文件未打开"));
    }
    else {
    QTextStream ss(&outfile);
    ss<<result;
    outfile.close();
    }

}
