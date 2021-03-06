#include "TextFileIO.h"

#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QTextStream>
#include <QApplication>

#include "MantidQtAPI/FileDialogHandler.h"

/**
 * Construct an object with a list of file filters
 */
TextFileIO::TextFileIO(QStringList fileFilters) : m_filters(fileFilters)
{

}

/**
 * Save to a file
 * @param filename An optional filename
 * @return True if the save was successful, false otherwise
 */
bool TextFileIO::save(const QString & txt, const QString & filename) const
{
  QString saved = filename;
  if(saved.isEmpty())
  {
    saved = askWhereToSave();
  }
  if(saved.isEmpty()) return true; //cancelled

  QFile file(saved);
  if( !file.open(QIODevice::WriteOnly) )
  {
    QMessageBox::critical(NULL, "MantidPlot - File error",
        QString("Could not open file \"%1\" for writing.").arg(saved));
    return false;
  }

  QTextStream writer(&file);
  QApplication::setOverrideCursor(Qt::WaitCursor);
  writer << txt;
  QApplication::restoreOverrideCursor();
  file.close();
  return true;
}

/// Open a save dialog
QString TextFileIO::askWhereToSave() const
{
  QString selectedFilter;
  QString filter = m_filters.join(";;");
  QString filename = MantidQt::API::FileDialogHandler::getSaveFileName(NULL, "MantidPlot - Save", "",filter, &selectedFilter);
  if( filename.isEmpty() ) return QString();
  if( QFileInfo(filename).suffix().isEmpty() )
  {
    QString ext = selectedFilter.section('(',1).section(' ', 0, 0);
    ext.remove(0,1);
    if( ext != ")" ) filename += ext;
  }
  return filename;

}
