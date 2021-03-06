#ifndef RLISTIMAGEMANAGER_H
#define RLISTIMAGEMANAGER_H

#include "winsockwrapper.h"
#include <QtCore>

#include "imagemanager.h"
#include "rmat.h"

class RListImageManager
{
public:
    RListImageManager();
    RListImageManager(QList<QUrl> urlList);
    ~RListImageManager();

    void loadData(QList<QUrl> urlList);


    //getters
    QList<QUrl> getUrlList();
    QList<ImageManager*> getImageManagerList() const;
    QList<QTableWidget*> getTableWidgetList() const;
    QList<RMat*> getRMatImageList();

    QVector<double> getMeanSeries() const;


private:

    void cleanLists();

    QList<QUrl> urlList;
    QList<ImageManager*> imageManagerList;
    QList<QTableWidget*> tableWidgetList;
    QList<RMat*> rMatImageList;

};

#endif // RLISTIMAGEMANAGER_H
