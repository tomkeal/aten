/*
	*** Main Window - Image Functions
	*** src/gui/mainwindow_image.cpp
	Copyright T. Youngs 2007-2016

	This file is part of Aten.

	Aten is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Aten is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Aten.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/mainwindow.h"
#include "base/sysfunc.h"
#include <main/aten.h>

// Bitmap Image Formats (conform to allowable pixmap formats in Qt)
const char* bitmapFormatFilters[AtenWindow::nBitmapFormats] = { "Windows Bitmap (*.bmp)", "Joint Photographic Experts Group (*.jpg)", "Portable Network Graphics (*.png)", "Portable Pixmap (*.ppm)", "X11 Bitmap (*.xbm)", "X11 Pixmap (*.xpm)" };
const char* bitmapFormatExtensions[AtenWindow::nBitmapFormats] = { "bmp", "jpg", "png", "ppm", "xbm", "xpm" };
AtenWindow::BitmapFormat AtenWindow::bitmapFormat(QString s, bool reportError)
{
	AtenWindow::BitmapFormat bf = (AtenWindow::BitmapFormat) enumSearch("bitmap format", AtenWindow::nBitmapFormats, bitmapFormatExtensions, s);
	if ((bf == AtenWindow::nBitmapFormats) && reportError) enumPrintValid(AtenWindow::nBitmapFormats, bitmapFormatExtensions);
	return bf;
}
AtenWindow::BitmapFormat AtenWindow::bitmapFormatFromFilter(const char* s)
{
	return (AtenWindow::BitmapFormat) enumSearch("bitmap format", AtenWindow::nBitmapFormats, bitmapFormatFilters,s);
}
const char* AtenWindow::bitmapFormatFilter(AtenWindow::BitmapFormat bf)
{
	return bitmapFormatFilters[bf];
}
const char* AtenWindow::bitmapFormatExtension(AtenWindow::BitmapFormat bf)
{
	return bitmapFormatExtensions[bf];
}

// Return current width of main view context
int AtenWindow::contextWidth()
{
	return ui.MainView->contextWidth();
}

// Return current height of main view context
int AtenWindow::contextHeight()
{
	return ui.MainView->contextHeight();
}

// Save automatically-named image of current scene
void AtenWindow::snapshotCurrentView()
{
	// Get image
	QPixmap pixmap = scenePixmap(exportImageDialog_.imageWidth(), exportImageDialog_.imageHeight(), exportImageDialog_.imageTransparent());

	// Construct filename
	QString fileName = aten_.workDir().absoluteFilePath(QString("Aten-Snapshot-%1.%2").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss"), exportImageDialog_.imageFormat()));

	pixmap.save(fileName, qPrintable(exportImageDialog_.imageFormat()));
}

// Return image of current view
QPixmap AtenWindow::scenePixmap(int width, int height, bool transparent)
{
	return ui.MainView->generateImage(width, height, transparent);
}

// Return pixmap of specified model
QPixmap AtenWindow::modelPixmap(Model* model, QSize pixmapSize)
{
	return ui.MainView->generateModelImage(model, pixmapSize.width(), pixmapSize.height());
}
