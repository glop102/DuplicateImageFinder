#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QDir>
#include <QFileDialog>
#include <QShortcut>
#include <QMessageBox>
#include "imagelibrary.h"
#include "scrollableimagedisplay.h"

/*
 * This is the window that contains all the main widgets
 * there are 3 areas
 * top - controls the directory to scan in and when to scan
 * middle - shows the images and lets users decide which they want
 * bottom - lets them select actions to apply to the images in the middle
 */

class MainWindow : public QWidget
{
    Q_OBJECT
public:
	typedef enum DISPLAY_STATE{SELECT_LOCATION,COLLISION_SELECTION,DISABLED} DisplayState;

	ImageLibrary imLib;
	QThread imLibThread;

	QHBoxLayout *locationSelection;
	QLineEdit* filepathLocation;
	QPushButton *changeLocation_button,*scanLocation_button;

	scrollableImageDisplay* imDisplay;

	QHBoxLayout *actionSelection;
	QPushButton *skipButton,*deleteAndRememberButton,*deleteButton;
	QShortcut *skipShortcut,*deleteAndRememberShortcut,*deleteShortcut;

	QGridLayout *totalLayout;

	int collisionIndex; // used to remember the posistion told to us from the imLib

    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
	void dialogNewLocation();
	void scanLocation();
	void displayNewCollision(QList<ImageInfo> collision, int curtPos);
	void collisionAction();
	void doneWithCurrentDirectory();
	void setDisplayState(DisplayState state);

signals:
	void startScanAtLocation(QString);
	void startFindingCollisions();
	void continueFindingCollisions(int);
	void deleteImages(QList<ImageInfo>);
	void rememberCollisions(QList<ImageInfo>);
};

#endif // MAINWINDOW_H
