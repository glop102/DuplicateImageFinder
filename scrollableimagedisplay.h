#ifndef SCROLLABLEIMAGEDISPLAY_H
#define SCROLLABLEIMAGEDISPLAY_H

#include <QScrollArea>
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include <QList>
#include <QPushButton>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QEvent>
#include <QResizeEvent>
#include <QWheelEvent>
#include <stdio.h>
#include "imageinfo.h"
#include "imagebutton.h"

/*
 * Rather simple thing
 * Is the middle widget that houses the images a user can click on
 * this is just to contain the horizontal list of these images
 * There are only 2 things of note
 *   heightOverride - used to fix the bad behavior of automatic resizing of the contained buttons
 *   resizeEvent - applys the heightOverride to the images as the main windows changes shape
 */

class scrollableImageDisplay : public QScrollArea
{
	Q_OBJECT
	QWidget *imDispArea;
	QList<ImageButton*> imageLabels;
	QHBoxLayout *layout;

	int heightOverride; // we remember the override height that we will use for the image buttons

public:
	explicit scrollableImageDisplay(QWidget *parent = 0);
	~scrollableImageDisplay();

	//bool eventFilter(QObject *, QEvent *); // catches resizes for the smaller widget so we restrict it's size
	void removeAllItems();
	QList<ImageInfo> getWantedImages(); // the green ones
	QList<ImageInfo> getUnwantedImages(); // the red ones

signals:

public slots:
	void displaySingleImage(QImage im);
	void displayImages(QList<ImageInfo> ims);
	void resizeEvent(QResizeEvent *event);
	void wheelEvent(QWheelEvent *);
};

#endif // SCROLLABLEIMAGEDISPLAY_H
