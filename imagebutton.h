#ifndef IMAGEBUTTON_H
#define IMAGEBUTTON_H

#include <QImage>
#include <QPixmap>
#include <QLabel>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <stdio.h>
#include <QMouseEvent>
#include <QSizePolicy>
#include "imageinfo.h"

/*
 * Hello to my future self
 * This widget is to simulate a toggle button. The background if green/red when clicked
 * This is the button in the middle of the scroll area of the program
 * The parent of this is a simple container widget with an HBoxLayout
 * and the parent of that widget is the QScrollArea
 *
 * this can display 2 types of images
 * a simple QImagewhere is tells you a size and shows the image
 * a less simple ImageInfo that the ImageLibrary gives out with the filename
 *
 * This does 3 jobs
 * display information (filename, size, the image itself)
 * resize the image to never be TALLER than the QScrollArea that contains these
 * keep track of it being kept or not by changing the background color when clicked
 */

class ImageButton : public QWidget
{
	Q_OBJECT
	QVBoxLayout *layout;
	QImage im;
	QPixmap pixmap;
	QLabel *imFilename,*imPath,*imSize,*imLabel;

	ImageInfo imInfo;
	int mousePos[2];
	int heightOverride; // we force ourselves to ALWAYS be this tall even if the resize event says otherwise
						// the layout in the parent widget keeps resizing us to larger if we dont do this
public:
	bool keeping;

	explicit ImageButton(QWidget *parent = 0);
	ImageButton(QImage);
	ImageButton(ImageInfo);
	~ImageButton();

	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void resizeEvent(QResizeEvent *event);

	ImageInfo getImageInfo();
	void setHeightOverride(int height);

signals:

public slots:
};

#endif // IMAGEBUTTON_H
