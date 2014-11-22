#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QtOpenGL>
#include "ui_MainWindow.h"

class SidePanel;
class AnimationPanel;
class EditorWindow;
class ThumbnailRenderer;
class VideoModel;
class MeshModel;
class KVFModel;
class QLabel;

class MainWindow : public QMainWindow, public Ui_MainWindow
{
	Q_OBJECT
public:
	MainWindow();
	virtual ~MainWindow();

	void setStatusBarStatistics(int vertexCount, int facesCount);
	void setStatusBarFPS(int msec);
	void clearStatusBar();

public slots:
	void loadModel();
	void saveModel();
    void chooseTexture();
	void resetTexture();

	void onFrameSwitched(MeshModel* model);
	void onModelUpdate(KVFModel* model);
	void onEditBoxNewFrameSelected(MeshModel* model);

	void onSidePanelMenuShowHide(bool checked);
	void onAnimationPanelMenuShowHide(bool checked);

	void onSaveScreenShot();
	void onSaveVideo();

	void onAbout();

signals:
	void videoModelLoaded(VideoModel* model);
	void frameSwitched(MeshModel* model);
	void textureChanged(GLuint textureRef);

private:
	SidePanel* sidePanel;
	AnimationPanel* animationPanel;
	EditorWindow *mainScene;

	QLabel* lblVertexCount;
	QLabel* lblFacesCount;
	QLabel* lblFPS;

	VideoModel* model;
	MeshModel* currentFrameModel;
	GLuint textureRef;

	ThumbnailRenderer* thumbnailRender;
};

#endif
