#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
	//================================================================================
	// get the layouts put together
	// TOP PART
	locationSelection     = new QHBoxLayout;
	filepathLocation      = new QLineEdit(QDir::currentPath());
	changeLocation_button = new QPushButton("Change Location");
	scanLocation_button	= new QPushButton("Scan!");
	locationSelection->addWidget(filepathLocation);
	locationSelection->addWidget(changeLocation_button);
	locationSelection->addWidget(scanLocation_button);

	// MIDDLE PART
	imDisplay = new scrollableImageDisplay;

	// BOTTOM PART
	actionSelection           = new QHBoxLayout;
	skipButton                = new QPushButton("Skip (&1)");
	deleteAndRememberButton   = new QPushButton("Delete/Remember (&2)");
	deleteButton              = new QPushButton("Delete (&3)");
	skipShortcut              = new QShortcut(QKeySequence("1"),this);
	deleteAndRememberShortcut = new QShortcut(QKeySequence("2"),this);
	deleteShortcut            = new QShortcut(QKeySequence("3"),this);
	actionSelection->addWidget(skipButton);
	actionSelection->addWidget(deleteAndRememberButton);
	actionSelection->addWidget(deleteButton);
	setDisplayState(SELECT_LOCATION);

	// putting the layouts into main layout
	totalLayout = new QGridLayout(this);
	totalLayout->addLayout(locationSelection,0,0);
	totalLayout->addWidget(imDisplay,1,0);
	totalLayout->addLayout(actionSelection,2,0);

	//making only the center row expand on resize
	//essentially sticks the top and bottoms to their respective places
	totalLayout->setRowStretch(0,0);
	totalLayout->setRowStretch(1,1);
	totalLayout->setRowStretch(2,0);

	//================================================================================
	// attach methods to events

	//buttons on the top
	connect( changeLocation_button,SIGNAL(clicked(bool)),this,SLOT(dialogNewLocation()) );
	connect( scanLocation_button,SIGNAL(clicked(bool)),this,SLOT(scanLocation()) );

	//displaying images from the imLib
	qRegisterMetaType< QList<ImageInfo> >("QList<ImageInfo>"); // tell the meta system about the type for the connection
	connect( &imLib,SIGNAL(newImageParsed(QImage)),             imDisplay,SLOT(displaySingleImage(QImage)),				  Qt::QueuedConnection );
	connect( this,  SIGNAL(startScanAtLocation(QString)),       &imLib,   SLOT(findNewFiles(QString)),                    Qt::QueuedConnection );
	connect( this,  SIGNAL(startFindingCollisions()),           &imLib,   SLOT(findCollisions()),                         Qt::QueuedConnection );
	connect( this,  SIGNAL(continueFindingCollisions(int)),     &imLib,   SLOT(findCollisions(int)),                      Qt::QueuedConnection );
	connect( &imLib,SIGNAL(newCollision(QList<ImageInfo>,int)), this,     SLOT(displayNewCollision(QList<ImageInfo>,int)),Qt::QueuedConnection );
	connect( &imLib,SIGNAL(doneFindingCollisions()),            this,     SLOT(doneWithCurrentDirectory()),               Qt::QueuedConnection );

	//the buttons on the bottom
	connect( skipButton,               SIGNAL(clicked(bool)),                        this,  SLOT(collisionAction()) );
	connect( deleteAndRememberButton,  SIGNAL(clicked(bool)),                        this,  SLOT(collisionAction()) );
	connect( deleteButton,             SIGNAL(clicked(bool)),                        this,  SLOT(collisionAction()) );
	connect( skipShortcut,             SIGNAL(activated()),							 this,  SLOT(collisionAction()) );
	connect( deleteAndRememberShortcut,SIGNAL(activated()),                          this,  SLOT(collisionAction()) );
	connect( deleteShortcut,           SIGNAL(activated()),                          this,  SLOT(collisionAction()) );
	connect( this,                     SIGNAL(deleteImages(QList<ImageInfo>)),		 &imLib,SLOT(deleteImages(QList<ImageInfo>)),      Qt::QueuedConnection );
	connect( this,                     SIGNAL(rememberCollisions(QList<ImageInfo>)), &imLib,SLOT(rememberCollisions(QList<ImageInfo>)),Qt::QueuedConnection );

	//================================================================================
	//start the seperate thread
	imLib.moveToThread(&imLibThread);
	imLibThread.start();

	//temp test stuff
	//imDisplay->displaySingleImage(QImage("/media/RAID/Documents/Pictures/tumblr/1486518304300.png"));
}

MainWindow::~MainWindow()
{
	delete locationSelection;
	delete filepathLocation;
	delete changeLocation_button;
	delete scanLocation_button;

	delete imDisplay;

	delete actionSelection;
	delete skipButton;
	delete deleteAndRememberButton;
	delete deleteButton;
	delete skipShortcut;
	delete deleteAndRememberShortcut;
	delete deleteShortcut;

	delete totalLayout;

	//tell the library to save and then wait on it to finish saving
	imLibThread.quit();
	imLibThread.wait();
}

void MainWindow::dialogNewLocation(){
	QString newLoc = QFileDialog::getExistingDirectory(0,"",filepathLocation->text());
	if(newLoc != "") // is empty is the user cancels
		filepathLocation->setText(newLoc);
}
void MainWindow::scanLocation(){
	setDisplayState(DISABLED);

	//get the filepath
	QString fp = filepathLocation->text();
	//tell the image library to scan (new thread)
	emit startScanAtLocation(fp);

	// now it is time to get some collisions
	emit startFindingCollisions();
}
void MainWindow::displayNewCollision(QList<ImageInfo> collision, int curtPos){
	//this is called when the image library finds a new set of images that collide
	//we need to make sure the action buttons are available to the user
	//and that we display the images to the screen
	//
	//the curtPos is given to us from the image library as the current image index that the collision was found at
	//this index needs to be handed back to the library when the user selects an action

	setDisplayState(COLLISION_SELECTION);
	collisionIndex = curtPos;
	imDisplay->displayImages(collision);
}

void MainWindow::collisionAction(){
	//This is called for any of the three action buttons on the bottom of the window
	//this changes what it does bassed on which button was clicked

	//dont let the user screw things up by disabling the buttons
	setDisplayState(DISABLED);

	QObject *buttonSource = QObject::sender(); // ask which button emited the signal
	if(buttonSource == skipButton || buttonSource == skipShortcut){
		//printf("Skip!!\n");
		emit continueFindingCollisions(collisionIndex);
	}else if(buttonSource == deleteAndRememberButton || buttonSource == deleteAndRememberShortcut){
		//printf("Delete+Remember\n");
		QList<ImageInfo> want = imDisplay->getWantedImages();
		QList<ImageInfo> not_want = imDisplay->getUnwantedImages();
		//printf("Wanted %d\nUnwanted %d\n",want.length(),not_want.length());
		emit rememberCollisions(want); // must do this one first
		emit deleteImages(not_want);
		emit continueFindingCollisions(collisionIndex);
	}else if(buttonSource == deleteButton || buttonSource == deleteShortcut){
		//printf("Delete Only\n");
		//QList<ImageInfo> want = imDisplay->getWantedImages();
		QList<ImageInfo> not_want = imDisplay->getUnwantedImages();
		//printf("Wanted %d\nUnwanted %d\n",want.length(),not_want.length());
		emit deleteImages(not_want);
		emit continueFindingCollisions(collisionIndex);
	} // else do nothing as we have no clue who it was that called us
}

void MainWindow::doneWithCurrentDirectory(){
	setDisplayState(SELECT_LOCATION);
	imDisplay->removeAllItems(); // dont leave things on the screen
	QMessageBox::information(this,"Done Processing","This Directory is Done");
}

void MainWindow::setDisplayState(DisplayState state){
	if(state == SELECT_LOCATION){
		filepathLocation->setEnabled(true);
		changeLocation_button->setEnabled(true);
		scanLocation_button->setEnabled(true);

		skipButton->setEnabled(false);
		deleteAndRememberButton->setEnabled(false);
		deleteButton->setEnabled(false);
		skipShortcut->setEnabled(false);
		deleteAndRememberShortcut->setEnabled(false);
		deleteShortcut->setEnabled(false);
	}else if(state == COLLISION_SELECTION){
		filepathLocation->setEnabled(false);
		changeLocation_button->setEnabled(false);
		scanLocation_button->setEnabled(false);

		skipButton->setEnabled(true);
		deleteAndRememberButton->setEnabled(true);
		deleteButton->setEnabled(true);
		skipShortcut->setEnabled(true);
		deleteAndRememberShortcut->setEnabled(true);
		deleteShortcut->setEnabled(true);
	}else if(state == DISABLED){
		filepathLocation->setEnabled(false);
		changeLocation_button->setEnabled(false);
		scanLocation_button->setEnabled(false);

		skipButton->setEnabled(false);
		deleteAndRememberButton->setEnabled(false);
		deleteButton->setEnabled(false);
		skipShortcut->setEnabled(false);
		deleteAndRememberShortcut->setEnabled(false);
		deleteShortcut->setEnabled(false);
	}
}
