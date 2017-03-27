#include "imagebutton.h"

ImageButton::ImageButton(QWidget *parent) : QWidget(parent){
	layout=0;
	imLabel=0;
	imSize=0;
	imFilename=0;
	imPath=0;
}
ImageButton::ImageButton(QImage im){
	this->im = im;
	pixmap = QPixmap::fromImage(this->im);
	imFilename = new QLabel("");
	imPath = new QLabel("");
	imSize = new QLabel( QString::number(im.width()) + 'x' + QString::number(im.height()) );
	imLabel = new QLabel;

	imLabel->setPixmap(pixmap);

	layout = new QVBoxLayout(this);
	layout->addWidget(imFilename);
	layout->addWidget(imPath);
	layout->addWidget(imSize);
	layout->addWidget(imLabel);
	layout->setStretch(0,0);
	layout->setStretch(1,0);
	layout->setStretch(2,1); // only the image is allowed to stretch
	this->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);

	keeping = true;
}
ImageButton::ImageButton(ImageInfo imi){
	imInfo = imi;

	im = QImage(imi.filePath + '/' + imi.fileName);
	pixmap = QPixmap::fromImage(im);
	imFilename = new QLabel(imi.fileName);
	imPath = new QLabel(imi.filePath);
	imSize = new QLabel( QString::number(im.width()) + 'x' + QString::number(im.height()) );
	imLabel = new QLabel;

	imLabel->setPixmap(pixmap);

	layout = new QVBoxLayout(this);
	layout->addWidget(imFilename);
	layout->addWidget(imPath);
	layout->addWidget(imSize);
	layout->addWidget(imLabel);
	layout->setStretch(0,0);
	layout->setStretch(1,0);
	layout->setStretch(2,1); // only the image is allowed to stretch
	this->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);

	keeping = true;

	QPalette p = this->palette();
	p.setColor(QPalette::Background, Qt::green);
	this->setAutoFillBackground(true);
	this->setPalette(p);

	this->show();
}

ImageButton::~ImageButton(){
	delete layout;
	delete imLabel;
	delete imSize;
	delete imPath;
	delete imFilename;
}

void ImageButton::mousePressEvent(QMouseEvent* e){
	mousePos[0]=e->x();
	mousePos[1]=e->y();
}
void ImageButton::mouseReleaseEvent(QMouseEvent* e){
	if(abs(mousePos[0] - e->x())<5 && abs(mousePos[1] - e->y())<5){ // if the mouse hasn't moved too much
		QPalette p = this->palette();
		if(keeping){
			keeping = false;
			p.setColor(QPalette::Background, Qt::red);
		}else{
			keeping = true;
			p.setColor(QPalette::Background, Qt::green);
		}
		this->setAutoFillBackground(true);
		this->setPalette(p);
	}
}

ImageInfo ImageButton::getImageInfo(){
	//WARNING
	//You are not guarenteed to get returned a valid object
	//If you never constructed this object with ImageInfo then this will not mean anything
	return imInfo;
}

void ImageButton::resizeEvent(QResizeEvent *event){
	//printf("%dx%d\n",event->size().width(),event->size().height());
	//int newImHeight = event->size().height();

	//printf("%dx%d\n",heightOverride,event->size().height());
	int newImHeight = heightOverride;
	newImHeight -= imFilename->height() + imPath->height() + imSize->height();
	// idk but some sort of extra padding is added in and this seems about the right size
	newImHeight -= imFilename->height() + imPath->height() + imSize->height();

	if(newImHeight < im.height())
		pixmap = QPixmap::fromImage(  im.scaledToHeight(newImHeight)  );
	else
		pixmap = QPixmap::fromImage( im );
	imLabel->setPixmap(pixmap);
}

void ImageButton::setHeightOverride(int height){
	heightOverride = height;
	this->resize(0,0); // just force a resize event
}
