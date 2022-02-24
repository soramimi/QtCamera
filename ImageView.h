#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QWidget>

class QVideoFrame;

class ImageView : public QWidget {
	Q_OBJECT
	friend class MyVideoSurface;
private:
	struct Private;
	Private *m;
protected:
	bool present(const QVideoFrame &frame);
	void paintEvent(QPaintEvent *event);
public:
	ImageView(QWidget *parent);
	~ImageView();
};

#endif // IMAGEVIEW_H
