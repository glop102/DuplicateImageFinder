#include "scrollableimagedisplay.h"

scrollableImageDisplay::scrollableImageDisplay(QWidget *parent) : QScrollArea(parent){
	imDispArea = new QWidget;
	layout = new QHBoxLayout(imDispArea);
	layout->setSizeConstraint(QLayout::SetMinAndMaxSize); // makes things acctually apear - something auto setting min and max size hints
	imDispArea->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
	imDispArea->show();

	//this->setBackgroundRole(QPalette::Dark); // this breaks the background color of child widgets
	this->setWidget(imDispArea);
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}
scrollableImageDisplay::~scrollableImageDisplay(){
	removeAllItems();
	delete layout;
	delete imDispArea;
}

void scrollableImageDisplay::removeAllItems(){
	for(int x=imageLabels.length()-1; x>=0; x--){
		layout->removeWidget(imageLabels[x]);
		delete imageLabels[x];
		imageLabels.removeLast();
	}
	imageLabels.clear();
}

QList<ImageInfo> scrollableImageDisplay::getWantedImages(){
	QList<ImageInfo> l;
	for(int x=0; x<imageLabels.length(); x++){
		if(imageLabels[x]->keeping)
			l.push_back(imageLabels[x]->getImageInfo());
	}
	return l;
}
QList<ImageInfo> scrollableImageDisplay::getUnwantedImages(){
	QList<ImageInfo> l;
	for(int x=0; x<imageLabels.length(); x++){
		if(! imageLabels[x]->keeping)
			l.push_back(imageLabels[x]->getImageInfo());
	}
	return l;
}

void scrollableImageDisplay::displaySingleImage(QImage im){
	removeAllItems();

	//make a new widget
	ImageButton *l = new ImageButton(im);
	l->setHeightOverride(heightOverride);
	imageLabels.push_back(l);

	layout->addWidget(l);
}
void scrollableImageDisplay::displayImages(QList<ImageInfo> ims){
	removeAllItems();

	for(int x=0; x<ims.length(); x++){
		ImageButton *l = new ImageButton(ims[x]);
		l->setHeightOverride(heightOverride);
		imageLabels.push_back(l);

		layout->addWidget(l);
	}
}

void scrollableImageDisplay::resizeEvent(QResizeEvent *event){
	heightOverride = event->size().height() - horizontalScrollBar()->height();
	for(int x=0; x<imageLabels.length(); x++)
		imageLabels[x]->setHeightOverride(heightOverride);
}

void scrollableImageDisplay::wheelEvent(QWheelEvent *ev){
	//this is only here to make things scroll horizontally and not vertically when using the mouse scrollwheel
	QWheelEvent h_scroll(ev->pos(),ev->delta(),ev->buttons(),ev->modifiers(),Qt::Horizontal);
	QScrollArea::wheelEvent(&h_scroll);
}
