
#include <QFileInfo>
#include <QUrl>
#include <QMediaPlaylist>

#include "playlistmodel.h"



PlaylistModel::PlaylistModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_playlist(0)
{
    
}


QMediaPlaylist *PlaylistModel::playlist() const
{
    return m_playlist;
}


void PlaylistModel::setPlaylist(QMediaPlaylist *playlist)
{
    if(m_playlist){
        disconnect(m_playlist, SIGNAL(mediaAboutToBeInserted(int, int)), this, SLOT(beginInsertItems(int, int)));
        disconnect(m_playlist, SIGNAL(mediaInserted(int, int)), this, SLOT(endInsertItems()));
        disconnect(m_playlist, SIGNAL(mediaAboutToBeRemoved(int, int)), this, SLOT(beginRemoveItems(int, int)));
        disconnect(m_playlist, SIGNAL(mediaRemoved(int, int)), this, SLOT(endRemoveItems()));
        disconnect(m_playlist, SIGNAL(mediaChanged(int, int)), this, SLOT(changeItems(int, int)));
    }
    
    beginResetModel();
    
    m_playlist = playlist;
    if(m_playlist){
        connect(m_playlist, SIGNAL(mediaAboutToBeInserted(int, int)), this, SLOT(beginInsertItems(int, int)));
        connect(m_playlist, SIGNAL(mediaInserted(int, int)), this, SLOT(endInsertItems()));
        connect(m_playlist, SIGNAL(mediaAboutToBeRemoved(int, int)), this, SLOT(beginRemoveItems(int, int)));
        connect(m_playlist, SIGNAL(mediaRemoved(int, int)), this, SLOT(endRemoveItems()));
        connect(m_playlist, SIGNAL(mediaChanged(int, int)), this, SLOT(changeItems(int, int)));
    }
    
    endResetModel();
    
}



bool PlaylistModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(role);
    m_data[index] = value;
    emit dataChanged(index, index);
    
    return true;
}


int PlaylistModel::rowCount(const QModelIndex &parent) const
{
    int rowCount = m_playlist && !parent.isValid() ? m_playlist->mediaCount() : 0;
    return rowCount;
}


int PlaylistModel::columnCount(const QModelIndex &parent) const
{
    return !parent.isValid() ? ColumnCount : 0;
}


QModelIndex PlaylistModel::index(int row, int column, const QModelIndex &parent) const
{
    bool validIndex = m_playlist && !parent.isValid() 
            && row >=0 && row < m_playlist->mediaCount() 
            && column >=0 && column < ColumnCount;
    
    return validIndex ? createIndex(row, column) : QModelIndex();
    
}

QModelIndex PlaylistModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);
    
    return QModelIndex();
}


QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
    if(index.isValid() && role == Qt::DisplayRole){
        QVariant value = m_data[index];
        if(!value.isValid() && index.column() == Title){
            // QUrl location = m_playlist->media(index.row().canonicalUrl());
            QUrl location = m_playlist->media(index.row()).canonicalUrl();
            return QFileInfo(location.path()).fileName();
        }
        return value;
    }
    
    return QVariant();
}


void PlaylistModel::beginInsertItems(int start, int end)
{
    //qDebug() << "SLOT: " << __FUNCTION__;
    
    m_data.clear();
    beginInsertRows(QModelIndex(), start, end);
}

void PlaylistModel::endInsertItems()
{
    //qDebug() << "SLOT: " << __FUNCTION__;
    //qDebug() << "file: " << __FILE__;
    //qDebug() << "line: " << __LINE__;
    // qDebug() << "method: " << __METHOD__; // 没有定义
    
    endInsertRows();
}

void PlaylistModel::beginRemoveItems(int start, int end)
{
    //qDebug() << "SLOT: " << __FUNCTION__;
    m_data.clear();
    beginRemoveRows(QModelIndex(), start, end);
}

void PlaylistModel::endRemoveItems()
{
    //qDebug() << "SLOT: " << __FUNCTION__;
    endRemoveRows();
}

void PlaylistModel::changeItems(int start, int end)
{
    //qDebug() << "SLOT: " << __FUNCTION__;
    m_data.clear();
    emit dataChanged(index(start, 0), index(end, ColumnCount));
}







